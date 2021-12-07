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

#include "native_test_class.h"

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <securec.h>
#include <sys/time.h>

#include <display_type.h>

#include "inative_test.h"
#include "util.h"

namespace OHOS {
sptr<Window> NativeTestFactory::CreateWindow(WindowType type,
                                             sptr<Surface> csurface,
                                             std::optional<uint32_t> did)
{
    auto wm = WindowManager::GetInstance();
    if (wm == nullptr) {
        return nullptr;
    }

    auto option = WindowOption::Get();
    if (option == nullptr) {
        return nullptr;
    }

    sptr<Window> window;
    option->SetWindowType(type);
    option->SetConsumerSurface(csurface);
    option->SetDisplay(did.value_or(defaultDisplayID));
    wm->CreateWindow(window, option);
    if (window == nullptr) {
        printf("NativeTestFactory::CreateWindow return nullptr\n");
        return nullptr;
    }

    return window;
}

sptr<NativeTestSync> NativeTestSync::CreateSync(DrawFunc drawFunc, sptr<Surface> &psurface, void *data)
{
    if (drawFunc != nullptr && psurface != nullptr) {
        sptr<NativeTestSync> nts = new NativeTestSync();
        nts->draw = drawFunc;
        nts->surface = psurface;
        RequestSync(std::bind(&NativeTestSync::Sync, nts, SYNC_FUNC_ARG), data);
        return nts;
    }
    return nullptr;
}

void NativeTestSync::Sync(int64_t, void *data)
{
    if (surface == nullptr) {
        printf("NativeTestSync surface is nullptr\n");
        return;
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
    if (data != nullptr) {
        rconfig = *reinterpret_cast<BufferRequestConfig *>(data);
    }

    SurfaceError ret = surface->RequestBufferNoFence(buffer, rconfig);
    if (ret == SURFACE_ERROR_NO_BUFFER) {
        RequestSync(std::bind(&NativeTestSync::Sync, this, SYNC_FUNC_ARG), data);
        return;
    } else if (ret != SURFACE_ERROR_OK || buffer == nullptr) {
        printf("NativeTestSync surface request buffer failed\n");
        return;
    }

    draw(buffer->GetVirAddr(), rconfig.width, rconfig.height, count);
    count++;

    BufferFlushConfig fconfig = {
        .damage = {
            .w = rconfig.width,
            .h = rconfig.height,
        },
    };
    surface->FlushBuffer(buffer, -1, fconfig);

    RequestSync(std::bind(&NativeTestSync::Sync, this, SYNC_FUNC_ARG), data);
}

sptr<NativeTestDrawer> NativeTestDrawer::CreateDrawer(DrawFunc drawFunc, sptr<Surface> &psurface, void *data)
{
    if (drawFunc != nullptr && psurface != nullptr) {
        sptr<NativeTestDrawer> ntd = new NativeTestDrawer();
        ntd->draw = drawFunc;
        ntd->surface = psurface;
        return ntd;
    }
    return nullptr;
}

void NativeTestDrawer::DrawOnce(void *data)
{
    RequestSync(std::bind(&NativeTestDrawer::Sync, this, SYNC_FUNC_ARG), data);
}

void NativeTestDrawer::Sync(int64_t, void *data)
{
    if (surface == nullptr) {
        printf("NativeTestDrawer surface is nullptr\n");
        return;
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
    if (data != nullptr) {
        rconfig = *reinterpret_cast<BufferRequestConfig *>(data);
    }

    SurfaceError ret = surface->RequestBufferNoFence(buffer, rconfig);
    if (ret == SURFACE_ERROR_NO_BUFFER) {
        DrawOnce(data);
        return;
    } else if (ret != SURFACE_ERROR_OK || buffer == nullptr) {
        printf("NativeTestDrawer surface request buffer failed\n");
        return;
    }

    draw(buffer->GetVirAddr(), rconfig.width, rconfig.height, count);
    count++;

    BufferFlushConfig fconfig = {
        .damage = {
            .w = rconfig.width,
            .h = rconfig.height,
        },
    };
    surface->FlushBuffer(buffer, -1, fconfig);
}

void NativeTestDraw::FlushDraw(void *vaddr, uint32_t width, uint32_t height, uint32_t count)
{
    auto addr = static_cast<uint8_t *>(vaddr);
    if (addr == nullptr) {
        return;
    }

    constexpr uint32_t bpp = 4;
    constexpr uint32_t color1 = 0xff / 3 * 0;
    constexpr uint32_t color2 = 0xff / 3 * 1;
    constexpr uint32_t color3 = 0xff / 3 * 2;
    constexpr uint32_t color4 = 0xff / 3 * 3;
    constexpr uint32_t bigDiv = 7;
    constexpr uint32_t smallDiv = 10;
    uint32_t c = count % (bigDiv * smallDiv);
    uint32_t stride = width * bpp;
    uint32_t beforeCount = height * c / bigDiv / smallDiv;
    uint32_t afterCount = height - beforeCount - 1;

    auto ret = memset_s(addr, stride * height, color3, beforeCount * stride);
    if (ret) {
        printf("memset_s: %s\n", strerror(ret));
    }

    ret = memset_s(addr + (beforeCount + 1) * stride, stride * height, color1, afterCount * stride);
    if (ret) {
        printf("memset_s: %s\n", strerror(ret));
    }

    for (uint32_t i = 0; i < bigDiv; i++) {
        ret = memset_s(addr + (i * height / bigDiv) * stride, stride * height, color4, stride);
        if (ret) {
            printf("memset_s: %s\n", strerror(ret));
        }
    }

    ret = memset_s(addr + beforeCount * stride, stride * height, color2, stride);
    if (ret) {
        printf("memset_s: %s\n", strerror(ret));
    }
}

void NativeTestDraw::ColorDraw(void *vaddr, uint32_t width, uint32_t height, uint32_t count)
{
    auto addr = static_cast<uint32_t *>(vaddr);
    if (addr == nullptr) {
        return;
    }

    constexpr uint32_t wdiv = 2;
    constexpr uint32_t colorTable[][wdiv] = {
        {0xffff0000, 0xffff00ff},
        {0xffff0000, 0xffffff00},
        {0xff00ff00, 0xffffff00},
        {0xff00ff00, 0xff00ffff},
        {0xff0000ff, 0xff00ffff},
        {0xff0000ff, 0xffff00ff},
        {0xff777777, 0xff777777},
        {0xff777777, 0xff777777},
    };
    const uint32_t hdiv = sizeof(colorTable) / sizeof(*colorTable);

    for (uint32_t i = 0; i < height; i++) {
        auto table = colorTable[i / (height / hdiv)];
        for (uint32_t j = 0; j < wdiv; j++) {
            auto color = table[j];
            for (uint32_t k = j * width / wdiv; k < (j + 1) * width / wdiv; k++) {
                addr[i * width + k] = color;
            }
        }
    }
}

void NativeTestDraw::BlackDraw(void *vaddr, uint32_t width, uint32_t height, uint32_t count)
{
    auto addr = static_cast<uint32_t *>(vaddr);
    if (addr == nullptr) {
        return;
    }

    for (uint32_t i = 0; i < width * height; i++) {
        addr[i] = 0xff000000;
    }
}

void NativeTestDraw::RedDraw(void *vaddr, uint32_t width, uint32_t height, uint32_t count)
{
    auto addr = static_cast<uint32_t *>(vaddr);
    if (addr == nullptr) {
        return;
    }

    for (uint32_t i = 0; i < width * height; i++) {
        addr[i] = 0xff0000ff;
    }
}

void NativeTestDraw::OrangeDraw(void *vaddr, uint32_t width, uint32_t height, uint32_t count)
{
    auto addr = static_cast<uint32_t *>(vaddr);
    if (addr == nullptr) {
        return;
    }

    for (uint32_t i = 0; i < width * height; i++) {
        addr[i] = 0xff007fff;
    }
}

void NativeTestDraw::YellowDraw(void *vaddr, uint32_t width, uint32_t height, uint32_t count)
{
    auto addr = static_cast<uint32_t *>(vaddr);
    if (addr == nullptr) {
        return;
    }

    for (uint32_t i = 0; i < width * height; i++) {
        addr[i] = 0xff00ffff;
    }
}

void NativeTestDraw::GreenDraw(void *vaddr, uint32_t width, uint32_t height, uint32_t count)
{
    auto addr = static_cast<uint32_t *>(vaddr);
    if (addr == nullptr) {
        return;
    }

    for (uint32_t i = 0; i < width * height; i++) {
        addr[i] = 0xff00ff00;
    }
}

void NativeTestDraw::BlueDraw(void *vaddr, uint32_t width, uint32_t height, uint32_t count)
{
    auto addr = static_cast<uint32_t *>(vaddr);
    if (addr == nullptr) {
        return;
    }

    for (uint32_t i = 0; i < width * height; i++) {
        addr[i] = 0xffff0000;
    }
}

void NativeTestDraw::LineDraw(void *vaddr, uint32_t width, uint32_t height, uint32_t count)
{
    auto addr = static_cast<uint32_t *>(vaddr);
    if (addr == nullptr) {
        return;
    }

    for (uint32_t i = 0; i < width * height; i++) {
        addr[i] = 0xffaaaaaa;
    }
}


void NativeTestDraw::RainbowDraw(void *vaddr, uint32_t width, uint32_t height, uint32_t count)
{
    auto addr = static_cast<uint32_t *>(vaddr);
    if (addr == nullptr) {
        return;
    }

    auto drawOneLine = [addr, width](uint32_t index, uint32_t color) {
        auto lineAddr = addr + index * width;
        for (uint32_t i = 0; i < width; i++) {
            lineAddr[i] = color;
        }
    };
    auto selectColor = [height](int32_t index) {
        auto func = [height](int32_t x) {
            int32_t h = height;

            constexpr double b = 3.0;
            constexpr double k = -1.0;
            auto ret = b + k * (((x % h) + h) % h) / (height / 0x6);
            ret = abs(ret) - 1.0;
            ret = fmax(ret, 0.0);
            ret = fmin(ret, 1.0);
            return uint32_t(ret * 0xff);
        };

        constexpr uint32_t bShift = 0;
        constexpr uint32_t gShift = 8;
        constexpr uint32_t rShift = 16;
        constexpr uint32_t bOffset = 0;
        constexpr uint32_t gOffset = -2;
        constexpr uint32_t rOffset = +2;
        return 0xff000000 +
            (func(index + bOffset * (height / 0x6)) << bShift) +
            (func(index + gOffset * (height / 0x6)) << gShift) +
            (func(index + rOffset * (height / 0x6)) << rShift);
    };

    constexpr uint32_t framerate = 100;
    uint32_t offset = (count % framerate) * height / framerate;
    for (uint32_t i = 0; i < height; i++) {
        auto color = selectColor(offset + i);
        drawOneLine(i, color);
    }
}

void NativeTestDraw::BoxDraw(void *vaddr, uint32_t width, uint32_t height, uint32_t count)
{
    auto addr = static_cast<uint32_t *>(vaddr);
    if (addr == nullptr) {
        return;
    }

    auto selectColor = [](int32_t index, int32_t total) {
        auto func = [](int32_t x, int32_t total) {
            int32_t h = total;

            constexpr double b = 3.0;
            constexpr double k = -1.0;
            auto ret = b + k * (((x % h) + h) % h) / (total / 0x6);
            ret = abs(ret) - 1.0;
            ret = fmax(ret, 0.0);
            ret = fmin(ret, 1.0);
            return uint32_t(ret * 0xff);
        };

        constexpr uint32_t bShift = 0;
        constexpr uint32_t gShift = 8;
        constexpr uint32_t rShift = 16;
        constexpr uint32_t bOffset = 0;
        constexpr uint32_t gOffset = -2;
        constexpr uint32_t rOffset = +2;
        return 0xff000000 +
            (func(index + bOffset * (total / 0x6), total) << bShift) +
            (func(index + gOffset * (total / 0x6), total) << gShift) +
            (func(index + rOffset * (total / 0x6), total) << rShift);
    };
    constexpr int32_t framecount = 50;
    uint32_t color = selectColor(count % (framecount * 0x6 * 0x2), framecount * 0x6 * 0x2);
    auto drawOnce = [&addr, &width, &height](int32_t percent, uint32_t color) {
        int32_t x1 = width / 0x2 * percent / framecount;
        int32_t x2 = width - 1 - x1;
        int32_t y1 = height / 0x2 * percent / framecount;
        int32_t y2 = height - 1 - y1;
        for (int32_t i = x1; i < x2; i++) {
            addr[y1 * width + i] = color;
            addr[y2 * width + i] = color;
        }
        for (int32_t j = y1; j < y2; j++) {
            addr[j * width + x1] = color;
            addr[j * width + x2] = color;
        }
    };
    auto abs = [](int32_t x) { return (x < 0) ? -x : x; };
    drawOnce(abs((count - 1) % (framecount * 0x2 - 1) - framecount), color);
    drawOnce(abs((count + 0) % (framecount * 0x2 - 1) - framecount), color);
    drawOnce(abs((count + 1) % (framecount * 0x2 - 1) - framecount), color);
}

void NativeTestDraw::BlurDraw(void *vaddr, uint32_t width, uint32_t height, uint32_t count)
{
    std::unique_ptr<uint32_t[]> offscreenAddr = std::make_unique<uint32_t[]>(width * height);
    for (uint32_t i = 0; i < width * height; i++) {
        offscreenAddr[i] = 0xff000000;
    }

    int32_t w = static_cast<int32_t>(width);
    int32_t h = static_cast<int32_t>(height);

    auto addr = reinterpret_cast<uint32_t *>(vaddr);
    auto addWeight = [&offscreenAddr, w, h](int32_t &r,
        int32_t &g, int32_t &b, int32_t &i, int32_t &j) {
        for (int32_t jj = -1; jj <= 1; jj++) {
            for (int32_t ii = -1; ii <= 1; ii++) {
                if (j + jj < 0 || j + jj >= h) {
                    continue;
                }
                if (i + ii < 0 || i + ii >= w) {
                    continue;
                }
                offscreenAddr[(jj + j) * w + ii + i] +=
                    ((r / 9) << 0x10) + ((g / 9) << 0x8) + ((b / 9) << 0x0);
            }
        }
    };
    for (int32_t j = 0; j < h; j++) {
        for (int32_t i = 0; i < w; i++) {
            int32_t r = (addr[j * w + i] & 0x00ff0000) >> 0x10;
            int32_t g = (addr[j * w + i] & 0x0000ff00) >> 0x08;
            int32_t b = (addr[j * w + i] & 0x000000ff) >> 0x00;
            addWeight(r, g, b, i, j);
        }
    }
    for (uint32_t i = 0; i < width * height; i++) {
        addr[i] = offscreenAddr[i];
    }
}
} // namespace OHOS
