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

#if 0

#include "../inc/ChemSense.hpp"

using namespace ADS124S08_detail;

ChemSense::ChemSense(ADS124S08& _ads, asio::io_context& io, results_handler_t onResults)
    : ads{_ads}
    , configs{{{RegInpMux::ADS_P_AIN0 + RegInpMux::ADS_N_AINCOM,
                RegPga::ADS_DELAY_14 | RegPga::ADS_PGA_ENABLED | RegPga::ADS_GAIN_2,
                RegDataRate::ADS_FILTERTYPE_LL | RegDataRate::ADS_CONVMODE_SS | RegDataRate::ADS_DR_5,
                RegRef::ADS_REFSEL_INT | RegRef::ADS_REFINT_ON_ALWAYS,
                RegIdacMag::DEFAULT_VAL,
                RegIdacMux::DEFAULT_VAL,
                RegVbias::DEFAULT_VAL},
               {RegInpMux::ADS_P_AIN1 + RegInpMux::ADS_N_AINCOM,
                RegPga::ADS_DELAY_14 | RegPga::ADS_PGA_ENABLED | RegPga::ADS_GAIN_2,
                RegDataRate::ADS_FILTERTYPE_LL | RegDataRate::ADS_CONVMODE_SS | RegDataRate::ADS_DR_5,
                RegRef::ADS_REFSEL_INT | RegRef::ADS_REFINT_ON_ALWAYS,
                RegIdacMag::DEFAULT_VAL,
                RegIdacMux::DEFAULT_VAL,
                RegVbias::DEFAULT_VAL},
               {RegInpMux::ADS_P_AIN9 + RegInpMux::ADS_N_AIN8,
                RegPga::ADS_DELAY_14 | RegPga::ADS_PGA_ENABLED | RegPga::ADS_GAIN_64,
                RegDataRate::ADS_FILTERTYPE_LL | RegDataRate::ADS_CONVMODE_SS | RegDataRate::ADS_DR_5,
                RegRef::ADS_REFSEL_P0 | RegRef::ADS_REFINT_ON_ALWAYS,
                RegIdacMag::ADS_IDACMAG_100,
                RegIdacMux::ADS_IDAC2_A10 | RegIdacMux::ADS_IDAC2_A11,
                RegVbias::DEFAULT_VAL},
               {RegInpMux::ADS_P_AIN3 + RegInpMux::ADS_N_AIN2,
                RegPga::ADS_DELAY_14 | RegPga::ADS_PGA_ENABLED | RegPga::ADS_GAIN_64,
                RegDataRate::ADS_FILTERTYPE_LL | RegDataRate::ADS_CONVMODE_SS | RegDataRate::ADS_DR_5,
                RegRef::ADS_REFSEL_P1 | RegRef::ADS_REFINT_ON_ALWAYS,
                RegIdacMag::ADS_IDACMAG_100,
                RegIdacMux::ADS_IDAC2_A4 | RegIdacMux::ADS_IDAC2_A5,
                RegVbias::DEFAULT_VAL}}}
    , timer(io, asio::chrono::milliseconds(250))
    , resultsHandler(onResults)
{
    timer.async_wait(std::bind(&ChemSense::onTimeout, this));
}

uint8_t ChemSense::update()
{

    results[current] = ads.readLastData();
    current = (current + 1) % 4;
    auto& config = configs[current];
    ads.writeRegs(config.inpMux, config.vbias);
    ads.pulse_start();
    return current;
}

void ChemSense::onTimeout()
{
    if (update() == 0) {
        resultsHandler(results);
    }
    timer.expires_at(timer.expiry() + asio::chrono::milliseconds(250));
    timer.async_wait(std::bind(&ChemSense::onTimeout, this));
}

#endif