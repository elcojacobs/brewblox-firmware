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
        auto size = area->x2 - area->x1 + area->y2 - area->y2;

        getInstance().display.setPos(area->x1, area->x2, area->y1, area->y2);

        uint8_t* buffer = static_cast<uint8_t*>(malloc(size * 3 * sizeof(uint8_t)));

        if (!buffer) {
            ESP_LOGE("Flush", "out of memory");
        }

        auto p_buf = buffer;
        for (auto c = color_p; c < color_p + size; c++) {
            *p_buf++ = c->ch.red << 3;
            *p_buf++ = c->ch.green << 2;
            *p_buf++ = c->ch.blue << 3;
        }

        getInstance().display.dmaWrite(buffer, size * 3, true);
        lv_disp_flush_ready(disp_drv);
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
        // kijken naar https://forum.lvgl.io/t/setting-16-bit-color-value-in-set-px-cb/1029/2 voor meer speed
        display.aquire_spi();
        display.init();
        lv_init();
        static lv_disp_buf_t disp_buf1;
        static lv_color_t buf1_1[480];
        lv_disp_buf_init(&disp_buf1, buf1_1, NULL, 480);

        static lv_disp_drv_t disp_drv;
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

    TFT035 display = TFT035();
};