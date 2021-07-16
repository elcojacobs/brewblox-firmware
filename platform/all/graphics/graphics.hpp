#pragma once
#include <lvgl.h>
#include <memory>
#include <vector>

namespace cbox {
class Box;
}

class TFT035;
class Layout;
class Bar;
class BaseWidget;

class Graphics {
public:
    static void monitor_flush(lv_disp_drv_t* disp_drv, const lv_area_t* area, lv_color_t* color_p);
    static void init(cbox::Box& box);
    static void update();
    static void tick(uint32_t elapsed);

private:
    static lv_disp_drv_t disp_drv;
    static std::unique_ptr<TFT035> display;
    static std::unique_ptr<Layout> layout;
};

class Layout {
public:
    Layout(cbox::Box& box);
    ~Layout();

    void update();
    void updateWidgets();
    void updateConfig();

    std::unique_ptr<Bar> bar;

private:
    cbox::Box& box;
    std::vector<std::unique_ptr<BaseWidget>> sensorWidgets;
    std::array<lv_obj_t*, 6> placeholders;
    lv_obj_t* mainContainer;
    lv_obj_t* grid;
};