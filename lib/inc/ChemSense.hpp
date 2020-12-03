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

#pragma once

#include "ADS124S08.hpp"
#include <array>

// class to interface 2 chemical potential sensors and 2 RTDs
class ChemSense {
public:
    ChemSense(ADS124S08& _ads);

    ~ChemSense() = default;

    uint8_t update();

private:
    ADS124S08& ads;

    std::array<ADS124S08_detail::ChannelConfig, 4> configs;
    uint8_t current = 0;

public:
    std::array<int32_t, 4> results;
};