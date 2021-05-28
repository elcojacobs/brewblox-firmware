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
    // static void my_set_px_cb(lv_disp_drv_t * disp_drv, uint8_t * buf, lv_coord_t buf_w, lv_coord_t x, lv_coord_t y, lv_color_t color, lv_opa_t opa)
    // {
    //     /* Write to the buffer as required for the display.
    //     * Write only 1-bit for monochrome displays mapped vertically:*/
    //     buf += (buf_w * y + x)*3;
    //     *buf++ = color.ch.red << 3 ;
    //     *buf++ = color.ch.green << 2;
    //     *buf = color.ch.blue << 3;
        
    // }

    static void monitor_flush(lv_disp_drv_t* disp_drv, const lv_area_t* area, lv_color_t* color_p)
    {
        auto size = (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1);
        if (!size) {
            ESP_LOGE("Flush", "Writing zero pixels");
        }

        getInstance().display.setPos(area->x1, area->x2, area->y1, area->y2);
        uint8_t* readPtr =  reinterpret_cast<uint8_t*>(color_p);
        uint8_t* writePtr = reinterpret_cast<uint8_t*>(color_p);

        for (auto index = 0; index<size; index++ ) {
            memcpy(writePtr,readPtr,3);
            readPtr+=4;
            writePtr+=3;
        }

        getInstance().display.dmaWrite(reinterpret_cast<uint8_t*>(color_p), size * 3, true);

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
        static lv_color_t buf1_2[480];
        lv_disp_buf_init(&disp_buf1, buf1_1, buf1_2, 480);

        
        lv_disp_drv_init(&disp_drv);

        disp_drv.buffer = &disp_buf1;
        disp_drv.flush_cb = Graphics::monitor_flush;
        disp_drv.hor_res = 320;
        disp_drv.ver_res = 480;
        disp_drv.rotated = LV_DISP_ROT_270;
        // disp_drv.set_px_cb = my_set_px_cb;

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
    TFT035 display = TFT035([&](){
        lv_disp_flush_ready(&disp_drv); // Dit moet alleen gebeuren bij sturen van data
    });
};