/*
 * Copyright 2014-2015 Matthew McGowan.
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
#include "Connections.h"
#include "DataStream.h"
#include "DataStreamConverters.h"
#include <functional>
#include <memory>
#include <vector>

namespace cbox {
class ConnectionPool {
private:
    std::vector<std::reference_wrapper<ConnectionSource>> connectionSources;
    std::vector<std::unique_ptr<Connection>> connections;

    CompositeDataOut<decltype(connections)> allConnectionsDataOut;
    DataOut* currentDataOut;

public:
    ConnectionPool(std::initializer_list<std::reference_wrapper<ConnectionSource>> list);

    void updateConnections();

    inline size_t size()
    {
        return connections.size();
    }

    void process(std::function<void(DataIn& in, DataOut& out)> handler);

    inline DataOut& logDataOut() const
    {
        return *currentDataOut;
    }

    inline void disconnect()
    {
        connections.clear();
    }

    void stopAll();

    void startAll();
};

} // end namespace cbox
