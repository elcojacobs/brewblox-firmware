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

#include "ActuatorAnalogWidget.h"
#include "BrewBlox.h"
#include "FixedPoint.h"

ActuatorAnalogWidget::ActuatorAnalogWidget(WidgetWrapper& myWrapper, const cbox::obj_id_t& id)
    : ProcessValueWidgetBase(myWrapper)
    , lookup(brewbloxBox().makeCboxPtr<ActuatorAnalogConstrained>(id))
{
    setClickHandler(this, onClickStatic);
}

void
ActuatorAnalogWidget::update(const WidgetSettings& settings)
{
    if (auto pAct = lookup.const_lock()) {
        setConnected();

        if (pAct->valueValid()) {
            setValue(to_string_dec(pAct->value(), 1));
        } else {
            setValue("");
        }
        if (pAct->settingValid()) {
            setSetting(to_string_dec(pAct->setting(), 1));

        } else {
            setSetting("");
        }

        char icons[2] = {0};
        if (auto pwmBlock = lookup.lock_as<ActuatorPwmBlock>()) {
            if (auto pwmTarget = pwmBlock->targetLookup().const_lock()) {
                switch (pwmTarget->state()) {
                case ActuatorPwm::State::Inactive:
                    icons[0] = 0x26;
                    break;
                case ActuatorPwm::State::Active:
                    icons[0] = 0x27;
                    break;
                }
            }
        }

        setIcons(icons);
        return;
    }
    setDisconnected();
}
