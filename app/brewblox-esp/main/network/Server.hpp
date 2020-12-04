#pragma once
#include "CircularBufferView.hpp"
#include "Session.hpp"


class Server {
    using tcp = asio::ip::tcp;

public:
    Server(asio::io_context& io_context,
           const tcp::endpoint& endpoint)
        : acceptor(io_context, endpoint)
    {
        do_accept();
    }

private:
    void do_accept()
    {
        acceptor.async_accept([this](asio::error_code error, tcp::socket socket) {
            if (!error) {
                std::make_shared<Session>(std::move(socket))->start();
            }
            do_accept();
        });
    }

    tcp::acceptor acceptor;
};