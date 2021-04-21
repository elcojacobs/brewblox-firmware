#include "Spark4.hpp"

// #include "SDCard.hpp"
#include "DS2482.hpp"
#include "OneWire.h"
#include "network/network.hpp"
#include <asio.hpp>

#include "ExpOwGpio.hpp"
#include "hal/hal_delay.h"
#include <esp_log.h>

extern "C" {
#ifdef ESP_PLATFORM
void app_main();
}
#else
int main(int /*argc*/, char** /*argv*/);
#endif

#ifdef ESP_PLATFORM
void app_main()
#else
int main(int /*argc*/, char** /*argv*/)
#endif
{
    Spark4::hw_init();    
    hal_delay_ms(100);
    network_init();
    // SDCard::test();
    auto oneWire1 = DS248x(0);
    auto oneWire2 = DS248x(1);
    auto oneWire3 = DS248x(2);
    auto exp1 = ExpOwGpio(0);

    if (oneWire1.init()) {
        ESP_LOGI("OW1", "ready");
    } else {
        ESP_LOGE("OW1", "not ready");
    }
    if (oneWire2.init()) {
        ESP_LOGI("OW2", "ready");
    } else {
        ESP_LOGE("OW2", "not ready");
    }
    if (oneWire3.init()) {
        ESP_LOGI("OW3", "ready");
    } else {
        ESP_LOGE("OW3", "not ready");
    }

    OneWire ow1(oneWire1);
    OneWire ow2(oneWire2);
    OneWire ow3(oneWire3);

    while (true) {
        OneWireAddress a;
        std::array<OneWire*, 3> ows = {&ow1, &ow2, &ow3};
        for (auto& ow : ows) {
            ow->reset_search();
            if (ow->search(a)) {
                auto s = a.toString();
                ESP_LOGI("OW", "%s", s.c_str());
            }
        }
        // exp1.gpio_status();
        // exp1.gpio_test();
        hal_delay_ms(10);
    }

    asio::io_context io;
    io.run();

#ifndef ESP_PLATFORM
    return 0;
#endif
}
