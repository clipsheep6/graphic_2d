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

#ifndef FRAMEWORKS_WM_SRC_TEST_DRAW_NATIVE_TEST_H
#define FRAMEWORKS_WM_SRC_TEST_DRAW_NATIVE_TEST_H

#include <cstdint>
#include <functional>
#include <optional>

#include <refbase.h>
#include <window_manager.h>

namespace OHOS {

class NativeTestFactory {
public:
    static sptr<Window> CreateWindow(WindowType type,
                                     sptr<Surface> csurface = nullptr,
                                     std::optional<uint32_t> did = std::nullopt);
    static inline int32_t defaultDisplayID = 0;
};

using DrawFunc = std::function<void(void *, uint32_t, uint32_t, uint32_t)>;
class NativeTestSync : public RefBase {
public:
    static sptr<NativeTestSync> CreateSync(DrawFunc drawFunc, sptr<Surface> &psurface, void *data = nullptr);

private:
    void Sync(int64_t, void *);

    sptr<Surface> surface = nullptr;
    DrawFunc draw = nullptr;
    uint32_t count = 0;
};

class NativeTestDrawer : public RefBase {
public:
    static sptr<NativeTestDrawer> CreateDrawer(DrawFunc drawFunc, sptr<Surface> &psurface, void *data = nullptr);

    void DrawOnce(void *data = nullptr);

private:
    void Sync(int64_t, void *);

    sptr<Surface> surface = nullptr;
    DrawFunc draw = nullptr;
    uint32_t count = 0;
};

class NativeTestDraw {
public:
    static void FlushDraw(void *vaddr, uint32_t width, uint32_t height, uint32_t count);
    static void ColorDraw(void *vaddr, uint32_t width, uint32_t height, uint32_t count);
    static void BlackDraw(void *vaddr, uint32_t width, uint32_t height, uint32_t count);
    static void RainbowDraw(void *vaddr, uint32_t width, uint32_t height, uint32_t count);
    static void BoxDraw(void *vaddr, uint32_t width, uint32_t height, uint32_t count);
    static void BlurDraw(void *vaddr, uint32_t width, uint32_t height, uint32_t count);

    static void RedDraw(void *vaddr, uint32_t width, uint32_t height, uint32_t count);
    static void OrangeDraw(void *vaddr, uint32_t width, uint32_t height, uint32_t count);
    static void YellowDraw(void *vaddr, uint32_t width, uint32_t height, uint32_t count);
    static void GreenDraw(void *vaddr, uint32_t width, uint32_t height, uint32_t count);
    static void BlueDraw(void *vaddr, uint32_t width, uint32_t height, uint32_t count);
    static void NullDraw(void *vaddr, uint32_t width, uint32_t height, uint32_t count);
    static void LineDraw(void *vaddr, uint32_t width, uint32_t height, uint32_t count);
};
} // namespace OHOS

#endif // FRAMEWORKS_WM_SRC_TEST_DRAW_NATIVE_TEST_H
