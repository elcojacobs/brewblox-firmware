#include "fonts/fonts.hpp"
#include "lvgl.h"

namespace style {

static lv_style_t bar;
static lv_style_t grid;
static lv_style_t maincontainer;

void init()
{
    lv_style_init(&bar);
    lv_style_set_bg_color(&bar, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_margin_all(&bar, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_all(&bar, LV_STATE_DEFAULT, 0);
    lv_style_set_border_width(&bar, LV_STATE_DEFAULT, 0);
    lv_style_set_radius(&bar, LV_STATE_DEFAULT, 0);
    lv_style_set_text_font(&bar, LV_STATE_DEFAULT, &fonts::main);

    lv_style_init(&grid);
    lv_style_set_bg_opa(&grid, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&grid, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_bg_grad_color(&grid, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_bg_grad_dir(&grid, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_style_set_border_color(&grid, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_pad_all(&grid, LV_STATE_DEFAULT, 10);
    lv_style_set_margin_all(&grid, LV_STATE_DEFAULT, 0);
    lv_style_set_radius(&grid, LV_STATE_DEFAULT, 0);

    lv_style_init(&maincontainer);
    lv_style_set_pad_all(&maincontainer, LV_STATE_DEFAULT, 0);

    lv_style_set_pad_top(&maincontainer, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_bottom(&maincontainer, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_left(&maincontainer, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_right(&maincontainer, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_inner(&maincontainer, LV_STATE_DEFAULT, 0);

    lv_style_set_margin_all(&maincontainer, LV_STATE_DEFAULT, 0);
    lv_style_set_border_width(&maincontainer, LV_STATE_DEFAULT, 0);
}
}