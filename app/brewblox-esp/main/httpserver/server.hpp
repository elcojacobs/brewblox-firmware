//
// server.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include "connection.hpp"
#include "connection_manager.hpp"
#include "request_handler.hpp"
#include <asio.hpp>
#include <string>

namespace http {
namespace server {

    /// The top-level class of the HTTP server.
    class server {
    public:
        server(const server&) = delete;
        server& operator=(const server&) = delete;

        /// Construct the server to listen on the specified TCP port and hook into exsiting io service
        explicit server(asio::io_context& io, const uint16_t& port);

        inline void add_uri_handler(std::string&& uri, std::string&& content_type, uri_content_generator_t&& content_generator)
        {
            request_handler_.add_uri_handler(std::move(uri), std::move(content_type), std::move(content_generator));
        };

    private:
        /// Perform an asynchronous accept operation.
        void do_accept();

        /// Wait for a request to stop the server.
        void do_await_stop();

        /// The io_context used to perform asynchronous operations.
        asio::io_context& io_context_;

        /// The signal_set is used to register for process termination notifications.
        // asio::signal_set signals_;

        /// Acceptor used to listen for incoming connections.
        asio::ip::tcp::acceptor acceptor_;

        /// The connection manager which owns all live connections.
        connection_manager connection_manager_;

        /// The handler for all incoming requests.
        request_handler request_handler_;
    };

} // namespace server
} // namespace http

#endif // HTTP_SERVER_HPP
