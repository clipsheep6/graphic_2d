/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "platform/ohos/overdraw/rs_overdraw_manager.h"
#include "platform/common/rs_log.h"
#include "parameter.h"

namespace OHOS {
namespace Rosen {
namespace {
// param set debug.graphic.overdraw true/false
constexpr const char *SWITCH_TEXT = "debug.graphic.overdraw";
constexpr const char *SWITCH_ENABLE_TEXT = "true";
} // namespace
RSOverdrawManager &RSOverdrawManager::GetInstance()
{
    static RSOverdrawManager instance;
    return instance;
}

RSOverdrawManager::RSOverdrawManager()
{
    char value[0x20];
    GetParameter(SWITCH_TEXT, "false", value, sizeof(value));
    OnOverdrawEnableCallback(SWITCH_TEXT, value, this);
    WatchParameter(SWITCH_TEXT, OnOverdrawEnableCallback, this);
}

bool RSOverdrawManager::IsEnabled() const
{
    return enabled_;
}

void RSOverdrawManager::SetRepaintCallback(std::function<void()> repaintCallback)
{
    repaintCallback_ = repaintCallback;
}

void RSOverdrawManager::OnOverdrawEnableCallback(const char *key, const char *value, void *context)
{
    auto& overdrawManager = *reinterpret_cast<RSOverdrawManager *>(context);
    bool enabled = false;
    if (strncmp(value, SWITCH_ENABLE_TEXT, strlen(SWITCH_ENABLE_TEXT)) == 0) {
        enabled = true;
        ROSEN_LOGI("%{public}s enable", key);
    } else {
        enabled = false;
        ROSEN_LOGI("%{public}s disable", key);
    }

    if (enabled != overdrawManager.enabled_) {
        overdrawManager.enabled_ = enabled;
        if (overdrawManager.repaintCallback_) {
            overdrawManager.repaintCallback_();
        }
    }
}

void RSOverdrawManager::StartOverDraw(std::shared_ptr<RSPaintFilterCanvas> canvas)
{
    auto gpuContext = canvas->GetGPUContext();
    if (gpuContext == nullptr) {
        ROSEN_LOGE("RSOverdrawManager::StartOverDraw failed: need gpu canvas");
        return;
    }

    auto width = canvas->GetWidth();
    auto height = canvas->GetHeight();
    Drawing::ImageInfo info =
        Drawing::ImageInfo { width, height, Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    overdrawSurface_ = Drawing::Surface::MakeRenderTarget(gpuContext.get(), false, info);
    if (!overdrawSurface_) {
        ROSEN_LOGE("RSUniRenderVisitor::StartOverDraw failed: surface is nullptr");
        return;
    }
    overdrawCanvas_ = std::make_shared<Drawing::OverDrawCanvas>(overdrawSurface_->GetCanvas());
    canvas->AddCanvas(overdrawCanvas_.get());
}

void RSOverdrawManager::FinishOverDraw(std::shared_ptr<RSPaintFilterCanvas> canvas)
{
    if (!overdrawSurface_) {
        ROSEN_LOGE("RSRenderThreadVisitor::FinishOverDraw overdrawSurface is nullptr");
        return;
    }
    auto image = overdrawSurface_->GetImageSnapshot();
    if (image == nullptr) {
        ROSEN_LOGE("RSRenderThreadVisitor::FinishOverDraw image is nullptr");
        return;
    }
    Drawing::Brush brush;
    auto colorFilter = Drawing::ColorFilter::CreateOverDrawColorFilter(colorArray_.data());
    Drawing::Filter filter;
    filter.SetColorFilter(colorFilter);
    brush.SetFilter(filter);
    canvas->AttachBrush(brush);
    canvas->DrawImage(*image, 0, 0, Drawing::SamplingOptions());
    canvas->DetachBrush();
    overdrawSurface_ = nullptr;
    overdrawCanvas_ = nullptr;
}

} // namespace Rosen
} // namespace OHOS
