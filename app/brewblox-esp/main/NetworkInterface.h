#pragma once
#include <array>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include <esp_netif.h>
#pragma GCC diagnostic pop
#include <string>

class NetworkInterface {
public:
    NetworkInterface(std::string&& name);

    NetworkInterface(const NetworkInterface&) = delete;

    NetworkInterface(NetworkInterface&&) = delete;

    NetworkInterface& operator=(const NetworkInterface&) = delete;

    NetworkInterface& operator=(NetworkInterface&&) = delete;

    virtual ~NetworkInterface();

    void set_host_name(const std::string& name);

    [[nodiscard]] bool is_connected() const;

    [[nodiscard]] std::string get_mac_address_string() const;

    [[nodiscard]] uint32_t get_local_ip() const;

    esp_err_t get_mac_address(std::array<uint8_t, 6>& m) const;

protected:
    void apply_host_name();

    esp_ip4_addr ip = {0};
    bool connected = false;
    esp_netif_t* interface{nullptr};
    static uint8_t interface_count;

    // esp_netif_set_hostname only copies a const char* pointer, so we need to keep this allocated here
    // max 32 characters
    char host_name[33] = "";
};
