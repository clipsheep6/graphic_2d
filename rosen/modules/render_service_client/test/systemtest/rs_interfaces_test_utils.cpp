/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "rs_interfaces_test_utils.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

RSInterfaceTestUtils::~RSInterfaceTestUtils()
{
    if (prevBuffer_ != nullptr) {
        SurfaceError ret = csurface_->ReleaseBuffer(prevBuffer_, -1);
        if (ret != SURFACE_ERROR_OK) {
            ROSEN_LOGE("buffer release failed");
            return;
        }
        ROSEN_LOGI("prevBuffer_ release success");
    }
    if (csurface_ != nullptr) {
        csurface_->UnregisterConsumerListener();
    }
    csurface_ = nullptr;
    psurface_ = nullptr;
    listener_ = nullptr;
    prevBuffer_ = nullptr;
    bufferHandle_ = nullptr;
}

bool RSInterfaceTestUtils::CreateSurface()
{
    csurface_ = Surface::CreateSurfaceAsConsumer();
    if (csurface_ == nullptr) {
        ROSEN_LOGE("csurface create failed");
        return false;
    }

    auto producer = csurface_->GetProducer();
    psurface_ = Surface::CreateSurfaceAsProducer(producer);
    if (psurface_ == nullptr) {
        ROSEN_LOGE("csurface create failed");
        return false;
    }

    listener_ = new BufferListener(*this);
    SurfaceError ret = csurface_->RegisterConsumerListener(listener_);
    if (ret != SURFACE_ERROR_OK) {
        ROSEN_LOGE("listener register failed");
        return false;
    }
    return true;
}

void RSInterfaceTestUtils::OnVsync()
{
    std::lock_guard<std::mutex> lock(mutex_);
    ROSEN_LOGI("RSInterfaceTestUtils::OnVsync");
    sptr<SurfaceBuffer> cbuffer = nullptr;
    int32_t fence = -1;
    int64_t timestamp = 0;
    OHOS::Rect damage;
    if (csurface_ == nullptr) {
        ROSEN_LOGE("csurface_ is null");
        return;
    }
    auto sret = csurface_->AcquireBuffer(cbuffer, fence, timestamp, damage);
    UniqueFd fenceFd(fence);
    if (cbuffer == nullptr || sret != OHOS::SURFACE_ERROR_OK) {
        ROSEN_LOGE("acquire buffer failed");
        return;
    }
    bufferHandle_ = cbuffer->GetBufferHandle();
    if (bufferHandle_ == nullptr) {
        ROSEN_LOGE("get bufferHandle failed");
        return;
    }
    if (defaultWidth_ == static_cast<uint32_t>(bufferHandle_->width) &&
        defaultHeight_ == static_cast<uint32_t>(bufferHandle_->height)) {
        successCount_++;
        ROSEN_LOGI("compareWH is successful in onVsync: %d", successCount_);
    } else {
        failCount_++;
    }
    if (cbuffer != prevBuffer_) {
        if (prevBuffer_ != nullptr) {
            SurfaceError ret = csurface_->ReleaseBuffer(prevBuffer_, -1);
            if (ret != SURFACE_ERROR_OK) {
                ROSEN_LOGE("buffer release failed");
                return;
            }
        }
        prevBuffer_ = cbuffer;
    }
}

void RSInterfaceTestUtils::RootNodeInit(std::shared_ptr<RSUIDirector> rsUiDirector, int width, int height)
{
    std::cout << "rs app demo Init Rosen Backend!" << std::endl;
    rootNode_ = RSRootNode::Create();
    rootNode_->SetBounds(0, 0, width, height);
    rootNode_->SetFrame(0, 0, width, height);
    rootNode_->SetBackgroundColor(SK_ColorRED);
    rsUiDirector->SetRoot(rootNode_->GetId());
}

sptr<Window> RSInterfaceTestUtils::CreateWindowByDisplayParam(DisplayId displayId, std::string name,
    int32_t x, int32_t y, int32_t w, int32_t h)
{
    sptr<WindowOption> option = new WindowOption();
    if (option == nullptr) {
        return nullptr;
    }
    Rect displayRect = {x, y, w, h};
    option->SetDisplayId(displayId);
    option->SetWindowRect(displayRect);
    option->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option->SetWindowName(name);
    sptr<Window> window = Window::Create(option->GetWindowName(), option);
    return window;
}
} // namespace ROSEN
} // namespace OHOS