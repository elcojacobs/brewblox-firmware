#if 0

#pragma once

#include "CircularBuffer.hpp"
#include "CircularBufferView.hpp"
#include "esp_log.h"
#include "hal/hal_delay.h"
#include <asio.hpp>
#include <functional>
using namespace std::placeholders;

class BufferedConnection : public std::enable_shared_from_this<BufferedConnection> {
    using tcp = asio::ip::tcp;

public:
    BufferedConnection(asio::ip::tcp::socket&& socket_)
        : socket(std::move(socket_))
        , timeout_timer(socket.get_executor())
        , onMessage(onMessage)
    {
        ESP_LOGI("BC", "constructed");
    }

    ~BufferedConnection()
    {
        ESP_LOGI("BC", "destructed");
        stop();
    }

    int flush_output()
    {
        start_write();
        return 0;
    }

    void start()
    {
        ESP_LOGI("BC", "started");
        // reset_timeout();
        // timeout_timer.async_wait(std::bind(&BufferedConnection::on_timeout, shared_from_this(), _1));
        buffer_out.set_sync_func(std::bind(&BufferedConnection::flush_output, shared_from_this()));
        start_read();
    }

    void stop()
    {
        ESP_LOGE("BC", "closing socket");
        writing = false;
        timeout_timer.cancel();
        socket.close();
        buffer_out.set_sync_func(nullptr);
    }

private:
    void start_read()
    {
        stopped = false;
        // Schedule asynchronous receiving of a data into free portion of buffer
        asio::async_read(socket, make_view(buffer_in), asio::transfer_at_least(1), std::bind(&BufferedConnection::on_read, shared_from_this(), _1, _2));
    }

    void on_read(asio::error_code error, std::size_t bytes_transferred)
    {
        if (stopped) {
            return;
        }

        // Check if an error has occurred or buffer full
        if (error) {
            ESP_LOGE("BC", "error %s %d", error.message().c_str(), bytes_transferred);
            stop();
            return;
        } else {
            ESP_LOGW("BC", "read %u %u %u", bytes_transferred, buffer_in.size(), buffer_in.in_avail());
        }

        if (onMessage) {
            onMessage(buffer_in, buffer_out);
        }
        // send reply
        start_write();
        // Read next message
        start_read();
    }

    void start_write()
    {
        ESP_LOGW("BC", "Start write %u %d", buffer_out.size(), stopped);
        if (stopped || writing) {
            return;
        }
        // Schedule asynchronous sending of the data
        asio::async_write(socket, make_view(buffer_out), std::bind(&BufferedConnection::on_write, shared_from_this(), _1, _2));
    }

    void on_write(asio::error_code error, std::size_t bytes_transferred)
    {
        // buffer_out.consume(bytes_transferred);
        if (!error) {
            // Check if there is more to send
            if (buffer_out.size()) {
                writing = true;
                start_write();
            } else {
                writing = false;
            }
        } else {
            ESP_LOGE("BC", "write error");
            stop();
        }
    }

    void reset_timeout()
    {
        // timeout_timer.expires_from_now(asio::chrono::seconds(30));
    }

    void on_timeout(const asio::error_code& e)
    {
        if (e) {
            return; // when timer is cancelled, don't trigger timeout
        }
        ESP_LOGE("BC", "timeout");
        stop();
    }

    bool is_connected()
    {
        return socket.is_open();
    }

    void set_handler(std::function<void(std::streambuf& in, std::streambuf& out)> onMessage_)
    {
        onMessage = onMessage_;
    }

    bool stopped = true;
    bool writing = false;
    tcp::socket socket;
    asio::steady_timer timeout_timer;
    CircularBuffer<4096> buffer_in;
    CircularBuffer<4096> buffer_out;
    std::function<void(std::istream& in, std::ostream& out)> onMessage;

    friend class CboxTcpConnection;
};

#endif