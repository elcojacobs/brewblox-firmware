#pragma once
#include "Temperature.h"
#include "lvgl.h"

class BaseWidget {
public:
    BaseWidget(lv_obj_t* grid, lv_color_t color);
    virtual ~BaseWidget() = default;

    BaseWidget(const BaseWidget&) = delete;
    BaseWidget& operator=(const BaseWidget&) = delete;

    virtual void update() = 0;
    static TempUnit tempUnit;

protected:
    lv_obj_t* obj;
    lv_obj_t* grid;
};