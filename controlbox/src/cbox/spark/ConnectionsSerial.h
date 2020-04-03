/*
 * Copyright 2018 BrewBlox / Elco Jacobs
 *
 * This file is part of Controlbox.
 *
 * Controlbox is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Controlbox.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "../Connections.h"
#include "spark_wiring_usbserial.h"

namespace cbox {

class SerialConnection : public StreamRefConnection<USBSerial> {
public:
    SerialConnection(USBSerial& ser)
        : StreamRefConnection(ser)
    {
    }
    virtual ~SerialConnection(){};

    virtual void stop() override final
    {
        _fetch_usbserial().unlock();
    }
};

class SerialConnectionSource : public ConnectionSource {
private:
    bool serial_enabled = false;
    USBSerial& ser;

public:
    SerialConnectionSource(USBSerial& serial)
        : ser(serial)
    {
    }

    std::unique_ptr<Connection> newConnection() override final
    {
        if (serial_enabled && ser.isConnected() && ser.try_lock()) {
            return std::make_unique<SerialConnection>(ser);
        }
        return nullptr;
    }

    virtual void start() override final
    {
        serial_enabled = true;
        ser.begin(115200);
    }

    virtual void stop() override final
    {
        ser.end();
        serial_enabled = false;
    }
};

} // end namespace cbox
