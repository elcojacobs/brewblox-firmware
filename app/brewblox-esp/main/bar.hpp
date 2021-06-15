#include "lvgl.h"
#include "ethernet-icon.c"
extern lv_font_t ethernet;
#define Ethernet_SYMBOL "\xEF\x9E\x96"

class Bar {

    public:
    Bar(lv_obj_t* mainContainer)
    {
        barObj = lv_obj_create(mainContainer, NULL);
        lv_obj_set_size(barObj, 480, 25);
        lv_obj_set_style_local_bg_color(barObj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
        lv_obj_set_style_local_margin_all(barObj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
        lv_obj_set_style_local_pad_all(barObj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
        lv_obj_set_style_local_border_width(barObj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
        lv_obj_set_style_local_radius(barObj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);

        label = lv_label_create(barObj, NULL);
        updateLabel();
        lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);
        lv_obj_align(label, NULL, LV_ALIGN_IN_LEFT_MID, 0, 0);

        // timeLabel = lv_label_create(barObj, NULL);
        // lv_label_set_align(timeLabel, LV_LABEL_ALIGN_CENTER);
        // lv_obj_align(timeLabel, NULL, LV_ALIGN_IN_RIGHT_MID, 0, 0);

    }
    Bar() {};

    void setEthernetIp(uint32_t ip) {
        this->ethernetIp = formatIp(ip);
        updateLabel();
    }

    void setWifiIp(uint32_t ip) {
        this->wifiIp = formatIp(ip);
        updateLabel();
    }

    void setTime(std::string time) {
        // this->time = time;
        updateLabel();
    }
    private:
    
    void updateLabel() {
            std::string t = std::string () + " " + Ethernet_SYMBOL + " " + this->ethernetIp + "  " + LV_SYMBOL_WIFI + " " + this->wifiIp;
            lv_label_set_text(this->label, t.c_str());
            // lv_label_set_text(this->timeLabel, "12:34");

    }

    std::string formatIp( uint32_t ip ) {
        return std::to_string((ip >> (8*0)) & 0xff) + "." + 
        std::to_string((ip >> (8*1)) & 0xff) + "." +
        std::to_string((ip >> (8*2)) & 0xff) + "." +
        std::to_string((ip >> (8*3)) & 0xff);
    }
    std::string ethernetIp = "xx.xx.xx.xx";
    std::string wifiIp = "xx.xx.xx.xx";
    lv_obj_t* barObj = nullptr;
    lv_obj_t* label = nullptr;
    lv_obj_t* timeLabel = nullptr;

};