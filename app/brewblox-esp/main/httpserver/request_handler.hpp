//
// request_handler.hpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_REQUEST_HANDLER_HPP
#define HTTP_REQUEST_HANDLER_HPP

#include "uri_handler.hpp"
#include <string>
#include <vector>

namespace http {
namespace server {

    struct reply;
    struct request;

    /// The common handler for all incoming requests.
    class request_handler {
    public:
        request_handler(const request_handler&) = delete;
        request_handler& operator=(const request_handler&) = delete;

        /// Construct with a directory containing files to be served.
        explicit request_handler();

        /// Handle a request and produce a reply.
        void handle_request(const request& req, reply& rep);

        void add_uri_handler(std::string&& uri, std::string&& content_type, uri_content_generator_t&& content_generator);

    private:
        /// Perform URL-decoding on a string. Returns false if the encoding was
        /// invalid.
        static bool url_decode(const std::string& in, std::string& out);
        std::vector<uri_handler> uri_handlers_;
    };

} // namespace server
} // namespace http

#endif // HTTP_REQUEST_HANDLER_HPP
