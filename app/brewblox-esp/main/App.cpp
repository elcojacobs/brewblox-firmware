#include "App.h"
#include "server.hpp"
// #include "smooth/core/Application.h"
// #include "smooth/core/Task.h"
// #include "smooth/core/logging/log.h"
// #include "smooth/core/network/Ethernet.h"
// #include "smooth/core/network/IPv4.h"
// #include "smooth/core/network/SecureServerSocket.h"
// #include "smooth/core/network/ServerSocket.h"
// #include "smooth/core/task_priorities.h"
// #include "wifi_creds.h"
#include <asio.hpp>

using namespace std::chrono;
using tcp = asio::ip::tcp;
// using namespace smooth::core;
// using namespace smooth::core::network;
// using namespace smooth::core::network::event;
// using namespace smooth::core::logging;

App::App()
//: EarlyInit(smooth::core::APPLICATION_BASE_PRIO, std::chrono::milliseconds(1000))
{
}

void
App::init()
{
    // Start socket dispatcher first of all so that it is
    // ready to receive network status events.
    // network::SocketDispatcher::instance();

    // Log::info("App::Init", "Starting Ethernet...");

    // ethernet.set_host_name("brewblox_wired");
    // ethernet.start();

    // Log::info("App::Init", "Starting WiFi...");
    // auto& wifi = get_wifi();
    // wifi.set_host_name("brewblox_wifi");
    // wifi.set_auto_connect(true);
    // wifi.set_ap_credentials(WIFI_SSID, WIFI_PASSWORD);
    // wifi.connect_to_ap();

    asio::io_context io_context;
    server srv(io_context, tcp::endpoint(tcp::v4(), 81));
    io_context.run();
}