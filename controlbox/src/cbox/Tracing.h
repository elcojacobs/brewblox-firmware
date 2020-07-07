/*
 * Copyright 2020 Elco Jacobs / BrewBlox
 *
 * This file is part of ControlBox
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
#include "ObjectIds.h"
#include <array>

namespace cbox {

namespace tracing {

    enum class Action : uint8_t {
        NONE = 0,
        SEND_OBJECT = 1,
        RECEIVE_OBJECT = 2,
        PERSIST_OBJECT = 3,
        UPDATE_OBJECT = 4,
        CREATE_OBJECT = 5,
        DELETE_OBJECT = 6,
        UPDATE_DISPLAY = 10,
        UPDATE_CONNECTIONS = 11,
        UPDATE_BLOCKS = 12,
        SYSTEM_TASKS = 13,
    };

    struct TraceEvent {
        Action action;
        // use raw uint16_t id's, using cbox id's overwrites backup memory on construction somewhere
        uint16_t id;
        uint16_t type;
    };

    void add(Action a, obj_id_t i, obj_type_t t);

    const std::array<TraceEvent, 10>& history();

    void unpause();

    void pause();
}
}