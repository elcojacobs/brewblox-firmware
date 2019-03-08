/*
 * Copyright 2014 BrewPi/Elco Jacobs.
 *
 * This file is part of BrewPi.
 * 
 * BrewPi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * BrewPi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with BrewPi.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "BrewPiTouch.h"
#include "spark_wiring.h"
#include <algorithm>
#include <limits.h>
#include <vector>

BrewPiTouch::BrewPiTouch(SPIArbiter& spia, const uint8_t cs, const uint8_t irq)
    : _spi(spia)
    , pinCS(cs)
    , pinIRQ(irq)
{
    config = BrewPiTouch::START;
    width = 1600;
    height = 1600;
    tftWidth = 320;
    tftHeight = 240;
    xOffset = 0;
    yOffset = 0;
    stabilityThreshold = 40;
}

BrewPiTouch::~BrewPiTouch()
{
}

void
BrewPiTouch::init()
{
    // default is:
    // 12bit (mode=0)
    // power down between conversions, penIRQ enabled (PD1,PD0 = 00)
    // Differential reference (SER/DFR = 0)
    pinMode(pinCS, OUTPUT);
    pinMode(pinIRQ, INPUT_PULLUP);
    // clock base is 120/2 = 60Mhz on the Photon and 72 Mhz on the core.
    // Minimum clock cycle is 200 + 200 = 400 ns
    // Fmax = 2.5 Mhz. 60 Mhz / 2.5 MHz = 24. Clock div of 32 is within margins
    _spi.setClockDivider(SPI_CLOCK_DIV32);
    _spi.setDataMode(SPI_MODE0);
    _spi.begin(pinCS);
    _spi.transfer(config);
    _spi.end();

    filterX.init(width / 2);
    filterX.setCoefficients(SETTLING_TIME_25_SAMPLES);
    filterY.init(height / 2);
    filterY.setCoefficients(SETTLING_TIME_25_SAMPLES);
    update();
}

void
BrewPiTouch::set8bit()
{
    config = config | MODE;
}

void
BrewPiTouch::set12bit()
{
    config = config & ~MODE;
}

bool
BrewPiTouch::is8bit() const
{
    return (config & MODE) ? 1 : 0;
}

bool
BrewPiTouch::is12bit() const
{
    return (config & MODE) ? 0 : 1;
}

uint16_t
BrewPiTouch::readChannel(uint8_t channel, bool singleEnded) const
{
    _spi.begin(); // will drive CS pin low, needed for conversion timing
    _spi.transfer((config & CHMASK)
                  | channel                                                            // select channel
                  | (singleEnded ? controlBits::SER | controlBits::PD1 : uint8_t(0))); // enable internal reference and apply single ended conversion
    delayMicroseconds(2);                                                              // make sure aquisition is complete, without checking busy pin
    uint16_t data = _spi.transfer(0);
    data = data << 8;
    data += _spi.transfer(0);
    uint8_t shift = is12bit() ? 4 : 8;
    if (singleEnded) {
        shift = shift - 1; // single ended conversions are delayed by 1 cycle
    }
    data = data >> shift;
    _spi.end(); // will drive CS pin high again
    return data;
}

uint16_t
BrewPiTouch::read5V() const
{
    return readChannel(CHBAT, true);
}

uint16_t
BrewPiTouch::read12V() const
{
    return readChannel(CHAUX, true);
}

bool
BrewPiTouch::isTouched() const
{
    return (digitalRead(pinIRQ) == HIGH ? 0 : 1);
}

int16_t
BrewPiTouch::getXRaw() const
{
    return filterX.readInput();
}

int16_t
BrewPiTouch::getYRaw() const
{
    return filterY.readInput();
}

int16_t
BrewPiTouch::getX() const
{
    int32_t val = getXRaw();      // create room for multiplication
    val -= xOffset;               // remove offset
    val = val * tftWidth / width; //scale
    return val;
}

int16_t
BrewPiTouch::getY() const
{
    int32_t val = getYRaw();        // create room for multiplication
    val -= yOffset;                 // remove offset
    val = val * tftHeight / height; //scale
    return val;
}

/*
 *  update() updates the x and y coordinates of the touch screen
 *  It reads numSamples values and takes the median
 *  The result is fed to the low pass filters
 */
bool
BrewPiTouch::update(uint16_t numSamples)
{
    std::vector<int16_t> samplesX;
    std::vector<int16_t> samplesY;

    bool valid = true;

    if (!isTouched()) {
        return false; // exit immediately when not touched to prevent claiming SPI
    }
    for (uint16_t i = 0; i < numSamples; i++) {
        if (!isTouched()) {
            valid = false;
            break;
        }
        samplesX.push_back(readChannel(CHX, false));
        samplesY.push_back(readChannel(CHY, false));
    }
    if (valid) {
        // get median
        size_t middle = samplesX.size() / 2;
        std::nth_element(samplesX.begin(), samplesX.begin() + middle, samplesX.end());
        std::nth_element(samplesY.begin(), samplesY.begin() + middle, samplesY.end());
        // feed to filter to check stability
        filterX.add(samplesX[middle]);
        filterY.add(samplesY[middle]);
    }
    return valid && isStable();
}

void
BrewPiTouch::setStabilityThreshold(int16_t threshold)
{
    stabilityThreshold = threshold;
}

/* isStable() returns true if the difference between the last sample and 
 * a low pass filtered value of past samples is under a certain threshold
 */
bool
BrewPiTouch::isStable() const
{
    if (abs(filterX.readInput() - filterX.readOutput()) > stabilityThreshold) {
        return false;
    }
    if (abs(filterY.readInput() - filterY.readOutput()) > stabilityThreshold) {
        return false;
    }
    return true;
}
