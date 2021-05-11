#pragma once

#include "CircularBuffer.hpp"
#include "CircularBufferView.hpp"
#include <asio.hpp>
#include <functional>
#include <ostream>
using namespace std::placeholders;

class BufferedConnection : public std::enable_shared_from_this<BufferedConnection> {
    using tcp = asio::ip::tcp;

public:
    BufferedConnection(asio::ip::tcp::socket&& socket)
        : socket(std::move(socket))
    {
    }

    void start()
    {
        // Start to receive incoming data
        read();
    }

private:
    void read()
    {
        // Schedule asynchronous receiving of a data
        asio::async_some(socket, make_view(buffer_in), std::bind(&BufferedConnection::on_read, shared_from_this(), _1, _2));
    }

    void on_read(asio::error_code error, std::size_t bytes_transferred)
    {
        // Check if an error has occurred
        if (error) {
            close();
        } else if (bytes_transfered == 0) {
            // circular buffer is full
            // any action needed here? TODO
        }
        // Read the next portion of the data
        read();
    }

    void write()
    {
        writing = true;
        // Schedule asynchronous sending of the data
        asio::async_write(socket, make_view(buffer_out), std::bind(&BufferedConnection::on_write, shared_from_this(), _1, _2));
    }

    void on_write(asio::error_code error, std::size_t bytes_transferred)
    {
        writing = false;

        if (!error) {
            // Check if there is something to send
            if (buffer_out.size()) {
                write();
            }
        } else {
            close();
        }
    }

    void close()
    {
        asio::error_code error;
        socket.close(error);
    }

    bool is_connected()
    {
        return socket.is_open();
    }

    tcp::socket socket;
    bool writing = false;
    CircularBuffer<4096> buffer_in;
    CircularBuffer<4096> buffer_out;

    friend class CboxTcpConnection;
};