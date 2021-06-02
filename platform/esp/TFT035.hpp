/*
 * Copyright 2020 BrewPi B.V./Elco Jacobs.
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
#include "esp32/rom/ets_sys.h"
#include "hal/hal_gpio.h"
#include "hal/hal_spi.h"
#include "hal/hal_spi_types.h"
#include <functional>

/**
 * A driver for the TFT035 display controller. 
 */
class TFT035 {
public:
    TFT035(std::function<void()> finishCallback);
    ~TFT035() = default;

    void init();

    void aquire_spi();
    void release_spi();
    bool writePixels(unsigned int xs, unsigned int xe, unsigned int ys, unsigned int ye, uint8_t* pixels, uint16_t nPixels);

    hal_spi_err_t setPos(unsigned int xs, unsigned int xe, unsigned int ys, unsigned int ye);

    hal_spi_err_t dmaWrite(uint8_t* tx_data, uint16_t tx_len, bool dc);
    hal_spi_err_t dmaWrite(uint8_t tx_data, bool dc);

    hal_spi_err_t writeCmd(const std::vector<uint8_t>& cmd);
    hal_spi_err_t write(const std::vector<uint8_t>& cmd);
    hal_spi_err_t writeCmd(uint8_t cmd);
    hal_spi_err_t write(uint8_t cmd);

    /// A list of the commands of the TFT035
    enum command : uint8_t {
        PGAMCTRL = 0xE0,
        NGAMCTRL = 0xE1,
        PWCTRL1 = 0XC0,
        PWCTRL2 = 0xC1,
        PWCTRL3 = 0xC5,
        MADCTL = 0x36,
        COLMOD = 0x3A,
        IFMODE = 0XB0,
        FRMCTR1 = 0xB1,
        INVTR = 0xB4,
        DISCTRL = 0XB6,
        SETIMAGE = 0XE9,
        ADJCTRL3 = 0xF7,
        SLPOUT = 0x11,
        DISON = 0x29,
        ALLPOFF = 0x22,
        ALLPON = 0x23
    };

private:
    SpiDevice spiDevice;
    std::function<void()> finishCallback;
    const hal_pin_t dc;
};
