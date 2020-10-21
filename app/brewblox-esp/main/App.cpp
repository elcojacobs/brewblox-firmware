#include "App.h"
#include "Ethernet.h"
#include "Wifi.h"
#include "server.hpp"
#include "wifi_creds.h"
#include <asio.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include <driver/gpio.h>
#include <nvs_flash.h>
#pragma GCC diagnostic pop

using namespace std::chrono;
using tcp = asio::ip::tcp;

App::App()
{
    nvs_flash_init();
    gpio_install_isr_service(0);
    esp_event_loop_create_default();
}

App::~App()
{
    esp_event_loop_delete_default();
    gpio_uninstall_isr_service();
    nvs_flash_deinit();
}

void App::start()
{
    init();
}

void App::init()
{
    esp_netif_init();

    auto& ethernet = get_ethernet();
    ethernet.set_host_name("brewblox_wired");
    ethernet.start();

    auto& wifi = get_wifi();
    wifi.set_host_name("brewblox_wifi");
    wifi.set_auto_connect(true);
    wifi.set_ap_credentials(WIFI_SSID, WIFI_PASSWORD);
    wifi.connect_to_ap();

    asio::io_context io_context;
    server srv(io_context, tcp::endpoint(tcp::v4(), 81));
    io_context.run();
}