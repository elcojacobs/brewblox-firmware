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
#include "system_event.h"
#include <cstdint>

void
printWiFiIp(char dest[16]);

void
printWifiSSID(char* dest, const uint8_t& maxLen);

int8_t
wifiSignal();

bool
serialConnected();

bool
setWifiCredentials(const char* ssid, const char* password, uint8_t security, uint8_t cipher);

void
handleNetworkEvent(system_event_t event, int param);

void
wifiInit();

bool
listeningModeEnabled();

void
manageConnections(uint32_t now);