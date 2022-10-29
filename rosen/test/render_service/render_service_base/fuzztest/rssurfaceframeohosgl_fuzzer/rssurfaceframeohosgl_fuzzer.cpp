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

#include "rssurfaceframeohosgl_fuzzer.h"
#include <securec.h>
#include <memory>
#include "platform/ohos/backend/rs_surface_frame_ohos_gl.h"
#include "render_context/render_context.h"

namespace OHOS {
namespace Rosen {
    namespace {
        const uint8_t* g_data = nullptr;
        size_t g_size = 0;
        size_t g_pos = 0;
    } // namespace

    template<class T>
    T GetData()
    {
        T object {};
        size_t objectSize = sizeof(object);
        if (g_data == nullptr || objectSize > g_size - g_pos) {
            return object;
        }
        errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
        if (ret != EOK) {
            return {};
        }
        g_pos += objectSize;
        return object;
    }

    bool RSSurfaceFrameOhosGlFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }
        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;
        RSSurfaceFrameOhosGl rsSurfaceFrameOhosGl(GetData<int32_t>(), GetData<int32_t>());
        RenderContext renderContext_;
        rsSurfaceFrameOhosGl.SetRenderContext(&renderContext_);
        rsSurfaceFrameOhosGl.GetBufferAge();
        rsSurfaceFrameOhosGl.GetReleaseFence();
        rsSurfaceFrameOhosGl.SetReleaseFence(GetData<int32_t>());
        rsSurfaceFrameOhosGl.SetDamageRegion(GetData<int32_t>(), GetData<int32_t>(), GetData<int32_t>(), GetData<int32_t>());
        RectI r1 = RectI(GetData<int32_t>(), GetData<int32_t>(), GetData<int32_t>(), GetData<int32_t>());
        RectI r2 = RectI(GetData<int32_t>(), GetData<int32_t>(), GetData<int32_t>(), GetData<int32_t>());
        RectI r3 = RectI(GetData<int32_t>(), GetData<int32_t>(), GetData<int32_t>(), GetData<int32_t>());
        RectI r4 = RectI(GetData<int32_t>(), GetData<int32_t>(), GetData<int32_t>(), GetData<int32_t>());
        std::vector<RectI> rects;
        rects.emplace_back(r1);
        rects.emplace_back(r2);
        rects.emplace_back(r3);
        rects.emplace_back(r4);
        rsSurfaceFrameOhosGl.SetDamageRegion(rects);
        return true;
    }
} //namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::Rosen::RSSurfaceFrameOhosGlFuzzTest(data, size);
    return 0;
}