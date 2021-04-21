/*
 * Copyright 2021 BrewPi B.V./Elco Jacobs.
 *
 * This file is part of Brewblox.
 * 
 * Brewblox is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Brewblox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Brewblox.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "DS2482.hpp"
#include "hal/hal_delay.h"

#define PTR_STATUS 0xf0
#define PTR_READ 0xe1
#define PTR_CONFIG 0xc3
#define PTR_PORTCONFIG 0xb4 //DS2484 only

bool DS248x::busyWait()
{

    if (!i2c_write({DS248X_SRP, PTR_STATUS})) {
        return false;
    }

    bool ready = false;
    for (uint8_t retries = 0; retries < 5; retries++) {
        auto result = i2c_read(1);
        if (result.size()) {
            mStatus = result[0];
            ready = (mStatus & DS248X_STATUS_BUSY) == 0;
            if (ready) {
                break;
            }
        }
        hal_delay_us(50);
    }
    if (!ready) {
        init(); // re-initialize driver, is this correct? TODO
    }
    return ready;
}

bool DS248x::init()
{
    hal_i2c_master_init();

    if (resetMaster()) {
        return configure(DS248X_CONFIG_APU);
    }
    return false;
}

bool DS248x::resetMaster()
{
    return i2c_write(DS248X_DRST);
}

bool DS248x::configure(uint8_t config)
{
    // when writing, upper bits should be inverse of lower bits
    // when reading, upper bits read as zero
    uint8_t config_byte = config + (~config << 4U);
    i2c_write({DS248X_WCFG, config_byte});
    auto result = i2c_read(1);
    if (result.size()) {
        return result[0] == config;
    }
    return false;
}

bool DS248x::selectChannel(uint8_t channel)
{
    uint8_t ch, ch_read;

    switch (channel) {
    case 0:
    default:
        ch = 0xf0;
        ch_read = 0xb8;
        break;
    case 1:
        ch = 0xe1;
        ch_read = 0xb1;
        break;
    case 2:
        ch = 0xd2;
        ch_read = 0xaa;
        break;
    case 3:
        ch = 0xc3;
        ch_read = 0xa3;
        break;
    case 4:
        ch = 0xb4;
        ch_read = 0x9c;
        break;
    case 5:
        ch = 0xa5;
        ch_read = 0x95;
        break;
    case 6:
        ch = 0x96;
        ch_read = 0x8e;
        break;
    case 7:
        ch = 0x87;
        ch_read = 0x87;
        break;
    };

    if (busyWait()) {
        if (i2c_write({DS248X_CHSL, ch})) {
            auto result = i2c_read(1);
            if (result.size()) {
                return ch_read == result[0];
            }
        }
    }

    return false;
}

bool DS248x::reset()
{
    bool ready = false;
    for (uint8_t retries = 0; retries < 10; retries++) {

        if (!i2c_write(DS248X_1WRS)) {
            // No ack received, onewire is busy
            hal_delay_ms(1);
            continue;
        }

        // read status until ready
        // todo: use repeated start condition ?
        while (!ready) {
            auto result = i2c_read(1);
            if (result.size() == 0) {
                // i2c error
                return false;
            }
            ready = (result[0] & DS248X_STATUS_BUSY) == 0;
        }
        return true;
    }
    return false;
}

bool DS248x::write(uint8_t b)
{
    if (busyWait()) {
        return i2c_write({{DS248X_1WWB, b}});
    }
    return false;
}

bool DS248x::read(uint8_t& b)
{
    if (busyWait()) {

        if (!i2c_write(DS248X_1WRB)) {
            return false;
        }

        for (uint8_t retries = 0; retries < 5; retries++) {
            auto result = i2c_read(1);
            if (result.size() == 0) {
                // i2c error
                return false;
            }
            bool ready = (mStatus & DS248X_STATUS_BUSY) == 0;
            if (ready) {
                if (i2c_write({DS248X_SRP, PTR_READ})) {
                    auto result = i2c_read(1);
                    if (result.size()) {
                        b = result[0];
                        return true;
                    }
                }
            }
            hal_delay_us(50);
        }
    }
    return false;
}

bool DS248x::write_bit(bool bit)
{
    if (busyWait()) {
        return i2c_write({DS248X_1WSB, bit ? uint8_t{0x80} : uint8_t{0}});
    }
    return false;
}

bool DS248x::read_bit(bool& bit)
{
    if (write_bit(1)) {
        bit = (mStatus & DS248X_STATUS_SBR) > 0;
        return true;
    }
    return false;
}

uint8_t
DS248x::search_triplet(bool search_direction)
{
    // 1-Wire Triplet (Case B)
    //   S AD,0 [A] 1WT [A] SS [A] Sr AD,1 [A] [Status] A [Status] A\ P
    //                                         \--------/
    //                           Repeat until 1WB bit has changed to 0
    //  [] indicates from slave
    //  SS indicates byte containing search direction bit value in msbit
    if (busyWait()) {
        if (i2c_write({DS248X_1WT, search_direction ? uint8_t{0x80} : uint8_t{0x00}})) {
        };
    }

    busyWait();
    return mStatus;
}