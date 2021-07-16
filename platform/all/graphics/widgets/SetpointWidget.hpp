#pragma once

#include "./BaseWidget.hpp"
#include "blox/SetpointSensorPairBlock.h"

class SetpointWidget : public BaseWidget {
public:
    SetpointWidget(lv_obj_t* grid, const cbox::CboxPtr<SetpointSensorPairBlock> ptr, const char* label, lv_color_t color)
        : BaseWidget(grid, color)
        , lookup(ptr)
    {
        makeObj(grid, label, "-", "-");
    }

    SetpointWidget(const SetpointWidget&) = delete;
    SetpointWidget& operator=(const SetpointWidget&) = delete;

    ~SetpointWidget()
    {
        lv_obj_del(obj);
    }
    void update()
    {
        if (auto ptr = lookup.const_lock()) {
            auto& pair = ptr->get();

            if (pair.valueValid()) {
                setValue1(temp_to_string(pair.value(), 2, tempUnit));
            } else {
                setValue1("-");
            }
            if (pair.settingValid()) {
                setValue2(temp_to_string(pair.setting(), 2, tempUnit));
            } else {
                setValue1("-");
            }
            return;
        }
    }
    void setLabel(std::string txt)
    {
        lv_label_set_text(label, txt.c_str());
        lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 50);
    }

    void setValue1(std::string txt)
    {
        lv_label_set_text(value1, txt.c_str());
        lv_obj_align(value1, NULL, LV_ALIGN_CENTER, 0, 0);
    }

    void setValue2(std::string txt)
    {
        lv_label_set_text(value2, txt.c_str());
        lv_obj_align(value2, NULL, LV_ALIGN_CENTER, 0, -40);
    }

private:
    void makeObj(lv_obj_t* grid, const char* labelTxt, const char* value1Txt, const char* value2Txt)
    {
        label = lv_label_create(obj, NULL);
        lv_label_set_text(label, labelTxt);
        lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 50);
        lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);
        lv_obj_reset_style_list(label, LV_LABEL_PART_MAIN);
        lv_obj_add_style(label, LV_LABEL_PART_MAIN, &style::block_text);

        value1 = lv_label_create(obj, NULL);
        lv_label_set_text(value1, value1Txt);
        lv_obj_align(value1, NULL, LV_ALIGN_CENTER, 0, 0);
        lv_label_set_align(value1, LV_LABEL_ALIGN_CENTER);
        lv_obj_reset_style_list(value1, LV_LABEL_PART_MAIN);
        lv_obj_add_style(value1, LV_LABEL_PART_MAIN, &style::bigNumber_text);

        value2 = lv_label_create(obj, NULL);
        lv_label_set_text(value2, value2Txt);
        lv_obj_align(value2, NULL, LV_ALIGN_CENTER, 0, -40);
        lv_label_set_align(value2, LV_LABEL_ALIGN_CENTER);
        lv_obj_reset_style_list(value2, LV_LABEL_PART_MAIN);
        lv_obj_add_style(value2, LV_LABEL_PART_MAIN, &style::block_text);
    }

    cbox::CboxPtr<SetpointSensorPairBlock> lookup;
    lv_obj_t* label;
    lv_obj_t* value1;
    lv_obj_t* value2;
};