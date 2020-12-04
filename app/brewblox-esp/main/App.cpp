#include "App.h"
#include "httpserver/server.hpp"
#include "network/Ethernet.h"
#include "network/Server.hpp"
#include "network/Wifi.h"
#include "network/wifi_creds.h"
#include <asio.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include <driver/gpio.h>
#include <nvs_flash.h>
#pragma GCC diagnostic pop

#include "ADS124S08.hpp"
#include "ChemSense.hpp"
#include "PCA9555.hpp"
#include "esp_log.h"
#include "hal/hal_i2c.h"
#include <string>

using namespace std::chrono;
using tcp = asio::ip::tcp;

App::App()
    : io_expander{0x20}
    , ads{
          0,                       // select spi idx 0 on platform
          -1,                      // no SS pin (io expander pin set in onAquire and onRelease)
          [this](bool pinIsHigh) { // set reset pin
              io_expander.set_output(3, pinIsHigh);
          },
          [this](bool pinIsHigh) { // set start pin
              io_expander.set_output(5, pinIsHigh);
          },
          [this]() {              // read ready pin state
              bool result = true; // default to high (not ready) in case of i2c error
              io_expander.get_input(10, result);
              return result;
          },
          [this]() { // cs low
              io_expander.set_output(4, false);
          },
          [this]() { // cs high
              io_expander.set_output(4, true);
          }}
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
    init_hw();
    init_asio();
    //init_tcp81();

    /*while (true) {
        auto nextChan = chemSense.update();
        if (nextChan == 0) {
            ESP_LOGI("adc read", "%d, %d, %d, %d",
                     chemSense.results[0],
                     chemSense.results[1],
                     chemSense.results[2],
                     chemSense.results[3]);
        }

        hal_delay_ms(245);
    }*/
}

void App::init_hw()
{
    hal_i2c_master_init();

    auto err = io_expander.set_directions(0x0700);
    if (err != 0) {
        ESP_LOGW("app", "io_expander error: %d", err);
    }

    io_expander.set_output(0, true);
    io_expander.set_output(1, false);
    io_expander.set_output(2, true);
    ESP_LOGW("app", "io_expander initialized");

    if (!ads.startup()) {
        ESP_LOGE("ADC", "Init failed");
        exit(1);
    }
}

// asio::io_context& App::get_io_context()
// {
//     static asio::io_context* context = new asio::io_context;
//     return *context;
// }

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

    asio::io_context io;
    Server srv(io, tcp::endpoint(tcp::v4(), 81));
    chemSense = new ChemSense{
        ads, io, [](const std::array<int32_t, 4>& results) {
            ESP_LOGI("adc read", "%d, %d, %d, %d",
                     results[0],
                     results[1],
                     results[2],
                     results[3]);
        }};

    std::string address = "brewblox-test.com";
    std::string port = "80";

    http::server::server s(io, address, port);
    io.run();
}

void App::init_tcp81()
{
    // Server srv(get_io_context(), tcp::endpoint(tcp::v4(), 81));
}