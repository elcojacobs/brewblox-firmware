#include "Wifi.hpp"
#include "copy_min_to_buffer.hpp"
#include <cstring>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_netif.h>
#include <esp_wifi_types.h>

#ifdef ESP_PLATFORM
#include "sdkconfig.h"
static_assert(CONFIG_ESP_SYSTEM_EVENT_TASK_STACK_SIZE >= 3072,
              "Need enough stack to be able to log in the event loop callback.");
#endif

Wifi::Wifi()
{
    esp_event_handler_instance_register(WIFI_EVENT,
                                        ESP_EVENT_ANY_ID,
                                        &Wifi::wifi_event_callback,
                                        this,
                                        &instance_wifi_event);

    esp_event_handler_instance_register(IP_EVENT,
                                        ESP_EVENT_ANY_ID,
                                        &Wifi::wifi_event_callback,
                                        this,
                                        &instance_ip_event);
}

Wifi::~Wifi()
{
    esp_event_handler_instance_unregister(IP_EVENT, ESP_EVENT_ANY_ID, instance_ip_event);
    esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_wifi_event);
    esp_wifi_disconnect();
    esp_wifi_stop();
    esp_wifi_deinit();
}

void Wifi::set_ap_credentials(const std::string& wifi_ssid, const std::string& wifi_password)
{
    this->ssid = wifi_ssid;
    this->password = wifi_password;
}

void Wifi::set_auto_connect(bool auto_connect)
{
    auto_connect_to_ap = auto_connect;
}

void Wifi::connect_to_ap()
{
    // Prepare to connect to the provided SSID and password
    wifi_init_config_t init = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&init);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_ps(WIFI_PS_NONE);

    wifi_config_t config;
    memset(&config, 0, sizeof(config));
    copy_min_to_buffer(ssid.begin(), ssid.length(), config.sta.ssid);
    copy_min_to_buffer(password.begin(), password.length(), config.sta.password);

    config.sta.bssid_set = false;

    // Store Wifi settings in RAM - it is the applications responsibility to store settings.
    esp_wifi_set_storage(WIFI_STORAGE_RAM);
    esp_wifi_set_config(WIFI_IF_STA, &config);

    close_if();
    interface = esp_netif_create_default_wifi_sta();
    apply_host_name();
    connect();
}

void Wifi::connect() const
{
#ifdef ESP_PLATFORM
    esp_wifi_start();
    esp_wifi_connect();
#else
    // Assume network is available when running under POSIX system.
#endif
}

void Wifi::wifi_event_callback(void* event_handler_arg,
                               esp_event_base_t event_base,
                               int32_t event_id,
                               void* event_data)
{
    // Note: be very careful with what you do in this method - it runs under the event task
    // (sys_evt) with a very small default stack.
    Wifi* wifi = reinterpret_cast<Wifi*>(event_handler_arg);

    if (event_base == WIFI_EVENT) {
        switch (event_id) {
        case WIFI_EVENT_STA_START: {
        } break;
        case WIFI_EVENT_STA_CONNECTED: {
            wifi->connected = true;
        } break;
        case WIFI_EVENT_STA_DISCONNECTED: {
            wifi->ip.addr = 0;
            wifi->connected = false;

            if (wifi->auto_connect_to_ap) {
                esp_wifi_stop();
                wifi->connect();
            }
        } break;
        case WIFI_EVENT_AP_START: {
            wifi->ip.addr = 0xC0A80401; // 192.168.4.1
        } break;
        case WIFI_EVENT_AP_STOP: {
            wifi->ip.addr = 0;
            ESP_LOGI("SoftAP", "AP stopped");
        } break;
        case WIFI_EVENT_AP_STACONNECTED: {
            auto data = reinterpret_cast<wifi_event_ap_staconnected_t*>(event_data);
            ESP_LOGI("SoftAP", "Station connected. MAC: %u:%u:%u:%u:%u:%u join, AID=%u",
                     data->mac[0],
                     data->mac[1],
                     data->mac[2],
                     data->mac[3],
                     data->mac[4],
                     data->mac[5],
                     data->aid);
        } break;
        case WIFI_EVENT_AP_STADISCONNECTED: {
            auto data = reinterpret_cast<wifi_event_ap_stadisconnected_t*>(event_data);

            ESP_LOGI("SoftAP", "Station disconnected. MAC: %u:%u:%u:%u:%u:%u join, AID=%u",
                     data->mac[0],
                     data->mac[1],
                     data->mac[2],
                     data->mac[3],
                     data->mac[4],
                     data->mac[5],
                     data->aid);
        } break;
        }
    } else if (event_base == IP_EVENT) {
        if (event_id == IP_EVENT_STA_GOT_IP
            || event_id == IP_EVENT_GOT_IP6
            || event_id == IP_EVENT_ETH_GOT_IP) {
            // auto ip_changed = event_id == IP_EVENT_STA_GOT_IP ? reinterpret_cast<ip_event_got_ip_t*>(event_data)->ip_changed : true;
            wifi->ip.addr = reinterpret_cast<ip_event_got_ip_t*>(event_data)->ip_info.ip.addr;
        } else if (event_id == IP_EVENT_STA_LOST_IP) {
            wifi->ip.addr = 0;
        }
    }
}

void Wifi::close_if()
{
    if (interface) {
        esp_netif_destroy(interface);
        interface = nullptr;
    }
}

void Wifi::start_softap(uint8_t max_conn)
{
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    wifi_config_t config{};

    copy_min_to_buffer(ssid.begin(), ssid.length(), config.ap.ssid);
    copy_min_to_buffer(password.begin(), password.length(), config.ap.password);

    config.ap.max_connection = max_conn;
    config.ap.authmode = password.empty() ? WIFI_AUTH_OPEN : WIFI_AUTH_WPA_WPA2_PSK;

    close_if();
    interface = esp_netif_create_default_wifi_ap();

    esp_wifi_set_mode(WIFI_MODE_AP);
    esp_wifi_set_config(wifi_interface_t(ESP_IF_WIFI_AP), &config);
    esp_wifi_start();

    ESP_LOGI("SoftAP", "SSID: %s; Auth %s", ssid.c_str(), (password.empty() ? "Open" : "WPA2/PSK"));

#ifndef ESP_PLATFORM

    // Assume network is available when running under POSIX system.
    publish_status(true, true);
#endif
}

Wifi& get_wifi()
{
    static Wifi* wifi = new Wifi();

    return *wifi;
}