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

#ifndef BS_NATIVE_WINDOW_H
#define BS_NATIVE_WINDOW_H

#include <window.h>

#include <refbase.h>
#include <graphic_error.h>
#include <surface.h>
#include <surface_buffer.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MKMAGIC(a, b, c, d) (((a) << 24) + ((b) << 16) + ((c) << 8) + ((d) << 0))

enum NativeObjectMagic {
    NATIVE_OBJECT_MAGIC_WINDOW = MKMAGIC('W', 'I', 'N', 'D'),
    NATIVE_OBJECT_MAGIC_WINDOW_BUFFER = MKMAGIC('W', 'B', 'U', 'F'),
};

struct NativeBufferRequestConfig {
    int32_t width;
    int32_t height;
    int32_t stride; // strideAlignment
    int32_t format; // PixelFormat
    int32_t usage;
    int32_t timeout;
};

struct NativeWindowMagic : public OHOS::RefBase
{
    NativeWindowMagic(NativeObjectMagic m) : magic(m) {}
    virtual ~NativeWindowMagic() {}
    NativeObjectMagic magic;
};

struct NativeWindow : public NativeWindowMagic {
    NativeWindow() : NativeWindowMagic(NATIVE_OBJECT_MAGIC_WINDOW), config{0} {}
    ~NativeWindow() {}
    NativeBufferRequestConfig config;
    OHOS::sptr<OHOS::Surface> surface = nullptr;
};

struct NativeWindowBuffer : public NativeWindowMagic {
    NativeWindowBuffer() : NativeWindowMagic(NATIVE_OBJECT_MAGIC_WINDOW_BUFFER) {}
    ~NativeWindowBuffer() {}
    OHOS::sptr<OHOS::SurfaceBuffer> sfbuffer = nullptr;
};

#ifdef __cplusplus
}
#endif

#endif