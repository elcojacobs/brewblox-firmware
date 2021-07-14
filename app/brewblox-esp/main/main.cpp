#include "Spark4.hpp"
#include "TFT035.hpp"

// #include "SDCard.hpp"
#include "DS248x.hpp"
#include "ExpansionGpio.hpp"
#include "OneWire.h"
#include "RecurringTask.hpp"
#include "TempSensor.h"
#include "brewblox_esp.hpp"
#include "graphics/graphics.hpp"
#include "graphics/widgets.hpp"
#include "hal/hal_delay.h"
#include "lvgl.h"
#include "network/BufferedConnection.hpp"
#include "network/CboxConnection.hpp"
#include "network/CboxConnectionSource.hpp"
#include "network/CboxServer.hpp"
#include "network/network.hpp"
#include <algorithm>
#include <asio.hpp>
#include <esp_heap_caps.h>
#include <esp_log.h>
#include <iomanip>
#include <sstream>

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
    // auto settings = DisplaySettingsBlock::settings();

    // static std::array<NormalWidget, 5> sensorWidgets{{
    //     NormalWidget(graphics.grid, "Widget 1", "IPA", "21.0"),
    //     NormalWidget(graphics.grid, "Widget 2", "Blond", "21.0"),
    //     NormalWidget(graphics.grid, "Widget 3", "Lager", "5.1"),
    //     NormalWidget(graphics.grid, "Widget 4", "Stout", "23.1"),
    //     NormalWidget(graphics.grid, "Widget 5", "Wit", "21.4"),
    // }};

    ESP_LOGI("Display", "Image written");

    asio::io_context io;
    static auto& box = makeBrewBloxBox(io);

    static auto graphics = Graphics::getInstance();
    graphics.setBox(&box);
    // static auto widget6 = PidWidget(graphics.grid);
    // widget6.setBar1(25);
    // widget6.setBar2(-80);

    static std::array<cbox::CboxPtr<TempSensor>, 5> sensors{{
        box.makeCboxPtr<TempSensor>(cbox::obj_id_t(100)),
        box.makeCboxPtr<TempSensor>(cbox::obj_id_t(101)),
        box.makeCboxPtr<TempSensor>(cbox::obj_id_t(102)),
        box.makeCboxPtr<TempSensor>(cbox::obj_id_t(103)),
        box.makeCboxPtr<TempSensor>(cbox::obj_id_t(104)),
    }};

    static CboxServer server(io, 8332, box);

    static auto displayTicker = RecurringTask(io, asio::chrono::milliseconds(100),
                                              RecurringTask::IntervalType::FROM_EXPIRY,
                                              []() {
                                                  //   auto w_it = sensorWidgets.begin();
                                                  //   for (auto& s_lookup : sensors) {
                                                  //       if (auto s = s_lookup.const_lock()) {
                                                  //           if (s->valid()) {
                                                  //               auto v = s->value();
                                                  //               auto temp_str = temp_to_string(v, 2, TempUnit::Celsius);
                                                  //               w_it->setValue2(temp_str);
                                                  //           } else {
                                                  //               w_it->setValue2("--.-");
                                                  //           }
                                                  //       } else {
                                                  //           w_it->setValue2("-");
                                                  //       }
                                                  //       w_it++;
                                                  //   }
                                                  auto& wifi = get_wifi();
                                                  graphics.bar.setWifiIp(wifi.get_local_ip());
                                                  graphics.bar.setWifiEnabled(wifi.is_connected());

                                                  graphics.updateWidgets();
                                                  auto& ethernet = get_ethernet();
                                                  graphics.bar.setEthernetIp(ethernet.get_local_ip());
                                                  graphics.bar.setEthernetEnabled(ethernet.is_connected());
                                                  graphics.updateConfig();
                                                  graphics.aquire_spi();

                                                  lv_task_handler();
                                                  graphics.release_spi();
                                              });
    displayTicker.start();

    static auto displayTimer = RecurringTask(io, asio::chrono::milliseconds(100),
                                             RecurringTask::IntervalType::FROM_EXPIRY,
                                             []() {
                                                 lv_tick_inc(100); // This must be set to the time it took!
                                             });
    displayTimer.start();

    static auto timeSetter = RecurringTask(io, asio::chrono::milliseconds(1000),
                                           RecurringTask::IntervalType::FROM_EXPIRY,
                                           []() {
                                               auto tickMinutes = asio::chrono::steady_clock::now().time_since_epoch() / asio::chrono::minutes(1);
                                               auto minutes = tickMinutes % (60);

                                               auto tickHours = asio::chrono::steady_clock::now().time_since_epoch() / asio::chrono::hours(1);
                                               auto hours = tickHours % (24);
                                               graphics.bar.setTime(hours, minutes);
                                           });
    timeSetter.start();

    static auto gpioTester = RecurringTask(io, asio::chrono::milliseconds(5000),
                                           RecurringTask::IntervalType::FROM_EXPIRY,
                                           []() {
                                               static ExpansionGpio* exp1 = new ExpansionGpio(0);
                                               static bool active = false;
                                               exp1->selfTest();
                                               exp1->drv_status();
                                               if (active) {
                                                   exp1->writeChannelConfig(1, IoArray::ChannelConfig::ACTIVE_HIGH);
                                               } else {
                                                   exp1->writeChannelConfig(1, IoArray::ChannelConfig::ACTIVE_LOW);
                                               }
                                               active = !active;
                                               box.discoverNewObjects();
                                           });
    gpioTester.start();

    io.run();

#ifndef ESP_PLATFORM
    return 0;
#endif
}
