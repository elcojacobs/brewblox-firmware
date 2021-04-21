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

#pragma once

#include "I2CDevice.hpp"
#include "OneWireLowLevelInterface.h"
#include <cstdint>

constexpr uint8_t DS248X_CONFIG_APU = (0x1 << 0);
constexpr uint8_t DS248X_CONFIG_PPM = (0x1 << 1);
constexpr uint8_t DS248X_CONFIG_SPU = (0x1 << 2);
constexpr uint8_t DS2484_CONFIG_WS = (0x1 << 3);

constexpr uint8_t DS248X_STATUS_BUSY = (0x1 << 0);
constexpr uint8_t DS248X_STATUS_PPD = (0x1 << 1);
constexpr uint8_t DS248X_STATUS_SD = (0x1 << 2);
constexpr uint8_t DS248X_STATUS_LL = (0x1 << 3);
constexpr uint8_t DS248X_STATUS_RST = (0x1 << 4);
constexpr uint8_t DS248X_STATUS_SBR = (0x1 << 5);
constexpr uint8_t DS248X_STATUS_TSB = (0x1 << 6);
constexpr uint8_t DS248X_STATUS_DIR = (0x1 << 7);

// I2C commands
constexpr uint8_t DS248X_DRST = 0xf0; // Device Reset
constexpr uint8_t DS248X_WCFG = 0xd2; // Write Configuration
constexpr uint8_t DS248X_CHSL = 0xc3; // Channel Select (DS248X-800 only)
constexpr uint8_t DS248X_SRP = 0xe1;  // Set Read Pointer
constexpr uint8_t DS248X_1WRS = 0xb4; // 1-Wire Reset
constexpr uint8_t DS248X_1WWB = 0xa5; // 1-Wire Write Byte
constexpr uint8_t DS248X_1WRB = 0x96; // 1-Wire Read Byte
constexpr uint8_t DS248X_1WSB = 0x87; // 1-Wire Single Bit
constexpr uint8_t DS248X_1WT = 0x78;  // 1-Wire Triplet
constexpr uint8_t DS248X_ADJP = 0xc3; // Adjust OneWire port config (DS2484 only))

class DS248x : public OneWireLowLevelInterface, public I2CDeviceBase<0x18> {
public:
    //Address is 0-3
    DS248x(uint8_t address)
        : I2CDeviceBase(address)
    {
    }

    DS248x(const DS248x&) = delete;
    DS248x(DS248x&&) = default;
    DS248x& operator=(const DS248x&) = delete;

    virtual ~DS248x() = default;

    virtual bool init() override final;

    bool configure(uint8_t config);

    // Perform the onewire reset function.  We will wait up to 250uS for
    // the bus to come high, if it doesn't then it is broken or shorted
    // and we return a 0;
    //
    // Returns 1 if a device asserted a presence pulse, 0 otherwise.
    virtual bool reset() override final;

    virtual bool write(uint8_t b) override final;
    virtual bool read(uint8_t& b) override final;

    virtual bool write_bit(bool bit) override final;
    virtual bool read_bit(bool& bit) override final;

    // DS248X specific functions below

    bool resetMaster();

    //DS2482-800 only
    bool selectChannel(uint8_t channel);

    //--------------------------------------------------------------------------
    // Use the DS248X help command '1-Wire triplet' to perform one bit of a
    // 1-Wire search.
    // This command does two read bits and one write bit. The write bit
    // is either the default direction (all device have same bit) or in case of
    // a discrepancy, the 'search_direction' parameter is used.
    //
    // Returns – The DS248X status byte result from the triplet command
    virtual uint8_t search_triplet(bool search_direction) override final;

private:
    uint8_t mStatus = 0;

    bool busyWait();         // blocks until ready or timeout, updates status
    uint8_t failedWaits = 0; // keep track of consecutive failed waits
};
