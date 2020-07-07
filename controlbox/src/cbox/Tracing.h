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
#include <algorithm>
#include <array>
#include <cstdint>

namespace cbox {

class Tracing {
public:
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
        Action action = Action::NONE;
        obj_id_t id = 0;
        obj_type_t type = 0;
    };

    class trace_t {
    public:
        trace_t()
            : writeEnabled(false)
        {
            last = history.begin();
        }

    private:
        std::array<TraceEvent, 10> history;
        decltype(history)::iterator last;
        bool writeEnabled;

        void add(Action a, obj_id_t i, obj_type_t t)
        {
            if (writeEnabled) {
                if (last->action == Action::PERSIST_OBJECT && last->id == i) {
                    return; // persisting a block can take a retry if a new block needs to be allocated, don't log twice.
                }
                last++;
                if (last == history.end()) {
                    last = history.begin();
                }
                *last = TraceEvent{a, i, t};
            }
        }
        friend class Tracing;
    };

    static trace_t trace; // place this array in backup RAM that is not reset on reboot

    static void add(Action a, obj_id_t i, obj_type_t t)
    {
        trace.add(a, i, t);
    }

    static const std::array<TraceEvent, 10>& history()
    {
        // history is kept as a circular buffer,
        // rotate array so oldest element is the first element before returning
        if (trace.last != trace.history.end() - 1) {
            auto oldest = trace.last + 1;
            std::rotate(trace.history.begin(), oldest, trace.history.end());
            trace.last = trace.history.end() - 1;
        }
        return trace.history;
    }

    static void unpause()
    {
        trace.writeEnabled = true;
    }
};
}