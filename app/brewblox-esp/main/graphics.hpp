#include "TFT035.hpp"
#include "esp_log.h"
#include "lvgl.h"

class Graphics {
public:
    lv_obj_t* grid = nullptr;

    static Graphics& getInstance()
    {
        static Graphics instance;

        return instance;
    }

    static void monitor_flush(lv_disp_drv_t* disp_drv, const lv_area_t* area, lv_color_t* color_p)
    {
        auto nPixels = (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1);
        if (!nPixels) {
            ESP_LOGE("Flush", "Writing zero pixels");
        }

        uint8_t* readPtr = reinterpret_cast<uint8_t*>(color_p);
        uint8_t* writePtr = reinterpret_cast<uint8_t*>(color_p);

        for (auto index = 0; index < nPixels; index++) {
            memcpy(writePtr, readPtr, 3);
            readPtr += 4;
            writePtr += 3;
        }
        getInstance().display.writePixels(area->x1, area->x2, area->y1, area->y2, reinterpret_cast<uint8_t*>(color_p), nPixels);
    }

    void handle()
    {
    }

    // temporary hack
    void aquire_spi()
    {
        display.aquire_spi();
    }

    void release_spi()
    {
        display.release_spi();
    }

private:
    Graphics()
    {
        display.aquire_spi();
        display.init();
        lv_init();
        static lv_disp_buf_t disp_buf1;
        static lv_color_t buf1_1[960];
        static lv_color_t buf1_2[960];
        lv_disp_buf_init(&disp_buf1, buf1_1, buf1_2, 960);

        lv_disp_drv_init(&disp_drv);

        disp_drv.buffer = &disp_buf1;
        disp_drv.flush_cb = Graphics::monitor_flush;
        disp_drv.hor_res = 320;
        disp_drv.ver_res = 480;
        disp_drv.rotated = LV_DISP_ROT_270;

        static lv_disp_t* disp;
        disp = lv_disp_drv_register(&disp_drv);
        gridInit();
        display.release_spi();
    }

    void gridInit()
    {
        grid = lv_cont_create(lv_scr_act(), NULL);
        lv_obj_align_origo(grid, NULL, LV_ALIGN_CENTER, 0, 0); /*This parametrs will be sued when realigned*/
        lv_cont_set_fit(grid, LV_FIT_PARENT);
        lv_cont_set_layout(grid, LV_LAYOUT_PRETTY_MID);
    }
    lv_disp_drv_t disp_drv;
    TFT035 display = TFT035([&]() {
        lv_disp_flush_ready(&disp_drv);
    });
};