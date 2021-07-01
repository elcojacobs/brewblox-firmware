#if 0

#pragma once
#include "CboxConnection.hpp"
#include "esp_log.h"

class CboxConnectionSource : public cbox::ConnectionSource {
    using tcp = asio::ip::tcp;

public:
    CboxConnectionSource(asio::io_context& io_context,
                         const uint16_t& port)
        : endpoint(asio::ip::address_v4::any(), port)
        , acceptor(io_context, endpoint)
    {
        start();
    }

    CboxConnectionSource(CboxConnectionSource&) = delete;
    CboxConnectionSource(CboxConnectionSource&&) = delete;

    ~CboxConnectionSource()
    {
        ESP_LOGE("ConSource", "destructed");
        stop();
    }

    std::unique_ptr<cbox::Connection> newConnection() override final
    {
        if (!waiting.empty()) {
            auto conn = std::move(waiting.back());
            waiting.pop_back();
            return std::move(conn);
        }
        return {};
    }

    virtual void stop() override final
    {
        acceptor.cancel();
    }

    virtual void start() override final
    {
        asio::error_code ec;
        acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true), ec);
        acceptor.open(endpoint.protocol(), ec);
        acceptor.bind(endpoint, ec);
        acceptor.listen(4, ec);
        ESP_LOGE("ConSource", "started %s", ec.message().c_str());
        do_accept();
    }

private:
    void do_accept()
    {
        acceptor.async_accept([this](asio::error_code error, tcp::socket socket) {
            if (!error) {
                waiting.push_back(std::unique_ptr<cbox::Connection>(new CboxTcpConnection(std::move(socket))));
            } else {
                ESP_LOGE("ConSource", "%s", error.message().c_str());
            }
            do_accept();
        });
    }

    // queued connections waiting to be passed to application
    std::vector<std::unique_ptr<cbox::Connection>> waiting;

    tcp::endpoint endpoint;
    tcp::acceptor acceptor;
};

#endif