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
#include "PCA9555.hpp"
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
    PCA9555 io_expander(0x20);

    auto err = io_expander.set_directions(0x0700);
    if (err != 0) {
        ESP_LOGW("app", "io_expander error: %d", err);
    }

    //io_expander.set_output(0, false);
    hal_delay_ms(500);
    io_expander.set_output(1, false);
    hal_delay_ms(200);
    io_expander.set_output(2, false);
    hal_delay_ms(200);
    io_expander.set_output(0, true);
    hal_delay_ms(200);
    io_expander.set_output(2, true);
    ESP_LOGW("app", "io_expander initialized");

    io_expander.set_output(5, false); // keep start pin low
    io_expander.set_output(5, true);  // keep reset pin high

    ADS124S08 ads(
        0,                               // select spi idx 0 on platform
        -1,                              // no SS pin (io expander pin set in onAquire and onRelease)
        [&io_expander](bool pinIsHigh) { // set reset pin
            io_expander.set_output(3, pinIsHigh);
        },
        [&io_expander](bool pinIsHigh) { // set start pin
            io_expander.set_output(5, pinIsHigh);
        },
        [&io_expander]() {      // read ready pin state
            bool result = true; // default to high (not ready) in case of i2c error
            io_expander.get_input(10, result);
            return result;
        },
        [&io_expander]() { // cs low
            io_expander.set_output(4, false);
        },
        [&io_expander]() { // cs high
            io_expander.set_output(4, true);
        });

    if (!ads.startup()) {
        ESP_LOGE("ADC", "Init failed");
        exit(1);
    }

    ads.stop();
    ads.start();
    uint16_t ticksElapsed = 0;
    while (true) {
        if (ads.ready() || ticksElapsed > 1000) {
            // read adc when ready, or on timeout to restart it
            ticksElapsed = 0;

            auto val = ads.readLastData();
            ads.pulse_start();

            ESP_LOGI("adc read", "%d", val);
        }
        ticksElapsed++;
        hal_delay_ms(1);
    }
    init_asio();
}

void App::init_asio()
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