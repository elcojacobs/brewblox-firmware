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
    };

    struct TraceEvent {
        Action action = Action::NONE;
        obj_id_t id = 0;
        obj_type_t type = 0;
    };
    static void unpause()
    {
        trace.writeEnabled = true;
    }

    class trace_t {
    public:
        trace_t()
            : idx(0)
            , writeEnabled(false)
        {
        }

        std::array<TraceEvent, 10> history;
        uint8_t idx;
        bool writeEnabled;

        void add(Action a, obj_id_t i, obj_type_t t)
        {
            if (writeEnabled) {
                history[idx] = TraceEvent{a, i, t};
                idx = idx + 1;
                if (idx >= history.size()) {
                    idx = 0;
                }
            }
        }
        friend class Tracing;
    };

    static trace_t trace; // place this array in backup RAM that is not reset on reboot

    static void add(Action a, obj_id_t i, obj_type_t t)
    {
        trace.add(a, i, t);
    }
};
}