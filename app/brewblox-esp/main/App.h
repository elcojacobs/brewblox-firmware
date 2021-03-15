#pragma once

// #include "ADS124S08.hpp"
// #include "ChemSense.hpp"
// #include "PCA9555.hpp"
#include "network/Server.hpp"

namespace asio {
class io_context;
}

class App {
public:
    App();
    ~App();

    void init_hw();
    // void init_asio();
    // void start();
    // void init_tcp81();

    // asio::io_context& get_io_context();
    // PCA9555 io_expander;
    // ADS124S08 ads;
    // ChemSense* chemSense;
    // Server tcpServer;
};
