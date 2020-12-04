#pragma once

#include <functional>
#include <string>
#include <vector>

namespace http {
namespace server {
    typedef std::function<void(std::string& content)> uri_content_generator_t;

    class uri_handler {
    public:
        uri_handler(std::string&& uri, std::string&& content_type, uri_content_generator_t&& content_generator)
            : uri_{uri}
            , content_type_{content_type}
            , content_generator_{content_generator}
        {
        }
        // allow move for vector reallocation, but don't allow copy
        uri_handler(uri_handler&&) = default;
        uri_handler(uri_handler&) = delete;

        std::string uri_;
        std::string content_type_;
        uri_content_generator_t content_generator_;
    };
} // namespace server
} // namespace http
