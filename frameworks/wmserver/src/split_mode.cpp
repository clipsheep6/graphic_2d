/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "split_mode.h"

#include <map>

#include <ianimation_service.h>
#include <window_manager_type.h>
#include <wms-server-protocol.h>

#include "wmserver.h"
#include "layout_controller.h"

#include <trace.h>
DEFINE_LOG_LABEL("SplitMode");

using namespace OHOS;

int32_t g_lineX = 0;
int32_t g_lineY = 0;

void GetAdjacentModeShowArea(int32_t &x, int32_t &y, int32_t &width, int32_t &height)
{
    struct layout layout = {};
    LayoutControllerCalcWindowDefaultLayout(WINDOW_TYPE_NORMAL, WINDOW_MODE_UNSET, NULL, &layout);
    x = layout.x;
    y = layout.y;
    width = layout.w;
    height = layout.h;
}

void ChangeAdjacentMode(struct WindowSurface *ws, enum AdjacentModeStatus status)
{
    wms_send_adjacent_mode_change(ws->controller->pWlResource, status);
    LOG_INFO("%d", status);
}

void ChangeWindowSize(struct WindowSurface *ws, int32_t w, int32_t h)
{
    wms_send_window_size_change(ws->controller->pWlResource, w, h);
}

struct WindowSurface *GetWindow(std::function<bool(struct WindowSurface *ws)> condition)
{
    struct WmsContext *ctx = GetWmsInstance();
    struct WindowSurface *ws = NULL;
    wl_list_for_each(ws, &ctx->wlListWindow, link) {
        if (condition(ws)) {
            return ws;
        }
    }
    return nullptr;
}

void ForeachWindow(std::function<bool(struct WindowSurface *ws)> condition,
    std::function<void(struct WindowSurface *ws)> action)
{
    struct WmsContext *ctx = GetWmsInstance();
    struct WindowSurface *ws = NULL;
    wl_list_for_each(ws, &ctx->wlListWindow, link) {
        if (condition(ws)) {
            action(ws);
        }
    }
}

struct WindowSurface *GetSplitedWindow()
{
    auto condition = [](struct WindowSurface *ws) {
        return ws->isSplited;
    };
    return GetWindow(condition);
}

void GetSplitedWindows(struct WindowSurface *&top, struct WindowSurface *&bottom)
{
    top = GetSplitedWindow();
    auto condition = [top](struct WindowSurface *ws) {
        return ws->isSplited && ws != top;
    };
    bottom = GetWindow(condition);

    if (top->y > bottom->y) {
        auto tmp = top;
        top = bottom;
        bottom = tmp;
    }
}

bool SetAdjacentModeNullProp()
{
    auto condition = [](struct WindowSurface *ws) {
        return ws->isSplited;
    };
    auto action = [](struct WindowSurface *ws) {
        ws->isSplited = false;
        int32_t defX = 0, defY = 0, defWidth = 0, defHeight = 0;
        GetAdjacentModeShowArea(defX, defY, defWidth, defHeight);
        SetWindowPosition(ws, defX, defY);
        wms_send_window_size_change(ws->controller->pWlResource, defWidth, defHeight);
        ChangeAdjacentMode(ws, ADJACENT_MODE_STATUS_RETAIN);
        return false;
    };
    ForeachWindow(condition, action);
    return true;
}

bool SetAdjacentModeUnenableProp()
{
    auto condition = [](struct WindowSurface *ws) {
        return ws->type == WINDOW_TYPE_NORMAL;
    };
    auto ws = GetWindow(condition);
    int32_t defX = 0, defY = 0, defWidth = 0, defHeight = 0;
    GetAdjacentModeShowArea(defX, defY, defWidth, defHeight);

    SetWindowPosition(ws, defX, defY);
    wms_send_window_size_change(ws->controller->pWlResource, defWidth, defHeight);
    ChangeWindowSize(ws, defWidth, defHeight);
    ws->isSplited = true;
    ChangeAdjacentMode(ws, ADJACENT_MODE_STATUS_VAGUE);
    return true;
}

bool SetAdjacentModeSingleProp()
{
    LOG_SCOPE();
    struct WindowSurface *ws = nullptr;
    struct WmsContext *ctx = GetWmsInstance();
    if (ctx->adjacentMode == ADJ_MODE_SINGLE) {
        return true;
    } else if (ctx->adjacentMode == ADJ_MODE_NULL) {
        if (IAnimationService::Get()) {
            IAnimationService::Get()->SplitModeCreateBackground();
        }
        auto condition = [](struct WindowSurface *ws) {
            return ws->type == WINDOW_TYPE_NORMAL;
        };
        ws = GetWindow(condition);
    } else if (ctx->adjacentMode == ADJ_MODE_SELECT) {
        ws = GetSplitedWindow();
    } else {
        LOG_ERROR("cannot from %d", ctx->adjacentMode);
        return false;
    }
    LOG_INFO("operation from %d", ctx->adjacentMode);
    LOG_INFO("operation to %d", ws->surfaceId);

    int32_t defX = 0, defY = 0, defWidth = 0, defHeight = 0;
    GetAdjacentModeShowArea(defX, defY, defWidth, defHeight);

    SetWindowPosition(ws, defX, defY + defHeight * 0.07);
    wms_send_window_size_change(ws->controller->pWlResource, defWidth, defHeight * 0.86);
    ws->isSplited = true;
    ChangeAdjacentMode(ws, ADJACENT_MODE_STATUS_VAGUE);
    return true;
}

bool SetAdjacentModeSelectProp()
{
    LOG_SCOPE();
    struct WmsContext *ctx = GetWmsInstance();
    if (ctx->adjacentMode != ADJ_MODE_SINGLE && ctx->adjacentMode != ADJ_MODE_SELECT) {
        LOG_ERROR("cannot from %d", ctx->adjacentMode);
        return false;
    }

    auto ws = GetSplitedWindow();
    int32_t defX = 0, defY = 0, defWidth = 0, defHeight = 0;
    GetAdjacentModeShowArea(defX, defY, defWidth, defHeight);

    weston_log("Select g_lineY: %{public}d %{public}p", g_lineY, ws);
    if (g_lineY > defY + defHeight * 0.5) {
        weston_log("Select bottom %{public}d", defY);
        SetWindowPosition(ws, defX, defY);
        wms_send_window_size_change(ws->controller->pWlResource, defWidth, defHeight * 0.48);
    } else {
        weston_log("Select top %{public}f", defY + defHeight * 0.52);
        SetWindowPosition(ws, defX, defY + defHeight * 0.52);
        wms_send_window_size_change(ws->controller->pWlResource, defWidth, defHeight * 0.48);
    }

    ctx->pLayoutInterface->commit_changes();
    ChangeAdjacentMode(ws, ADJACENT_MODE_STATUS_VAGUE);
    return true;
}

bool SetAdjacentModeConfirmProp()
{
    if (IAnimationService::Get()) {
        IAnimationService::Get()->SplitModeCreateMiddleLine();
    }
    auto condition = [](struct WindowSurface *ws) {
        return ws->type == WINDOW_TYPE_NORMAL;
    };
    auto win1 = GetWindow(condition);
    auto win2 = GetSplitedWindow();
    int32_t defX = 0, defY = 0, defWidth = 0, defHeight = 0;
    GetAdjacentModeShowArea(defX, defY, defWidth, defHeight);

    int x = 0, y = 0, w = 0, h = 0;
    if (win2->y > defY + defHeight * 0.5) {
        x = defX;
        y = defY;
    } else {
        x = defX;
        y = defY + defHeight * (1 + 0.04) * 0.5;
    }

    win1->isSplited = true;

    w = win2->width;
    h = win2->height;

    SetWindowPosition(win1, x, y);
    wms_send_window_size_change(win1->controller->pWlResource, w, h);

    ChangeAdjacentMode(win1, ADJACENT_MODE_STATUS_CLEAR);
    ChangeAdjacentMode(win2, ADJACENT_MODE_STATUS_CLEAR);
    return true;
}

bool SetAdjacentModeMidTouchDown()
{
    auto condition = [](struct WindowSurface *ws) {
        return ws->isSplited;
    };
    auto action = [](struct WindowSurface *ws) {
        ChangeAdjacentMode(ws, ADJACENT_MODE_STATUS_VAGUE);
    };
    ForeachWindow(condition, action);
    return true;
}

bool SetAdjacentModeMidTouchMove()
{
    struct WindowSurface *topWindow;
    struct WindowSurface *bottomWindow;
    GetSplitedWindows(topWindow, bottomWindow);

    int32_t defX = 0, defY = 0, defWidth = 0, defHeight = 0;
    GetAdjacentModeShowArea(defX, defY, defWidth, defHeight);

    wms_send_window_size_change(topWindow->controller->pWlResource, defWidth, g_lineY - defY);
    ChangeAdjacentMode(topWindow, ADJACENT_MODE_STATUS_VAGUE);

    SetWindowPosition(bottomWindow, defX, g_lineY + defHeight * 0.04);
    wms_send_window_size_change(bottomWindow->controller->pWlResource, defWidth,
                                defHeight - (g_lineY - defY)  - defHeight * 0.04);
    ChangeAdjacentMode(bottomWindow, ADJACENT_MODE_STATUS_VAGUE);
    return true;
}

bool SetAdjacentModeMidTouchUp()
{
    struct WindowSurface *topWindow;
    struct WindowSurface *bottomWindow;
    GetSplitedWindows(topWindow, bottomWindow);

    int32_t x = 0, y = 0, w = 0, h = 0;
    GetAdjacentModeShowArea(x, y, w, h);

    int32_t diff = g_lineY - y;
    if (0.2 * h <= diff && diff <= h * 0.8) {
        ChangeAdjacentMode(topWindow, ADJACENT_MODE_STATUS_VAGUE);
        ChangeAdjacentMode(bottomWindow, ADJACENT_MODE_STATUS_VAGUE);
        return true;
    }

    auto longWindow = topWindow;
    auto shortWindow = bottomWindow;
    if (bottomWindow->height > topWindow->height) {
        longWindow = bottomWindow;
        shortWindow = topWindow;
    }
    ChangeAdjacentMode(shortWindow, ADJACENT_MODE_STATUS_DESTROY);

    longWindow->isSplited = false;
    SetWindowPosition(longWindow, x, y);
    wms_send_window_size_change(longWindow->controller->pWlResource, w, h);
    ChangeAdjacentMode(longWindow, ADJACENT_MODE_STATUS_RETAIN);

    struct WmsContext *ctx = GetWmsInstance();
    ctx->adjacentMode = ADJ_MODE_NULL;
    ctx->pLayoutInterface->surface_change_top(longWindow->layoutSurface);
    ctx->pLayoutInterface->commit_changes();
    return true;
}

std::map<uint32_t, bool(*)()> g_calls = {
    {ADJ_MODE_NULL, SetAdjacentModeNullProp},
    {ADJ_MODE_UNENABLE, SetAdjacentModeUnenableProp},
    {ADJ_MODE_SINGLE, SetAdjacentModeSingleProp},
    {ADJ_MODE_SELECT, SetAdjacentModeSelectProp},
    {ADJ_MODE_CONFIRM, SetAdjacentModeConfirmProp},
    {ADJ_MODE_DIVIDER_TOUCH_DOWN, SetAdjacentModeMidTouchDown},
    {ADJ_MODE_DIVIDER_TOUCH_MOVE, SetAdjacentModeMidTouchMove},
    {ADJ_MODE_DIVIDER_TOUCH_UP, SetAdjacentModeMidTouchUp},
};

void ControllerSetAdjacentMode(struct wl_client *client,
                               struct wl_resource *resource,
                               uint32_t type, int32_t x, int32_t y)
{
    LOG_SCOPE();
    LOG_INFO("type: %d", type);
    IAnimationService::Init();

    if (g_calls.find(type) == g_calls.end()) {
        wms_send_reply_error(resource, WMS_ERROR_INVALID_PARAM);
        return;
    }

    if (type == ADJ_MODE_SELECT || type == ADJ_MODE_DIVIDER_TOUCH_MOVE) {
        g_lineX = x;
        g_lineY = y;
    }

    if (g_calls[type]()) {
        struct WmsContext *ctx = GetWmsInstance();
        ctx->adjacentMode = type;
        wms_send_reply_error(resource, WMS_ERROR_OK);
    } else {
        wms_send_reply_error(resource, WMS_ERROR_INVALID_PARAM);
    }
    return;
}
