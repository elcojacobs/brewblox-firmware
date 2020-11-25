#include "App.h"
#include "network/Ethernet.h"
#include "network/Wifi.h"
#include "network/server.hpp"
#include "network/wifi_creds.h"
#include <asio.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include <driver/gpio.h>
#include <nvs_flash.h>
#pragma GCC diagnostic pop

#include "ADS124S08.h"
#include "PCA9571.hpp"
#include "hal/hal_i2c.h"

#include "esp_log.h"

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
    hal_i2c_master_init();
    PCA9571 io_expander;
    io_expander.set_pins(0xFF);
    io_expander.set_pin(0, false);
    hal_delay_ms(500);
    io_expander.set_pin(1, false);
    hal_delay_ms(500);
    io_expander.set_pin(2, false);
    hal_delay_ms(500);
    io_expander.set_pin(0, true);
    hal_delay_ms(500);
    io_expander.set_pin(2, true);
    ESP_LOGW("app", "io_expander initialized");

    ADS124S08 ads(
        0, -1,
        [&io_expander](bool pinIsHigh) { //reset
            io_expander.set_pin(3, pinIsHigh);
        },
        [](bool pinIsHigh) { //start
            hal_gpio_write(2, pinIsHigh);
        },
        [&io_expander]() {
            io_expander.set_pin(4, false);
        },
        [&io_expander]() {
            io_expander.set_pin(4, true);
        });

    while (true) {
        // ESP_LOGW("tick", "");
        hal_delay_ms(1);
        ads.startup();
    }
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