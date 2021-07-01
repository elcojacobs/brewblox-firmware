#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include <esp_wifi.h>
#pragma GCC diagnostic pop
#include "NetworkInterface.hpp"

/// Wifi management class
class Wifi : public NetworkInterface {
public:
    Wifi();

    Wifi(const Wifi&) = delete;

    Wifi(Wifi&&) = delete;

    Wifi& operator=(const Wifi&) = delete;

    Wifi& operator=(Wifi&&) = delete;

    ~Wifi();

    /// Sets the credentials for the Wifi network
    /// \param wifi_ssid The SSID
    /// \param wifi_password The password
    void set_ap_credentials(const std::string& wifi_ssid, const std::string& wifi_password);

    /// Enables, disables auto reconnect on loss of Wifi connection.
    /// \param auto_connect
    void set_auto_connect(bool auto_connect);

    /// Initiates the connection to the AP.
    void connect_to_ap();

    /// Returns a value indicating if the required settings are set.
    /// \return true or false.
    [[nodiscard]] bool is_configured() const
    {
        return ssid.length() > 0 && password.length() > 0;
    }

    static void wifi_event_callback(void* event_handler_arg,
                                    esp_event_base_t event_base,
                                    int32_t event_id,
                                    void* event_data);

    /// Start providing an access point
    /// \param max_conn maximum number of clients to connect to this AP
    void start_softap(uint8_t max_conn = 1);

private:
    void connect() const;

    void close_if();

    bool auto_connect_to_ap = false;
    std::string ssid{};
    std::string password{};

    esp_netif_t* interface{nullptr};
    esp_event_handler_instance_t instance_wifi_event{};
    esp_event_handler_instance_t instance_ip_event{};
};

Wifi& get_wifi();
