/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <iostream>

#include "bitmap_test.h"
#include "camera_test.h"
#include "dms_mock.h"
#include "filter_test.h"
#include "image_test.h"
#include "matrix_test.h"
#include "path_effect_test.h"
#include "path_test.h"
#include "shader_test.h"
#include "shadow_test.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;
using namespace std;

static bool isInit = false;
static bool isGPU = false;
static uint32_t screenWidth = 0;
static uint32_t screenHeight = 0;
static uint32_t screenFreshRate = 0;

static void TestHello()
{
    std::cout << "Drawing module test start.\n";
    std::cout << "If you want to get more information, \n";
    std::cout << "please type 'hilog | grep Drawing' in another hdc shell window\n";
    std::cout << "-------------------------------------------------------\n";
}

static void TestInit()
{
    isInit = true;
    std::cout << "-------------------------------------------------------\n";
    std::cout << "Drawing module testInit start.\n";

#ifdef ACE_ENABLE_GPU
    std::cout << "ACE_ENABLE_GPU is enabled.\n";
    isGPU = true;
#else
    std::cout << "ACE_ENABLE_GPU is disabled.\n";
    isGPU = false;
#endif

#ifdef ACE_ENABLE_GL
    std::cout << "ACE_ENABLE_GL is enabled.\n";
#else
    std::cout << "ACE_ENABLE_GL is disabled.\n";
#endif
    DisplayId id = DmsMock::GetInstance().GetDefaultDisplayId();
    std::cout << "Drawing default screen id is " << id << ".\n";
    auto activeModeInfo = DmsMock::GetInstance().GetDisplayActiveMode(id);
    if (activeModeInfo) {
        screenWidth = activeModeInfo->GetScreenWidth();
        screenHeight = activeModeInfo->GetScreenHeight();
        screenFreshRate = activeModeInfo->GetScreenFreshRate();
        std::cout << "Display " << id << " active mode info:\n";
        std::cout << "Width: " << screenWidth << ", Height: " << screenHeight;
        std::cout << ", FreshRate: " << screenFreshRate << "Hz.\n";
    } else {
        std::cout << "Display " << id << " has no active mode!\n";
    }
    std::cout << "Drawing module testInit end.\n";
    std::cout << "-------------------------------------------------------\n";
    return;
}

int main()
{
    TestHello();
    TestInit();
    BitmapTest::GetInstance().BitmapTestCase(screenWidth, screenHeight);
    CameraTest::GetInstance().CameraTestCase(screenWidth, screenHeight);
    FilterTest::GetInstance().FilterTestCase(screenWidth, screenHeight);
    ImageTest::GetInstance().ImageTestCase(screenWidth, screenHeight);
    MatrixTest::GetInstance().MatrixTestCase(screenWidth, screenHeight);
    PathTest::GetInstance().PathTestCase(screenWidth, screenHeight);
    PathEffectTest::GetInstance().PathEffectTestCase(screenWidth, screenHeight);
    ShaderTest::GetInstance().ShaderTestCase(screenWidth, screenHeight);
    ShadowTest::GetInstance().ShadowTestCase(screenWidth, screenHeight);
    std::cout << "Drawing module test end.\n";
    return 0;
}