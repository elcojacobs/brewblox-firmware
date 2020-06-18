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

class DS2408Mock : public OneWireMockDevice {
private:
    uint8_t registers[13] = {0};
    uint8_t& cmd = registers[0];
    uint8_t& addrl = registers[1];
    uint8_t& addrh = registers[2];
    uint8_t& pins = registers[3];
    uint8_t& latches = registers[4]; // a zero means latch disabled for the DS2408
    uint8_t& activity = registers[5];
    uint8_t& conditialSearchMask = registers[6];
    uint8_t& conditialSearchPolarity = registers[7];
    uint8_t& status = registers[8];
    uint8_t& crcl = registers[11];
    uint8_t& crch = registers[12];
    uint8_t externalPullDowns = 0x00;
    uint8_t sampleCounter = 0;
    uint8_t samples[32] = {0};

public:
    static constexpr uint8_t family_code{0x29};

    DS2408Mock(const OneWireAddress& address)
        : OneWireMockDevice(address)
    {
        status = 0x08; // Only power on reset bit is high
        registers[9] = 0xFF;
        registers[10] = 0xFF;
        update();
    }

    void update()
    {
        // conditional search is not implemented in this mock
        // neither is RSTZ output
        // neither are activity latches
        if (!parasite) {
            status |= uint8_t(0x80);
        } else {
            status &= ~uint8_t(0x80);
        }

        pins = latches & externalPullDowns;
        uint16_t crc = OneWireCrc16(registers, 11);
        crch = crc >> 8;
        crcl = crc & 0xFF;
    }

    virtual void processImpl(uint8_t newCmd) override final
    {
        if (newCmd) { // repeat last command if no new command received (0x00)
            cmd = newCmd;
        }
        switch (cmd) {
        case 0xF0: // Read PIO registers, start address is 0x0088
        {
            addrl = recv();
            addrh = recv();
            update();
            uint16_t idx = (uint16_t(addrh) << 8) | addrl - uint16_t(0x0088) + 3;
            while (idx < 13) {
                send(registers[idx]);
            }
        } break;
        case 0xF5: // Channel access read, can be repeatedly read. 32x pins followed by inverted 16-bit CRC
        {
            if (sampleCounter < 32) {
                pins = latches & externalPullDowns;
                send(pins);
                samples[sampleCounter] = pins;
            }
            if (sampleCounter == 32) {
                uint16_t crc = OneWireCrc16(registers, 11);
                crch = crc >> 8;
                crcl = crc & 0xFF;
                send(crch);
            } else {
                sampleCounter = 0;
                send(crcl);
            }
        } break;
        case 0x5A: // Channel access write
        {
            uint8_t newLatches = recv();
            uint8_t inverted = ~recv();
            if (newLatches == inverted) {
                send(0xAA); // confirm
                latches = newLatches;
                send(pins);
            }
        } break;
        default:
            break;
        }
    }

    virtual void resetImpl() override final
    {
        cmd = 0x00;
    }
};
