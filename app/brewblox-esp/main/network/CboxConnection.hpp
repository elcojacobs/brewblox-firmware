#pragma once

#include "BufferedConnection.hpp"
#include "cbox/Connections.h"
#include "cbox/DataStreamIo.h"

class CboxTcpConnection : public cbox::Connection {
public:
    explicit CboxTcpConnection(asio::ip::tcp::socket&& socket)
        : conn(std::move(socket))
        , in_std(&conn.buffer_in)
        , out_std(&conn.buffer_out)
        , in_cbox(in_std)
        , out_cbox(out_std)
    {
    }
    ~CboxTcpConnection() = default;

    virtual cbox::DataOut& getDataOut() override final
    {
        return out_cbox;
    };
    virtual cbox::DataIn& getDataIn() override final
    {
        return in_cbox;
    };
    virtual bool isConnected() override final
    {
        return conn.is_connected();
    };
    virtual void stop() override final
    {
        conn.close();
    }

private:
    BufferedConnection conn;
    std::istream in_std;
    std::ostream out_std;
    cbox::IStreamDataIn in_cbox;
    cbox::OStreamDataOut out_cbox;
};