//
// connection_manager.hpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "CboxConnection.hpp"
#include <set>

/// Manages open connections so that they may be cleanly stopped when the server
/// needs to shut down.
class CboxConnectionManager {
public:
    CboxConnectionManager(const CboxConnectionManager&) = delete;
    CboxConnectionManager& operator=(const CboxConnectionManager&) = delete;

    /// Construct a connection manager.
    CboxConnectionManager();

    /// Add the specified connection to the manager and start it.
    void start(CboxConnectionPtr c);

    /// Stop the specified connection.
    void stop(CboxConnectionPtr c);

    /// Stop all connections.
    void stop_all();

private:
    /// The managed connections.
    std::set<CboxConnectionPtr> connections;
};
