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
#include "Board.h"
#include "BrewBlox.h"
#include "MDNS.h"
#include "cbox/Tracing.h"
#include "deviceid_hal.h"
#include "reset.h"
#include "spark_wiring_tcpclient.h"
#include "spark_wiring_tcpserver.h"
#include "spark_wiring_usbserial.h"
#include "spark_wiring_wifi.h"
#include <cstdio>

uint32_t localIp = 0;
bool mdns_started = false;
bool http_started = false;

constexpr uint16_t webPort = PLATFORM_ID == PLATFORM_GCC ? 8380 : 80;
static TCPServer httpserver(webPort); // Serve a simple page with instructions

void
printWiFiIp(char dest[16])
{
    IPAddress ip = localIp;
    snprintf(dest, 16, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
}

int8_t wifiSignalRssi = 2;

// only update signal strength periodically to reduce number of calls into wifi driver
void
updateWifiSignal()
{
    if (spark::WiFi.ready()) {
        auto rssi = wlan_connected_rssi();
        if (rssi == 0) {
            // means caller should retry, wait until next update for retry
            return;
        }
        IPAddress ip = spark::WiFi.localIP();
        localIp = ip.raw().ipv4;
        wifiSignalRssi = rssi;
        return;
    }
    localIp = 0;
    wifiSignalRssi = 2;
}

int8_t
wifiSignal()
{
    return wifiSignalRssi;
}

bool
wifiConnected()
{
    // WiFi.ready() ensures underlying wifi driver has been initialized
    // wifiSignalRssi is set above an ensures an IP address is assigned and we have signal
    // checking ready() too ensures that a disconnected is detected immediately
    return wifiSignalRssi < 0 && spark::WiFi.ready();
}

bool
serialConnected()
{
    return HAL_USB_USART_Is_Connected(HAL_USB_USART_SERIAL);
}

bool
setWifiCredentials(const char* ssid, const char* password, uint8_t security, uint8_t cipher)
{
    return spark::WiFi.setCredentials(ssid, password, security, cipher);
};

void
printWifiSSID(char* dest, const uint8_t& maxLen)
{
    if (wifiConnected()) {
        strncpy(dest, spark::WiFi.SSID(), maxLen);
    } else {
        dest[0] = 0;
    }
}

bool
listeningModeEnabled()
{
    return spark::WiFi.listening();
}

inline uint8_t
d2h(uint8_t bin)
{
    return uint8_t(bin + (bin > 9 ? 'A' - 10 : '0'));
}

std::string
deviceIdStringInit()
{
    std::string hex;
    hex.reserve(25);
    uint8_t id[12];
    HAL_device_ID(id, 12);
    for (uint8_t i = 0; i < 12; i++) {
        hex.push_back(d2h(uint8_t(id[i] & 0xF0) >> 4));
        hex.push_back(d2h(uint8_t(id[i] & 0xF)));
    }
    return hex;
}

const std::string&
deviceIdString()
{
    static auto hexId = deviceIdStringInit();
    return hexId;
}

MDNS&
theMdns()
{
    static MDNS* theStaticMDNS = new MDNS(deviceIdString());
    return *theStaticMDNS;
}

void
manageConnections(uint32_t now)
{
    static uint32_t lastConnected = 0;
    static uint32_t lastChecked = 0;
    static uint32_t lastAnnounce = 0;
    cbox::tracing::add(AppTrace::MANAGE_CONNECTIVITY);
    if (now - lastChecked >= 1000) {
        updateWifiSignal();
        lastChecked = now;
    }
    if (wifiConnected()) {
        lastConnected = now;
        if ((!mdns_started) || ((now - lastAnnounce) > 300000)) {
            cbox::tracing::add(AppTrace::MDNS_START);
            // explicit announce every 5 minutes
            mdns_started = theMdns().begin(true);
            lastAnnounce = now;
        }
        if (!http_started) {
            cbox::tracing::add(AppTrace::HTTP_START);
            http_started = httpserver.begin();
        }

        if (mdns_started) {
            cbox::tracing::add(AppTrace::MDNS_PROCESS);
            theMdns().processQueries();
        }
        if (http_started) {
            while (true) {
                TCPClient client = httpserver.available();
                if (client) {
                    cbox::tracing::add(AppTrace::HTTP_RESPONSE);
                    const uint8_t start[] =
                        "HTTP/1.1 200 Ok\n\n<html><body>"
                        "<p>Your BrewBlox Spark is online but it does not run its own web server. "
                        "Please install a BrewBlox server to connect to it using the BrewBlox protocol.</p>"
                        "<p>Device ID = ";
                    const uint8_t end[] = "</p></body></html>\n\n";

                    client.write(start, sizeof(start), 10);
                    if (!client.getWriteError() && client.status()) {
                        client.write(reinterpret_cast<const uint8_t*>(deviceIdString().data()), 24, 10);
                    }
                    if (!client.getWriteError() && client.status()) {
                        client.write(end, sizeof(end), 10);
                    }
                    client.stop();
                } else {
                    break;
                }
            }
        }
    } else {
        mdns_started = false;
        http_started = false;

        if (now - lastConnected > 60000) {
            // after 60 seconds without WiFi, trigger reconnect
            // wifi is expected to reconnect automatically. This is a failsafe in case it does not
            cbox::tracing::add(AppTrace::WIFI_CONNECT);
            if (!spark::WiFi.connecting()) {
                spark::WiFi.connect(WIFI_CONNECT_SKIP_LISTEN);
            }
            lastConnected = now; // retry again after 60s
        }
    }
}

void
initMdns()
{
    MDNS& mdns = theMdns();
    mdns.addService(MDNS::Protocol::TCP, "_http", deviceIdString(), 80);

    std::string hwEntry("HW=Spark ");
    switch (getSparkVersion()) {
    case SparkVersion::V1:
        hwEntry += "1";
        break;
    case SparkVersion::V2:
        hwEntry += "2";
        break;
    case SparkVersion::V3:
        hwEntry += "3";
        break;
    }

    mdns.addService(MDNS::Protocol::TCP, "_brewblox", deviceIdString(), 8332,
                    {"VERSION=" stringify(GIT_VERSION),
                     std::string("ID=") + deviceIdString(),
                     "PLATFORM=" stringify(PLATFORM_ID),
                     hwEntry});
}

void
wifiInit()
{
    System.disable(SYSTEM_FLAG_RESET_NETWORK_ON_CLOUD_ERRORS);
    spark::WiFi.setListenTimeout(45);
    spark::WiFi.connect(WIFI_CONNECT_SKIP_LISTEN);
    initMdns();
}
