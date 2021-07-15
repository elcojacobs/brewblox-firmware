#include "styles.hpp"
#include "fonts/fonts.hpp"

namespace style {

lv_style_t bar;
lv_style_t grid;
lv_style_t maincontainer;
lv_style_t block;
lv_style_t block_text;
lv_style_t bigNumber_text;

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

    lv_style_init(&block);
    lv_style_set_radius(&block, LV_STATE_DEFAULT, 10);
    lv_style_set_bg_opa(&block, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&block, LV_STATE_DEFAULT, LV_COLOR_BLUE);
    // lv_style_set_bg_grad_color(&block, LV_STATE_DEFAULT, LV_COLOR_BLUE);
    // lv_style_set_bg_grad_dir(&block, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_style_set_border_color(&block, LV_STATE_DEFAULT, LV_COLOR_BLUE);
    lv_style_set_value_color(&block, LV_STATE_DEFAULT, LV_COLOR_WHITE);

    lv_style_init(&bigNumber_text);
    lv_style_set_text_color(&bigNumber_text, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_text_font(&bigNumber_text, LV_STATE_DEFAULT, &fonts::main_36); /*Set a larger font*/

    lv_style_init(&block_text);
    lv_style_set_text_color(&block_text, LV_STATE_DEFAULT, LV_COLOR_WHITE);
}
}