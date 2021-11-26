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

#include "egl_surface_it_test.h"

#include <securec.h>
#include <display_gralloc.h>
#include <gtest/gtest.h>
#include <hilog/log.h>
#include <surface.h>
#include <egl_surface.h>
#include <window_manager.h>

using namespace OHOS;

class EglSurfaceItTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        sptr<WindowManager> wm = WindowManager::GetInstance();
        sptr<WindowOption> option = WindowOption::Get();
        wm->Init();
        wm->CreateWindow(window_, option);
        ASSERT_NE(window_, nullptr);

        producer_ = window_->GetProducer();
        int32_t w = producer_->GetDefaultWidth();
        int32_t h = producer_->GetDefaultHeight();
        std::cout << "default width: " << w << ", height: " << h << std::endl;
        eglRenderSurface_ = EglRenderSurface::CreateEglRenderSurfaceAsProducer(producer_);
    }
    static void TearDownTestCase()
    {
        window_ = nullptr;
        producer_ = nullptr;
        eglRenderSurface_ = nullptr;
    }

protected:
    static inline sptr<Window> window_ = nullptr;
    static inline sptr<IBufferProducer> producer_ = nullptr;
    static inline sptr<EglRenderSurface> eglRenderSurface_ = nullptr;
};

namespace {
constexpr int64_t SEC_TO_USEC = 1000 * 1000;
constexpr int64_t OneFrameTime = 1 * SEC_TO_USEC / 60; // 1second, 60Hz
constexpr int64_t PERFORMANCE_COUNT = 1000;

static int64_t GetNowTime()
{
    struct timeval time;
    gettimeofday(&time, nullptr);
    return SEC_TO_USEC * time.tv_sec + time.tv_usec;
}


/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_EGL_SURFACE_IT_TEST_0100
 * @tc.name    TestCreateEglRenderSurfaceAsProducer1
 * @tc.desc    CreateEglRenderSurfaceAsProducer
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(EglSurfaceItTest, TestCreateEglRenderSurfaceAsProducer1, testing::ext::TestSize.Level1)
{
    ASSERT_NE(eglRenderSurface_, nullptr);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_EGL_SURFACE_IT_TEST_0200
 * @tc.name    TestCreateEglRenderSurfaceAsProducer2
 * @tc.desc    CreateEglRenderSurfaceAsProducer
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(EglSurfaceItTest, TestCreateEglRenderSurfaceAsProducer2, testing::ext::TestSize.Level2)
{
    sptr<IBufferProducer> producer = window_->GetProducer();

    auto startTime = GetNowTime();
    sptr<EglRenderSurface> eglsurface = EglRenderSurface::CreateEglRenderSurfaceAsProducer(producer);
    ASSERT_NE(eglsurface, nullptr);
    auto endTime = GetNowTime();
    auto timeSpend = endTime - startTime;
    ASSERT_LT(timeSpend, OneFrameTime);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_EGL_SURFACE_IT_TEST_0300
 * @tc.name    TestCreateEglRenderSurfaceAsProducer3
 * @tc.desc    CreateEglRenderSurfaceAsProducer
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(EglSurfaceItTest, TestCreateEglRenderSurfaceAsProducer3, testing::ext::TestSize.Level2)
{
    sptr<IBufferProducer> producer = window_->GetProducer();

    auto startTime = GetNowTime();
    for (int32_t i = 0; i < PERFORMANCE_COUNT; i++) {
        sptr<EglRenderSurface> eglsurface = EglRenderSurface::CreateEglRenderSurfaceAsProducer(producer);
        ASSERT_NE(eglsurface, nullptr);
    }
    auto endTime = GetNowTime();
    auto timeSpend = endTime - startTime;
    ASSERT_LT(timeSpend, OneFrameTime * PERFORMANCE_COUNT);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_EGL_SURFACE_IT_TEST_0400
 * @tc.name    TestInitContext1
 * @tc.desc    InitContext
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(EglSurfaceItTest, TestInitContext1, testing::ext::TestSize.Level1)
{
    SurfaceError sret = eglRenderSurface_->InitContext();
    ASSERT_EQ(sret, SURFACE_ERROR_OK);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_EGL_SURFACE_IT_TEST_0500
 * @tc.name    TestInitContext2
 * @tc.desc    InitContext
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(EglSurfaceItTest, TestInitContext2, testing::ext::TestSize.Level2)
{
    sptr<IBufferProducer> producer = window_->GetProducer();
    sptr<EglRenderSurface> eglsurface = EglRenderSurface::CreateEglRenderSurfaceAsProducer(producer);

    auto startTime = GetNowTime();
    SurfaceError sret = eglsurface->InitContext();
    ASSERT_EQ(sret, SURFACE_ERROR_OK);
    auto endTime = GetNowTime();
    auto timeSpend = endTime - startTime;
    ASSERT_LT(timeSpend, OneFrameTime);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_EGL_SURFACE_IT_TEST_0600
 * @tc.name    TestInitContext3
 * @tc.desc    InitContext
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(EglSurfaceItTest, TestInitContext3, testing::ext::TestSize.Level2)
{
    sptr<IBufferProducer> producer = window_->GetProducer();

    auto startTime = 0;
    auto endTime = 0;
    auto timeSpend = 0;
    for (int32_t i = 0; i < PERFORMANCE_COUNT; i++) {
        sptr<EglRenderSurface> eglsurface = EglRenderSurface::CreateEglRenderSurfaceAsProducer(producer);
        startTime = GetNowTime();
        SurfaceError sret = eglsurface->InitContext();
        ASSERT_EQ(sret, SURFACE_ERROR_OK);
        endTime = GetNowTime();
        timeSpend += endTime - startTime;
    }
    ASSERT_LT(timeSpend, OneFrameTime * PERFORMANCE_COUNT);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_EGL_SURFACE_IT_TEST_0700
 * @tc.name    GetEglDisplay1
 * @tc.desc    GetEglDisplay
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(EglSurfaceItTest, TestGetEglDisplay1, testing::ext::TestSize.Level1)
{
    EGLDisplay eglDisplay = eglRenderSurface_->GetEglDisplay();
    ASSERT_NE(eglDisplay, nullptr);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_EGL_SURFACE_IT_TEST_0800
 * @tc.name    GetEglDisplay2
 * @tc.desc    GetEglDisplay
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(EglSurfaceItTest, TestGetEglDisplay2, testing::ext::TestSize.Level2)
{
    sptr<IBufferProducer> producer = window_->GetProducer();
    sptr<EglRenderSurface> eglsurface = EglRenderSurface::CreateEglRenderSurfaceAsProducer(producer);

    EGLDisplay eglDisplay = eglsurface->GetEglDisplay();
    ASSERT_EQ(eglDisplay, EGL_NO_DISPLAY);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_EGL_SURFACE_IT_TEST_0900
 * @tc.name    GetEglDisplay3
 * @tc.desc    GetEglDisplay
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(EglSurfaceItTest, TestGetEglDisplay3, testing::ext::TestSize.Level2)
{
    auto startTime = GetNowTime();
    EGLDisplay eglDisplay = eglRenderSurface_->GetEglDisplay();
    ASSERT_NE(eglDisplay, nullptr);
    auto endTime = GetNowTime();
    auto timeSpend = endTime - startTime;
    ASSERT_LT(timeSpend, OneFrameTime);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_EGL_SURFACE_IT_TEST_1000
 * @tc.name    GetEglDisplay3
 * @tc.desc    GetEglDisplay
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(EglSurfaceItTest, TestGetEglDisplay4, testing::ext::TestSize.Level2)
{
    auto startTime = GetNowTime();
    for (int32_t i = 0; i < PERFORMANCE_COUNT; i++) {
        EGLDisplay eglDisplay = eglRenderSurface_->GetEglDisplay();
        ASSERT_NE(eglDisplay, nullptr);
    }
    auto endTime = GetNowTime();
    auto timeSpend = endTime - startTime;
    ASSERT_LT(timeSpend, OneFrameTime * PERFORMANCE_COUNT);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_EGL_SURFACE_IT_TEST_1100
 * @tc.name    GetEglContext1
 * @tc.desc    GetEglContext
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(EglSurfaceItTest, TestGetEglContext1, testing::ext::TestSize.Level1)
{
    EGLContext eglContext = eglRenderSurface_->GetEglContext();
    ASSERT_NE(eglContext, nullptr);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_EGL_SURFACE_IT_TEST_1200
 * @tc.name    GetEglContext2
 * @tc.desc    GetEglContext
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(EglSurfaceItTest, TestGetEglContext2, testing::ext::TestSize.Level2)
{
    sptr<IBufferProducer> producer = window_->GetProducer();
    sptr<EglRenderSurface> eglsurface = EglRenderSurface::CreateEglRenderSurfaceAsProducer(producer);

    EGLContext eglContext = eglsurface->GetEglContext();
    ASSERT_EQ(eglContext, EGL_NO_CONTEXT);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_EGL_SURFACE_IT_TEST_1300
 * @tc.name    GetEglContext3
 * @tc.desc    GetEglContext
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(EglSurfaceItTest, TestGetEglContext3, testing::ext::TestSize.Level2)
{
    auto startTime = GetNowTime();
    EGLContext eglContext = eglRenderSurface_->GetEglContext();
    ASSERT_NE(eglContext, nullptr);
    auto endTime = GetNowTime();
    auto timeSpend = endTime - startTime;
    ASSERT_LT(timeSpend, OneFrameTime);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_EGL_SURFACE_IT_TEST_1400
 * @tc.name    GetEglContext4
 * @tc.desc    GetEglContext
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(EglSurfaceItTest, TestGetEglContext4, testing::ext::TestSize.Level2)
{
    auto startTime = GetNowTime();
    for (int32_t i = 0; i < PERFORMANCE_COUNT; i++) {
        EGLContext eglContext = eglRenderSurface_->GetEglContext();
        ASSERT_NE(eglContext, nullptr);
    }
    auto endTime = GetNowTime();
    auto timeSpend = endTime - startTime;
    ASSERT_LT(timeSpend, OneFrameTime * PERFORMANCE_COUNT);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_EGL_SURFACE_IT_TEST_1500
 * @tc.name    GetEglFbo1
 * @tc.desc    GetEglFbo
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(EglSurfaceItTest, TestGetEglFbo1, testing::ext::TestSize.Level1)
{
    GLuint eglFbo = eglRenderSurface_->GetEglFbo();
    ASSERT_NE(eglFbo, 0);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_EGL_SURFACE_IT_TEST_1600
 * @tc.name    GetEglFbo2
 * @tc.desc    GetEglFbo
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(EglSurfaceItTest, TestGetEglFbo2, testing::ext::TestSize.Level2)
{
    sptr<IBufferProducer> producer = window_->GetProducer();
    sptr<EglRenderSurface> eglsurface = EglRenderSurface::CreateEglRenderSurfaceAsProducer(producer);

    GLuint eglFbo = eglsurface->GetEglFbo();
    ASSERT_EQ(eglFbo, 0);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_EGL_SURFACE_IT_TEST_1700
 * @tc.name    GetEglFbo3
 * @tc.desc    GetEglFbo
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(EglSurfaceItTest, TestGetEglFbo3, testing::ext::TestSize.Level2)
{
    auto startTime = GetNowTime();
    GLuint eglFbo = eglRenderSurface_->GetEglFbo();
    ASSERT_NE(eglFbo, 0);
    auto endTime = GetNowTime();
    auto timeSpend = endTime - startTime;
    ASSERT_LT(timeSpend, OneFrameTime);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_EGL_SURFACE_IT_TEST_1800
 * @tc.name    GetEglFbo4
 * @tc.desc    GetEglFbo
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(EglSurfaceItTest, TestGetEglFbo4, testing::ext::TestSize.Level2)
{
    auto startTime = GetNowTime();
    for (int32_t i = 0; i < PERFORMANCE_COUNT; i++) {
        GLuint eglFbo = eglRenderSurface_->GetEglFbo();
        ASSERT_NE(eglFbo, 0);
    }
    auto endTime = GetNowTime();
    auto timeSpend = endTime - startTime;
    ASSERT_LT(timeSpend, OneFrameTime * PERFORMANCE_COUNT);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_EGL_SURFACE_IT_TEST_1900
 * @tc.name    SwapBuffers1
 * @tc.desc    SwapBuffers
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(EglSurfaceItTest, TestSwapBuffers1, testing::ext::TestSize.Level1)
{
    SurfaceError sret = eglRenderSurface_->SwapBuffers();
    ASSERT_EQ(sret, SURFACE_ERROR_OK);
    usleep(OneFrameTime);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_EGL_SURFACE_IT_TEST_2000
 * @tc.name    SwapBuffers2
 * @tc.desc    SwapBuffers
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(EglSurfaceItTest, TestSwapBuffers2, testing::ext::TestSize.Level2)
{
    sptr<IBufferProducer> producer = window_->GetProducer();
    sptr<EglRenderSurface> eglsurface = EglRenderSurface::CreateEglRenderSurfaceAsProducer(producer);

    SurfaceError sret = eglsurface->SwapBuffers();
    ASSERT_EQ(sret, SURFACE_ERROR_INIT);
    usleep(OneFrameTime);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_EGL_SURFACE_IT_TEST_2100
 * @tc.name    SwapBuffers3
 * @tc.desc    SwapBuffers
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(EglSurfaceItTest, TestSwapBuffers3, testing::ext::TestSize.Level2)
{
    auto startTime = GetNowTime();
    SurfaceError sret = eglRenderSurface_->SwapBuffers();
    ASSERT_EQ(sret, SURFACE_ERROR_OK);
    auto endTime = GetNowTime();
    auto timeSpend = endTime - startTime;
    ASSERT_LT(timeSpend, OneFrameTime);
    usleep(OneFrameTime);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_EGL_SURFACE_IT_TEST_2200
 * @tc.name    SwapBuffers4
 * @tc.desc    SwapBuffers
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(EglSurfaceItTest, TestSwapBuffers4, testing::ext::TestSize.Level2)
{
    auto startTime = 0;
    auto endTime = 0;
    auto timeSpend = 0;
    for (int32_t i = 0; i < PERFORMANCE_COUNT; i++) {
        startTime = GetNowTime();
        SurfaceError sret = eglRenderSurface_->SwapBuffers();
        ASSERT_EQ(sret, SURFACE_ERROR_OK);
        endTime = GetNowTime();
        timeSpend += endTime - startTime;
        usleep(OneFrameTime);
    }
    ASSERT_LT(timeSpend, OneFrameTime * PERFORMANCE_COUNT);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_EGL_SURFACE_IT_TEST_2300
 * @tc.name    SetWidthAndHeight1
 * @tc.desc    SetWidthAndHeight
 * @tc.size    Level1
 * @tc.type    Function
 */
HWTEST_F(EglSurfaceItTest, TestSetWidthAndHeight1, testing::ext::TestSize.Level1)
{
    SurfaceError sret = eglRenderSurface_->SetWidthAndHeight(100, 100);
    ASSERT_EQ(sret, SURFACE_ERROR_OK);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_EGL_SURFACE_IT_TEST_2400
 * @tc.name    SetWidthAndHeight2
 * @tc.desc    SetWidthAndHeight
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(EglSurfaceItTest, TestSetWidthAndHeight2, testing::ext::TestSize.Level2)
{
    SurfaceError sret = eglRenderSurface_->SetWidthAndHeight(-1, -1);
    ASSERT_EQ(sret, SURFACE_ERROR_INVALID_PARAM);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_EGL_SURFACE_IT_TEST_2500
 * @tc.name    SetWidthAndHeight3
 * @tc.desc    SetWidthAndHeight
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(EglSurfaceItTest, TestSetWidthAndHeight3, testing::ext::TestSize.Level2)
{
    SurfaceError sret = eglRenderSurface_->SetWidthAndHeight(0, 0);
    ASSERT_EQ(sret, SURFACE_ERROR_INVALID_PARAM);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_EGL_SURFACE_IT_TEST_2600
 * @tc.name    SetWidthAndHeight4
 * @tc.desc    SetWidthAndHeight
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(EglSurfaceItTest, TestSetWidthAndHeight4, testing::ext::TestSize.Level2)
{
    SurfaceError sret = eglRenderSurface_->SetWidthAndHeight(10000, 10000);
    ASSERT_EQ(sret, SURFACE_ERROR_OK);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_EGL_SURFACE_IT_TEST_2700
 * @tc.name    SetWidthAndHeight5
 * @tc.desc    SetWidthAndHeight
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(EglSurfaceItTest, TestSetWidthAndHeight5, testing::ext::TestSize.Level2)
{
    auto startTime = GetNowTime();
    SurfaceError sret = eglRenderSurface_->SetWidthAndHeight(100, 100);
    ASSERT_EQ(sret, SURFACE_ERROR_OK);
    auto endTime = GetNowTime();
    auto timeSpend = endTime - startTime;
    ASSERT_LT(timeSpend, OneFrameTime);
}

/*
 * @tc.number    FRAMEWORKS_SURFACE_TEST_MODULETEST_EGL_SURFACE_IT_TEST_2800
 * @tc.name    SetWidthAndHeight6
 * @tc.desc    SetWidthAndHeight
 * @tc.size    Level2
 * @tc.type    Function
 */
HWTEST_F(EglSurfaceItTest, TestSetWidthAndHeight6, testing::ext::TestSize.Level2)
{
    auto startTime = GetNowTime();
    for (int32_t i = 0; i < PERFORMANCE_COUNT; i++) {
        SurfaceError sret = eglRenderSurface_->SetWidthAndHeight(100, 100);
        ASSERT_EQ(sret, SURFACE_ERROR_OK);
    }
    auto endTime = GetNowTime();
    auto timeSpend = endTime - startTime;
    ASSERT_LT(timeSpend, OneFrameTime * PERFORMANCE_COUNT);
}

}
