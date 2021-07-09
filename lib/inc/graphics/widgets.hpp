#pragma once

#include "graphics/fonts/fonts.hpp"
#include "lvgl.h"
#include "styles.hpp"
#include <string>

class baseWidget {
public:
    baseWidget(lv_obj_t* grid)
    {
        makeStyle();
        obj = lv_obj_create(grid, NULL);
        lv_obj_set_size(obj, 145, 132);
        lv_obj_reset_style_list(obj, LV_BTN_PART_MAIN);
        lv_obj_add_style(obj, LV_BTN_PART_MAIN, &style::block);
    }

protected:
    lv_obj_t* obj;

private:
    void makeStyle()
    {

        // lv_style_init(&style);
        // lv_style_set_radius(&style, LV_STATE_DEFAULT, 10);
        // lv_style_set_bg_opa(&style, LV_STATE_DEFAULT, LV_OPA_COVER);
        // lv_style_set_bg_color(&style, LV_STATE_DEFAULT, LV_COLOR_BLUE);
        // lv_style_set_bg_grad_color(&style, LV_STATE_DEFAULT, LV_COLOR_BLUE);
        // lv_style_set_bg_grad_dir(&style, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
        // lv_style_set_border_color(&style, LV_STATE_DEFAULT, LV_COLOR_BLUE);
        // lv_style_set_value_color(&style, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    }
    // lv_style_t style;
};
class NormalWidget : public baseWidget {
public:
    NormalWidget(lv_obj_t* grid)
        : baseWidget(grid)
    {
        makeObj(grid, "label", "value1", "value2");
    }
    NormalWidget(lv_obj_t* grid, std::string labelTxt, std::string value1Txt, std::string value2Txt)
        : baseWidget(grid)
    {
        makeObj(grid, labelTxt.c_str(), value1Txt.c_str(), value2Txt.c_str());
    }
    void destroy()
    {
        lv_obj_del(obj);
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
        lv_obj_add_style(value1, LV_LABEL_PART_MAIN, &style::block_text);

        value2 = lv_label_create(obj, NULL);
        lv_label_set_text(value2, value2Txt);
        lv_obj_align(value2, NULL, LV_ALIGN_CENTER, 0, -40);
        lv_label_set_align(value2, LV_LABEL_ALIGN_CENTER);
        lv_obj_reset_style_list(value2, LV_LABEL_PART_MAIN);
        lv_obj_add_style(value2, LV_LABEL_PART_MAIN, &style::block_text);
    }

    lv_obj_t* label;
    lv_obj_t* value1;
    lv_obj_t* value2;
};

class PidWidget {
public:
    PidWidget(lv_obj_t* grid)
    {
        makeObj(grid, "PID", "1.0", "2.0", "3.0", "4.0");
    }
    PidWidget(lv_obj_t* grid, std::string labelTxt, std::string value1Txt, std::string value2Txt, std::string value3Txt, std::string value4Txt)
    {
        makeObj(grid, labelTxt.c_str(), value1Txt.c_str(), value2Txt.c_str(), value3Txt.c_str(), value4Txt.c_str());
    }

    ~PidWidget()
    {
        lv_obj_del(obj);
    }
    void setLabel(std::string txt)
    {
        lv_label_set_text(label, txt.c_str());
    }

    void setValue1(std::string txt)
    {
        lv_label_set_text(value1, txt.c_str());
    }

    void setValue2(std::string txt)
    {
        lv_label_set_text(value2, txt.c_str());
    }
    void setValue3(std::string txt)
    {
        lv_label_set_text(value3, txt.c_str());
    }
    void setValue4(std::string txt)
    {
        lv_label_set_text(value4, txt.c_str());
    }

    void setBar1(int8_t persentage)
    {
        lv_bar_set_value(bar1, persentage, LV_ANIM_ON);
    }
    void setBar2(int8_t persentage)
    {
        lv_bar_set_value(bar2, persentage, LV_ANIM_ON);
    }
    void setBar3(int8_t persentage)
    {
        lv_bar_set_value(bar3, persentage, LV_ANIM_ON);
    }

private:
    void makeObj(lv_obj_t* grid, const char* labelTxt, const char* value1Txt, const char* value2Txt, const char* value3Txt, const char* value4Txt)
    {
        obj = lv_obj_create(grid, NULL);
        lv_obj_set_size(obj, 145, 132);
        lv_obj_set_style_local_margin_all(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);

        label = lv_label_create(obj, NULL);
        lv_label_set_text(label, labelTxt);
        lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 47);
        lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);
        lv_obj_set_style_local_text_font(label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &fonts::main_16);

        value1 = lv_label_create(obj, NULL);
        lv_label_set_text(value1, value1Txt);
        lv_obj_align(value1, NULL, LV_ALIGN_CENTER, -30, -30);
        lv_label_set_align(value1, LV_LABEL_ALIGN_CENTER);
        lv_obj_set_style_local_text_font(value1, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &fonts::main_20);

        value2 = lv_label_create(obj, NULL);
        lv_label_set_text(value2, value2Txt);
        lv_obj_align(value2, NULL, LV_ALIGN_CENTER, -30, -52);
        lv_label_set_align(value2, LV_LABEL_ALIGN_CENTER);
        lv_obj_set_style_local_text_font(value2, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &fonts::main_20);

        value3 = lv_label_create(obj, NULL);
        lv_label_set_text(value3, value3Txt);
        lv_obj_align(value3, NULL, LV_ALIGN_CENTER, 30, -30);
        lv_label_set_align(value3, LV_LABEL_ALIGN_CENTER);
        lv_obj_set_style_local_text_font(value3, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &fonts::main_20);

        value4 = lv_label_create(obj, NULL);
        lv_label_set_text(value4, value4Txt);
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
    lv_obj_t* obj;
    lv_obj_t* label;
    lv_obj_t* value1;
    lv_obj_t* value2;
    lv_obj_t* value3;
    lv_obj_t* value4;
    lv_obj_t* bar1;
    lv_obj_t* bar2;
    lv_obj_t* bar3;
};