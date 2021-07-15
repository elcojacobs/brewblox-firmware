#pragma once
#include "./BaseWidget.hpp"

class PidWidget : public BaseWidget {
public:
    PidWidget(lv_obj_t* grid, cbox::CboxPtr<PidBlock>&& ptr, const char* label, lv_color_t color)
        : BaseWidget(grid, color)
        , lookup(ptr)
    {
        makeObj(label);
    }

    PidWidget(const PidWidget&) = delete;
    PidWidget& operator=(const PidWidget&) = delete;

    ~PidWidget()
    {
        lv_obj_del(obj);
    }

    void update()
    {
        if (auto ptr = lookup.const_lock()) {

            setBar1(int32_t(ptr->get().p()));
            setBar2(int32_t(ptr->get().i()));
            setBar3(int32_t(ptr->get().d()));

            auto& inputLookup = ptr->getInputLookup();
            auto& outputLookup = ptr->getOutputLookup();
            auto input = inputLookup.const_lock();
            if (input && input->valueValid()) {
                setValue1(temp_to_string(input->value(), 1, tempUnit));
            } else {
                setValue1("-");
            }
            if (input && input->settingValid()) {
                setValue2(temp_to_string(input->setting(), 1, tempUnit));
            } else {
                setValue2("-");
            }

            auto output = outputLookup.const_lock();
            if (output && output->valueValid()) {
                setValue3(temp_to_string(output->value(), 1, tempUnit));
            } else {
                setValue3("-");
            }
            if (output && output->settingValid()) {
                setValue4(temp_to_string(output->setting(), 1, tempUnit));
            } else {
                setValue4("-");
            }

            return;
        }
    }
    void setLabel(std::string txt)
    {
        lv_label_set_text(label, txt.c_str());
    }

    void setValue1(std::string txt)
    {
        lv_label_set_text(value1, txt.c_str());
        lv_obj_align(value1, NULL, LV_ALIGN_CENTER, -30, -30);
    }

    void setValue2(std::string txt)
    {
        lv_label_set_text(value2, txt.c_str());
        lv_obj_align(value2, NULL, LV_ALIGN_CENTER, -30, -52);
    }
    void setValue3(std::string txt)
    {
        lv_label_set_text(value3, txt.c_str());
        lv_obj_align(value3, NULL, LV_ALIGN_CENTER, 30, -30);
    }
    void setValue4(std::string txt)
    {
        lv_label_set_text(value4, txt.c_str());
        lv_obj_align(value4, NULL, LV_ALIGN_CENTER, 30, -52);
    }

    void setBar1(int32_t persentage)
    {
        lv_bar_set_value(bar1, persentage, LV_ANIM_ON);
    }
    void setBar2(int32_t persentage)
    {
        lv_bar_set_value(bar2, persentage, LV_ANIM_ON);
    }
    void setBar3(int32_t persentage)
    {
        lv_bar_set_value(bar3, persentage, LV_ANIM_ON);
    }

private:
    void makeObj(const char* labelTxt)
    {
        label = lv_label_create(obj, NULL);
        lv_label_set_text(label, labelTxt);
        lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 47);
        lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);
        lv_obj_set_style_local_text_font(label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &fonts::main_16);

        value1 = lv_label_create(obj, NULL);
        lv_label_set_text(value1, "-");
        lv_obj_align(value1, NULL, LV_ALIGN_CENTER, -30, -30);
        lv_label_set_align(value1, LV_LABEL_ALIGN_CENTER);
        lv_obj_set_style_local_text_font(value1, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &fonts::main_20);

        value2 = lv_label_create(obj, NULL);
        lv_label_set_text(value2, "-");
        lv_obj_align(value2, NULL, LV_ALIGN_CENTER, -30, -52);
        lv_label_set_align(value2, LV_LABEL_ALIGN_CENTER);
        lv_obj_set_style_local_text_font(value2, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &fonts::main_20);

        value3 = lv_label_create(obj, NULL);
        lv_label_set_text(value3, "-");
        lv_obj_align(value3, NULL, LV_ALIGN_CENTER, 30, -30);
        lv_label_set_align(value3, LV_LABEL_ALIGN_CENTER);
        lv_obj_set_style_local_text_font(value3, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &fonts::main_20);

        value4 = lv_label_create(obj, NULL);
        lv_label_set_text(value4, "-");
        lv_obj_align(value4, NULL, LV_ALIGN_CENTER, 30, -52);
        lv_label_set_align(value4, LV_LABEL_ALIGN_CENTER);
        lv_obj_set_style_local_text_font(value4, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &fonts::main_20);

        bar1 = lv_bar_create(obj, NULL);
        lv_obj_set_size(bar1, 100, 10);
        lv_bar_set_type(bar1, LV_BAR_TYPE_SYMMETRICAL);
        lv_bar_set_range(bar1, -100, 100);
        lv_obj_align(bar1, NULL, LV_ALIGN_CENTER, 0, -5);
        lv_bar_set_anim_time(bar1, 2000);
        lv_bar_set_value(bar1, 0, LV_ANIM_OFF);

        bar2 = lv_bar_create(obj, NULL);
        lv_obj_set_size(bar2, 100, 10);
        lv_bar_set_type(bar2, LV_BAR_TYPE_SYMMETRICAL);
        lv_bar_set_range(bar2, -100, 100);
        lv_obj_align(bar2, NULL, LV_ALIGN_CENTER, 0, 10);
        lv_bar_set_anim_time(bar2, 2000);
        lv_bar_set_value(bar2, 0, LV_ANIM_OFF);

        bar3 = lv_bar_create(obj, NULL);
        lv_obj_set_size(bar3, 100, 10);
        lv_bar_set_type(bar3, LV_BAR_TYPE_SYMMETRICAL);
        lv_bar_set_range(bar3, -100, 100);
        lv_obj_align(bar3, NULL, LV_ALIGN_CENTER, 0, 25);
        lv_bar_set_anim_time(bar3, 2000);
        lv_bar_set_value(bar3, 0, LV_ANIM_OFF);
    }
    lv_obj_t* label;
    lv_obj_t* value1;
    lv_obj_t* value2;
    lv_obj_t* value3;
    lv_obj_t* value4;
    lv_obj_t* bar1;
    lv_obj_t* bar2;
    lv_obj_t* bar3;
    cbox::CboxPtr<PidBlock> lookup;
};