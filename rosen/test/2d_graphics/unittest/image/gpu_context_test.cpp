/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"

#include "draw/color.h"
#include "image/gpu_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class GpuContextTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GpuContextTest::SetUpTestCase() {}
void GpuContextTest::TearDownTestCase() {}
void GpuContextTest::SetUp() {}
void GpuContextTest::TearDown() {}

/**
 * @tc.name: GPUContextCreateTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(GpuContextTest, GPUContextCreateTest001, TestSize.Level1)
{
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
}

/**
 * @tc.name: FlushTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(GpuContextTest, FlushTest001, TestSize.Level1)
{
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    gpuContext->Flush();
}

/**
 * @tc.name: PerformDeferredCleanupTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(GpuContextTest, PerformDeferredCleanupTest001, TestSize.Level1)
{
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    std::chrono::milliseconds msNotUsed;
    gpuContext->PerformDeferredCleanup(msNotUsed);
}

/**
 * @tc.name: GetResourceCacheLimitsTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(GpuContextTest, GetResourceCacheLimitsTest001, TestSize.Level1)
{
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    int32_t maxResource = 0;
    size_t maxResourceBytes = 0;
    gpuContext->GetResourceCacheLimits(maxResource, maxResourceBytes);
}

/**
 * @tc.name: GetResourceCacheLimitsTest002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(GpuContextTest, GetResourceCacheLimitsTest002, TestSize.Level1)
{
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    int32_t maxResource = 10;
    size_t maxResourceBytes = 1000;
    gpuContext->GetResourceCacheLimits(maxResource, maxResourceBytes);
}

/**
 * @tc.name: SetResourceCacheLimitsTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(GpuContextTest, SetResourceCacheLimitsTest001, TestSize.Level1)
{
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    int32_t maxResource = 0;
    size_t maxResourceBytes = 0;
    gpuContext->SetResourceCacheLimits(maxResource, maxResourceBytes);
}

/**
 * @tc.name: SetResourceCacheLimitsTest002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(GpuContextTest, SetResourceCacheLimitsTest002, TestSize.Level1)
{
    std::unique_ptr<GPUContext> gpuContext = std::make_unique<GPUContext>();
    ASSERT_TRUE(gpuContext != nullptr);
    int32_t maxResource = 100;
    size_t maxResourceBytes = 1000;
    gpuContext->SetResourceCacheLimits(maxResource, maxResourceBytes);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS