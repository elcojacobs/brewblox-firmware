/*
 * Copyright 2018 BrewPi/Elco Jacobs.
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

#include "DS2408.h"
#include "Logger.h"
#include "OneWireCrc.h"
#include <cstring>

static constexpr uint8_t READ_PIO_REG = 0xF0;
static constexpr uint8_t ACCESS_READ = 0xF5;
static constexpr uint8_t ACCESS_WRITE = 0x5A;
static constexpr uint8_t ACK_SUCCESS = 0xAA;
static constexpr uint8_t ACK_ERROR = 0xFF;

// all addresses have upper bits 0x00
static constexpr uint8_t ADDRESS_UPPER = 0x00;
static constexpr uint8_t ADDRESS_PIO_STATE_LOWER = 0x88;
static constexpr uint8_t ADDRESS_LATCH_STATE_LOWER = 0x89;

bool
DS2408::writeNeeded() const
{
    return !connected() || desiredLatches != latches;
}

bool
DS2408::update()
{
    selectRom();

    // Compute the 1-Wire CRC16 and compare it against the received CRC.
    // Put everything in one buffer so we can compute the CRC easily.
    uint8_t buf[13];

    buf[0] = READ_PIO_REG;            // Read PIO Registers
    buf[1] = ADDRESS_PIO_STATE_LOWER; // LSB address
    buf[2] = ADDRESS_UPPER;           // MSB address
    oneWire.write_bytes(buf, 3);      // Write 3 cmd bytes
    oneWire.read_bytes(&buf[3], 10);  // Read 6 data bytes, 2 0xFF, CRC16

    uint16_t crcCalculated = OneWireCrc16(buf, 11);
    // device sends CRC inverted
    uint16_t crcReceived = ~((uint16_t(buf[12]) << 8) | uint16_t(buf[11]));
    bool success = crcCalculated == crcReceived;
    connected(success);

    if (success) {
        pins = buf[3];
        latches = buf[4];
        activity = buf[5];
        cond_search_mask = buf[6];
        cond_search_pol = buf[7];
        status = buf[8];
        dirty = false;
    }
    if (writeNeeded()) {
        if (selectRom()) {
            uint8_t bytes[3] = {ACCESS_WRITE, desiredLatches, uint8_t(~desiredLatches)};

            if (oneWire.write_bytes(bytes, 3)) {
                /* Acknowledgement byte, 0xAA for success, 0xFF for failure. */
                uint8_t ack;
                if (oneWire.read(ack) && ack == ACK_SUCCESS) {
                    success = success && oneWire.read(pins);
                }
            };
        }
        connected(success);
    }

    oneWire.reset();

    return success;
}

bool
DS2408::senseChannelImpl(uint8_t channel, State& result) const
{

    // to reduce onewire communication, we assume the last read value in update() is correct
    // only in update(), actual onewire communication will take place to get the latest state
    if (connected() && validChannel(channel)) {
        uint8_t mask = uint8_t{0x01} << (channel - 1);
        bool pinState = (pins & mask) > 0;
        result = pinState ? State::Inactive : State::Active;
        return true;
    }
    result = State::Unknown;
    return false;
}

bool
DS2408::writeChannelImpl(uint8_t channel, ChannelConfig config)
{
    bool latchEnabled = config == ChannelConfig::ACTIVE_HIGH;
    uint8_t mask = uint8_t{0x01} << (channel - 1);

    if (latchEnabled) {
        desiredLatches &= ~mask;
    } else {
        desiredLatches |= mask;
    }
    if (!connected()) {
        return false;
    }
    if (writeNeeded()) {
        // only directly update when connected, to prevent disconnected devices to continuously try to update
        // they will reconnect in the normal update tick, which should happen every second
        return update();
    }
    return true;
}
