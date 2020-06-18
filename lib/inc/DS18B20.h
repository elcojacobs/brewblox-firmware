/*
 * Copyright 2018 BrewPi B.V.
 *
 * This file is part of the BrewBlox Control Library.
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

#pragma once

#include "OneWireAddress.h"
#include "OneWireDevice.h"
#include "TempSensor.h"
#include "Temperature.h"

class OneWire;

#define ONEWIRE_TEMP_SENSOR_PRECISION (4)

class DS18B20 final : public TempSensor, public OneWireDevice {
    class ScratchPad {
    public:
        ScratchPad()
        {
            for (uint8_t i = 0; i < 9; i++) {
                data[i] = 0;
            }
        }
        ~ScratchPad() = default;

        uint8_t data[9];

        uint8_t& operator[](uint8_t i)
        {
            return data[i];
        }
        const uint8_t& operator[](uint8_t i) const
        {
            return data[i];
        }

        bool valid()
        {
            return OneWire::crc8(data, 8) == data[8];
        }
    };

public:
private:
    temp_t m_calibrationOffset;
    temp_t m_cachedValue = 0;

public:
    /**
	 * Constructs a new onewire temp sensor.
	 * /param bus	The onewire bus this sensor is on.
	 * /param address	The onewire address for this sensor. If all bytes are 0 in the address, the first temp sensor
	 *    on the bus is used.
	 * /param calibration	A temperature value that is added to all readings. This can be used to calibrate the sensor.	 
	 */
    DS18B20(OneWire& bus, OneWireAddress _address = 0, const temp_t& _calibrationOffset = 0)
        : OneWireDevice(bus, _address)
        , m_calibrationOffset(_calibrationOffset)
    {
    }
    DS18B20(const DS18B20&) = delete;
    DS18B20& operator=(const DS18B20&) = delete;

    virtual ~DS18B20() = default;

    virtual bool valid() const override final
    {
        return m_connected;
    }

    virtual temp_t value() const override final; // return cached value
    void update();                               // read from hardware sensor

    void setCalibration(temp_t const& calib)
    {
        m_calibrationOffset = calib;
    }

    temp_t getCalibration() const
    {
        return m_calibrationOffset;
    }

private:
    void init();

    void connected(bool _connected);

    void requestConversion();

    /**
	 * Reads the temperature. If successful, constrains the temp to the range of the temperature type and
	 * updates lastRequestTime. On successful, leaves lastRequestTime alone and returns DEVICE_DISCONNECTED.
	 */
    temp_t readAndConstrainTemp();

    bool readScratchPad(ScratchPad& scratchPad)
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
    writeScratchPad(const ScratchPad& scratchPad, bool copyToEeprom)
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
    recallScratchpad()
    {
        selectRom();
        oneWire.write(RECALLSCRATCH);
        oneWire.reset();
    }

    bool
    isParasitePowered()
    {
        selectRom();
        oneWire.write(READPOWERSUPPLY);
        // Parasite powered sensors pull the bus low
        bool result = oneWire.read_bit() == 0;
        oneWire.reset();
        return result;
    }

    void startConversion();

    int16_t getRawTemp()
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
};
