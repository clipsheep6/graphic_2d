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

#ifndef FRAMEWORKS_SURFACE_TEST_UNITTEST_BUFFER_SHARED_TEST_H
#define FRAMEWORKS_SURFACE_TEST_UNITTEST_BUFFER_SHARED_TEST_H

#include <map>

#include <display_type.h>
#include <gtest/gtest.h>
#include <surface.h>
#include "test_header.h"

#include "buffer_extra_data_impl.h"
#include "buffer_queue.h"

namespace OHOS {
class BufferSharedTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_RGBA_8888,
        .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
        .timeout = 0,
    };

    static inline BufferRequestConfig requestConfig2 = {
        .width = 0x200,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_RGBA_8888,
        .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
        .timeout = 0,
    };

    static inline BufferFlushConfig flushConfig = {
        .damage = {
            .w = 0x100,
            .h = 0x100,
        },
    };

    static inline BufferFlushConfig flushConfig2 = {
        .damage = {
            .w = 0x200,
            .h = 0x100,
        },
    };
    static inline int64_t timestamp = 0;
    static inline Rect damage = {};
    static inline sptr<Surface> surface = nullptr;
    static inline sptr<IBufferProducer> producer_1 = nullptr;
    static inline sptr<IBufferProducer> producer_2 = nullptr;
    static inline sptr<Surface> producerSurface_1 = nullptr;
    static inline sptr<Surface> producerSurface_2 = nullptr;
    static inline sptr<IBufferConsumerListener> listener_1 = nullptr;
    static inline sptr<SurfaceBuffer> buffer_1 = nullptr;
    static inline sptr<SurfaceBuffer> buffer_2 = nullptr;
    static inline sptr<SurfaceBuffer> sbuffer_1 = nullptr;
    static inline sptr<SurfaceBuffer> sbuffer_2 = nullptr;
    // static inline sptr<IBufferConsumerListener> listener_2 = nullptr;
    static inline int32_t fence = -1;
};
} // namespace OHOS


#endif // FRAMEWORKS_SURFACE_TEST_UNITTEST_BUFFER_QUEUE_TEST_H
