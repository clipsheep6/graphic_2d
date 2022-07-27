/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include <iostream>
#include "securec.h"
#include "surface.h"
#include "display_type.h"
#include "wm/window.h"

#include "transaction/rs_transaction.h"
#include "ui/rs_surface_node.h"
// #include "window_option.h"

using namespace OHOS;
using namespace OHOS::Rosen;
using namespace std;

sptr<Surface> GetWindowSurface(uint32_t w, uint32_t h)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowRect({ 0, 0, w, h});
    option->SetWindowType(Rosen::WindowType::WINDOW_TYPE_APP_LAUNCHING);
    option->SetWindowMode(Rosen::WindowMode::WINDOW_MODE_FLOATING);
    sptr<OHOS::Rosen::Window> previewWindow = Rosen::Window::Create("xcomponent_window", option);
    if (previewWindow == nullptr || previewWindow->GetSurfaceNode() == nullptr) {
        cout << "previewWindow is nullptr" << endl;
        return nullptr;
    }
    previewWindow->Show();
    auto surfaceNode = previewWindow->GetSurfaceNode();
    surfaceNode->SetFrameGravity(Rosen::Gravity::RESIZE);
    Rosen::RSTransaction::FlushImplicitTransaction();
    return surfaceNode->GetSurface();
}

struct PriData {
    ExtDataHandle handle;
    int data;
};

int main()
{
    sptr<Surface> surface = GetWindowSurface(700, 1080);
    if (surface == nullptr) {
        return 0;
    }

    sptr<OHOS::SurfaceBuffer> buffer;
    int32_t releaseFence;
    OHOS::BufferRequestConfig config = {
        .width = 700,
        .height = 1080,
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_YCBCR_422_P,
        .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
    };

    OHOS::SurfaceError ret = surface->RequestBuffer(buffer, releaseFence, config);

    if (buffer == nullptr) {
        cout << "request buffer failed: buffer is nullptr" << endl;;
        return 0;
    }
    if (buffer->GetVirAddr() == nullptr) {
        cout << "get virAddr failed: virAddr is nullptr"<< endl;
        return 0;
    }

    OHOS::BufferFlushConfig flushConfig = {
        .damage = {
            .w = buffer->GetWidth(),
            .h = buffer->GetHeight(),
        },
    };
    ret = surface->FlushBuffer(buffer, -1, flushConfig);
    sleep(1000);
}