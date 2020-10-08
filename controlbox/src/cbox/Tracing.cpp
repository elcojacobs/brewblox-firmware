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

#include "Tracing.h"
#include "ObjectIds.h"
#include <algorithm>
#include <array>
#include <cstdint>

namespace cbox {

namespace tracing {
    namespace detail {
        __attribute__((section(".retained_user"))) std::array<TraceEvent, 10> historyRetained = std::array<TraceEvent, 10>{TraceEvent{uint8_t(tracing::Action::NONE), 0, 0}};
        __attribute__((section(".retained_user"))) uint8_t lastIdx = 0;
        bool writeEnabled = false;
    }

    void add(uint8_t a, obj_id_t i, obj_type_t t)
    {
        using namespace detail;
        if (writeEnabled) {
            if (historyRetained[lastIdx].action == uint8_t(Action::PERSIST_OBJECT) && historyRetained[lastIdx].id == i) {
                return; // persisting a block can take a retry if a new block needs to be allocated, don't log twice.
            }
            lastIdx = (lastIdx < 9) ? lastIdx + 1 : 0;

            historyRetained[lastIdx] = TraceEvent{a, i, t};
        }
    }

    const std::array<TraceEvent, 10>& history()
    {
        // history is kept as a circular buffer,
        // rotate array so oldest element is the first element before returning
        using namespace detail;
        if (lastIdx != 9) {
            std::rotate(historyRetained.begin(), historyRetained.begin() + lastIdx + 1, historyRetained.end());
            lastIdx = 9;
        }
        return historyRetained;
    }

    void unpause()
    {
        detail::writeEnabled = true;
    }

    void pause()
    {
        detail::writeEnabled = false;
    }
}
}