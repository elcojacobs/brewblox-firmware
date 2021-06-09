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
#include "hal/hal_spi.hpp"
#include "hal/hal_spi_types.h"
#include <functional>

/**
 * A driver for the TFT035 display controller. 
 */
class TFT035 {
public:
    /** 
    * Constructs the driver.
    * 
    * @param finishCallback The callback to be called when the pixels are transfered to the screen.
    */
    TFT035(std::function<void()> finishCallback);
    ~TFT035() = default;

    /// Initialises the display driver.
    void init();

    /// Aquire the spi bus for writing to the display.
    void aquire_spi();

    /// Releases the spi bus after writing to the display.
    void release_spi();

    /**
     * Writing a n of pixels to the screen in a defined area.
     * 
     * @param xs The start postition of the x of the writing area.
     * @param xe The end postition of the x of the writing area.
     * @param ys The start postition of the y of the writing area.
     * @param ye The end postition of the y of the writing area.
     * @param pixels A pointer to the pixels to be written. The caller is responsible for dealocating this memory.
     * @param nPixels The number of pixels to be written.
     * @return Returns true if no error has occured, false if the write has failed.
     */
    bool writePixels(unsigned int xs, unsigned int xe, unsigned int ys, unsigned int ye, uint8_t* pixels, uint16_t nPixels);

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

    spi::error setPos(unsigned int xs, unsigned int xe, unsigned int ys, unsigned int ye);

    spi::error dmaWrite(uint8_t* tx_data, uint16_t tx_len, bool dc);
    spi::error dmaWrite(uint8_t tx_data, bool dc);

    spi::error writeCmd(const std::vector<uint8_t>& cmd);
    spi::error write(const std::vector<uint8_t>& cmd);
    spi::error writeCmd(uint8_t cmd);
    spi::error write(uint8_t cmd);
};
