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

#include "native_window.h"

#include <map>
#include "surface_type.h"
#include <securec.h>

struct NativeWindow* CreateNativeWindowFromSurface(void* pSuface)
{
    if (pSuface == nullptr) {
        return nullptr;
    }
    NativeWindow* nativeWindow = new NativeWindow();
    nativeWindow->surface =
                *reinterpret_cast<OHOS::sptr<OHOS::Surface> *>(pSuface);
    (void)memset_s(&(nativeWindow->config), sizeof(struct NativeBufferRequestConfig),
                0, sizeof(struct NativeBufferRequestConfig));

    NativeObjectReference(nativeWindow);
    return nativeWindow;
}

void DestoryNativeWindow(struct NativeWindow *window)
{
    if (window == nullptr) {
        return;
    }
    // unreference nativewindow object
    NativeObjectUnreference(window);
}

struct NativeWindowBuffer* CreateNativeWindowBufferFromSurfaceBuffer(void* pSurfaceBuffer)
{
    if (pSurfaceBuffer == nullptr) {
        return nullptr;
    }
    NativeWindowBuffer *nwBuffer = new NativeWindowBuffer();
    nwBuffer->sfbuffer = *reinterpret_cast<OHOS::sptr<OHOS::SurfaceBuffer> *>(pSurfaceBuffer);
    NativeObjectReference(nwBuffer);
    return nwBuffer;
}
void DestoryNativeWindowBuffer(struct NativeWindowBuffer* buffer)
{
    if (buffer == nullptr) {
        return;
    }
    NativeObjectUnreference(buffer);
}

int32_t NativeWindowRequestBuffer(struct NativeWindow *window,
    struct NativeWindowBuffer **buffer, int *fenceFd)
{
    if (window == nullptr || buffer == nullptr || fenceFd == nullptr) {
        return OHOS::Rosen::BS_ERROR_NULLPTR;
    }
    OHOS::BufferRequestConfig config = {
        .width = window->config.width,
        .height = window->config.height,
        .strideAlignment = window->config.stride,
        .format = window->config.format,
        .usage = window->config.usage,
        .timeout = window->config.timeout,
    };
    OHOS::sptr<OHOS::SurfaceBuffer> sfbuffer;
    window->surface->RequestBuffer(sfbuffer, *fenceFd, config);
    NativeWindowBuffer *nwBuffer = new NativeWindowBuffer();
    nwBuffer->sfbuffer = sfbuffer;
    // reference nativewindowbuffer object
    NativeObjectReference(nwBuffer);
    *buffer = nwBuffer;
    return OHOS::Rosen::BS_ERROR_OK;
}

int32_t NativeWindowFlushBuffer(struct NativeWindow *window, struct NativeWindowBuffer *buffer,
    int fenceFd, struct Region region)
{
    if (window == nullptr || buffer == nullptr) {
        return OHOS::Rosen::BS_ERROR_NULLPTR;
    }
    OHOS::BufferFlushConfig config = {
        .damage.x = region.rects->x,
        .damage.y = region.rects->y,
        .damage.w = region.rects->w,
        .damage.h = region.rects->h,
        .timestamp = 0,
    };
    window->surface->FlushBuffer(buffer->sfbuffer, fenceFd, config);

    // unreference nativewindowbuffer object
    NativeObjectUnreference(buffer);
    return OHOS::Rosen::BS_ERROR_OK;
}

int32_t NativeWindowCancelBuffer(struct NativeWindow *window, struct NativeWindowBuffer *buffer)
{
    if (window == nullptr || buffer == nullptr) {
        return OHOS::Rosen::BS_ERROR_NULLPTR;
    }
    window->surface->CancelBuffer(buffer->sfbuffer);

    // unreference nativewindowbuffer object
    NativeObjectUnreference(buffer);
    return OHOS::Rosen::BS_ERROR_OK;
}

static int32_t InternalHanleNativeWindowOpt(struct NativeWindow *window, int code, va_list args)
{
    switch (code) {
        case SET_USAGE:
        {
            int32_t usage = va_arg(args, int32_t);
            window->config.usage = usage;
            break;
        }
        case SET_BUFFER_GEOMETRY:
        {
            int32_t width = va_arg(args, int32_t);
            int32_t height = va_arg(args, int32_t);
            window->config.height = height;
            window->config.width = width;
            break;
        }
        case SET_FORMAT:
        {
            int32_t format = va_arg(args, int32_t);
            window->config.format = format;
            break;
        }
        case SET_STRIDE:
        {
            uint32_t stride = va_arg(args, uint32_t);
            window->config.stride = stride;
            break;
        }
        case GET_USAGE:
        {
            int32_t *value = va_arg(args, int32_t*);
            uint32_t usage = window->config.usage;
            *value = usage;
            break;
        }
        case GET_BUFFER_GEOMETRY:
        {
            int32_t *height = va_arg(args, int32_t*);
            int32_t *width = va_arg(args, int32_t*);
            *height = window->config.height;
            *width = window->config.width;
            break;
        }
        case GET_FORMAT:
        {
            int32_t *format = va_arg(args, int32_t*);
            *format = window->config.format;
            break;
        }
        case GET_STRIDE:
        {
            int32_t *stride = va_arg(args, int32_t*);
            *stride = window->config.stride;
            break;
        }
        // [TODO] add others
        default:
            break;
    }
    return OHOS::Rosen::BS_ERROR_OK;
}

int32_t NativeWindowHandleOpt(struct NativeWindow *window, int code, ...)
{
    if (window == nullptr) {
        return OHOS::Rosen::BS_ERROR_NULLPTR;
    }
    va_list args;
    va_start(args, code);
    InternalHanleNativeWindowOpt(window, code, args);
    va_end(args);
    return OHOS::Rosen::BS_ERROR_OK;
}

BufferHandle *GetBufferHandleFromNative(struct NativeWindowBuffer *buffer)
{
    if (buffer == nullptr) {
        return nullptr;
    }
    return buffer->sfbuffer->GetBufferHandle();
}

int32_t GetNativeObjectMagic(void *obj)
{
    if (obj == nullptr) {
        return OHOS::Rosen::BS_ERROR_NULLPTR;
    }
    NativeWindowMagic* nativeWindowMagic = reinterpret_cast<NativeWindowMagic *>(obj);
    return nativeWindowMagic->magic;
}

int32_t NativeObjectReference(void *obj)
{
    if (obj == nullptr) {
        return OHOS::Rosen::BS_ERROR_NULLPTR;
    }
    switch(GetNativeObjectMagic(obj)) {
        case NATIVE_OBJECT_MAGIC_WINDOW:
        case NATIVE_OBJECT_MAGIC_WINDOW_BUFFER:
            break;
        default:
            return OHOS::Rosen::BS_ERROR_TYPE_ERROR;
    }
    OHOS::RefBase *ref = reinterpret_cast<OHOS::RefBase *>(obj);
    ref->IncStrongRef(ref);
    return OHOS::Rosen::BS_ERROR_OK;
}

int32_t NativeObjectUnreference(void *obj)
{
    if (obj == nullptr) {
        return OHOS::Rosen::BS_ERROR_NULLPTR;
    }
    switch(GetNativeObjectMagic(obj)) {
        case NATIVE_OBJECT_MAGIC_WINDOW:
        case NATIVE_OBJECT_MAGIC_WINDOW_BUFFER:
            break;
        default:
            return OHOS::Rosen::BS_ERROR_TYPE_ERROR;
    }
    OHOS::RefBase *ref = reinterpret_cast<OHOS::RefBase *>(obj);
    ref->DecStrongRef(ref);
    return OHOS::Rosen::BS_ERROR_OK;
}


