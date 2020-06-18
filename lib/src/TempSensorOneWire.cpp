/*
 * Copyright 2012-2013 BrewPi/Elco Jacobs.
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
