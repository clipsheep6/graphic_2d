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

#include "native_window.h"

#include <map>
#include <cinttypes>
#include "buffer_log.h"
#include "display_type.h"
#include "external_window.h"
#include "surface_type.h"
#include "sync_fence.h"

#ifndef weak_alias
    #define weak_alias(old, new) \
        extern __typeof(old) new __attribute__((__weak__, __alias__(#old)))
#endif

using namespace OHOS;

OHNativeWindow* CreateNativeWindowFromSurface(void* pSuface)
{
    if (pSuface == nullptr) {
        BLOGE("para error, please check input parameter");
        return nullptr;
    }
    OHNativeWindow* nativeWindow = new OHNativeWindow();
    nativeWindow->surface =
                *reinterpret_cast<OHOS::sptr<OHOS::Surface> *>(pSuface);
    nativeWindow->config.width = nativeWindow->surface->GetDefaultWidth();
    nativeWindow->config.height = nativeWindow->surface->GetDefaultHeight();
    nativeWindow->config.usage = HBM_USE_CPU_READ | HBM_USE_MEM_DMA;
    nativeWindow->config.format = PIXEL_FMT_RGBA_8888;
    nativeWindow->config.strideAlignment = 8;   // default stride is 8
    nativeWindow->config.timeout = 3000;        // default timout is 3000 ms
    nativeWindow->config.colorGamut = ColorGamut::COLOR_GAMUT_SRGB;
    nativeWindow->config.transform = TransformType::ROTATE_NONE;
    nativeWindow->config.scalingMode = ScalingMode::SCALING_MODE_SCALE_TO_WINDOW;

    NativeObjectReference(nativeWindow);
    return nativeWindow;
}

void DestoryNativeWindow(OHNativeWindow *window)
{
    if (window == nullptr) {
        BLOGE("para error, please check input parameter");
        return;
    }
    // unreference nativewindow object
    NativeObjectUnreference(window);
}

OHNativeWindowBuffer* CreateNativeWindowBufferFromSurfaceBuffer(void* pSurfaceBuffer)
{
    if (pSurfaceBuffer == nullptr) {
        BLOGE("para error, please check input parameter");
        return nullptr;
    }
    OHNativeWindowBuffer *nwBuffer = new OHNativeWindowBuffer();
    nwBuffer->sfbuffer = *reinterpret_cast<OHOS::sptr<OHOS::SurfaceBuffer> *>(pSurfaceBuffer);
    NativeObjectReference(nwBuffer);
    return nwBuffer;
}
void DestoryNativeWindowBuffer(OHNativeWindowBuffer* buffer)
{
    if (buffer == nullptr) {
        BLOGE("para error, please check input parameter");
        return;
    }
    NativeObjectUnreference(buffer);
}

int32_t NativeWindowRequestBuffer(OHNativeWindow *window,
    OHNativeWindowBuffer **buffer, int *fenceFd)
{
    if (window == nullptr || buffer == nullptr || fenceFd == nullptr) {
        BLOGE("para error, please check input parameter");
        return OHOS::GSERROR_INVALID_ARGUMENTS;
    }
    OHOS::sptr<OHOS::SurfaceBuffer> sfbuffer;
    OHOS::sptr<OHOS::SyncFence> relaeaseFence = OHOS::SyncFence::INVALID_FENCE;
    if (window->surface->RequestBuffer(sfbuffer, relaeaseFence, window->config) != OHOS::GSError::GSERROR_OK ||
        sfbuffer == nullptr) {
        BLOGE("API failed, please check RequestBuffer funcation ret");
        return OHOS::GSERROR_NO_BUFFER;
    }
    OHNativeWindowBuffer *nwBuffer = new OHNativeWindowBuffer();
    nwBuffer->sfbuffer = sfbuffer;
    *buffer = nwBuffer;
    *fenceFd = relaeaseFence->Dup();
    return OHOS::GSERROR_OK;
}

int32_t NativeWindowFlushBuffer(OHNativeWindow *window, OHNativeWindowBuffer *buffer,
    int fenceFd, struct Region region)
{
    if (window == nullptr || buffer == nullptr || window->surface == nullptr) {
        BLOGE("para error, please check input parameter");
        return OHOS::GSERROR_INVALID_ARGUMENTS;
    }

    OHOS::BufferFlushConfig config;
    if ((region.rectNumber != 0) && (region.rects != nullptr)) {
        config.damage.x = region.rects->x;
        config.damage.y = region.rects->y;
        config.damage.w = static_cast<int32_t>(region.rects->w);
        config.damage.h = static_cast<int32_t>(region.rects->h);
        config.timestamp = window->uiTimestamp;
    } else {
        config.damage.x = 0;
        config.damage.y = 0;
        config.damage.w = window->config.width;
        config.damage.h = window->config.height;
        config.timestamp = window->uiTimestamp;
    }

    OHOS::sptr<OHOS::SyncFence> acquireFence = new OHOS::SyncFence(fenceFd);
    window->surface->FlushBuffer(buffer->sfbuffer, acquireFence, config);

    return OHOS::GSERROR_OK;
}

int32_t NativeWindowCancelBuffer(OHNativeWindow *window, OHNativeWindowBuffer *buffer)
{
    if (window == nullptr || buffer == nullptr) {
        BLOGE("para error, please check input parameter");
        return OHOS::GSERROR_INVALID_ARGUMENTS;
    }

    window->surface->CancelBuffer(buffer->sfbuffer);
    return OHOS::GSERROR_OK;
}

static int32_t InternalHanleNativeWindowOpt(OHNativeWindow *window, int code, va_list args)
{
    switch (code) {
        case SET_USAGE: {
            int32_t usage = va_arg(args, int32_t);
            window->config.usage = usage;
            break;
        }
        case SET_BUFFER_GEOMETRY: {
            int32_t width = va_arg(args, int32_t);
            int32_t height = va_arg(args, int32_t);
            window->config.height = height;
            window->config.width = width;
            break;
        }
        case SET_FORMAT: {
            int32_t format = va_arg(args, int32_t);
            window->config.format = format;
            break;
        }
        case SET_STRIDE: {
            int32_t stride = va_arg(args, int32_t);
            window->config.strideAlignment = stride;
            break;
        }
        case SET_TIMEOUT: {
            int32_t timeout = va_arg(args, int32_t);
            window->config.timeout = timeout;
            break;
        }
        case SET_COLOR_GAMUT: {
            int32_t colorGamut = va_arg(args, int32_t);
            window->config.colorGamut = static_cast<ColorGamut>(colorGamut);
            break;
        }
        case SET_TRANSFORM : {
            int32_t transform = va_arg(args, int32_t);
            window->config.transform = static_cast<TransformType>(transform);
            break;
        }
        case SET_UI_TIMESTAMP : {
            uint64_t uiTimestamp = va_arg(args, uint64_t);
            window->uiTimestamp = static_cast<int64_t>(uiTimestamp);
            break;
        }
        case SET_SCALING_MODE : {
            int32_t scalingMode = va_arg(args, int32_t);
            window->config.scalingMode = static_cast<ScalingMode>(scalingMode);
            break;
        }
        case GET_USAGE: {
            int32_t *value = va_arg(args, int32_t*);
            int32_t usage = window->config.usage;
            *value = usage;
            break;
        }
        case GET_BUFFER_GEOMETRY: {
            int32_t *height = va_arg(args, int32_t*);
            int32_t *width = va_arg(args, int32_t*);
            *height = window->config.height;
            *width = window->config.width;
            break;
        }
        case GET_FORMAT: {
            int32_t *format = va_arg(args, int32_t*);
            *format = window->config.format;
            break;
        }
        case GET_STRIDE: {
            int32_t *stride = va_arg(args, int32_t*);
            *stride = window->config.strideAlignment;
            break;
        }
        case GET_TIMEOUT : {
            int32_t *timeout = va_arg(args, int32_t*);
            *timeout = window->config.timeout;
            break;
        }
        case GET_COLOR_GAMUT: {
            int32_t *colorGamut = va_arg(args, int32_t*);
            *colorGamut = static_cast<int32_t>(window->config.colorGamut);
            break;
        }
        case GET_TRANSFORM: {
            int32_t *transform = va_arg(args, int32_t*);
            *transform = static_cast<int32_t>(window->config.transform);
            break;
        }
        case GET_SCALING_MODE: {
            int32_t *scalingMode = va_arg(args, int32_t*);
            *scalingMode = static_cast<int32_t>(window->config.scalingMode);
            break;
        }
        default:
            break;
    }
    return OHOS::GSERROR_OK;
}

int32_t NativeWindowHandleOpt(OHNativeWindow *window, int code, ...)
{
    if (window == nullptr) {
        BLOGE("para error, please check input parameter");
        return OHOS::GSERROR_INVALID_ARGUMENTS;
    }
    va_list args;
    va_start(args, code);
    InternalHanleNativeWindowOpt(window, code, args);
    va_end(args);
    return OHOS::GSERROR_OK;
}

BufferHandle *GetBufferHandleFromNative(OHNativeWindowBuffer *buffer)
{
    if (buffer == nullptr) {
        BLOGE("para error, please check input parameter");
        return nullptr;
    }
    return buffer->sfbuffer->GetBufferHandle();
}

int32_t GetNativeObjectMagic(void *obj)
{
    if (obj == nullptr) {
        BLOGE("para error, please check input parameter");
        return OHOS::GSERROR_INVALID_ARGUMENTS;
    }
    NativeWindowMagic* nativeWindowMagic = reinterpret_cast<NativeWindowMagic *>(obj);
    return nativeWindowMagic->magic;
}

int32_t NativeObjectReference(void *obj)
{
    if (obj == nullptr) {
        BLOGE("para error, please check input parameter");
        return OHOS::GSERROR_INVALID_ARGUMENTS;
    }
    switch (GetNativeObjectMagic(obj)) {
        case NATIVE_OBJECT_MAGIC_WINDOW:
        case NATIVE_OBJECT_MAGIC_WINDOW_BUFFER:
            break;
        default:
            return OHOS::GSERROR_TYPE_ERROR;
    }
    OHOS::RefBase *ref = reinterpret_cast<OHOS::RefBase *>(obj);
    ref->IncStrongRef(ref);
    return OHOS::GSERROR_OK;
}

int32_t NativeObjectUnreference(void *obj)
{
    if (obj == nullptr) {
        BLOGE("para error, please check input parameter");
        return OHOS::GSERROR_INVALID_ARGUMENTS;
    }
    switch (GetNativeObjectMagic(obj)) {
        case NATIVE_OBJECT_MAGIC_WINDOW:
        case NATIVE_OBJECT_MAGIC_WINDOW_BUFFER:
            break;
        default:
            return OHOS::GSERROR_TYPE_ERROR;
    }
    OHOS::RefBase *ref = reinterpret_cast<OHOS::RefBase *>(obj);
    ref->DecStrongRef(ref);
    return OHOS::GSERROR_OK;
}

int32_t NativeWindowSetMetaData(OHNativeWindow *window, int32_t sequence, int32_t size,
                                const OHHDRMetaData *metaData)
{
    if (window == nullptr || window->surface == nullptr || sequence < 0 || size <= 0 || metaData == nullptr) {
        BLOGE("para error, please check input parameter");
        return OHOS::GSERROR_INVALID_ARGUMENTS;
    }

    std::vector<HDRMetaData> data(reinterpret_cast<const HDRMetaData *>(metaData),
                                  reinterpret_cast<const HDRMetaData *>(metaData) + size);
    return window->surface->SetMetaData(sequence, data);
}

int32_t NativeWindowSetMetaDataSet(OHNativeWindow *window, int32_t sequence, OHHDRMetadataKey key,
                                   int32_t size, const uint8_t *metaData)
{
    if (window == nullptr || window->surface == nullptr || sequence < 0 ||
        key < OHHDRMetadataKey::OH_MATAKEY_RED_PRIMARY_X || key > OHHDRMetadataKey::OH_MATAKEY_HDR_VIVID ||
        size <= 0 || metaData == nullptr) {
        BLOGE("para error, please check input parameter");
        return OHOS::GSERROR_INVALID_ARGUMENTS;
    }
    std::vector<uint8_t> data(metaData, metaData + size);
    return window->surface->SetMetaDataSet(sequence, static_cast<HDRMetadataKey>(key), data);
}

NativeWindow::NativeWindow() : NativeWindowMagic(NATIVE_OBJECT_MAGIC_WINDOW), surface(nullptr)
{
}

NativeWindow::~NativeWindow()
{
}

NativeWindowBuffer::~NativeWindowBuffer()
{
}

NativeWindowBuffer::NativeWindowBuffer() : NativeWindowMagic(NATIVE_OBJECT_MAGIC_WINDOW_BUFFER), sfbuffer(nullptr)
{
}

weak_alias(CreateNativeWindowFromSurface, OH_NativeWindow_CreateNativeWindow);
weak_alias(DestoryNativeWindow, OH_NativeWindow_DestroyNativeWindow);
weak_alias(CreateNativeWindowBufferFromSurfaceBuffer, OH_NativeWindow_CreateNativeWindowBufferFromSurfaceBuffer);
weak_alias(DestoryNativeWindowBuffer, OH_NativeWindow_DestroyNativeWindowBuffer);
weak_alias(NativeWindowRequestBuffer, OH_NativeWindow_NativeWindowRequestBuffer);
weak_alias(NativeWindowFlushBuffer, OH_NativeWindow_NativeWindowFlushBuffer);
weak_alias(NativeWindowCancelBuffer, OH_NativeWindow_NativeWindowAbortBuffer);
weak_alias(NativeWindowHandleOpt, OH_NativeWindow_NativeWindowHandleOpt);
weak_alias(GetBufferHandleFromNative, OH_NativeWindow_GetBufferHandleFromNative);
weak_alias(NativeObjectReference, OH_NativeWindow_NativeObjectReference);
weak_alias(NativeObjectUnreference, OH_NativeWindow_NativeObjectUnreference);
weak_alias(GetNativeObjectMagic, OH_NativeWindow_GetNativeObjectMagic);
weak_alias(NativeWindowSetMetaData, OH_NativeWindow_NativeWindowSetMetaData);
weak_alias(NativeWindowSetMetaDataSet, OH_NativeWindow_NativeWindowSetMetaDataSet);
