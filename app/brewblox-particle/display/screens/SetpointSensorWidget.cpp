/*
 * Copyright 2018 BrewPi B.V.
 *
 * This file is part of BrewPi.
 *
 * BrewPi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BrewPi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with BrewPi.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "SetpointSensorWidget.h"
#include "brewblox_particle.hpp"

SetpointSensorWidget::SetpointSensorWidget(WidgetWrapper& myWrapper, const cbox::obj_id_t& id)
    : ProcessValueWidgetBase(myWrapper)
    , lookup(brewbloxBox().makeCboxPtr<SetpointSensorPairBlock>(id))
{
    setClickHandler(this, onClickStatic);
}

void SetpointSensorWidget::update(const WidgetSettings& settings)
{
    if (auto ptr = lookup.const_lock()) {
        setConnected();
        auto& pair = ptr->get();

        char icons[3] = {0};
        if (pair.valueValid()) {
            setValue(temp_to_string(pair.value(), 1, settings.tempUnit));
            icons[0] = '\x29';
        } else {
            setValue("");
            icons[0] = '\x2B';
        }
        if (pair.settingValid()) {
            setSetting(temp_to_string(pair.setting(), 1, settings.tempUnit));
            icons[1] = '\x2A';
        } else {
            setSetting("");
        }
        setIcons(icons);
        return;
    }
    setDisconnected();
}
