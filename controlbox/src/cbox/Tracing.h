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

// allow aplication to define custom actions

namespace cbox {

namespace tracing {
    enum Action : uint8_t {
        NONE = 0,
        // all commands
        READ_OBJECT = 1,              // stream an object to the data out
        WRITE_OBJECT = 2,             // stream new data into an object from the data in
        CREATE_OBJECT = 3,            // add a new object
        DELETE_OBJECT = 4,            // delete an object by id
        LIST_ACTIVE_OBJECTS = 5,      // list active objects
        READ_STORED_OBJECT = 6,       // list single object from persistent storage
        LIST_STORED_OBJECTS = 7,      // list objects saved to persistent storage
        CLEAR_OBJECTS = 8,            // remove all user objects
        REBOOT = 9,                   // reboot the system
        FACTORY_RESET = 10,           // erase all settings and reboot
        LIST_COMPATIBLE_OBJECTS = 11, // list object IDs implementing the requested interface
        DISCOVER_NEW_OBJECTS = 12,    // discover newly connected objects that support auto discovery

        // object actions
        CONSTRUCT_OBJECT = 20,
        DESTRUCT_OBJECT = 21,
        STREAM_FROM_OBJECT = 22,
        STREAM_TO_OBJECT = 23,
        UPDATE_OBJECT = 24,
        PERSIST_OBJECT = 25,
        LOAD_STORED_OBJECT = 26,
        UPDATE_OBJECTS = 27,
        UPDATE_CONNECTIONS = 28,
    };

    struct TraceEvent {
        uint8_t action;
        // use raw uint16_t id's, using cbox id's overwrites backup memory on construction somewhere
        uint16_t id;
        uint16_t type;
    };

    void add(uint8_t a, obj_id_t i, obj_type_t t);
    inline void add(uint8_t a)
    {
        add(a, 0, 0);
    }

    const std::array<TraceEvent, 10>& history();

    void unpause();

    void pause();
}
}