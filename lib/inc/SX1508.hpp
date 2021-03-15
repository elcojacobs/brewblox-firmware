/*
 * Copyright 2021 BrewPi B.V./Elco Jacobs.
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
 * along with BrewBlox.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "I2CDevice.hpp"
#include <stdint.h>

class SX1508 : public I2CDeviceBase<0x20> {
public:
    SX1508(uint8_t lower_address)
        : I2CDeviceBase(lower_address)
    {
        reset();
    };
    ~SX1508() = default;

    enum class RegAddr : uint8_t {
        // Register I/O Bank
        inputDisable = 0x00,
        longSlew = 0x01,
        lowDrive = 0x02,
        pullUp = 0x03,
        pullDown = 0x04,
        openDrain = 0x05,
        polarity = 0x06,
        dir = 0x07,
        data = 0x08,
        interruptMask = 0x09,
        senseHigh = 0x0A,
        senseLow = 0x0B,
        interruptSource = 0x0C,
        eventStatus = 0x0D,
        levelShifter = 0x0E,
        clock = 0x0F,
        misc = 0x10,
        ledDriverEnable = 0x11,

        // Debounce and Keypad register
        debounceConfig = 0x12,
        debounceEnable = 0x13,
        keyConfig = 0x14,
        keyData = 0x15,

        // LED Driver (PWM, blinking, breathing)
        iOn0 = 0x16,
        iOn1 = 0x17,
        tOn2 = 0x18,
        iOn2 = 0x19,
        off2 = 0x1A,
        tOn3 = 0x1B,
        iOn3 = 0x1C,
        off3 = 0x1D,
        tRise3 = 0x1E,
        tFall3 = 0x1F,
        iOn4 = 0x20,
        iOn5 = 0x21,
        tOn6 = 0x22,
        iOn6 = 0x23,
        off6 = 0x24,
        tOn7 = 0x25,
        iOn7 = 0x26,
        off7 = 0x27,
        tRise7 = 0x28,
        tFall7 = 0x29,

        // Miscellaneous
        highInput = 0x2A,

        // Software Reset
        reset = 0x7D,

        // Test (not to be written)
        test1 = 0x7E,
        test2 = 0x7F,
    };

    void reset();
    hal_i2c_err_t write_reg(RegAddr addr, uint8_t data);
    // void write_regs(RegAddr addr, const uint8_t* data, size_t len);
    hal_i2c_err_t read_reg(RegAddr addr, uint8_t& result);
};
