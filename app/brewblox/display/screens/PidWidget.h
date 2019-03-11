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

#pragma once
#include "WidgetBase.h"
#include "blox/PidBlock.h"
#include "cbox/CboxPtr.h"
#include "d4d.hpp"

class PidWidget : public WidgetBase {
private:
    char inputTarget_buf[12] = {0};
    char inputValue_buf[12] = {0};
    char outputTarget_buf[12] = {0};
    char outputValue_buf[12] = {0};
    char icons_buf[6] = {0};

    const D4D_OBJECT* labelRelations[2];

    D4D_STR_PROPERTIES inputTarget_strPrties = {D4D_LBL_FNT_PRTY_DEFAULT, (D4D_ALIGN_H_CENTER_MASK | D4D_ALIGN_V_CENTER_MASK)};
    D4D_STR_PROPERTIES inputValue_strPrties = {D4D_LBL_FNT_PRTY_DEFAULT, (D4D_ALIGN_H_CENTER_MASK | D4D_ALIGN_V_CENTER_MASK)};
    D4D_STR_PROPERTIES outputTarget_strPrties = {D4D_LBL_FNT_PRTY_DEFAULT, (D4D_ALIGN_H_CENTER_MASK | D4D_ALIGN_V_CENTER_MASK)};
    D4D_STR_PROPERTIES outputValue_strPrties = {D4D_LBL_FNT_PRTY_DEFAULT, (D4D_ALIGN_H_CENTER_MASK | D4D_ALIGN_V_CENTER_MASK)};
    D4D_STR_PROPERTIES icons_strPrties = {D4D_LBL_FNT_PRTY_DEFAULT, (D4D_ALIGN_H_CENTER_MASK | D4D_ALIGN_V_CENTER_MASK)};

    D4D_LABEL inputTargetLabel = {{inputTarget_buf, D4D_TEXT_LEN(inputTarget_buf), FONT_NUMBER_MEDIUM, &inputTarget_strPrties, 12, 0}};
    D4D_LABEL inputValueLabel = {{inputValue_buf, D4D_TEXT_LEN(inputValue_buf), FONT_NUMBER_MEDIUM, &inputValue_strPrties, 12, 0}};
    D4D_LABEL outputTargetLabel = {{outputTarget_buf, D4D_TEXT_LEN(outputTarget_buf), FONT_NUMBER_MEDIUM, &outputTarget_strPrties, 12, 0}};
    D4D_LABEL outputValueLabel = {{outputValue_buf, D4D_TEXT_LEN(outputValue_buf), FONT_NUMBER_MEDIUM, &outputValue_strPrties, 12, 0}};
    D4D_LABEL iconsLabel = {{icons_buf, D4D_TEXT_LEN(icons_buf), FONT_ICON, &icons_strPrties, 6, 0}};

    D4D_OBJECT_DATA inputTargetData = {((D4D_OBJECT_F_VISIBLE | D4D_OBJECT_F_NOTINIT) & D4D_OBJECT_F_SYSTEM_MASK), NULL};
    D4D_OBJECT_DATA inputValueData = {((D4D_OBJECT_F_VISIBLE | D4D_OBJECT_F_NOTINIT) & D4D_OBJECT_F_SYSTEM_MASK), NULL};
    D4D_OBJECT_DATA outputTargetData = {((D4D_OBJECT_F_VISIBLE | D4D_OBJECT_F_NOTINIT) & D4D_OBJECT_F_SYSTEM_MASK), NULL};
    D4D_OBJECT_DATA outputValueData = {((D4D_OBJECT_F_VISIBLE | D4D_OBJECT_F_NOTINIT) & D4D_OBJECT_F_SYSTEM_MASK), NULL};
    D4D_OBJECT_DATA iconsData = {((D4D_OBJECT_F_VISIBLE | D4D_OBJECT_F_NOTINIT) & D4D_OBJECT_F_SYSTEM_MASK), NULL};

    D4D_OBJECT inputTarget;
    D4D_OBJECT inputValue;
    D4D_OBJECT outputTarget;
    D4D_OBJECT outputValue;
    D4D_OBJECT icons;

    cbox::CboxPtr<PidBlock> lookup;

public:
    PidWidget(WidgetWrapper& myWrapper, const cbox::obj_id_t& id);
    virtual ~PidWidget() = default;

    void
    setIcons(const char* txt)
    {
        setAndEnable(&icons, txt);
    }

    void
    setDisconnected()
    {
        setIcons("\0x20f");
        D4D_EnableObject(&inputValue, false);
        D4D_EnableObject(&inputTarget, false);
        D4D_EnableObject(&outputValue, false);
        D4D_EnableObject(&outputTarget, false);
        enableBackground(false);
    }

    void
    setConnected()
    {
        enableBackground(true);
        wrapper.colorScheme.bckgDis = wrapper.colorScheme.bckg;
    }

    virtual void update(const WidgetSettings& settings) override final;

    static void
    onClickStatic(void* thisPtr)
    {
    }
    void
    onClick()
    {
    }

private:
    void drawPidRect(const fp12_t& v, D4D_COOR yPos);
    void drawPidRects(const Pid& pid);
};