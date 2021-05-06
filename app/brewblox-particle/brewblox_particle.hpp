/*
 * Copyright 2018 BrewPi B.V.
 *
 * This file is part of BrewBlox.
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

#pragma once

#include "cbox/Box.h"

// forward declarations
namespace cbox {
class StringStreamConnectionSource;
}
class OneWire;

// create a static Box object on first use and return a reference to it
cbox::Box&
brewbloxBox();

// create a static OneWire object on first use and return a reference to it
OneWire&
theOneWire();

void updateBrewbloxBox();

const char*
versionCsv();

void logEvent(const std::string& event);

enum AppTrace : uint8_t {
    UPDATE_DISPLAY = 101,
    SYSTEM_TASKS = 102,
    MANAGE_CONNECTIVITY = 103,
    MDNS_START = 104,
    MDNS_PROCESS = 105,
    HTTP_START = 106,
    HTTP_STOP = 107,
    HTTP_RESPONSE = 108,
    WIFI_CONNECT = 109,
    FIRMWARE_UPDATE_STARTED = 110,
};
