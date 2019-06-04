/* Includes ------------------------------------------------------------------*/
#include "application.h"
#include <vector>

SYSTEM_THREAD(ENABLED);
SYSTEM_MODE(SEMI_AUTOMATIC);

int buzz = A2;

IPAddress ip = uint32_t(0);
bool connected = false;

void
printWiFiIp(char dest[16])
{
    snprintf(dest, 16, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
}

std::vector<system_event_t> events;

void
handleNetworkEvent(system_event_t event, int param)
{
    switch (param) {
    case network_status_connected:
        ip = WiFi.localIP();
        connected = true;
        Particle.connect();
        break;
    default:
        ip = uint32_t(0);
        connected = false;
        break;
    }
    events.push_back(param);
}

void
setup()
{
    pinMode(buzz, OUTPUT);
    digitalWrite(buzz, HIGH);
    delay(200);
    digitalWrite(buzz, LOW);
    Serial.begin();
}

void
loop()
{
    char ipString[16];
    printWiFiIp(ipString);
    Serial.println();
    Serial.print("IP: ");
    Serial.println(ipString);
    delay(5000);
    for (auto& event : events) {
        Serial.print(uint32_t(event));
        Serial.print(',');
    }
}