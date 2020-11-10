/*
 * Copyright 2018 BrewPi B.V.
 *
 * This file is part of the BrewBlox Control Library.
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

#include "ActuatorDigitalConstrained.h"

namespace ADConstraints {
template <>
uint8_t typeId<MinOnTime>()
{
    return 1;
}

template <>
uint8_t typeId<MinOffTime>()
{
    return 2;
}

template <>
uint8_t typeId<DelayedOn>()
{
    return 3;
}

template <>
uint8_t typeId<DelayedOff>()
{
    return 4;
}

template <>
uint8_t typeId<Mutex>()
{
    return 5;
}

template <>
uint8_t typeId<MaxOnTime>()
{
    return 6;
}
}