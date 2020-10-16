#include "App.h"
#include "smooth/core/Application.h"
#include "smooth/core/Task.h"
#include "smooth/core/logging/log.h"
#include "smooth/core/network/Ethernet.h"
#include "smooth/core/network/IPv4.h"
#include "smooth/core/network/SecureServerSocket.h"
#include "smooth/core/network/ServerSocket.h"
#include "smooth/core/task_priorities.h"
//#include "wifi_creds.h"

using namespace std::chrono;
using namespace smooth::core;
using namespace smooth::core::network;
using namespace smooth::core::network::event;
using namespace smooth::core::logging;

App::App()
    : EarlyInit(smooth::core::APPLICATION_BASE_PRIO, std::chrono::milliseconds(1000))
{
}

void
App::init()
{
    // Start socket dispatcher first of all so that it is
    // ready to receive network status events.
    network::SocketDispatcher::instance();

    Log::info("App::Init", "Starting Ethernet...");

    ethernet.init();

    // network::Wifi& wifi = get_wifi();
    // wifi.set_host_name("BrewbloxESP");
    // wifi.set_auto_connect(true);
    // wifi.set_ap_credentials(WIFI_SSID, WIFI_PASSWORD);
    // wifi.connect_to_ap();

    // The server creates StreamingClients which are self-sufficient and never seen by the main
    // application (unless the implementor adds such bindings).
    server = ServerSocket<StreamingClient, StreamingProtocol, void>::create(*this, 5, 5);
    server->start(std::make_shared<IPv4>("0.0.0.0", 8080));

    // Point your browser to http://localhost:8080 and watch the output.
    // Or, if you're on linux, do "echo ` date` | nc localhost 8080 -w1"
}
