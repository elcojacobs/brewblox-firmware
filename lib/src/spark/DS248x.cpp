/*
  DS2482/DS2484 library for Arduino
  Copyright (C) 2009 Paeae Technologies

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

        crc code is from OneWire library


Updates:
        dec 5th, 2009: included a search function fix by Don T
        see http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1242137666

        2015: BrewPi: minor changes to field names to indicate DS2484 support.
              Dont call init() in ctor.
 */

#include "../inc/DS248x.h"
#include "spark_wiring.h"
#include "spark_wiring_i2c.h"

#define PTR_STATUS 0xf0
#define PTR_READ 0xe1
#define PTR_CONFIG 0xc3
#define PTR_PORTCONFIG 0xb4 //DS2484 only

bool
DS248x::busyWait()
{
    Wire.beginTransmission(mAddress);
    Wire.write(DS248X_SRP);
    Wire.write(PTR_STATUS);
    if (Wire.endTransmission() != 0) {
        return false;
    }

    for (uint8_t retries = 0; retries < 5; retries++) {
        if (Wire.requestFrom(mAddress, size_t{1})) {
            mStatus = Wire.read();
            if ((mStatus & DS248X_STATUS_BUSY) == 0) {
                return true;
            }
            delayMicroseconds(50);
        }
    }
    init();
    return false;
}

bool
DS248x::init()
{
    Wire.reset();
    Wire.setTimeout(1);
    Wire.begin();

    resetMaster();
    return configure(DS248X_CONFIG_APU);
}

void
DS248x::resetMaster()
{
    Wire.beginTransmission(mAddress);
    Wire.write(DS248X_DRST);
    Wire.endTransmission();
}

bool
DS248x::configure(uint8_t config)
{
    busyWait(); // continue even if busy
    Wire.beginTransmission(mAddress);
    Wire.write(DS248X_WCFG);
    Wire.write(config | (~config) << 4);
    Wire.endTransmission();

    if (Wire.requestFrom(mAddress, size_t{1})) {
        return config == Wire.read();
    }

    return false;
}

bool
DS248x::selectChannel(uint8_t channel)
{
    uint8_t ch, ch_read;

    switch (channel) {
    case 0:
    default:
        ch = 0xf0;
        ch_read = 0xb8;
        break;
    case 1:
        ch = 0xe1;
        ch_read = 0xb1;
        break;
    case 2:
        ch = 0xd2;
        ch_read = 0xaa;
        break;
    case 3:
        ch = 0xc3;
        ch_read = 0xa3;
        break;
    case 4:
        ch = 0xb4;
        ch_read = 0x9c;
        break;
    case 5:
        ch = 0xa5;
        ch_read = 0x95;
        break;
    case 6:
        ch = 0x96;
        ch_read = 0x8e;
        break;
    case 7:
        ch = 0x87;
        ch_read = 0x87;
        break;
    };

    if (busyWait()) {

        Wire.beginTransmission(mAddress);
        Wire.write(DS248X_CHSL);
        Wire.write(ch);
        if (Wire.endTransmission() == 0) {
            if (Wire.requestFrom(mAddress, size_t{1})) {
                return ch_read == Wire.read();
            }
        }
    }

    return false;
}

bool
DS248x::reset()
{
    if (busyWait()) {
        Wire.beginTransmission(mAddress);
        Wire.write(DS248X_1WRS);
        if (!Wire.endTransmission()) {
            if (busyWait()) {
                return (mStatus & DS248X_STATUS_PPD) > 0;
            }
        }
    }
    return false;
}

bool
DS248x::write(uint8_t b)
{
    if (busyWait()) {
        Wire.beginTransmission(mAddress);
        Wire.write(DS248X_1WWB);
        Wire.write(b);
        return Wire.endTransmission() == 0;
    }
    return false;
}

bool
DS248x::read(uint8_t& b)
{
    if (busyWait()) {
        Wire.beginTransmission(mAddress);
        Wire.write(DS248X_1WRB);
        if (Wire.endTransmission() != 0) {
            return false;
        }
        if (busyWait()) {
            Wire.beginTransmission(mAddress);
            Wire.write(DS248X_SRP);
            Wire.write(PTR_READ);
            if (Wire.endTransmission() != 0) {
                return false;
            }
            if (Wire.requestFrom(mAddress, size_t{1})) {

                b = Wire.read();
                return true;
            }
        }
    }
    return false;
}

bool
DS248x::write_bit(bool bit)
{
    if (busyWait()) {
        Wire.beginTransmission(mAddress);
        Wire.write(DS248X_1WSB);
        Wire.write(bit ? 0x80 : 0);
        return Wire.endTransmission() == 0;
    }
    return false;
}

bool
DS248x::read_bit(bool& bit)
{
    if (write_bit(1)) {
        bit = (mStatus & DS248X_STATUS_SBR) > 0;
        return true;
    }
    return false;
}

uint8_t
DS248x::search_triplet(bool search_direction)
{
    // 1-Wire Triplet (Case B)
    //   S AD,0 [A] 1WT [A] SS [A] Sr AD,1 [A] [Status] A [Status] A\ P
    //                                         \--------/
    //                           Repeat until 1WB bit has changed to 0
    //  [] indicates from slave
    //  SS indicates byte containing search direction bit value in msbit
    if (busyWait()) {
        Wire.beginTransmission(mAddress);
        Wire.write(DS248X_1WT);
        Wire.write(search_direction ? 0x80 : 0x00);
        Wire.endTransmission();
    }

    busyWait();
    return mStatus;
}
