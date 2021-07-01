#pragma once
#include "CboxConnection.hpp"
#include "CboxConnectionManager.hpp"
#include "esp_log.h"

class CboxServer {
public:
    explicit CboxServer(asio::io_context& io_context_,
                        const uint16_t& port_,
                        cbox::Box& box_)
        : io_context(io_context_)
        //signals_(io_context_),
        , acceptor(io_context)
        , connection_manager{}
        , box(box_)
    {
        // Register to handle the signals that indicate when the server should exit.
        // It is safe to register for the same signal multiple times in a program,
        // provided all registration for the specified signal is made through Asio.
        //   signals_.add(SIGINT);
        //   signals_.add(SIGTERM);
        // #if defined(SIGQUIT)
        //   signals_.add(SIGQUIT);
        // #endif // defined(SIGQUIT)

        do_await_stop();

        asio::ip::tcp::endpoint endpoint(asio::ip::address_v4::any(), port_);
        acceptor.open(endpoint.protocol());
        acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
        acceptor.bind(endpoint);
        acceptor.listen();

        do_accept();
    }

    CboxServer(const CboxServer&) = delete;
    CboxServer& operator=(const CboxServer&) = delete;

    ~CboxServer() = default;

    void do_accept()
    {
        acceptor.async_accept(
            [this](std::error_code ec, asio::ip::tcp::socket socket) {
                // Check whether the server was stopped by a signal before this
                // completion handler had a chance to run.
                if (!acceptor.is_open()) {
                    return;
                }

                if (!ec) {
                    connection_manager.start(std::make_shared<CboxTcpConnection>(
                        std::move(socket), connection_manager, box));
                }

                do_accept();
            });
    }

    void do_await_stop()
    {
        // signals_.async_wait(
        //     [this](std::error_code /*ec*/, int /*signo*/) {
        //         // The server is stopped by cancelling all outstanding asynchronous
        //         // operations. Once all operations have finished the io_context::run()
        //         // call will exit.
        //     });
        acceptor.close();
        connection_manager.stop_all();
    }

    asio::io_context& io_context;
    asio::ip::tcp::acceptor acceptor;
    CboxConnectionManager connection_manager;
    cbox::Box& box;
};
