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

#include <ianimation_service.h>
#include <window_manager_type.h>
#include <wms-server-protocol.h>

#include "wmserver.h"
#include "layout_controller.h"

#define ADJ_MODE_HALF_PERCENT 0.5
#define ADJ_MODE_SINGLE_PERCENT 0.07
#define ADJ_MODE_MIDLINE_WIDTH_PERCENT 0.2
#define ADJ_MODE_MIDLINE_HEIGHT_PERCENT 0.04
#define ADJ_MODE_MIDLINE_EXIT_PERCENT 0.2
#define ADJ_MODE_WINDOW_COUNT_MAX 2

#define LOG_LABEL "wms-controller"

#define LOGD(fmt, ...) weston_log("%{public}s debug %{public}d:%{public}s " fmt "\n", \
    LOG_LABEL, __LINE__, __func__, ##__VA_ARGS__)

namespace OHOS {
int32_t IAnimationServiceInit()
{
    return static_cast<int32_t>(IAnimationService::Init());
}

int32_t IAnimationServiceSplitModeCreateBackground()
{
    if (IAnimationService::Get() == nullptr) {
        return 1;
    }
    return static_cast<int32_t>(IAnimationService::Get()->SplitModeCreateBackground());
}

int32_t IAnimationServiceSplitModeCreateMiddleLine()
{
    if (IAnimationService::Get() == nullptr) {
        return 1;
    }
    return static_cast<int32_t>(IAnimationService::Get()->SplitModeCreateMiddleLine());
}

void GetAdjacentModeShowArea(struct WmsContext *ctx,
                             int32_t *x, int32_t *y, int32_t *width, int32_t *height)
{
    struct layout layout = {};
    LayoutControllerCalcWindowDefaultLayout(WINDOW_TYPE_NORMAL, WINDOW_MODE_UNSET, NULL, &layout);
    *x = layout.x;
    *y = layout.y;
    *width = layout.w;
    *height = layout.h;
}

static void SetAdjacentModeNullProp(struct wl_resource *resource)
{
    LOGD("start.");
    struct WmsContext *ctx = GetWmsInstance();
    struct WindowSurface *windowSurface = NULL;
    int32_t defX = 0, defY = 0, defWidth = 0, defHeight = 0;
    GetAdjacentModeShowArea(ctx, &defX, &defY, &defWidth, &defHeight);
    wl_list_for_each(windowSurface, &ctx->wlListWindow, link) {
        if (windowSurface->adjMode == WMS_ADJACENT_MODE_MEMBER || 
            windowSurface->adjMode == WMS_ADJACENT_MODE_BACK || 
            windowSurface->adjMode == WMS_ADJACENT_MODE_LINE)
        {

            windowSurface->adjMode = WMS_ADJACENT_MODE_UNSET;
            wms_send_adjacent_mode_change(resource,
                                          windowSurface->surfaceId,
                                          windowSurface->x,
                                          windowSurface->y,
                                          windowSurface->width,
                                          windowSurface->height,
                                          ADJACENT_MODE_STATUS_DESTROY);
        }
    }
    LOGD("end.");
}


static void SetAdjacentModeUnenabelProp(struct wl_resource *resource)
{
    LOGD("start.");
    struct WmsContext *ctx = GetWmsInstance();
    struct WindowSurface *windowSurface = NULL;
    int32_t defX = 0, defY = 0, defWidth = 0, defHeight = 0;
    GetAdjacentModeShowArea(ctx, &defX, &defY, &defWidth, &defHeight);
    wl_list_for_each(windowSurface, &ctx->wlListWindow, link) {
        if (windowSurface->type == WINDOW_TYPE_NORMAL)
        {
            SetWindowPosition(windowSurface, defX, defY);
            SetWindowSize(windowSurface, defWidth, defHeight);
            windowSurface->adjMode = WMS_ADJACENT_MODE_MEMBER;

            wms_send_adjacent_mode_change(resource,
                                          windowSurface->surfaceId,
                                          windowSurface->x,
                                          windowSurface->y,
                                          windowSurface->width,
                                          windowSurface->height,
                                          ADJACENT_MODE_STATUS_VAGUE);
            break;
        }
    }
    LOGD("end.");
    return;
}

static void SetAdjacentModeSingleProp(struct wl_resource *resource)
{
    LOGD("start.");
    struct WmsContext *ctx = GetWmsInstance();
    struct WindowSurface *windowSurface = NULL;
    int32_t defX = 0, defY = 0, defWidth = 0, defHeight = 0;
    GetAdjacentModeShowArea(ctx, &defX, &defY, &defWidth, &defHeight);
    wl_list_for_each(windowSurface, &ctx->wlListWindow, link) {
        if (windowSurface->type == WINDOW_TYPE_NORMAL) {
            SetWindowPosition(windowSurface, defX, defY + defHeight * ADJ_MODE_SINGLE_PERCENT);
            wms_send_window_size_change(windowSurface->controller->pWlResource,
                defWidth, defHeight / 2);
            windowSurface->adjMode = WMS_ADJACENT_MODE_MEMBER;

            wms_send_adjacent_mode_change(windowSurface->controller->pWlResource,
                                          windowSurface->surfaceId,
                                          windowSurface->x,
                                          windowSurface->y,
                                          windowSurface->width,
                                          windowSurface->height,
                                          ADJACENT_MODE_STATUS_VAGUE);
            break;
        }
    }

    LOGD("end.");
    return;
}

static void SetAdjacentModeSelectProp(struct wl_resource *resource, int32_t x, int32_t y)
{
    LOGD("start.");
    struct WmsContext *ctx = GetWmsInstance();
    struct WindowSurface *windowSurface = NULL;
    int32_t defX = 0, defY = 0, defWidth = 0, defHeight = 0;
    GetAdjacentModeShowArea(ctx, &defX, &defY, &defWidth, &defHeight);

    wl_list_for_each(windowSurface, &ctx->wlListWindow, link) {
        if (windowSurface->adjMode == WMS_ADJACENT_MODE_MEMBER)
        {
            break;
        }
    }

    if(y > defY + defHeight * ADJ_MODE_HALF_PERCENT)
    {
        SetWindowPosition(windowSurface, defX, defY);
        SetWindowSize(windowSurface, defWidth, 
            defHeight * (1 - ADJ_MODE_MIDLINE_HEIGHT_PERCENT) * ADJ_MODE_HALF_PERCENT);
    }
    else
    {
        SetWindowPosition(windowSurface, defX, 
            defY + defHeight * (1 + ADJ_MODE_MIDLINE_HEIGHT_PERCENT) * ADJ_MODE_HALF_PERCENT);
        SetWindowSize(windowSurface, defWidth,
            defHeight * (1 - ADJ_MODE_MIDLINE_HEIGHT_PERCENT) * ADJ_MODE_HALF_PERCENT);
    }

    wms_send_adjacent_mode_change(resource,
                                  windowSurface->surfaceId,
                                  windowSurface->x,
                                  windowSurface->y,
                                  windowSurface->width,
                                  windowSurface->height,
                                  ADJACENT_MODE_STATUS_VAGUE);

    LOGD("end.");
    return;
}

static void SetAdjacentModeConfirmProp(struct wl_resource *resource)
{
    LOGD("start.");
    struct WmsContext *ctx = GetWmsInstance();
    struct WindowSurface *winOther = NULL;
    struct WindowSurface *pWindowSurface = NULL;
    int32_t defX = 0, defY = 0, defWidth = 0, defHeight = 0;
    GetAdjacentModeShowArea(ctx, &defX, &defY, &defWidth, &defHeight);

    wl_list_for_each(winOther, &ctx->wlListWindow, link) {
        if (winOther->type == WINDOW_TYPE_NORMAL)
        {
            break;
        }
    }

    wl_list_for_each(pWindowSurface, &ctx->wlListWindow, link) {
        if (pWindowSurface->adjMode == WMS_ADJACENT_MODE_MEMBER)
        {
            int x = 0, y = 0, w = 0, h = 0;
            if(pWindowSurface->y > defY + defHeight * ADJ_MODE_HALF_PERCENT)
            {
                x = defX;
                y = defY;
            }
            else
            {
                x = defX;
                y = defY + defHeight * (1 + ADJ_MODE_MIDLINE_HEIGHT_PERCENT) * ADJ_MODE_HALF_PERCENT;
            }


            winOther->adjMode = WMS_ADJACENT_MODE_MEMBER;

            w = pWindowSurface->width;
            h = pWindowSurface->height;

            SetWindowPosition(winOther, x, y);
            SetWindowSize(winOther, w, h);
            
            wms_send_adjacent_mode_change(resource,
                                        winOther->surfaceId,
                                        winOther->x,
                                        winOther->y,
                                        winOther->width,
                                        winOther->height,
                                        ADJACENT_MODE_STATUS_VAGUE);
            break;
        }
    }

    return;
}

static void SetAdjacentModeFinalProp(struct wl_resource *resource)
{
    LOGD("start.");
    struct WmsContext *ctx = GetWmsInstance();
    struct WindowSurface *windowSurface = NULL;
    int32_t defX = 0, defY = 0, defWidth = 0, defHeight = 0;
    GetAdjacentModeShowArea(ctx, &defX, &defY, &defWidth, &defHeight);

    wl_list_for_each(windowSurface, &ctx->wlListWindow, link) {
        if (windowSurface->adjMode == WMS_ADJACENT_MODE_MEMBER ||
            windowSurface->adjMode == WMS_ADJACENT_MODE_BACK ||
            windowSurface->adjMode == WMS_ADJACENT_MODE_LINE)
        {
                wms_send_adjacent_mode_change(resource,
                                  windowSurface->surfaceId,
                                  windowSurface->x,
                                  windowSurface->y,
                                  windowSurface->width,
                                  windowSurface->height,
                                  ADJACENT_MODE_STATUS_CLEAR);
        }
    }

    return;
}

static void SetAdjacentModeMidTouchDown(struct wl_resource *resource)
{
    LOGD("start.");
    struct WmsContext *ctx = GetWmsInstance();
    struct WindowSurface *windowSurface = NULL;
    int32_t defX = 0, defY = 0, defWidth = 0, defHeight = 0;
    GetAdjacentModeShowArea(ctx, &defX, &defY, &defWidth, &defHeight);
    wl_list_for_each(windowSurface, &ctx->wlListWindow, link) {
        if (windowSurface->adjMode == WMS_ADJACENT_MODE_MEMBER ||
            windowSurface->adjMode == WMS_ADJACENT_MODE_BACK ||
            windowSurface->adjMode == WMS_ADJACENT_MODE_LINE)
        {
                wms_send_adjacent_mode_change(resource,
                                  windowSurface->surfaceId,
                                  windowSurface->x,
                                  windowSurface->y,
                                  windowSurface->width,
                                  windowSurface->height,
                                  ADJACENT_MODE_STATUS_VAGUE);
        }
    }

    LOGD("end.");
    return;
}

static void SetAdjacentModeMidTouchMove(struct wl_resource *resource, int32_t x, int32_t y)
{
    LOGD("start.");
    struct WmsContext *ctx = GetWmsInstance();
    struct WindowSurface *windowDefault = NULL;
    struct WindowSurface *windowOther = NULL;
    struct WindowSurface *windowLineSurface = NULL;
    int32_t defX = 0, defY = 0, defWidth = 0, defHeight = 0;

    GetAdjacentModeShowArea(ctx, &defX, &defY, &defWidth, &defHeight);

    wl_list_for_each(windowLineSurface, &ctx->wlListWindow, link) {
        if (windowLineSurface->adjMode == WMS_ADJACENT_MODE_LINE)
        {
            break;
        }
    }

    wl_list_for_each(windowDefault, &ctx->wlListWindow, link) {
        if (windowDefault->adjMode == WMS_ADJACENT_MODE_MEMBER)
        {
            if(windowDefault->y < windowLineSurface->y)
            {
                SetWindowSize(windowDefault, defWidth, y - defY);

                windowDefault->x = defX;
                windowDefault->y = defY;
                windowDefault->width = defWidth;
                windowDefault->height = y - defY;

                wms_send_adjacent_mode_change(resource,
                                            windowDefault->surfaceId,
                                            windowDefault->x,
                                            windowDefault->y,
                                            windowDefault->width,
                                            windowDefault->height,
                                            ADJACENT_MODE_STATUS_VAGUE);
                break;
            }
        }
    }

    wl_list_for_each(windowOther, &ctx->wlListWindow, link) {
        if (windowOther->adjMode == WMS_ADJACENT_MODE_MEMBER)
        {
            if(windowOther->y >= windowLineSurface->y)
            {
                SetWindowPosition(windowOther, defX, y + defHeight * ADJ_MODE_MIDLINE_HEIGHT_PERCENT);
                SetWindowSize(windowOther, defWidth,
                    defHeight - (y - defY)  - defHeight * ADJ_MODE_MIDLINE_HEIGHT_PERCENT);

                windowOther->x = defX;
                windowOther->y = y + defHeight * ADJ_MODE_MIDLINE_HEIGHT_PERCENT;
                windowOther->width = defWidth;
                windowOther->height = defHeight - (y - defY)  - defHeight * ADJ_MODE_MIDLINE_HEIGHT_PERCENT;

                wms_send_adjacent_mode_change(resource,
                                            windowOther->surfaceId,
                                            windowOther->x,
                                            windowOther->y,
                                            windowOther->width,
                                            windowOther->height,
                                            ADJACENT_MODE_STATUS_VAGUE);
                break;
            }
        }
    }

    SetWindowPosition(windowLineSurface, windowLineSurface->x, y);

    LOGD("end.");
    return;
}

static void SetAdjacentModeMidTouchUp(struct wl_resource *resource)
{
    LOGD("start.");
    struct WmsContext *ctx = GetWmsInstance();

    struct WindowSurface *winBak = NULL;
    struct WindowSurface *winLin = NULL;
    struct WindowSurface *winDef = NULL;
    struct WindowSurface *winOth = NULL;
    int32_t defX = 0, defY = 0, defWidth = 0, defHeight = 0;
    bool bExit = false;
    GetAdjacentModeShowArea(ctx, &defX, &defY, &defWidth, &defHeight);

    wl_list_for_each(winLin, &ctx->wlListWindow, link) {
        if (winLin->adjMode == WMS_ADJACENT_MODE_LINE)
        {
            if(winLin->y - defY > defHeight * (1 - ADJ_MODE_MIDLINE_EXIT_PERCENT) || 
               winLin->y - defY < defHeight * ADJ_MODE_MIDLINE_EXIT_PERCENT)
            {
                bExit = true;
            }
            break;
        }
    }

    wl_list_for_each(winBak, &ctx->wlListWindow, link) {
        if (winBak->adjMode == WMS_ADJACENT_MODE_BACK)
        {
            break;
        }
    }

    wl_list_for_each(winDef, &ctx->wlListWindow, link) {
        if (winDef->adjMode == WMS_ADJACENT_MODE_MEMBER)
        {
            if(winDef->height + 1 >= defHeight * ADJ_MODE_HALF_PERCENT)
            {
                break;
            }
            
        }
    }

    wl_list_for_each(winOth, &ctx->wlListWindow, link) {
        if (winOth->adjMode == WMS_ADJACENT_MODE_MEMBER)
        {
            if(winOth->height < defHeight * ADJ_MODE_HALF_PERCENT)
            {
                break;
            }
        }
    }

    if(!bExit)
    {

        wms_send_adjacent_mode_change(resource,
                            winBak->surfaceId,
                            winBak->x,
                            winBak->y,
                            winBak->width,
                            winBak->height,
                            ADJACENT_MODE_STATUS_VAGUE);
        wms_send_adjacent_mode_change(resource,
                            winOth->surfaceId,
                            winOth->x,
                            winOth->y,
                            winOth->width,
                            winOth->height,
                            ADJACENT_MODE_STATUS_VAGUE);
    }
    else
    {
        wms_send_adjacent_mode_change(resource,
                            winBak->surfaceId,
                            winBak->x,
                            winBak->y,
                            winBak->width,
                            winBak->height,
                            ADJACENT_MODE_STATUS_DESTROY);
        wms_send_adjacent_mode_change(resource,
                            winLin->surfaceId,
                            winLin->x,
                            winLin->y,
                            winLin->width,
                            winLin->height,
                            ADJACENT_MODE_STATUS_DESTROY);
        wms_send_adjacent_mode_change(resource,
                            winOth->surfaceId,
                            winOth->x,
                            winOth->y,
                            winOth->width,
                            winOth->height,
                            ADJACENT_MODE_STATUS_DESTROY);

        ctx->adjacentMode = ADJ_MODE_NULL;
        winDef->adjMode = WMS_ADJACENT_MODE_UNSET;
        SetWindowPosition(winDef, defX, defY);
        SetWindowSize(winDef, defWidth, defHeight);
        wms_send_adjacent_mode_change(resource,
                            winDef->surfaceId,
                            winDef->x,
                            winDef->y,
                            winDef->width,
                            winDef->height,
                            ADJACENT_MODE_STATUS_RETAIN);
        ctx->pLayoutInterface->surface_change_top(winDef->layoutSurface);
    }

    LOGD("end.");
    return;
}

void ControllerSetAdjacentMode(struct wl_client *client,
                                      struct wl_resource *resource,
                                      uint32_t type, int32_t x, int32_t y)
{
    LOGD("start.type = %{public}d", type);
    IAnimationServiceInit();
    struct WmsContext *ctx = GetWmsInstance();

    switch (type)
    {
        case ADJ_MODE_NULL:
            ctx->adjacentMode = ADJ_MODE_NULL;
            SetAdjacentModeNullProp(resource);
            break;
        case ADJ_MODE_UNENABEL:
            ctx->adjacentMode = ADJ_MODE_UNENABEL;
            SetAdjacentModeUnenabelProp(resource);
            break;
        case ADJ_MODE_SINGLE:
            ctx->adjacentMode = ADJ_MODE_SINGLE;
            SetAdjacentModeSingleProp(resource);
            break;
        case ADJ_MODE_SELECT:
            ctx->adjacentMode = ADJ_MODE_SELECT;
            SetAdjacentModeSelectProp(resource, x, y);
            break;
        case ADJ_MODE_CONFIRM:
            ctx->adjacentMode = ADJ_MODE_CONFIRM;
            SetAdjacentModeConfirmProp(resource);
            break;
        case ADJ_MODE_FINAL:
            ctx->adjacentMode = ADJ_MODE_FINAL;
            SetAdjacentModeFinalProp(resource);
            IAnimationServiceSplitModeCreateMiddleLine();
            break;
        case ADJ_MODE_DIVIDER_TOUCH_DOWN:
            ctx->adjacentMode = ADJ_MODE_DIVIDER_TOUCH_DOWN;
            SetAdjacentModeMidTouchDown(resource);
            break;
        case ADJ_MODE_DIVIDER_TOUCH_MOVE:
            ctx->adjacentMode = ADJ_MODE_DIVIDER_TOUCH_DOWN;
            SetAdjacentModeMidTouchMove(resource, x, y);
            break;
        case ADJ_MODE_DIVIDER_TOUCH_UP:
            ctx->adjacentMode = ADJ_MODE_DIVIDER_TOUCH_DOWN;
            SetAdjacentModeMidTouchUp(resource);
            break;
        default:
            break;
    }
    wms_send_reply_error(resource, WMS_ERROR_OK);
    LOGD("end.");
    return;
}

bool ResetAdjacentWindowProp(struct wl_resource *resource, struct WindowSurface *windowSurface)
{
    struct WmsContext *ctx = GetWmsInstance();
    struct WindowSurface *pWindowSurface = NULL;
    int32_t defX = 0, defY = 0, defWidth = 0, defHeight = 0;
    GetAdjacentModeShowArea(ctx, &defX, &defY, &defWidth, &defHeight);

    if(ctx->adjacentMode  ==  ADJ_MODE_SINGLE)
    {
        ctx->pLayoutInterface->surface_change_top(windowSurface->layoutSurface);

        wl_list_for_each(pWindowSurface, &ctx->wlListWindow, link) {
            if (pWindowSurface->adjMode == WMS_ADJACENT_MODE_BACK)
            {
                return false;
            }
        }

        IAnimationServiceSplitModeCreateBackground();
        windowSurface->adjMode = WMS_ADJACENT_MODE_BACK;
        wl_list_for_each(pWindowSurface, &ctx->wlListWindow, link) {
            if (pWindowSurface->adjMode == WMS_ADJACENT_MODE_MEMBER)
            {
                ctx->pLayoutInterface->surface_change_top(pWindowSurface->layoutSurface);
            }
        }
        return true;
    }

    if (ctx->adjacentMode  == ADJ_MODE_SELECT)
    {
        
        return true;
    }

    if(ctx->adjacentMode  ==  ADJ_MODE_FINAL)
    {
        wl_list_for_each(pWindowSurface, &ctx->wlListWindow, link) {
            if (pWindowSurface->type == WMS_ADJACENT_MODE_LINE)
            {
                LOGD("WMS_ADJACENT_MODE_MEMBER test2");
                return true;
            }
        }
        windowSurface->adjMode = WMS_ADJACENT_MODE_LINE;
        windowSurface->x = defX;
        windowSurface->y = defY + defHeight * (1 - ADJ_MODE_MIDLINE_HEIGHT_PERCENT) * ADJ_MODE_HALF_PERCENT;
        windowSurface->width = defWidth;
        windowSurface->height = defHeight * ADJ_MODE_MIDLINE_HEIGHT_PERCENT;
        return true;
    }

    return true;
}
} // namespace OHOS
