/*
Copyright (c) 2007, Jim Studt  (original old version - many contributors since)

The latest version of this library may be found at:
  http://www.pjrc.com/teensy/td_libs_OneWire.html

OneWire has been maintained by Paul Stoffregen (paul@pjrc.com) since
January 2010.  At the time, it was in need of many bug fixes, but had
been abandoned the original author (Jim Studt).  None of the known
contributors were interested in maintaining OneWire.  Paul typically
works on OneWire every 6 to 12 months.  Patches usually wait that
long.  If anyone is interested in more actively maintaining OneWire,
please contact Paul.

Brewblox version:
  Many changes to adapt the library to work with a low level driver and to simplify
  the search algorithm. Added the OneWireAddress class for managing addresses.
  Removed a lot of unused code.
  Cherry picked changes from the original library since 2.2.

Version 2.2:
  Teensy 3.0 compatibility, Paul Stoffregen, paul@pjrc.com
  Arduino Due compatibility, http://arduino.cc/forum/index.php?topic=141030
  Fix DS18B20 example negative temperature
  Fix DS18B20 example's low res modes, Ken Butcher
  Improve reset timing, Mark Tillotson
  Add const qualifiers, Bertrik Sikken
  Add initial value input to crc16, Bertrik Sikken
  Add target_search() function, Scott Roberts

Version 2.1:
  Arduino 1.0 compatibility, Paul Stoffregen
  Improve temperature example, Paul Stoffregen
  DS250x_PROM example, Guillermo Lovato
  PIC32 (chipKit) compatibility, Jason Dangel, dangel.jason AT gmail.com
  Improvements from Glenn Trewitt:
  - crc16() now works
  - check_crc16() does all of calculation/checking work.
  - Added read_bytes() and write_bytes(), to reduce tedious loops.
  - Added ds2408 example.
  Delete very old, out-of-date readme file (info is here)

Version 2.0: Modifications by Paul Stoffregen, January 2010:
http://www.pjrc.com/teensy/td_libs_OneWire.html
  Search fix from Robin James
    http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1238032295/27#27
  Use direct optimized I/O in all cases
  Disable interrupts during timing critical sections
    (this solves many random communication errors)
  Disable interrupts during read-modify-write I/O
  Reduce RAM consumption by eliminating unnecessary
    variables and trimming many to 8 bits
  Optimize both crc8 - table version moved to flash

Modified to work with larger numbers of devices - avoids loop.
Tested in Arduino 11 alpha with 12 sensors.
26 Sept 2008 -- Robin James
http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1238032295/27#27

Updated to work with arduino-0008 and to include skip() as of
2007/07/06. --RJL20

Modified to calculate the 8-bit CRC directly, avoiding the need for
the 256-byte lookup table to be loaded in RAM.  Tested in arduino-0010
-- Tom Pollard, Jan 23, 2008

Jim Studt's original library was modified by Josh Larios.

Tom Pollard, pollard@alum.mit.edu, contributed around May 20, 2008

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Much of the code was inspired by Derek Yerger's code, though I don't
think much of that remains.  In any event that was..
    (copyleft) 2006 by Derek Yerger - Free to distribute freely.

The CRC code was excerpted and inspired by the Dallas Semiconductor
sample code bearing this copyright.
//---------------------------------------------------------------------------
// Copyright (C) 2000 Dallas Semiconductor Corporation, All Rights Reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY,  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL DALLAS SEMICONDUCTOR BE LIABLE FOR ANY CLAIM, DAMAGES
// OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// Except as contained in this notice, the name of Dallas Semiconductor
// shall not be used except as stated in the Dallas Semiconductor
// Branding Policy.
//--------------------------------------------------------------------------
 */

#include "../inc/OneWire.h"
#include "../inc/OneWireAddress.h"
#include "../inc/OneWireCrc.h"

void
OneWire::write_bytes(const uint8_t* buf, uint16_t count)
{
    for (uint16_t i = 0; i < count; i++) {
        driver.write(buf[i]);
    }
}

void
OneWire::read_bytes(uint8_t* buf, uint16_t count)
{
    for (uint16_t i = 0; i < count; i++) {
        buf[i] = driver.read();
    }
}

//
// Do a ROM select
//

void
OneWire::select(const OneWireAddress& rom)
{
    driver.write(0x55); // Choose ROM

    for (uint8_t i = 0; i < 8; i++) {
        driver.write(rom[i]);
    }
}

//
// Do a ROM skip
//

void
OneWire::skip()
{
    driver.write(0xCC); // Skip ROM
}

void
OneWire::reset_search()
{
    // reset the search state
    ROM_NO = 0;
    lastDiscrepancy = 64;
    lastDeviceFlag = false;
    lockedSearchBits = 0;
}

void
OneWire::target_search(uint8_t family_code)
{
    // set the search state to find SearchFamily type devices
    ROM_NO = family_code;
    lastDiscrepancy = 64;
    lockedSearchBits = 8;
    lastDeviceFlag = false;
}

bool
OneWire::search(OneWireAddress& newAddr)
{
    uint8_t id_bit_nr = 0;
    bool search_result = false;
    bool search_direction = true;
    bool id_bit, cmp_id_bit;
    uint8_t last_zero = 64;

    // if the last call was not the last one
    if (!lastDeviceFlag) {
        if (!reset()) {
            return false;
        }

        // issue the search command
        // Conditional search/alarm search would send 0xEC here, but we don't use it
        write(0xF0); // NORMAL SEARCH.

        // loop to do the search
        do {
            // repeat search until last search had a discrepancy
            if (id_bit_nr < lastDiscrepancy || id_bit_nr < lockedSearchBits) {
                search_direction = ROM_NO.getBit(id_bit_nr);
            } else {
                // pick opposite of previous search
                search_direction = (id_bit_nr == lastDiscrepancy);
            }

            // Perform a triple operation on the driver which will perform 2 read bits and 1 write bit
            uint8_t status = driver.search_triplet(search_direction);
            // check bit results in status byte
            id_bit = (status & 0b00100000) > 0;
            cmp_id_bit = (status & 0b01000000) > 0;
            search_direction = (status & 0b10000000) > 0;

            if (id_bit && cmp_id_bit) {
                // no devices on bus
                break;
            } else {
                if ((!id_bit) && (!cmp_id_bit) && (!search_direction)) {
                    last_zero = id_bit_nr;
                }

                ROM_NO.setBit(id_bit_nr, search_direction);
                id_bit_nr++;
            }
        } while (id_bit_nr < 64);

        if (id_bit_nr >= 64 && ROM_NO.valid()) {
            lastDiscrepancy = last_zero;
            if (last_zero == 64 || last_zero < lockedSearchBits) {
                // no discrepancies or discrepancy is for device outside of target_search
                lastDeviceFlag = true;
            };
            search_result = true;
        }
    }

    if (search_result) {
        newAddr = ROM_NO;
    }

    return search_result;
}