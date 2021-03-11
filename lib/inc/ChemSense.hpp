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
#include "FixedPoint.h"
#include <array>
#include <asio.hpp>

// class to interface 2 chemical potential sensors and 2 RTDs
class ChemSense {
public:
    typedef std::function<void(const std::array<int32_t, 4>&)> results_handler_t;
    ChemSense(ADS124S08& _ads, asio::io_context& io, results_handler_t onResults);

    ~ChemSense() = default;

    uint8_t update();

    static inline std::array<fp12_t, 4> convertToMilliVolt(const std::array<int32_t, 4>& results)
    {
        return {
            cnl::wrap<safe_elastic_fixed_point<10, 22>>(results[0]) * 625,
            cnl::wrap<safe_elastic_fixed_point<10, 22>>(results[1]) * 625,
            cnl::wrap<safe_elastic_fixed_point<6, 26>>(results[2]) * 1875,
            cnl::wrap<safe_elastic_fixed_point<6, 26>>(results[3]) * 1875,
        };
    }

private:
    ADS124S08& ads;

    std::array<ADS124S08_detail::ChannelConfig, 4> configs;
    uint8_t current = 0;

    asio::steady_timer timer;
    std::array<int32_t, 4> results;

    void onTimeout();
    results_handler_t resultsHandler;
};

// calibration dec 7 2020 (19.5 deg C):
// PH 4.0: 166.6 mV
// PH 7.0: -5.6 mV
// PH 10.0: -171.5 mV
// RTD1: 107.632
// RTD2: 107.585
