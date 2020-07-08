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

#include "WidgetsScreen.h"
#include "ActuatorAnalogWidget.h"
#include "Board.h"
#include "BrewBlox.h"
#include "PidWidget.h"
#include "SetpointSensorWidget.h"
#include "TempSensorWidget.h"
#include "blox/DisplaySettingsBlock.h"
#include "connectivity.h"
#include "memory_info.h"
#include <algorithm>
#include <array>
#include <vector>

char wifi_ip[16] = "0.0.0.0";
char wifi_icon[] = "\x22";

#define SCR_PV_X0 5
#define SCR_PV_Y0 20
#define SCR_PV_CX 100
#define SCR_PV_CX_GAP 5
#define SCR_PV_CX_OFFSET (SCR_PV_CX + SCR_PV_CX_GAP)
#define SCR_PV_CY 98
#define SCR_PV_CY_GAP 5
#define SCR_PV_CY_OFFSET (SCR_PV_CY + SCR_PV_CY_GAP)

char icon_str[2] = "\x21";
char usb_str[4] = "USB";
D4D_DECLARE_LABEL(scrWidgets_usb_icon, icon_str, 0, 0, 20, 20, D4D_LBL_F_DEFAULT, AS_D4D_COLOR_SCHEME(&TOP_BAR_SCHEME), FONT_ICON, nullptr, nullptr);
D4D_DECLARE_LABEL(scrWidgets_usb_text, usb_str, 18, 0, 20, 20, D4D_LBL_F_DEFAULT, AS_D4D_COLOR_SCHEME(&TOP_BAR_SCHEME), FONT_REGULAR, nullptr, nullptr);

D4D_DECLARE_LABEL(scrWidgets_wifi_icon, wifi_icon, 40, 0, 20, 20, D4D_LBL_F_DEFAULT, AS_D4D_COLOR_SCHEME(&TOP_BAR_SCHEME), FONT_ICON, nullptr, nullptr);

#undef D4D_LBL_TXT_PRTY_DEFAULT
#define D4D_LBL_TXT_PRTY_DEFAULT (D4D_TXT_PRTY_ALIGN_H_LEFT_MASK | D4D_TXT_PRTY_ALIGN_V_CENTER_MASK)
D4D_DECLARE_LABEL(scrWidgets_wifi_ip, wifi_ip, 58, 0, 15 * 6, 20, D4D_LBL_F_DEFAULT, AS_D4D_COLOR_SCHEME(&TOP_BAR_SCHEME), FONT_REGULAR, nullptr, nullptr);

#undef D4D_LBL_TXT_PRTY_DEFAULT
#define D4D_LBL_TXT_PRTY_DEFAULT (D4D_TXT_PRTY_ALIGN_H_CENTER_MASK | D4D_TXT_PRTY_ALIGN_V_CENTER_MASK)
char screen_title[40] = "Edit this screen in the web interface";
D4D_DECLARE_LABEL(scrWidgets_title, screen_title, 40, 220, 240, 20, D4D_LBL_F_DEFAULT, AS_D4D_COLOR_SCHEME(&TOP_BAR_SCHEME), FONT_REGULAR, nullptr, nullptr);

std::array<WidgetWrapper, 6> widgetWrappers = {
    WidgetWrapper(0),
    WidgetWrapper(1),
    WidgetWrapper(2),
    WidgetWrapper(3),
    WidgetWrapper(4),
    WidgetWrapper(5),
};

D4D_DECLARE_SCREEN_BEGIN(widgets_screen, scrWidgets_, 0, 0, (D4D_COOR)(D4D_SCREEN_SIZE_LONGER_SIDE), (D4D_COOR)(D4D_SCREEN_SIZE_SHORTER_SIDE), nullptr, 0, nullptr, (D4D_SCR_F_DEFAULT | D4D_SCR_F_TOUCHENABLE), nullptr)
&scrWidgets_usb_icon,
    &scrWidgets_usb_text,
    &scrWidgets_wifi_icon,
    &scrWidgets_wifi_ip,
    &scr_mem_icon,
    &scr_mem_text,
    &scrWidgets_title,
    widgetWrappers[0].pObj(),
    widgetWrappers[1].pObj(),
    widgetWrappers[2].pObj(),
    widgetWrappers[3].pObj(),
    widgetWrappers[4].pObj(),
    widgetWrappers[5].pObj(),
    D4D_DECLARE_SCREEN_END();

std::vector<std::unique_ptr<WidgetBase>> WidgetsScreen::widgets;
WidgetSettings WidgetsScreen::widgetSettings = {
    TempUnit::Celsius,
};

void
WidgetsScreen::loadSettings()
{
    auto& settings = DisplaySettingsBlock::settings();
    if (settings.name[0] != 0) {
        D4D_SetText(&scrWidgets_title, settings.name);
    }
    widgetSettings.tempUnit = TempUnit(settings.tempUnit);

    widgets.clear();
    pb_size_t numWidgets = std::min(settings.widgets_count, pb_size_t(sizeof(settings.widgets) / sizeof(settings.widgets[0])));
    widgets.resize(numWidgets);
    for (pb_size_t i = 0; i < numWidgets; ++i) {
        blox_DisplaySettings_Widget widgetDfn = settings.widgets[i];
        auto pos = widgetDfn.pos;
        if (pos == 0 || pos > 6) {
            continue; // invalid position on screen
        }

        WidgetWrapper& wrapper = widgetWrappers[pos - 1];
        wrapper.setName(widgetDfn.name);
        wrapper.setColor(widgetDfn.color[0], widgetDfn.color[1], widgetDfn.color[2]);

        switch (widgetDfn.which_WidgetType) {
        case blox_DisplaySettings_Widget_tempSensor_tag:
            widgets.push_back(std::make_unique<TempSensorWidget>(wrapper, cbox::obj_id_t(widgetDfn.WidgetType.tempSensor)));
            break;
        case blox_DisplaySettings_Widget_setpointSensorPair_tag:
            widgets.push_back(std::make_unique<SetpointSensorWidget>(wrapper, cbox::obj_id_t(widgetDfn.WidgetType.setpointSensorPair)));
            break;
        case blox_DisplaySettings_Widget_actuatorAnalog_tag:
            widgets.push_back(std::make_unique<ActuatorAnalogWidget>(wrapper, cbox::obj_id_t(widgetDfn.WidgetType.actuatorAnalog)));
            break;
        case blox_DisplaySettings_Widget_pid_tag:
            widgets.push_back(std::make_unique<PidWidget>(wrapper, cbox::obj_id_t(widgetDfn.WidgetType.pid)));
            break;
        default:
            break;
        }
    }

    if (settings.brightness >= 20) {
        displayBrightness(settings.brightness);
    }

    D4D_InvalidateScreen(&widgets_screen, D4D_TRUE);
}

void
WidgetsScreen::activate()
{
    D4D_ActivateScreen(&widgets_screen, D4D_TRUE);
}

void
WidgetsScreen::updateUsb()
{
    bool connected = serialConnected();
    D4D_EnableObject(&scrWidgets_usb_icon, connected);
    D4D_EnableObject(&scrWidgets_usb_text, connected);
}

void
WidgetsScreen::updateWiFi()
{
    auto signal = wifiSignal();
    printWiFiIp(wifi_ip);

    bool connected = true;
    if (signal >= 0) {
        wifi_icon[0] = 0x22;
        connected = false;
    } else if (signal >= -65) {
        wifi_icon[0] = 0x25;
    } else if (signal >= -70) {
        wifi_icon[0] = 0x24;
    } else {
        wifi_icon[0] = 0x23;
    }
    if (connected != D4D_IsEnabled(const_cast<D4D_OBJECT*>(&scrWidgets_wifi_ip))) {
        D4D_InvalidateObject(&scrWidgets_wifi_ip, D4D_FALSE); // force rewriting IP to display
        D4D_EnableObject(&scrWidgets_wifi_icon, connected);
        D4D_EnableObject(&scrWidgets_wifi_ip, connected);
    }
}

void
WidgetsScreen::updateWidgets()
{
    for (auto& w : widgets) {
        if (w) {
            w->update(widgetSettings);
        }
    }
}

void
WidgetsScreen::unload()
{
    widgets.resize(0); // clear dynamically allocated memory.
}

void
scrWidgets_OnInit()
{
}

void
scrWidgets_OnMain()
{
    if (DisplaySettingsBlock::newSettingsReceived()) {
        WidgetsScreen::loadSettings();
    }
    WidgetsScreen::updateUsb();
    WidgetsScreen::updateWiFi();
    updateRamDisplay();
    WidgetsScreen::updateWidgets();
}

void
scrWidgets_OnActivate()
{
    WidgetsScreen::loadSettings();
}

void
scrWidgets_OnDeactivate()
{
    WidgetsScreen::unload();
}

uint8_t
scrWidgets_OnObjectMsg(D4D_MESSAGE* pMsg)
{
    return D4D_FALSE; // don't block further processing
}
