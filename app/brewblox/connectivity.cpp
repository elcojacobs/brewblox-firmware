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

#include "connectivity.h"
#include "spark_wiring_usbserial.h"
#include "spark_wiring_wifi.h"
#include <cstdio>

void
printWiFiIp(char dest[16])
{
    IPAddress ip = spark::WiFi.localIP();
    snprintf(dest, 16, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
}

int8_t
wifiSignal()
{
    if (!spark::WiFi.ready()) {
        return 2;
    }
    auto rssi = wlan_connected_rssi();

    return rssi != std::numeric_limits<decltype(rssi)>::min() ? rssi / 100 : 2;

    return 2;
}

bool
serialConnected()
{
    return _fetch_usbserial().isConnected();
}

bool
setWifiCredentials(const char* ssid, const char* password, uint8_t security, uint8_t cipher)
{
    return spark::WiFi.setCredentials(ssid, password, security, cipher);
};

void
printWifiSSID(char* dest, const uint8_t& maxLen)
{
    if (spark::WiFi.ready()) {
        strncpy(dest, spark::WiFi.SSID(), maxLen);
    } else {
        dest[0] = 0;
    }
}