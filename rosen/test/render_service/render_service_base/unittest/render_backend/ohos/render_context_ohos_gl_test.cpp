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

#include <EGL/egl.h>

#include "window.h"

#include "render_context_factory.h"
#include "ohos/render_context_ohos_gl.h"

using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RenderContextOhosGlTest : public testing::Test {
public:
    static constexpr HiviewDFX::HiLogLabel LOG_LABEL = { LOG_CORE, 0, "RenderContextOhosGlTest" };
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

};

void RenderContextOhosGlTest::SetUpTestCase() {}
void RenderContextOhosGlTest::TearDownTestCase() {}
void RenderContextOhosGlTest::SetUp() {}
void RenderContextOhosGlTest::TearDown() {}

/**
 * @tc.name: InitTest001
 * @tc.desc: test
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextOhosGlTest, InitTest001, Function | SmallTest | Level2)
{
    std::shared_ptr<RenderContextBase> renderContextOhosGl = RenderContextFactory::CreateRenderContext();
    renderContextOhosGl->Init();
    std::shared_ptr<RSRenderSurfaceFrame> frame = renderContextOhosGl->GetRSRenderSurfaceFrame();
    ASSERT_NE(frame->eglDisplay, EGL_NO_DISPLAY);
    ASSERT_NE(frame->eglContext, EGL_NO_CONTEXT);
}

/**
 * @tc.name: IsContextReady001
 * @tc.desc: test
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextOhosGlTest, IsContextReady001, Function | SmallTest | Level2)
{
    std::shared_ptr<RenderContextBase> renderContextOhosGl = RenderContextFactory::CreateRenderContext();
    std::shared_ptr<RSRenderSurfaceFrame> frame = renderContextOhosGl->GetRSRenderSurfaceFrame();
    ASSERT_EQ(frame->eglContext, EGL_NO_CONTEXT);
}

/**
 * @tc.name: IsContextReady002
 * @tc.desc: test
 * @tc.type: FUNC
 */
HWTEST_F(RenderContextOhosGlTest, IsContextReady002, Function | SmallTest | Level2)
{
    std::shared_ptr<RenderContextBase> renderContextOhosGl = RenderContextFactory::CreateRenderContext();
    std::shared_ptr<RSRenderSurfaceFrame> frame = renderContextOhosGl->GetRSRenderSurfaceFrame();
    renderContextOhosGl->Init();
    ASSERT_NE(frame->eglContext, EGL_NO_DISPLAY);
}
}
}