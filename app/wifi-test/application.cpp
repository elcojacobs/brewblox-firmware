#include "application.h"
#include <inttypes.h>

SYSTEM_MODE(MANUAL);
SYSTEM_THREAD(ENABLED);
SerialLogHandler traceLog(LOG_LEVEL_TRACE);

static TCPServer tcpServer = TCPServer(6666);
static TCPClient tcpClient;
bool tcpServerRunning;
volatile uint32_t localIp = 0;
volatile bool wifiIsConnected = false;
volatile bool inSetupMode = false;

// Toggle LED pin to see that application loop is not blocked.
// You could use D7 when testing with a Photon.
// I'm using another pin here, because D7 is one of the SWD pins used by the debugger
const int LED_PIN = P1S0;

void
stopTcp()
{
    tcpServer.stop();
    tcpClient.stop();
    tcpServerRunning = false;
    Serial.print("TCP server stopped\n");
}

void
startTcp()
{
    tcpServer.begin();
    tcpServerRunning = true;
    Serial.print("TCP server started\n");
}

void
handleNetworkEvent(system_event_t event, int param)
{
    switch (param) {
    case network_status_connected: {
        IPAddress ip = spark::WiFi.localIP();
        localIp = ip.raw().ipv4;
        wifiIsConnected = true;
    } break;
    default:
        localIp = uint32_t(0);
        wifiIsConnected = false;
        break;
    }

    Serial.printf("network event %d\n", param);
}

void
handleCloudEvent(system_event_t event, int param)
{
    switch (param) {
    case cloud_status_connecting:
        break;
    case cloud_status_connected:
        break;
    case cloud_status_disconnecting:
        break;
    case cloud_status_disconnected:
        break;
    default:
        break;
    }
    Serial.printf("cloud event %d\n", param);
}

// return time that has passed since timeStamp, take overflow into account
system_tick_t
timeSince(system_tick_t previousTime)
{
    system_tick_t currentTime = millis();
    if (currentTime >= previousTime) {
        return currentTime - previousTime;
    } else {
        // overflow has occurred
        return (currentTime + 1440000) - (previousTime + 1440000); // add a day to both for calculation
    }
}

void
onSetupModeBegin()
{
    inSetupMode = true;
}

void
onSetupModeEnd()
{
    inSetupMode = false;
}

int8_t
getWifiSignal()
{
    if (!wifiIsConnected) {
        return 2;
    }

    wlan_connected_info_t info = {0};
    info.size = sizeof(info);
    int r = wlan_connected_info(nullptr, &info, nullptr);
    if (r == 0) {
        return info.rssi != std::numeric_limits<int32_t>::min() ? info.rssi / 100 : 2;
    }
    return 2;
}

void
setup()
{
    System.disable(SYSTEM_FLAG_RESET_NETWORK_ON_CLOUD_ERRORS);
    spark::WiFi.setListenTimeout(30);
    spark::WiFi.connect(WIFI_CONNECT_SKIP_LISTEN);
    System.on(setup_begin, onSetupModeBegin);
    System.on(setup_end, onSetupModeEnd);
    System.on(network_status, handleNetworkEvent);
    System.on(cloud_status, handleCloudEvent);

    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);

    WiFi.connect(WIFI_CONNECT_SKIP_LISTEN);
}

void
printWifiSSID(char* dest, const uint8_t& maxLen)
{
    if (wifiIsConnected) {
        strncpy(dest, spark::WiFi.SSID(), maxLen);
    } else {
        dest[0] = 0;
    }
}

bool
wifiConnected()
{
    return wifiIsConnected;
}

bool
setupModeActive()
{
    return inSetupMode;
}

void
loop()
{
    static system_tick_t last_update = millis();
    static system_tick_t lastLedToggle = millis();

    if (wifiConnected()) {
        if (!tcpServerRunning) {
            startTcp();
        }
    } else {
        if (tcpServerRunning) {
            stopTcp();
        }
    }

    if (tcpServerRunning) {
        if (tcpClient.status()) {
            bool noErrors = true;
            while (noErrors && tcpClient.available() > 0) {
                int received = tcpClient.read();
                switch (received) {
                case ' ':
                case '\n':
                case '\r':
                    break;
                case 't': {
                    size_t result = tcpClient.write("toc");                  // send toc back over tcp
                    Serial.printf("hw->py: toc (%d bytes sent) \n", result); // confirm toc sent over tcp
                } break;
                default:
                    if (received < 0) {
                        Serial.printf("Receive error: %d\n", received); // confirm toc sent over tcp
                        noErrors = false;
                    } else {
                        Serial.printf("py->hw: %c\n", received); // confirm character received from tcp
                    }
                    break;
                }
            }
        }
        // listen for a new client, drop the old one if a new client arrives
        TCPClient newClient = tcpServer.available();
        if (newClient) {
            Serial.print("New TCP client\n");
            tcpClient.write("New TCP client arrived, dropping you."); // stop old client
            tcpClient.stop();                                         // stop old client
            tcpClient = newClient;
        }
    }

    // print status on serial every second
    if (timeSince(last_update) >= 1000UL) {

        IPAddress ip = {0, 0, 0, 0};
        int signal = 0;

        last_update = millis();
        bool wifiReady = WiFi.ready();
        if (wifiReady) {
            IPAddress ip = WiFi.localIP(); // <-- HARDFAULT on this line when going into setup mode

            int signal = getWifiSignal(); // WiFi.RSSI(); results in much larger binary due to use of floats
        }
        int clientConnected = tcpClient.connected();

        // Use alternatives below to see if it avoids the hardfault

        // bool wifiReady = false;
        // int clientConnected = 0;

        Serial.printf(
            "WiFi.ready(): %d\t\t"
            "IP: %d.%d.%d.%d\t"
            "RSSI: %d\t"
            "TCP client connected: %d\t\t"
            "setup: %d\t\t"
            "millis(): %" PRIu32 "\n",
            wifiReady,
            ip[0], ip[1], ip[2], ip[3],
            signal,
            clientConnected,
            uint8_t(inSetupMode),
            last_update);
    }

    if (timeSince(lastLedToggle) >= 200UL) {
        lastLedToggle = millis();
        static bool ledOn = true;
        ledOn = !ledOn;
        digitalWrite(LED_PIN, ledOn);
    }
}
