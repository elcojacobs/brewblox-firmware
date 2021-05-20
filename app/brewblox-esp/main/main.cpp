#include "Spark4.hpp"
#include "TFT035.hpp"

// #include "SDCard.hpp"
#include "DS248x.hpp"
#include "ExpansionGpio.hpp"
#include "OneWire.h"
#include "RecurringTask.hpp"
#include "TempSensor.h"
#include "brewblox_esp.hpp"
#include "graphics.hpp"
#include "hal/hal_delay.h"
#include "lvgl.h"
#include "network/BufferedConnection.hpp"
#include "network/CboxConnection.hpp"
#include "network/CboxConnectionSource.hpp"
#include "network/CboxServer.hpp"
#include "network/network.hpp"
#include "widgets.hpp"
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

    static auto graphics = Graphics::getInstance();
    static std::array<NormalWidget, 5> sensorWidgets{{
        NormalWidget(graphics.grid, "Widget 1", "IPA", "21.0"),
        NormalWidget(graphics.grid, "Widget 2", "Blond", "21.0"),
        NormalWidget(graphics.grid, "Widget 3", "Lager", "5.1"),
        NormalWidget(graphics.grid, "Widget 4", "Stout", "23.1"),
        NormalWidget(graphics.grid, "Widget 5", "Wit", "21.4"),
    }};

    static auto widget6 = PidWidget(graphics.grid);
    ESP_LOGI("Display", "Image written");

    asio::io_context io;
    static auto& box = makeBrewBloxBox(io);

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
                                                  auto w_it = sensorWidgets.begin();
                                                  for (auto& s_lookup : sensors) {
                                                      if (auto s = s_lookup.const_lock()) {
                                                          if (s->valid()) {
                                                              auto v = s->value();
                                                              auto temp_str = temp_to_string(v, 2, TempUnit::Celsius);
                                                              w_it->setValue2(temp_str);
                                                          } else {
                                                              w_it->setValue2("--.-");
                                                          }
                                                      } else {
                                                          w_it->setValue2("-");
                                                      }
                                                      w_it++;
                                                  }

                                                    auto tick = asio::chrono::steady_clock::now().time_since_epoch() / asio::chrono::milliseconds(1);
                                                  Graphics::getInstance().aquire_spi();
                                                //   lv_obj_invalidate(graphics.grid); // keep writing full display for testing
                                                  lv_tick_inc(100);                 // This must be set to the time it took!
                                                  lv_task_handler();
                                                  Graphics::getInstance().release_spi();
                                                    auto tock = asio::chrono::steady_clock::now().time_since_epoch() / asio::chrono::milliseconds(1);
                                                    uint32_t duration = tock - tick;
                                                    ESP_LOGE("display tick", "duration  %u", duration);
                                              });
    displayTicker.start();

    static auto gpioTester = RecurringTask(io, asio::chrono::milliseconds(5000),
                                           RecurringTask::IntervalType::FROM_EXPIRY,
                                           []() {
                                               static ExpansionGpio* exp1 = new ExpansionGpio(0);
                                               static bool active = false;
                                               exp1->test();
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
