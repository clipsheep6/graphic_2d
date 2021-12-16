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

#include "subwindow_video_impl.h"

#include <display_type.h>

#include "window_impl.h"
#include "window_manager_hilog.h"
#include "wl_shm_buffer_factory.h"
#include "wl_subsurface_factory.h"
#include "wl_surface_factory.h"

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "WMSubwindowVideoImpl"};
}

GSError SubwindowVideoImpl::CheckAndNew(sptr<SubwindowVideoImpl> &svi,
                                        const sptr<Window> &window,
                                        const sptr<SubwindowOption> &option,
                                        sptr<WlSurface> &parent)
{
    auto windowImpl = static_cast<WindowImpl *>(window.GetRefPtr());
    if (windowImpl == nullptr) {
        WMLOGFE("WindowImpl is nullptr");
        return GSERROR_INVALID_ARGUMENTS;
    }
    parent = windowImpl->GetWlSurface();

    svi = new SubwindowVideoImpl();
    if (svi == nullptr) {
        WMLOGFE("new SubwindowVideoImpl return nullptr");
        return GSERROR_NO_MEM;
    }
    svi->pw = window;
    return GSERROR_OK;
}

GSError SubwindowVideoImpl::CreateWlSurface(sptr<SubwindowVideoImpl> &svi,
                                            const sptr<WlSurface> &parentWlSurface)
{
    svi->wlSurface = SingletonContainer::Get<WlSurfaceFactory>()->Create();
    if (svi->wlSurface == nullptr) {
        WMLOGFE("WlSurfaceFactory::Create return nullptr");
        return GSERROR_API_FAILED;
    }

    auto subsurfaceFactory = SingletonContainer::Get<WlSubsurfaceFactory>();
    svi->wlSubsurface = subsurfaceFactory->Create(svi->wlSurface, parentWlSurface);
    if (svi->wlSubsurface == nullptr) {
        WMLOGFE("WlSubsurface::Create return nullptr");
        return GSERROR_API_FAILED;
    }

    svi->wlSubsurface->SetPosition(svi->attr.GetX(), svi->attr.GetY());
    svi->wlSubsurface->PlaceBelow(parentWlSurface);
    svi->wlSubsurface->SetDesync();
    return GSERROR_OK;
}

GSError SubwindowVideoImpl::CreateLayer(sptr<SubwindowVideoImpl> &svi)
{
#ifdef TARGET_CPU_ARM
    LayerInfo layerInfo = {
        .width = svi->attr.GetWidth(),
        .height = svi->attr.GetHeight(),
        .type = LAYER_TYPE_SDIEBAND,
        .bpp = 0x8,
        .pixFormat = PIXEL_FMT_RGBA_8888,
    };
    int32_t ret = VideoDisplayManager::CreateLayer(layerInfo, svi->layerId, svi->csurface);
#else
    svi->layerId = -1;
    int32_t ret = DISPLAY_FAILURE;
#endif
    if (ret) {
        WMLOGFE("SubwindowVideoImpl::CreateLayer return nullptr");
        return GSERROR_API_FAILED;
    }

    if (svi->csurface != nullptr) {
        svi->csurface->SetDefaultWidthAndHeight(svi->attr.GetWidth(), svi->attr.GetHeight());
        svi->csurface->SetDefaultUsage(HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA);
    } else {
        WMLOGFE("SubwindowVideoImpl::CreateLayer csurface is nullptr");
    }
    return GSERROR_OK;
}

GSError SubwindowVideoImpl::CreateSHMBuffer(sptr<SubwindowVideoImpl> &svi)
{
    auto width = svi->attr.GetWidth();
    auto height = svi->attr.GetHeight();
    auto format = WL_SHM_FORMAT_XRGB8888;
    svi->wlBuffer = SingletonContainer::Get<WlSHMBufferFactory>()->Create(width, height, format);
    if (svi->wlBuffer == nullptr) {
        WMLOGFE("WlSHMBufferFactory::Create return nullptr");
        return GSERROR_API_FAILED;
    }
    return GSERROR_OK;
}

GSError SubwindowVideoImpl::CreateSurface(sptr<SubwindowVideoImpl> &svi)
{
#ifdef TARGET_CPU_ARM
    svi->display = new (std::nothrow) VideoDisplayManager();
    if (svi->display == nullptr) {
        WMLOGFE("new VideoDisplayManager failed");
        return GSERROR_NO_MEM;
    }

    auto producer = svi->display->AttachLayer(svi->csurface, svi->layerId);
    if (producer == nullptr) {
        WMLOGFE("VideoDisplayManager attach layer failed");
        return GSERROR_API_FAILED;
    }

    svi->psurface = Surface::CreateSurfaceAsProducer(producer);
#else
    return GSERROR_API_FAILED;
#endif
    return GSERROR_OK;
}

GSError SubwindowVideoImpl::Create(sptr<Subwindow> &subwindow,
                                   const sptr<Window> &window,
                                   const sptr<SubwindowOption> &option)
{
    sptr<SubwindowVideoImpl> svi = nullptr;
    sptr<WlSurface> parentWlSurface = nullptr;
    auto wret = CheckAndNew(svi, window, option, parentWlSurface);
    if (wret != GSERROR_OK) {
        return wret;
    }

    svi->attr.SetWidthHeight(option->GetWidth(), option->GetHeight());
    svi->attr.SetXY(option->GetX(), option->GetY());

    wret = CreateWlSurface(svi, parentWlSurface);
    if (wret != GSERROR_OK) {
        return wret;
    }

    wret = CreateLayer(svi);
    if (wret != GSERROR_OK) {
        return wret;
    }

    wret = CreateSHMBuffer(svi);
    if (wret != GSERROR_OK) {
        return wret;
    }

    svi->wlSurface->SetSurfaceType(WL_SURFACE_TYPE_VIDEO);
    svi->wlSurface->Attach(svi->wlBuffer, 0, 0);
    svi->wlSurface->Damage(0, 0, svi->attr.GetWidth(), svi->attr.GetHeight());
    svi->wlSurface->Commit();

    wret = CreateSurface(svi);
    if (wret != GSERROR_OK) {
        return wret;
    }

    wret = svi->Move(option->GetX(), option->GetY());
    if (wret != GSERROR_OK) {
        return wret;
    }

    subwindow = svi;
    WMLOGFI("Create Video Subwindow Success");
    return GSERROR_OK;
}

sptr<Surface> SubwindowVideoImpl::GetSurface() const
{
    return psurface;
}

GSError SubwindowVideoImpl::Move(int32_t x, int32_t y)
{
    WMLOGFI("(subwindow video) x: %{public}d, y: %{public}d", x, y);
    if (display == nullptr) {
        WMLOGFE("display layer is not create");
        return GSERROR_NOT_INIT;
    }

#ifdef TARGET_CPU_ARM
    IRect rect = {};
    int32_t ret = display->GetRect(layerId, rect);
    if (ret != DISPLAY_SUCCESS) {
        WMLOGFE("get rect fail, ret:%{public}d", ret);
        return GSERROR_API_FAILED;
    }

    attr.SetXY(x, y);

    rect.x = x;
    rect.y = y;
    auto parentWindow = pw.promote();
    if (parentWindow != nullptr) {
        rect.x += parentWindow->GetX();
        rect.y += parentWindow->GetY();
    }

    WMLOGFI("(subwindow video) rect.x: %{public}d, rect.y: %{public}d", rect.x, rect.y);
    ret = display->SetRect(layerId, rect);
    if (ret != DISPLAY_SUCCESS) {
        WMLOGFE("set rect fail, ret:%{public}d", ret);
        return GSERROR_API_FAILED;
    }

    wlSubsurface->SetPosition(attr.GetX(), attr.GetY());
#endif
    return GSERROR_OK;
}

GSError SubwindowVideoImpl::Resize(uint32_t width, uint32_t height)
{
    WMLOGFI("(subwindow video) %{public}u x %{public}u", width, height);
    if (display == nullptr) {
        WMLOGFE("display layer is not create");
        return GSERROR_NOT_INIT;
    }
#ifdef TARGET_CPU_ARM
    IRect rect = {};
    int32_t ret = display->GetRect(layerId, rect);
    if (ret != DISPLAY_SUCCESS) {
        WMLOGFE("get rect fail, ret:%d", ret);
        return GSERROR_API_FAILED;
    }

    attr.SetWidthHeight(width, height);
    rect.w = width;
    rect.h = height;
    ret = display->SetRect(layerId, rect);
    if (ret != DISPLAY_SUCCESS) {
        WMLOGFE("set rect fail, ret:%{public}d", ret);
        return GSERROR_API_FAILED;
    }
#endif
    return GSERROR_OK;
}

GSError SubwindowVideoImpl::Destroy()
{
    WMLOGFI("(subwindow video) Destroy");
    csurface = nullptr;
    psurface = nullptr;
    wlSubsurface = nullptr;
    wlSurface = nullptr;
    wlBuffer = nullptr;
#ifdef TARGET_CPU_ARM
    if (display != nullptr) {
        display->DetachLayer(layerId);
    }
    VideoDisplayManager::DestroyLayer(layerId);
#endif
    display = nullptr;
    return GSERROR_OK;
}

void SubwindowVideoImpl::OnPositionChange(WindowPositionChangeFunc func)
{
    attr.OnPositionChange(func);
}

void SubwindowVideoImpl::OnSizeChange(WindowSizeChangeFunc func)
{
    attr.OnSizeChange(func);
}

SubwindowVideoImpl::~SubwindowVideoImpl()
{
    Destroy();
}
} // namespace OHOS
