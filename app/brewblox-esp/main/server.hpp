#pragma once
#include "circular_buffer_view.hpp"
#include "session.hpp"

class server {
    using tcp = asio::ip::tcp;

public:
    server(asio::io_context& io_context,
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
                std::make_shared<session>(std::move(socket))->start();
            }
            do_accept();
        });
    }

    tcp::acceptor acceptor;
};