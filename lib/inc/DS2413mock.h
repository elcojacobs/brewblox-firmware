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

#include "OneWireMockDevice.h"

class DS2413Mock : public OneWireMockDevice {
private:
    bool latchA = true; // a zero means latch disabled for the DS2413
    bool latchB = true;
    bool pinA = true;
    bool pinB = true;
    bool externalPullDownA = false;
    bool externalPullDownB = false;
    uint8_t lastCommand = 0x00;

public:
    static constexpr uint8_t family_code{0x3A};

    DS2413Mock(const OneWireAddress& address)
        : OneWireMockDevice(address)
    {
    }

    virtual void processImpl(uint8_t cmd) override final
    {
        switch (cmd) {
        case 0xF5: // PIO ACCESS READ
        {
            lastCommand = 0xF5;
            sendStatus();
            // will repeat status on each read without a write first until reset, not implemented
        } break;
        case 0x5A: // PIO ACCESS WRITE
        {
            lastCommand = 0x5A;
            receiveConfig();
            // If no new command is sent by the master, it will act as if a PIO WRITE command was sent until reset
            // This is not implemented in the mock
        } break;
        case 0x00: // PIO ACCESS WRITE
            if (lastCommand == 0x5F) {
                sendStatus();
            } else if (lastCommand == 0x5A) {
                receiveConfig();
            } else {
                send(0xFF);
            }
            break;
        default:
            break;
        }
    }

    virtual void resetImpl() override final
    {
        lastCommand = 0x00;
    }

    void
    sendStatus()
    {
        uint8_t status = 0x00;
        pinA = latchA;
        pinB = latchB;
        if (pinA) {
            status |= 0b0001;
        }
        if (latchA) {
            status |= 0b0010;
        }
        if (pinB) {
            status |= 0b0100;
        }
        if (latchB) {
            status |= 0b1000;
        }
        status |= (~status) << 4; // upper bits are complemet for error checking
        send(status);
    }

    void receiveConfig()
    {
        uint8_t received = recv();
        uint8_t complement = ~recv();
        if (received == complement) {
            latchA = (received & 0x1) > 0;
            latchB = (received & 0x2) > 0;
            send(0xAA); // confirmation byte
            sendStatus();
        } else {
            send(0xFF);
            lastCommand = 0x00;
        }
    }
};
