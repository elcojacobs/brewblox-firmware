#pragma once
#include "CboxConnection.hpp"

class CboxConnectionSource : public cbox::ConnectionSource {
    using tcp = asio::ip::tcp;

public:
    CboxConnectionSource(asio::io_context& io_context,
                         const uint16_t& port)
        : endpoint(asio::ip::tcp::v4(), port)
        , acceptor(io_context, endpoint)
    {
        do_accept();
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
        do_accept();
    }

private:
    void do_accept()
    {
        acceptor.async_accept([this](asio::error_code error, tcp::socket socket) {
            if (!error) {
                waiting.emplace_back(new CboxTcpConnection(std::move(socket)));
            }
            do_accept();
        });
    }

    // queued connections waiting to be passed to application
    std::vector<std::unique_ptr<cbox::Connection>> waiting;

    tcp::endpoint endpoint;
    tcp::acceptor acceptor;
};
