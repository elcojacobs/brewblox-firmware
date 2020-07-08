/*
 * Copyright 2018-2020 BrewPi B.V.
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
#include "OneWireCrc.h"
#include "OneWireDevice.h"
#include "TempSensor.h"
#include "Temperature.h"

class OneWire;

class DS18B20 final : public TempSensor, public OneWireDevice {
    class ScratchPad {
    public:
        ScratchPad()
        {
            data[8] = 1; // force CRC error when data is not changed
        }
        ~ScratchPad() = default;

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
            return OneWireCrc8(data, 8) == data[8];
        }

    private:
        uint8_t data[9] = {0};
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

    static constexpr uint8_t familyCode{0x28};

    virtual bool valid() const override final
    {
        return connected();
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

    void requestConversion();

    /**
	 * Reads the temperature. If successful, constrains the temp to the range of the temperature type and
	 * updates lastRequestTime. On successful, leaves lastRequestTime alone and returns DEVICE_DISCONNECTED.
	 */
    temp_t readAndConstrainTemp();

    bool readScratchPad(ScratchPad& scratchPad);

    void writeScratchPad(const ScratchPad& scratchPad, bool copyToEeprom);

    void recallScratchpad();

    bool isParasitePowered();

    void startConversion();

    int16_t getRawTemp();
};
