#include "Ethernet.hpp"
#include <cstring>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include <esp_eth.h>
#include <esp_eth_netif_glue.h>
#include <esp_event.h>
#include <esp_netif.h>
#pragma GCC diagnostic pop

Ethernet::Ethernet()
{
    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_ETH();
    interface = esp_netif_new(&cfg);
    esp_eth_set_default_handlers(interface);

    esp_event_handler_instance_register(ETH_EVENT,
                                        ESP_EVENT_ANY_ID,
                                        &Ethernet::eth_event_callback,
                                        this,
                                        &instance_eth_event);

    esp_event_handler_instance_register(IP_EVENT,
                                        ESP_EVENT_ANY_ID,
                                        &Ethernet::eth_event_callback,
                                        this,
                                        &instance_ip_event);

    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    phy_config.phy_addr = 0;
    phy_config.reset_gpio_num = -1;
    phy_config.reset_timeout_ms = 2000;
    mac_config.smi_mdc_gpio_num = 23;
    mac_config.smi_mdio_gpio_num = 18;
    mac = esp_eth_mac_new_esp32(&mac_config);
#if CONFIG_SMOOTH_ETH_PHY_MOCK
    phy = esp_eth_phy_new_mock(&phy_config);
#else
    phy = esp_eth_phy_new_lan8742(&phy_config);
#endif
    esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy);
    esp_eth_driver_install(&config, &eth_handle);
}

Ethernet::~Ethernet()
{
    esp_event_handler_instance_unregister(IP_EVENT, ESP_EVENT_ANY_ID, instance_ip_event);
    esp_event_handler_instance_unregister(ETH_EVENT, ESP_EVENT_ANY_ID, instance_eth_event);

    if (eth_handle) {
        esp_eth_stop(eth_handle);
    }
}

esp_err_t Ethernet::start()
{
    apply_host_name();
    auto err = esp_netif_attach(interface, esp_eth_new_netif_glue(eth_handle));

    if (err == ESP_OK) {
        /* start Ethernet driver state machine */
        err = esp_eth_start(eth_handle);
    }
    return err;
}

void Ethernet::eth_event_callback(void* event_handler_arg,
                                  esp_event_base_t event_base,
                                  int32_t event_id,
                                  void* event_data)
{
    // Note: be very careful with what you do in this method - it runs under the event task
    // (sys_evt) with a very small default stack.
    Ethernet* eth = reinterpret_cast<Ethernet*>(event_handler_arg);

    if (event_base == ETH_EVENT) {
        switch (event_id) {
        case ETHERNET_EVENT_START: {
        } break;
        case ETHERNET_EVENT_STOP: {
            eth->ip.addr = 0;
        } break;
        case ETHERNET_EVENT_CONNECTED: {
            eth->connected = true;
        } break;
        case ETHERNET_EVENT_DISCONNECTED: {
            eth->ip.addr = 0;
            eth->connected = false;
        } break;
        }
    } else if (event_base == IP_EVENT) {
        if (event_id == IP_EVENT_ETH_GOT_IP) {
            eth->ip.addr = reinterpret_cast<ip_event_got_ip_t*>(event_data)->ip_info.ip.addr;
        }
    }
}

Ethernet& get_ethernet()
{
    static Ethernet* eth = new Ethernet();
    return *eth;
}
