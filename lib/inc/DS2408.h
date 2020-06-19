/*
 * Copyright 2013 Matthew McGowan 
 * Copyright 2013 BrewPi/Elco Jacobs.
 *
 * This file is part of BrewBlox.
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

#include <cstdint>

#include "IoArray.h"
#include "OneWireDevice.h"

#define DS2408_FAMILY_ID 0x29

/**
 * Provides access to a OneWire-addressable 8-channel I/O device.
 * Each output has a pull-down transistor, which can be enabled by writing 0 to the pio output latch.
 * This pulls the output pin to GND.
 *
 * When the output latch is disabled, the pio can be read as digital input (sense).
 * This is the power on-default if a reset signal is pulled low. Without reset, the state is random.
 */
class DS2408 : public OneWireDevice, public IoArray {

private:
    uint8_t desiredLatches = 0xFF;
    bool dirty = true;
    uint8_t pins = 0xFF;             // 0 = pio logic state, 0x0088 on chip
    uint8_t latches = 0xFF;          // 1 = output latch state
    uint8_t activity = 0xFF;         // 2 = activity latch state
    uint8_t cond_search_mask = 0x00; // 3 = conditional search channel selection mask
    uint8_t cond_search_pol = 0x00;  // 4 = conditional search channel polarity selection
    uint8_t status = 0x08;           // 5 = control/status register

public:
    /**
     * Constructor initializes both caches to 0xFF.
     * This means the output latches are disabled and all pins are sensed high
     */
    DS2408(OneWire& oneWire, OneWireAddress address = 0)
        : OneWireDevice(oneWire, address)
        , IoArray(8)
    {
    }

    /**
     * Destructor is default.
     */
    virtual ~DS2408() = default;

    bool update();
    bool writeNeeded() const;

    // generic ArrayIo interface
    virtual bool senseChannelImpl(uint8_t channel, State& result) const override final;

    virtual bool writeChannelImpl(uint8_t channel, ChannelConfig config) override final;

    virtual bool supportsFastIo() const override final
    {
        return false;
    }
};
