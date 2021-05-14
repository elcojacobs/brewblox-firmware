/*
 * Copyright 2014-2020 BrewPi B.V. / Elco Jacobs
 * based on earlier work of Matthew McGowan.
 *
 * This file is part of BrewBlox.
 *
 * Controlbox is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Controlbox.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "DataStreamConverters.h"

#include "Crc.h"
#include "DataStream.h"
#include <cstdint>

namespace cbox {

#if 0 // currently not used
/**
 * Fetches the next significant data byte from the stream.
 * Sets hasData and data.
 * @param set to true if more data is optional, true if data is expected and should be waited for.
 */
void
TextIn::fetchNextData(bool optional)
{
    optional = !inLine;
    while (commentLevel >= 0 && !hasData && (_in->hasNext())) {
        if (_in->available()) {
            data = 0xFF;
            uint8_t d = _in->next();
            inLine = true;
            if (d == '<') {
                commentLevel++;
            } else if (d == '>') {
                commentLevel--;
            } else if (d == '\n' || d == '\r') {
                commentLevel = -1;
                data = 0; // exit the loop on end of line
                inLine = false;
            } else if (!commentLevel && isxdigit(char(d))) {
                hasData = true;
                data = d;
            }
        }
    }
}
#endif

} // end namespace cbox
