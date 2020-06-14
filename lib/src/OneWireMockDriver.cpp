/*
 * Copyright 2020 BrewPi B.V.
 *
 * This file is part of the BrewBlox Control Library.
 *
 * BrewBlox is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BrewBlox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with BrewBlox.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "OneWireMockDriver.h"

void
OneWireMockDriver::communicate()
{
    // device reading most bytes determines how many are dropped from the buffer
    while (!masterToSlave.empty()) {
        uint8_t bytes_read = 0;
        for (auto& device : devices) {
            bytes_read = std::max(bytes_read, device->respond(*this));
        }
        while (bytes_read-- > 0) {
            masterToSlave.pop_front();
        }
    }
}

uint8_t
OneWireMockDriver::countActiveDevices()
{
    uint8_t count = 0;
    for (auto& device : devices) {
        if (device->present()) {
            count++;
        }
    }
    return count;
}

uint8_t
OneWireMockDriver::search_triplet(uint8_t* search_direction, uint8_t* id_bit, uint8_t* cmp_id_bit)
{
    bool new_id_bit = true;
    bool new_cmp_id_bit = true;
    for (auto& device : devices) {
        (*device).search_triplet_read(&new_id_bit, &new_cmp_id_bit);
    }
    if (!new_id_bit && !new_cmp_id_bit) {
        // both 1's and 0's on the bus, search direction is not changed
    } else if (!new_id_bit && new_cmp_id_bit) {
        *search_direction = 1;
    } else {
        *search_direction = 0;
    }
    *id_bit = new_id_bit;
    *cmp_id_bit = new_cmp_id_bit;
    for (auto& device : devices) {
        (*device).search_triplet_write(*search_direction != 0);
    }

    return 0;
};
