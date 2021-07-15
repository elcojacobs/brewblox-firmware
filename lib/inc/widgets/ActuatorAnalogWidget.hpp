#pragma once

#include "blox/ActuatorPwmBlock.h"
#include "lvgl.h"
#include "widgets/BaseWidget.hpp"

class ActuatorAnalogWidget : public BaseWidget {
public:
    ActuatorAnalogWidget(lv_obj_t* grid, const cbox::CboxPtr<ActuatorAnalogConstrained> ptr, const char* label, TempUnit tempUnit, lv_color_t color)
        : BaseWidget(grid, color)
        , lookup(ptr)
        , tempUnit(tempUnit)
    {
        makeObj(grid, label, "-", "-");
    }

    ActuatorAnalogWidget(const ActuatorAnalogWidget&) = delete;
    ActuatorAnalogWidget& operator=(const ActuatorAnalogWidget&) = delete;

    ~ActuatorAnalogWidget()
    {
        lv_obj_del(obj);
    }
    void update()
    {
        if (auto pAct = lookup.const_lock()) {
            if (pAct->valueValid()) {
                setValue1(temp_to_string(pAct->value(), 2, tempUnit));
            } else {
                setValue1("-");
            }
            if (pAct->settingValid()) {
                setValue2(temp_to_string(pAct->setting(), 2, tempUnit));

            } else {
                setValue1("-");
            }

            if (auto pwmBlock = lookup.lock_as<ActuatorPwmBlock>()) {
                lv_obj_set_hidden(led, false);
                if (auto pwmTarget = pwmBlock->targetLookup().const_lock()) {
                    switch (pwmTarget->state()) {
                    case ActuatorPwm::State::Inactive:
                        lv_led_off(led);
                        break;
                    case ActuatorPwm::State::Active:
                        lv_led_on(led);
                        break;
                    case ActuatorPwm::State::Unknown:
                        lv_obj_set_hidden(led, true);
                        break;
                    }
                }
            } else {
                lv_obj_set_hidden(led, true);
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

        led = lv_led_create(obj, NULL);
        lv_obj_set_size(led, 16, 16);
        lv_obj_align(led, NULL, LV_ALIGN_CENTER, 00, 30);
    }

    cbox::CboxPtr<ActuatorAnalogConstrained> lookup;
    lv_obj_t* led;
    lv_obj_t* label;
    lv_obj_t* value1;
    lv_obj_t* value2;
    TempUnit tempUnit;
};