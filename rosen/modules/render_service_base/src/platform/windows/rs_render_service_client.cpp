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

#include "transaction/rs_render_service_client.h"

#include "platform/common/rs_log.h"
#include "rs_surface_windows.h"
#include "rs_vsync_client_windows.h"

namespace OHOS {
namespace Rosen {
std::shared_ptr<RSIRenderClient> RSIRenderClient::CreateRenderServiceClient()
{
    return nullptr;
}

void RSRenderServiceClient::CommitTransaction(std::unique_ptr<RSTransactionData>& transactionData)
{
}

void RSRenderServiceClient::ExecuteSynchronousTask(const std::shared_ptr<RSSyncTask>& task)
{
}

class BufferConsumerListener : public IBufferConsumerListener {
    wptr<Surface> csurface_ = nullptr;
    UseSurfaceToRenderFunc onRender_ = nullptr;

public:
    BufferConsumerListener(const wptr<Surface> &csurface, UseSurfaceToRenderFunc onRender)
        : csurface_(csurface), onRender_(onRender)
    {
    }

    void OnBufferAvailable()
    {
        if (csurface_ == nullptr) {
            return;
        }

        auto csurface = csurface_.promote();
        if (csurface && onRender_) {
            onRender_(csurface);
        }
    }
};

std::shared_ptr<RSSurface> RSRenderServiceClient::CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config)
{
    // static for only once call
    static sptr<Surface> csurface_ = nullptr;
    if (csurface_ == nullptr) {
        csurface_ = Surface::CreateSurfaceAsConsumer("RSRenderServiceClient_Surface");
        sptr<IBufferConsumerListener> listener = new BufferConsumerListener(csurface_, config.onRender);
        csurface_->RegisterConsumerListener(listener);
    }

    auto produer = csurface_->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(produer);
    std::shared_ptr<RSSurface> rssurface = std::make_shared<RSSurfaceWindows>(psurface);
    return rssurface;
}

class VSyncReceiverWindows : public IVSyncReceiver {
    std::unique_ptr<RSVsyncClient> client_ = nullptr;

public:
    VsyncError Init() override
    {
        client_ = RSVsyncClient::Create();
        if (client_ == nullptr) {
            return GSERROR_NO_MEM;
        }

        return GSERROR_OK;
    }

    VsyncError RequestNextVSync(FrameCallback callback) override
    {
        if (client_ == nullptr) {
            return GSERROR_NOT_INIT;
        }

        auto func = [callback](int64_t time) {
            callback.callback_(time, callback.userData_);
        };
        client_->SetVsyncCallback(func);
        client_->RequestNextVsync();
        return GSERROR_OK;
    }

    VsyncError SetVSyncRate(FrameCallback callback, int32_t rate) override
    {
        if (client_ == nullptr) {
            return GSERROR_NOT_INIT;
        }

        return GSERROR_OK;
    }
};

std::shared_ptr<IVSyncReceiver> RSRenderServiceClient::CreateVSyncReceiver(
    const std::string& name,
    const std::shared_ptr<OHOS::AppExecFwk::EventHandler> &looper)
{
    return std::make_shared<VSyncReceiverWindows>();
}

bool RSRenderServiceClient::TakeSurfaceCapture(NodeId id, std::shared_ptr<SurfaceCaptureCallback> callback,
    float scaleX, float scaleY)
{
    return false;
}

ScreenId RSRenderServiceClient::GetDefaultScreenId()
{
    return 0;
}

ScreenId RSRenderServiceClient::CreateVirtualScreen(const std::string &name, uint32_t width, uint32_t height,
    sptr<Surface> surface, ScreenId mirrorId, int32_t flags)
{
    return INVALID_SCREEN_ID;
}

int32_t RSRenderServiceClient::SetVirtualScreenSurface(ScreenId id, sptr<Surface> surface)
{
    return RENDER_SERVICE_NULL;
}

void RSRenderServiceClient::RemoveVirtualScreen(ScreenId id)
{
}

int32_t RSRenderServiceClient::SetScreenChangeCallback(const ScreenChangeCallback &callback)
{
    return 0;
}

void RSRenderServiceClient::SetScreenActiveMode(ScreenId id, uint32_t modeId)
{
}

void RSRenderServiceClient::SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status)
{
}

RSScreenModeInfo RSRenderServiceClient::GetScreenActiveMode(ScreenId id)
{
    return {};
}

std::vector<RSScreenModeInfo> RSRenderServiceClient::GetScreenSupportedModes(ScreenId id)
{
    return {};
}

RSScreenCapability RSRenderServiceClient::GetScreenCapability(ScreenId id)
{
    return {};
}

ScreenPowerStatus RSRenderServiceClient::GetScreenPowerStatus(ScreenId id)
{
    return {};
}

RSScreenData RSRenderServiceClient::GetScreenData(ScreenId id)
{
    return {};
}

int32_t RSRenderServiceClient::GetScreenBacklight(ScreenId id)
{
    return {};
}

void RSRenderServiceClient::SetScreenBacklight(ScreenId id, uint32_t level)
{
}

bool RSRenderServiceClient::RegisterBufferAvailableListener(NodeId id, const BufferAvailableCallback &callback, bool isFromRenderThread)
{
    return {};
}

bool RSRenderServiceClient::UnregisterBufferAvailableListener(NodeId id)
{
    return {};
}

int32_t RSRenderServiceClient::GetScreenSupportedColorGamuts(ScreenId id, std::vector<ScreenColorGamut>& mode)
{
    return {};
}

int32_t RSRenderServiceClient::GetScreenColorGamut(ScreenId id, ScreenColorGamut& mode)
{
    return {};
}

int32_t RSRenderServiceClient::SetScreenColorGamut(ScreenId id, int32_t modeIdx)
{
    return {};
}

int32_t RSRenderServiceClient::SetScreenGamutMap(ScreenId id, ScreenGamutMap mode)
{
    return {};
}

int32_t RSRenderServiceClient::GetScreenGamutMap(ScreenId id, ScreenGamutMap& mode)
{
    return {};
}

bool RSRenderServiceClient::RequestRotation(ScreenId id, ScreenRotation rotation)
{
    return {};
}

ScreenRotation RSRenderServiceClient::GetRotation(ScreenId id)
{
    return {};
}

int32_t RSRenderServiceClient::GetScreenHDRCapability(ScreenId id, RSScreenHDRCapability& screenHdrCapability)
{
    return {};
}
} // namespace Rosen
} // namespace OHOS
