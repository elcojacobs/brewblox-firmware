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
}

uint8_t
OneWireMockDriver::search_triplet(bool search_direction)
{
    bool id_bit = true;
    bool cmp_id_bit = true;
    for (auto& device : devices) {
        device->search_triplet_read(&id_bit, &cmp_id_bit);
    }

    if (id_bit != cmp_id_bit) {
        search_direction = !id_bit;
    }
    if (id_bit && cmp_id_bit) {
        search_direction = true; // error condition
    }

    for (auto& device : devices) {
        device->search_triplet_write(search_direction);
    }
    uint8_t status = 0x00;
    if (id_bit) {
        status |= 0b00100000;
    }
    if (cmp_id_bit) {
        status |= 0b01000000;
    }
    if (search_direction) {
        status |= 0b10000000;
    }
    return status;
}