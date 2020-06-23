/*
 * Copyright 2012-2020 BrewPi/Elco Jacobs.
 * Copyright 2013 Matthew McGowan.
 *
 * This file is part of BrewPi.
 * 
 * BrewPi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * BrewPi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with BrewPi.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "../inc/Logger.h"

#include "../inc/DS18B20.h"
#include "../inc/OneWire.h"
#include "../inc/OneWireAddress.h"
#include "../inc/Temperature.h"

// OneWire commands
static constexpr const uint8_t STARTCONVO = 0x44;      // Start a new conversion to be read from scratchpad 750ms later
static constexpr const uint8_t COPYSCRATCH = 0x48;     // Copy scratchpad to EEPROM
static constexpr const uint8_t READSCRATCH = 0xBE;     // Read scratchpad
static constexpr const uint8_t WRITESCRATCH = 0x4E;    // Write alarm temp and config to scratchpad
static constexpr const uint8_t RECALLSCRATCH = 0xB8;   // Copy EEPROM to scratchpad
static constexpr const uint8_t READPOWERSUPPLY = 0xB4; // Determine if device needs parasite power
static constexpr const uint8_t ALARMSEARCH = 0xEC;     // Query bus for devices with an alarm condition

// Scratchpad locations
static constexpr const uint8_t TEMP_LSB = 0;
static constexpr const uint8_t TEMP_MSB = 1;
static constexpr const uint8_t HIGH_ALARM_TEMP = 2;
static constexpr const uint8_t LOW_ALARM_TEMP = 3;
static constexpr const uint8_t CONFIGURATION = 4;
static constexpr const uint8_t INTERNAL_BYTE = 5;
static constexpr const uint8_t COUNT_REMAIN = 6;
static constexpr const uint8_t COUNT_PER_C = 7;
static constexpr const uint8_t SCRATCHPAD_CRC = 8;

// error values
static constexpr const int16_t DEVICE_DISCONNECTED_RAW = -2032;
static constexpr const int16_t RESET_DETECTED_RAW = -2031;

/**
 * Initializes the temperature sensor.
 * This method should be called when the sensor is first created and also any time the sensor reports it has been reset.
 * This re-intializes the reset detection.
 */
void
DS18B20::init()
{
    ScratchPad scratchPad;
    bool writeSettings = false;

    // parasitic power is not supported, it is unreliable at higher temperatures or with many devices on the bus
    if (isParasitePowered()) {
        return;
    }
    // Reload settings from EEPROM so we can skip writing EEPROM if the values are already set
    recallScratchpad();

    if (!readScratchPad(scratchPad)) {
        return;
    }

    // always use 12 bits
    if (scratchPad[CONFIGURATION] != 0x7F) {
        scratchPad[CONFIGURATION] = 0x7F;
        writeSettings = true;
    }

    // Make sure that HIGH_ALARM_TEMP is set to zero in EEPROM
    // This value will be loaded when the device powers on
    if (scratchPad[HIGH_ALARM_TEMP]) { // conditional to avoid wear on eeprom.
        scratchPad[HIGH_ALARM_TEMP] = 0;
        writeSettings = true;
    }

    if (writeSettings) {
        writeScratchPad(scratchPad, true); // save settings to eeprom
    }
    // Write HIGH_ALARM_TEMP again, but don't save to EEPROM, so that it reverts to 0 on reset
    // from this point on, if we read a scratchpad with a  different value than 1 HIGH_ALARM (detectedReset() returns true)
    // it means the device has reset and reloaed the 0 value from EEPROM or the previous write of the scratchpad above was unsuccessful.
    // Either way, initConnection() should be called again
    scratchPad[HIGH_ALARM_TEMP] = 1;
    writeScratchPad(scratchPad, false);

    startConversion();
}

void
DS18B20::startConversion()
{
    selectRom();
    oneWire.write(STARTCONVO);
    oneWire.reset();
}

void
DS18B20::connected(bool _connected)
{
    if (m_connected == _connected) {
        return; // state stays the same
    }
    m_connected = _connected;
    if (m_connected) {
        // CL_LOG_WARN("OneWire temp sensor connected: ") << getDeviceAddress().toString();
    } else {
        // CL_LOG_WARN("OneWire temp sensor disconnected: ") << getDeviceAddress().toString();
    }
}

temp_t
DS18B20::value() const
{
    if (!m_connected) {
        return 0;
    }

    return m_cachedValue;
}

void
DS18B20::update()
{
    m_cachedValue = readAndConstrainTemp();
    startConversion();
}

temp_t
DS18B20::readAndConstrainTemp()
{
    // difference in precision between DS18B20 format and temperature format
    static constexpr const int32_t scale = 1 << (cnl::_impl::fractional_digits<temp_t>::value - 4);
    int32_t tempRaw;
    bool success;

    tempRaw = getRawTemp();
    success = tempRaw > RESET_DETECTED_RAW;

    if (tempRaw == RESET_DETECTED_RAW) {
        // retry re-init if the sensor is present, but needs a reset
        init();
    }

    connected(success);

    if (!success) {
        return 0;
    }

    temp_t temp = cnl::wrap<temp_t>(tempRaw * scale) + m_calibrationOffset;

    return temp;
}

bool
DS18B20::readScratchPad(ScratchPad& scratchPad)
{
    for (uint8_t retries = 0; retries < 2; retries++) {
        selectRom();
        oneWire.write(READSCRATCH);
        for (uint8_t i = 0; i < 9; i++) {
            scratchPad[i] = oneWire.read();
        }
        oneWire.reset();
        if (scratchPad.valid()) {
            return true;
        }
    }
    return false;
}

void
DS18B20::writeScratchPad(const ScratchPad& scratchPad, bool copyToEeprom)
{
    selectRom();
    oneWire.write(WRITESCRATCH);

    for (uint8_t i = HIGH_ALARM_TEMP; i <= CONFIGURATION; i++) {
        oneWire.write(scratchPad[i]); // high alarm temp
    }

    // save the newly written values to eeprom
    if (copyToEeprom) {
        selectRom();
        oneWire.write(COPYSCRATCH);
    }
    oneWire.reset();
}

void
DS18B20::recallScratchpad()
{
    selectRom();
    oneWire.write(RECALLSCRATCH);
    oneWire.reset();
}

bool
DS18B20::
    isParasitePowered()
{
    selectRom();
    oneWire.write(READPOWERSUPPLY);
    // Parasite powered sensors pull the bus low
    bool result = oneWire.read_bit() == 0;
    oneWire.reset();
    return result;
}

int16_t
DS18B20::getRawTemp()
{
    ScratchPad scratchPad;
    if (!readScratchPad(scratchPad)) {
        return DEVICE_DISCONNECTED_RAW;
    }
    // return DEVICE_DISCONNECTED when a reset has been detected to force it to be reconfigured
    // we detect a reset by creating a mismatch beteween the eeprom and the on device scratchpad
    // On reset, the EEPROM value will be reloaded, signaling that a reset has  occurred
    if (scratchPad[HIGH_ALARM_TEMP] != 1) {
        return RESET_DETECTED_RAW;
    }
    int16_t rawTemperature = (((int16_t)scratchPad[TEMP_MSB]) << 8) | scratchPad[TEMP_LSB];
    return rawTemperature;
}
