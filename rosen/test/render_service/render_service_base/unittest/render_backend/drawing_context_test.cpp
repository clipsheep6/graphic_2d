/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include <hilog/log.h>
#include <memory>

#include "GLES3/gl32.h"

#include "include/core/SkCanvas.h"
#include "include/core/SkColorSpace.h"
#include "include/core/SkImageInfo.h"
#include "include/core/SkSurface.h"
#include "include/gpu/GrBackendSurface.h"
#include "include/gpu/gl/GrGLInterface.h"

#include "render_context/shader_cache.h"
#include "utils/log.h"

#include "drawing_context.h"
#include "render_context_base.h"

using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class DrawingContextTest : public testing::Test {
public:
    static constexpr HiviewDFX::HiLogLabel LOG_LABEL = { LOG_CORE, 0, "DrawingContextTest" };
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline std::unique_ptr<DrawingContext> drawingContext__;
    static inline std::shared_ptr<RSRenderSurfaceFrame> frame_;
};

void DrawingContextTest::SetUpTestCase() {
    drawingContext__ = std::make_unique<DrawingContext>();
    frame_ = std::make_shared<RSRenderSurfaceFrame>();
}
void DrawingContextTest::TearDownTestCase() {}
void DrawingContextTest::SetUp() {}
void DrawingContextTest::TearDown() {}

/**
 * @tc.name: SetUpGrContextTest001
 * @tc.desc: test
 * @tc.type: FUNC
 */
HWTEST_F(DrawingContextTest, SetUpGrContextTest001, Function | SmallTest | Level2)
{
    bool res = drawingContext__->SetUpGrContext();
#if defined(RS_ENABLE_GL)
    ASSERT_TRUE(res);
#else
    ASSERT_EQ(!res);
#endif
}

/**
 * @tc.name: GetGrContextTest001
 * @tc.desc: test
 * @tc.type: FUNC
 */
HWTEST_F(DrawingContextTest, GetGrContextTest001, Function | SmallTest | Level2)
{
    drawingContext__->SetUpGrContext();
#if defined(NEW_SKIA)    
    sk_sp<GrDirectContext> grContext = drawingContext_->GetGrContext();
#else
    sk_sp<GrContext> grContext = drawingContext_->GetGrContext();
#endif
    ASSERT_TRUE(grContext == nullptr);
}

/**
 * @tc.name: GetGrContextTest002
 * @tc.desc: test
 * @tc.type: FUNC
 */
HWTEST_F(DrawingContextTest, GetGrContextTest002, Function | SmallTest | Level2)
{
#if defined(NEW_SKIA)    
    sk_sp<GrDirectContext> grContext = drawingContext_->GetGrContext();
#else
    sk_sp<GrContext> grContext = drawingContext_->GetGrContext();
#endif
    ASSERT_TRUE(grContext != nullptr);
}

/**
 * @tc.name: AcquireSurfaceTest001
 * @tc.desc: test
 * @tc.type: FUNC
 */
HWTEST_F(DrawingContextTest, AcquireSurfaceTest001, Function | SmallTest | Level2)
{
    std::shared_ptr<RSRenderSurfaceFrame> frame_ = std::make_shared<RSRenderSurfaceFrame>();
    frame_->width_ = INVALID_WIDTH;
    frame_->height_ = INVALID_HEIGHT;
    RenderType renderType = RenderType::GLES;
    sk_sp<SkSurface> skSurface = drawingContext_->AcquireSurface(frame_, renderType);
    ASSERT_TRUE(skSurface != nullptr);
}

/**
 * @tc.name: AcquireSurfaceTest002
 * @tc.desc: test
 * @tc.type: FUNC
 */
HWTEST_F(DrawingContextTest, AcquireSurfaceInGLESTest002, Function | SmallTest | Level2)
{
    std::shared_ptr<RSRenderSurfaceFrame> frame_ = std::make_shared<RSRenderSurfaceFrame>();
    frame_->width_ = INVALID_WIDTH;
    frame_->height_ = INVALID_HEIGHT;
    RenderType renderType = RenderType::RASTER;
    sk_sp<SkSurface> skSurface = drawingContext_->AcquireSurface(frame_, renderType);
    ASSERT_TRUE(skSurface != nullptr);
}

/**
 * @tc.name: AcquireSurfaceTest003
 * @tc.desc: test
 * @tc.type: FUNC
 */
HWTEST_F(DrawingContextTest, AAcquireSurfaceInRasterTest003, Function | SmallTest | Level2)
{
    frame_->width_ = INVALID_WIDTH;
    frame_->height_ = INVALID_HEIGHT;
    RenderType renderType = RenderType::VULKAN;
    sk_sp<SkSurface> skSurface = drawingContext_->AcquireSurface(frame_, renderType);
    ASSERT_TRUE(skSurface != nullptr);
}

/**
 * @tc.name: AcquireSurfaceTest004
 * @tc.desc: test
 * @tc.type: FUNC
 */
HWTEST_F(DrawingContextTest, AAcquireSurfaceInRasterTest004, Function | SmallTest | Level2)
{
    RenderType renderType = RenderType::GLES;
    sk_sp<SkSurface> skSurface = drawingContext_->AcquireSurface(frame_, renderType);
    ASSERT_TRUE(skSurface == nullptr);
}

/**
 * @tc.name: AcquireSurfaceTest005
 * @tc.desc: test
 * @tc.type: FUNC
 */
HWTEST_F(DrawingContextTest, AAcquireSurfaceInRasterTest004, Function | SmallTest | Level2)
{
    RenderType renderType = RenderType::RASTER;
    sk_sp<SkSurface> skSurface = drawingContext_->AcquireSurface(frame_, renderType);
    ASSERT_TRUE(skSurface == nullptr);
}

/**
 * @tc.name: AcquireSurfaceTest006
 * @tc.desc: test
 * @tc.type: FUNC
 */
HWTEST_F(DrawingContextTest, AAcquireSurfaceInRasterTest004, Function | SmallTest | Level2)
{
    RenderType renderType = RenderType::VULKAN;
    sk_sp<SkSurface> skSurface = drawingContext_->AcquireSurface(frame_, renderType);
    ASSERT_TRUE(skSurface == nullptr);
}
}
}