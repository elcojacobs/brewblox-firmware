#include "Spark4.hpp"
#include "TFT035.hpp"

// #include "SDCard.hpp"
#include "DS248x.hpp"
#include "OneWire.h"
#include "network/network.hpp"
#include <asio.hpp>

#include "ExpOwGpio.hpp"
#include "hal/hal_delay.h"
#include "lvgl.h"
#include <algorithm>
#include <esp_heap_caps.h>
#include <esp_log.h>
auto display = TFT035();

void monitor_flush(lv_disp_drv_t* disp_drv, const lv_area_t* area, lv_color_t* color_p)
{

    auto size = area->x2 - area->x1 + area->y2 - area->y2;

    display.setPos(area->x1, area->x2, area->y1, area->y2);
    uint8_t * buffer =  static_cast<uint8_t*>(heap_caps_malloc(size * 3 * sizeof(uint8_t), MALLOC_CAP_DMA));
    // uint8_t *buffer  = new uint8_t[size*3];
    // buffer = new (heap_caps_malloc(size * 3 * sizeof(uint8_t), MALLOC_CAP_DMA)) uint8_t [size * 3];
    // uint8_t* buffer[size * 3] = new(heap_caps_malloc(size * 3 * sizeof(uint8_t), MALLOC_CAP_DMA)) uint8_t[size * 3]{};
    auto iterator = buffer;
 
    std::for_each(color_p, color_p + size, [&](lv_color_t& color) {
        *iterator = color.ch.red << 3;
        iterator++;
        *iterator = color.ch.green << 2;
        iterator++;
        *iterator = color.ch.blue << 3;
        iterator++;
    });
    display.dmaWrite(buffer,size*3,true);
    lv_disp_flush_ready(disp_drv);
    ESP_LOGI("Display", "row written");
}
void displayTest()
{
    lv_init();
    static lv_disp_draw_buf_t disp_buf1;
    static lv_color_t buf1_1[480];
    lv_disp_draw_buf_init(&disp_buf1, buf1_1, NULL, 480);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf = &disp_buf1;
    disp_drv.flush_cb = monitor_flush;
    disp_drv.hor_res = 320;
    disp_drv.ver_res = 480;

    static lv_disp_t* disp;
    disp = lv_disp_drv_register(&disp_drv);

    lv_obj_t* btn = lv_btn_create(lv_scr_act()); /*Add a button the current screen*/
    // lv_obj_set_pos(btn, 10, 10);                            /*Set its position*/
    lv_obj_set_size(btn, 240, 100); /*Set its size*/
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, -40);

    lv_obj_t* label = lv_label_create(btn); /*Add a label to the button*/
    lv_label_set_text(label, "HET WERKT");  /*Set the labels text*/
    lv_obj_center(label);
}
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
    // auto oneWire1 = DS248x(0);
    // auto oneWire2 = DS248x(1);
    // auto oneWire3 = DS248x(2);
    // auto exp1 = ExpOwGpio(0);
    ESP_LOGI("Display", "initing");
    display.init();
    displayTest();
    ESP_LOGI("Display", "Image written");
    // if (oneWire1.init()) {
    //     ESP_LOGI("OW1", "ready");
    // } else {
    //     ESP_LOGE("OW1", "not ready");
    // }
    // if (oneWire2.init()) {
    //     ESP_LOGI("OW2", "ready");
    // } else {
    //     ESP_LOGE("OW2", "not ready");
    // }
    // if (oneWire3.init()) {
    //     ESP_LOGI("OW3", "ready");
    // } else {
    //     ESP_LOGE("OW3", "not ready");
    //}

    // OneWire ow1(oneWire1);
    // OneWire ow2(oneWire2);
    // OneWire ow3(oneWire3);

    while (true) {
        lv_tick_inc(1); // This must be set to the time it took!
        lv_task_handler();
        //     OneWireAddress a;
        //     std::array<OneWire*, 3> ows = {&ow1, &ow2, &ow3};
        //     for (auto& ow : ows) {
        //         ow->reset_search();
        //         if (ow->search(a)) {
        //             auto s = a.toString();
        //             ESP_LOGI("OW", "%s", s.c_str());
        //         }
        //     }
        //     // exp1.gpio_status();
        //     // exp1.gpio_test();
        hal_delay_ms(1);
    }

    asio::io_context io;
    io.run();

#ifndef ESP_PLATFORM
    return 0;
#endif
}
