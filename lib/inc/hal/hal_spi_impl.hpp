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

#include "hal_spi_types.h"

namespace platform_spi {
using namespace spi;
/**
 * Initialises the spi device.
 * 
 * @param settings The settings struct containing the configuration of the spi device.
 * @return If any error will occur an non zero result will indicate an error has happened.
 */
error_t init(Settings& settings);

/**
 * Deinitialises the spi device.
 * 
 * @param settings The settings struct containing the configuration of the spi device.
 * @return If any error will occur an non zero result will indicate an error has happened.
 */
void deInit(Settings& settings);

/**
 * Writes a n amount of bytes to the spi device
 * 
 * The user will be responsible for deallocating the data pointer.
 * 
 * @param settings The settings struct containing the configuration of the spi device.
 * @param data A pointer to the bytes to be send.
 * @param size The amount of bytes to be send.
 * @return If any error will occur a non zero result will indicate an error has happened.
 */
error_t write(Settings& settings, const uint8_t* data, size_t size);

/**
 * Writes a n amount of bytes to the spi device
 * 
 * The user will be responsible for deallocating the data pointer.
 * 
 * @param settings The settings struct containing the configuration of the spi device.
 * @param data A pointer to the bytes to be send.
 * @param size The amount of bytes to be send.
 * @param callbacks The callbacks to be called before and after the transaction. 
 * @return If any error will occur a non zero result will indicate an error has happened.
 */
error_t dmaWrite(Settings& settings, const uint8_t* data, size_t size, const CallbacksBase* callbacks);
/**
 * Writes a n amount of bytes to the spi device and reads the same amount of bytes.
 * 
 * The user will be responsible for deallocating the data pointers.
 * 
 * @param settings The settings struct containing the configuration of the spi device.
 * @param tx A pointer to the bytes to be send.
 * @param txSize The amount of bytes to be send.
 * @param rx A pointer to free space in memory where the received bytes will be written to.
 * @param rxSize The amount of bytes to be received.
 * @param pre A functionpointer to a function which will be called right before the transfer will take place. 
 * @param post A functionpointer to a function which will be called right after the transfer will take place. This can be used for example for deallocation purpuses.
 * @return If any error will occur a non zero result will indicate an error has happened.
 */
error_t writeAndRead(Settings& settings, const uint8_t* tx, size_t txSize, uint8_t* rx, size_t rxSize);

void aquire_bus(Settings& settings);
void release_bus(Settings& settings);
}
