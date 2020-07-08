/*
 * Copyright 2018 BrewPi B.V.
 *
 * This file is part of BrewBlox.
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

#include "warning_screen.h"
#include "../fonts/fonts.h"
#include "Buzzer.h"
#include "SmallColorScheme.h"
#include "d4d.hpp"

char warningHeader[] = "WARNING";
char warningTxt[] = "Short detected on OneWire";
char tapTxt[] = "Tap to dismiss";

constexpr auto warningColorScheme = SmallColorScheme{
    D4D_COLOR_RGB(255, 182, 86),  ///< The object background color in standard state
    D4D_COLOR_RGB(255, 182, 86),  ///< The object background color in disabled state
    D4D_COLOR_RGB(255, 182, 86),  ///< The object background color in focused state
    D4D_COLOR_RGB(255, 182, 86),  ///< The object background color in captured state
    D4D_COLOR_RGB(0, 0, 0),       ///< The object fore color in standard state
    D4D_COLOR_RGB(128, 128, 128), ///< The object fore color in disabled state
    D4D_COLOR_RGB(0, 0, 0),       ///< The object fore color in focused state
    D4D_COLOR_RGB(0, 0, 0),       ///< The object fore color in captured state
};

D4D_DECLARE_LABEL(scrWarning_header, warningHeader, 30, 70, 260, 30, D4D_LBL_F_DEFAULT, AS_D4D_COLOR_SCHEME(&warningColorScheme), FONT_KAKWA_BIG, nullptr, nullptr);
D4D_DECLARE_LABEL(scrWarning_txt, warningTxt, 30, 100, 260, 60, D4D_LBL_F_DEFAULT, AS_D4D_COLOR_SCHEME(&warningColorScheme), FONT_KAKWA, nullptr, nullptr);
D4D_DECLARE_LABEL(scrWarning_tap, tapTxt, 30, 160, 260, 20, D4D_LBL_F_DEFAULT, AS_D4D_COLOR_SCHEME(&warningColorScheme), FONT_KAKWA, nullptr, nullptr);

D4D_DECLARE_SCREEN_BEGIN(screen_warning, ScrWarning_, 0, 0, (D4D_COOR)(D4D_SCREEN_SIZE_LONGER_SIDE), (D4D_COOR)(D4D_SCREEN_SIZE_SHORTER_SIDE), nullptr, 0, nullptr, (D4D_SCR_F_TOUCHENABLE), nullptr)
&scrWarning_header,
    &scrWarning_txt,
    &scrWarning_tap,
    D4D_DECLARE_SCREEN_END();

void
WarningScreen::activate()
{
    D4D_EnableObject(&scrWarning_tap, false);
    D4D_ActivateScreen(&screen_warning, D4D_FALSE);
}

void
ScrWarning_OnInit()
{
}

void
ScrWarning_OnMain()
{
}

void
ScrWarning_OnActivate()
{
    Buzzer.beep(2, 200);
}

void
ScrWarning_OnDeactivate()
{
}

uint8_t
ScrWarning_OnObjectMsg(D4D_MESSAGE* pMsg)
{
    if (pMsg->nMsgId == D4D_MSG_TOUCHED) {
        D4D_EscapeScreen();
    }
    return D4D_FALSE; // don't block further processing
}
