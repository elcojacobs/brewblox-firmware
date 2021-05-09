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

#include "ConnectionPool.h"
#include <algorithm>

namespace cbox {

ConnectionPool::ConnectionPool(std::initializer_list<std::reference_wrapper<ConnectionSource>> list)
    : connectionSources(list)
    , allConnectionsDataOut(connections, [](const decltype(connections)::value_type& conn) -> DataOut& { return conn->getDataOut(); })
    , currentDataOut(&allConnectionsDataOut)
{
}

void ConnectionPool::updateConnections()
{
    connections.erase(
        std::remove_if(connections.begin(), connections.end(), [](const decltype(connections)::value_type& conn) {
            return !conn->isConnected(); // remove disconnected connections from pool
        }),
        connections.end());

    for (auto& source : connectionSources) {
        while (true) {
            auto con = source.get().newConnection();
            if (con) {
                if (connections.size() >= 4) {
                    auto oldest = connections.begin();
                    auto& out = (*oldest)->getDataOut();
                    const char message[] = "<!Max connections exceeded, closing oldest>";
                    out.writeBuffer(message, sizeof(message) / sizeof(message[0]));
                    connections.erase(oldest);
                }
                auto& out = con->getDataOut();
                connectionStarted(out);
                connections.push_back(std::move(con));
            } else {
                break;
            }
        }
    }
}

void ConnectionPool::process(std::function<void(DataIn& in, DataOut& out)> handler)
{
    tracing::add(tracing::Action::UPDATE_CONNECTIONS);
    updateConnections();
    for (auto& conn : connections) {
        DataIn& in = conn->getDataIn();
        DataOut& out = conn->getDataOut();
        currentDataOut = &out;
        handler(in, out);
    }
    currentDataOut = &allConnectionsDataOut;
}

void ConnectionPool::stopAll()
{
    disconnect();
    for (auto& source : connectionSources) {
        source.get().stop();
    }
}

void ConnectionPool::startAll()
{
    for (auto& source : connectionSources) {
        source.get().start();
    }
}

} // end namespace cbox
