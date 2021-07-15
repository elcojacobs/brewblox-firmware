#pragma once

#include "lvgl.h"
#include "widgets/BaseWidget.hpp"

class TemperatureWidget : public BaseWidget {
public:
    TemperatureWidget(lv_obj_t* grid, const cbox::CboxPtr<TempSensor> ptr, const char* label, TempUnit tempUnit, lv_color_t color)
        : BaseWidget(grid, color)
        , lookup(ptr)
        , tempUnit(tempUnit)
    {
        makeObj(grid, label, "-");
    }

    TemperatureWidget(const TemperatureWidget&) = delete;
    TemperatureWidget& operator=(const TemperatureWidget&) = delete;

    ~TemperatureWidget()
    {
        lv_obj_del(obj);
    }
    void update()
    {
        if (auto ptr = lookup.const_lock()) {
            if (ptr->valid()) {
                setValue(temp_to_string(ptr->value(), 2, tempUnit));
            } else {
                setValue("-");
            }
            return;
        }
    }
    void setLabel(std::string txt)
    {
        lv_label_set_text(label, txt.c_str());
        lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 50);
    }

    void setValue(std::string txt)
    {
        lv_label_set_text(value, txt.c_str());
        lv_obj_align(value, NULL, LV_ALIGN_CENTER, 0, 0);
    }

private:
    void makeObj(lv_obj_t* grid, const char* labelTxt, const char* valueTxt)
    {
        label = lv_label_create(obj, NULL);
        lv_label_set_text(label, labelTxt);
        lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 50);
        lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);
        lv_obj_reset_style_list(label, LV_LABEL_PART_MAIN);
        lv_obj_add_style(label, LV_LABEL_PART_MAIN, &style::block_text);

        value = lv_label_create(obj, NULL);
        lv_label_set_text(value, valueTxt);
        lv_obj_align(value, NULL, LV_ALIGN_CENTER, 0, 0);
        lv_label_set_align(value, LV_LABEL_ALIGN_CENTER);
        lv_obj_reset_style_list(value, LV_LABEL_PART_MAIN);
        lv_obj_add_style(value, LV_LABEL_PART_MAIN, &style::bigNumber_text);
    }

    cbox::CboxPtr<TempSensor> lookup;
    lv_obj_t* label;
    lv_obj_t* value;
    TempUnit tempUnit;
};