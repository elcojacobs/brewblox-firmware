#pragma once

#include <array>
#include <string>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include <esp_eth.h>
#include <esp_event_base.h>
#pragma GCC diagnostic pop
#include "NetworkInterface.h"

class Ethernet : public NetworkInterface {
public:
    Ethernet();

    Ethernet(const Ethernet&) = delete;

    Ethernet(Ethernet&&) = delete;

    Ethernet& operator=(const Ethernet&) = delete;

    Ethernet& operator=(Ethernet&&) = delete;

    ~Ethernet();

    esp_err_t start();

    static void eth_event_callback(void* event_handler_arg,
                                   esp_event_base_t event_base,
                                   int32_t event_id,
                                   void* event_data);

private:
    void connect() const;

    esp_event_handler_instance_t instance_eth_event{};
    esp_event_handler_instance_t instance_ip_event{};
    esp_eth_mac_t* mac{nullptr};
    esp_eth_phy_t* phy{nullptr};
    esp_eth_handle_t eth_handle{nullptr};
};

Ethernet& get_ethernet();
