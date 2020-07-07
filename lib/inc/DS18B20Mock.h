/*
 * Copyright 2020 BrewPi B.V.
 *
 * This file is part of the Brewblox Control Library.
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
 * along with Brewblox.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "OneWireCrc.h"
#include "OneWireMockDevice.h"
#include "Temperature.h"

class DS18B20Mock : public OneWireMockDevice {
private:
    uint8_t scratchpad[9];
    uint8_t eeprom[3];
    bool parasite = false;

public:
    static constexpr uint8_t family_code{0x28};

    DS18B20Mock(const OneWireAddress& address)
        : OneWireMockDevice(address)
    {
        scratchpad[0] = 0x40; // TLSB
        scratchpad[1] = 0x01; // TMSB // default to 20 deg
        scratchpad[2] = 0x4B; // THRE
        scratchpad[3] = 0x46; // TLRE
        scratchpad[4] = 0x7F; // Conf
        scratchpad[5] = 0xFF; // 0xFF
        scratchpad[6] = 0x00; // Reset
        scratchpad[7] = 0x10; // 0x10
        scratchpad[8] = OneWireCrc8(scratchpad, 8);
        eeprom[0] = scratchpad[2];
        eeprom[1] = scratchpad[3];
        eeprom[2] = scratchpad[4];
    }

    virtual void processImpl(uint8_t cmd) override final
    {
        switch (cmd) {
        case 0x4E: // WRITE SCRATCHPAD
            // write 3 byte of data to scratchpad[2:4], ds18s20 only first 2 bytes (TH, TL)
            recv(&scratchpad[2], 3);
            scratchpad[8] = OneWireCrc8(scratchpad, 8);
            break;
        case 0xBE: // READ SCRATCHPAD
            send(scratchpad, 9);
            break;

        case 0x48: // COPY SCRATCHPAD to EEPROM
            eeprom[0] = scratchpad[2];
            eeprom[1] = scratchpad[3];
            eeprom[2] = scratchpad[4];
            break;

        case 0xB8: // RECALL EEPROM
            scratchpad[2] = eeprom[0];
            scratchpad[3] = eeprom[1];
            scratchpad[4] = eeprom[2];
            break;

        case 0xB4: // READ POWER SUPPLY
        {
            // 1: externally powered, 0: parasite power
            send(parasite ? 0x00 : 0x80);
        } break;

        case 0x44: // CONVERT
            break;
        default:
            break;
        }
    }

    static constexpr const int16_t scale = 1 << (cnl::_impl::fractional_digits<temp_t>::value - 4);
    void setTemperature(temp_t temperature)
    {
        int16_t raw = cnl::unwrap(temperature) / scale;
        scratchpad[0] = raw & 0xFF;
        scratchpad[1] = (int8_t)(raw >> 8);
        scratchpad[8] = OneWireCrc8(scratchpad, 8);
    }
    temp_t getTemperature() const
    {
        int16_t rawTemperature = (((int16_t)scratchpad[1]) << 8) | scratchpad[0];
        int32_t shifted = int32_t(rawTemperature) * scale;
        return cnl::wrap<temp_t>(shifted);
    }
};
