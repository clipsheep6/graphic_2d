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

#include "cursor.h"
#include "cursor_log.h"
#include <cstdio>
#include <functional>
#include <iostream>
#include <fstream>
#include <raw_parser.h>


#include <display_type.h>
#include <ipc_object_stub.h>
#include <multimodal_event_handler.h>
#include <window_manager.h>
#include <ipc_object_stub.h>
#include "key_event_handler.h"
#include "touch_event_handler.h"
#include "graphic_common.h"



namespace OHOS {

int Cursor::Init(std::string fileName)
{
    printf("init\n");
    auto wm = WindowManager::GetInstance();
    auto wret = wm->Init();
    if (wret != WM_OK) {
        printf("WindowManager::Init failed\n");
        return 0;
    }

    wm->GetDisplays(displays);

    auto pret = resource.Parse(fileName);
    if (pret) {
        printf("raw file parase failed\n");
        return 0;
    }

    auto option = WindowOption::Get();
    option->SetWindowType(WINDOW_TYPE_NORMAL);
    option->SetX(x);
    option->SetY(y);
    option->SetWidth(resource.GetWidth());
    option->SetHeight(resource.GetHeight());
    wm->CreateWindow(window, option);
    if (window == nullptr) {
        printf("CreateWindow failed!\n");
        return 0;
    }

    width = window->GetWidth();
    height = window->GetHeight();
    window->SwitchTop();
    auto surface = window->GetSurface();
    if (surface == nullptr) {
        printf("NativeTestDrawer surface is nullptr\n");
        return 0;
    }

    sptr<SurfaceBuffer> buffer;
    BufferRequestConfig rconfig = {
        .width = surface->GetDefaultWidth(),
        .height = surface->GetDefaultHeight(),
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_RGBA_8888,
        .usage = surface->GetDefaultUsage(),
        .timeout = 0,
    };
    SurfaceError ret = surface->RequestBufferNoFence(buffer, rconfig);
    if (ret != SURFACE_ERROR_OK || buffer == nullptr) {
        printf("surface request buffer failed\n");
        return 0;
    }

    draw(buffer->GetVirAddr(), rconfig.width, rconfig.height, 0);
    BufferFlushConfig fconfig = {
        .damage = {
            .w = rconfig.width,
            .h = rconfig.height,
        },
    };
    surface->FlushBuffer(buffer, -1, fconfig);
    return 0;
}

int Cursor::ListenMouse()
{
    token_ = new IPCObjectStub(u"token");
    MMIEventHdl.RegisterStandardizedEventHandle(token_, window->GetID(), this);
    return 0;
}

void Cursor::draw(void *vaddr, uint32_t width, uint32_t height, uint32_t count)
{
    auto addr = static_cast<uint32_t *>(vaddr);
    if (addr == nullptr) {
        return;
    }
    resource.GetNextData(vaddr);
}

bool Cursor::OnTouch(const TouchEvent &event)
{
    // int32_t index = event.GetIndex();
    // printf("GetSourceDevice %d\n", event.GetSourceDevice());
    // printf("GetAction %d\n", event.GetAction());
    // printf("GetOriginEventType %d\n", event.GetOriginEventType());
    // printf("GetPhase %d\n", event.GetPhase());
    // printf("GetX %f\n", event.GetPointerPosition(index).GetX());
    // printf("GetY %f\n", event.GetPointerPosition(index).GetY());
    // printf("GetEventType %d\n\n", event.GetEventType());
    auto pmevent = (MouseEvent*)event.GetMultimodalEvent();
    if (pmevent != nullptr) {
        auto &mevent = *pmevent;
        OHOS::MmiPoint mmiPoint = mevent.GetCursor();
        if (mevent.GetAction() == 0 ) {
            if (window->GetX() + mmiPoint.GetX() > displays[0].width) {
                downX = displays[0].width;
            } else if (window->GetX() + mmiPoint.GetX() <= 0) {
                downX = 0;
            } else {
                downX = window->GetX() + mmiPoint.GetX();
            }

            if (window->GetY() + mmiPoint.GetY() > displays[0].height) {
                downY = displays[0].height;
            } else if (window->GetY() + mmiPoint.GetY() <= 0) {
                downY = 0;
            } else {
                downY = window->GetY() + mmiPoint.GetY();
            }

            auto ret = window->Move(downX, downY)->Await();
            if (ret != WM_OK) {
                printf("Move failed\n");
                return 0;
            }
            return true;
        }
    } else {
        printf("mouseEvent is nullptr\n");
        return false;
    }
    return false;
}
} // namespce OHOS
