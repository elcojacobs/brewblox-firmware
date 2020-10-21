#include "NetworkInterface.h"
#include <esp_err.h>
#include <esp_log.h>
#include <esp_netif.h>
#include <sstream>

uint8_t NetworkInterface::interface_count = 0;

NetworkInterface::NetworkInterface(std::string&& name)
{
}

NetworkInterface::~NetworkInterface()
{
}

void
NetworkInterface::set_host_name(const std::string& name)
{
    // this only sets the host name string.
    // before network interface is started in subclass, it should call apply_host_name
    auto copied = name.copy(host_name, 32);
    host_name[copied] = 0; // null terminate
}

void
NetworkInterface::apply_host_name()
{
    if (interface) {
        esp_netif_set_hostname(interface, host_name);
    }
}

// attention: access to this function might have a threading issue.
// It should be called from the main thread only!
uint32_t
NetworkInterface::get_local_ip() const
{
    return ip.addr;
}

esp_err_t
NetworkInterface::get_mac_address(std::array<uint8_t, 6>& m) const
{
    esp_err_t err = ESP_FAIL;
    if (interface) {
        err = esp_netif_get_mac(interface, m.data());
    }

    return err;
}

std::string
NetworkInterface::get_mac_address_string() const
{
    std::stringstream mac;

    std::array<uint8_t, 6> m;
    esp_err_t err = get_mac_address(m);

    if (err == ESP_OK) {
        for (const auto& v : m) {
            if (mac.tellp() > 0) {
                mac << "_";
            }

            mac << std::hex << static_cast<int>(v);
        }
    }

    return mac.str();
}
