#include "fonts/fonts.hpp"
#include "lvgl.h"
#include "styles.hpp"
#include <ctime>
#include <string>

class Bar {

public:
    Bar(lv_obj_t* mainContainer)
    {
        barObj = lv_obj_create(mainContainer, NULL);
        lv_obj_set_size(barObj, 480, 25);
        lv_obj_add_style(barObj, LV_CONT_PART_MAIN, &style::bar);
        // lv_obj_set_style_local_bg_color(barObj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
        // lv_obj_set_style_local_margin_all(barObj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
        // lv_obj_set_style_local_pad_all(barObj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
        // lv_obj_set_style_local_border_width(barObj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
        // lv_obj_set_style_local_radius(barObj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
        // lv_obj_set_style_local_text_font(barObj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &main_font);

        label = lv_label_create(barObj, NULL);
        lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);
        lv_obj_align(label, NULL, LV_ALIGN_IN_LEFT_MID, 0, 0);

        timeLabel = lv_label_create(barObj, NULL);
        lv_label_set_align(timeLabel, LV_LABEL_ALIGN_CENTER);

        lv_label_set_text(this->timeLabel, time);
        lv_obj_align(timeLabel, NULL, LV_ALIGN_IN_RIGHT_MID, -5, 0);
        updateLabel();
    }

    Bar(){};

    void setEthernetIp(uint32_t ip)
    {
        this->ethernetIp = formatIp(ip);
        updateLabel();
    }

    void setWifiIp(uint32_t ip)
    {
        this->wifiIp = formatIp(ip);
        updateLabel();
    }
    void setWifiEnabled(bool enabled)
    {
        this->wifiEnabled = enabled;
        updateLabel();
    }
    void setEthernetEnabled(bool enabled)
    {
        this->ethernetEnabled = enabled;
        updateLabel();
    }

    void updateTime()
    {
        struct timeval tv;
        time_t nowtime;
        struct tm* nowtm;

        gettimeofday(&tv, NULL);
        nowtime = tv.tv_sec;
        nowtm = localtime(&nowtime);
        strftime(time, sizeof(time), "%H:%M:%S", nowtm);

        updateLabel();
    }

private:
    void updateLabel()
    {
        std::string wifi;
        if (wifiEnabled) {
            wifi = std::string() + symbols::wifi + " " + this->wifiIp;
        } else {
            wifi = symbols::wifi_off;
        }

        std::string ethernet;
        if (ethernetEnabled) {
            ethernet = std::string() + symbols::ethernet + " " + this->ethernetIp;
        } else {
            ethernet = symbols::ethernet;
        }
        std::string t = std::string() + " " + ethernet + "  " + wifi;
        lv_label_set_text(this->label, t.c_str());
        lv_obj_align(label, NULL, LV_ALIGN_IN_LEFT_MID, 0, 0);
        lv_label_set_text(this->timeLabel, time);
    }

    std::string formatIp(uint32_t ip)
    {
        return std::to_string((ip >> (8 * 0)) & 0xff) + "." + std::to_string((ip >> (8 * 1)) & 0xff) + "." + std::to_string((ip >> (8 * 2)) & 0xff) + "." + std::to_string((ip >> (8 * 3)) & 0xff);
    }

    std::string ethernetIp = "xx.xx.xx.xx";
    std::string wifiIp = "xx.xx.xx.xx";
    bool wifiEnabled = false;
    bool ethernetEnabled = false;

    char time[10] = "00:00:00";
    lv_obj_t* barObj = nullptr;
    lv_obj_t* label = nullptr;
    lv_obj_t* timeLabel = nullptr;
};