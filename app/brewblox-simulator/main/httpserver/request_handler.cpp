//
// request_handler.cpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "request_handler.hpp"
#include "mime_types.hpp"
#include "reply.hpp"
#include "request.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace http {
namespace server {

    request_handler::request_handler()
    {
    }

    void request_handler::handle_request(const request& req, reply& rep)
    {
        // Decode url to path.
        std::string request_path;
        if (!url_decode(req.uri, request_path)) {
            rep = reply::stock_reply(reply::bad_request);
            return;
        }

        // Request path must be absolute and not contain "..".
        if (request_path.empty() || request_path[0] != '/'
            || request_path.find("..") != std::string::npos) {
            rep = reply::stock_reply(reply::bad_request);
            return;
        }

        // // If path ends in slash (i.e. is a directory) then add "index.html".
        // if (request_path[request_path.size() - 1] == '/') {
        //     request_path += "index.html";
        // }

        // // Determine the file extension.
        // std::size_t last_slash_pos = request_path.find_last_of("/");
        // std::size_t last_dot_pos = request_path.find_last_of(".");
        // std::string extension;
        // if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos) {
        //     extension = request_path.substr(last_dot_pos + 1);
        // }

        std::string content_type;

        for (const auto& h : uri_handlers_) {
            if (request_path.compare(h.uri_) == 0) {

                h.content_generator_(rep.content);

                rep.status = reply::ok;
                rep.headers.resize(2);
                rep.headers[0].name = "Content-Length";
                rep.headers[0].value = std::to_string(rep.content.size());
                rep.headers[1].name = "Content-Type";
                rep.headers[1].value = h.content_type_;
                return;
            }
        }

        // no handler found

        rep = reply::stock_reply(reply::not_found);
        return;
    }

    bool request_handler::url_decode(const std::string& in, std::string& out)
    {
        out.clear();
        out.reserve(in.size());
        for (std::size_t i = 0; i < in.size(); ++i) {
            if (in[i] == '%') {
                if (i + 3 <= in.size()) {
                    int value = 0;
                    std::istringstream is(in.substr(i + 1, 2));
                    if (is >> std::hex >> value) {
                        out += static_cast<char>(value);
                        i += 2;
                    } else {
                        return false;
                    }
                } else {
                    return false;
                }
            } else if (in[i] == '+') {
                out += ' ';
            } else {
                out += in[i];
            }
        }
        return true;
    }

    void request_handler::add_uri_handler(std::string&& uri, std::string&& content_type, uri_content_generator_t&& content_generator)
    {
        uri_handlers_.emplace_back(std::move(uri), std::move(content_type), std::move(content_generator));
    }

} // namespace server
} // namespace http
