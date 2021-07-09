#include "TFT035.hpp"
#include "bar.hpp"
#include "blox/DisplaySettingsBlock.h"
#include "graphics/widgets.hpp"
#include "lvgl.h"

class Graphics {
public:
    lv_obj_t* grid = nullptr;

    static Graphics& getInstance()
    {
        static Graphics instance;

        return instance;
    }

    void updateConfig()
    {
        if (DisplaySettingsBlock::newSettingsReceived()) {
            auto settings = DisplaySettingsBlock::settings();
            for (auto sensor : sensorWidgets) {
                sensor.destroy();
            }
            sensorWidgets.clear();
            for (uint16_t x = 0; x < settings.widgets_count; x++) {
                sensorWidgets.emplace_back(grid, settings.widgets[x].name, "test", "21.0");
            }
            // sensorWidgets[0].setLabel(settings.name);
            // here we update stuff
        }
    }
    static void monitor_flush(lv_disp_drv_t* disp_drv, const lv_area_t* area, lv_color_t* color_p)
    {
        auto nPixels = (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1);
        if (!nPixels) {
            // Log here when a better debug log is available.
        }

        uint8_t* readPtr = reinterpret_cast<uint8_t*>(color_p);
        uint8_t* writePtr = reinterpret_cast<uint8_t*>(color_p);

        for (auto index = 0; index < nPixels; index++) {
            *writePtr = *(readPtr + 2);
            *(writePtr + 1) = *(readPtr + 1);
            *(writePtr + 2) = *readPtr;

            readPtr += 4;
            writePtr += 3;
        }
        getInstance().display.writePixels(area->x1, area->x2, area->y1, area->y2, reinterpret_cast<uint8_t*>(color_p), nPixels);
    }

    void aquire_spi()
    {
        display.aquire_spi();
    }

    void release_spi()
    {
        display.release_spi();
    }
    Bar bar;

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
        lv_disp_set_bg_color(disp, LV_COLOR_BLACK);

        style::init();

        gridInit();
        display.release_spi();
    }

    void gridInit()
    {

        static auto mainContainer = lv_cont_create(lv_scr_act(), NULL);
        lv_cont_set_fit(mainContainer, LV_FIT_PARENT);
        lv_cont_set_layout(mainContainer, LV_LAYOUT_PRETTY_MID);
        lv_obj_add_style(mainContainer, LV_CONT_PART_MAIN, &style::maincontainer);

        bar = Bar(mainContainer);

        grid = lv_cont_create(mainContainer, NULL);
        lv_obj_align_origo(grid, NULL, LV_ALIGN_CENTER, 0, 0); /*This parametrs will be sued when realigned*/
        lv_obj_set_size(grid, 480, 295);
        lv_cont_set_layout(grid, LV_LAYOUT_PRETTY_MID);
        lv_obj_add_style(grid, LV_CONT_PART_MAIN, &style::grid);

        // sensorWidgets.push_back(NormalWidget(grid, "Widget 1", "IPA", "21.0"));
        // sensorWidgets.push_back(NormalWidget(grid, "Widget 2", "Blond", "21.0"));
        // sensorWidgets.push_back(NormalWidget(grid, "Widget 3", "Lager", "5.1"));
        // sensorWidgets.push_back(NormalWidget(grid, "Widget 4", "Stout", "23.1"));
        // sensorWidgets.push_back(NormalWidget(grid, "Widget 5", "Wit", "21.4"));
    }
    std::vector<NormalWidget> sensorWidgets;
    lv_disp_drv_t disp_drv;
    TFT035 display = TFT035([this]() {
        lv_disp_flush_ready(&disp_drv);
    });
};
