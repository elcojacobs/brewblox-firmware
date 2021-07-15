#pragma once

class BaseWidget {
public:
    BaseWidget(lv_obj_t* grid, lv_color_t color)
        : grid(grid)
    {
        obj = lv_obj_create(grid, NULL);
        lv_obj_set_size(obj, 145, 132);
        lv_obj_reset_style_list(obj, LV_BTN_PART_MAIN);
        lv_obj_add_style(obj, LV_BTN_PART_MAIN, &style::block);
        lv_obj_set_style_local_bg_color(obj, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, color);
        lv_obj_align(grid, NULL, LV_ALIGN_CENTER, 0, 0);
    }
    virtual ~BaseWidget() {}

    BaseWidget(const BaseWidget&) = delete;
    BaseWidget& operator=(const BaseWidget&) = delete;

    virtual void update() = 0;

protected:
    lv_obj_t* obj;
    lv_obj_t* grid;
};