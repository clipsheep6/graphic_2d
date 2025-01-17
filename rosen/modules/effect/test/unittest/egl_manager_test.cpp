/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "egl_manager_test.h"
#include "egl_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

/**
 * @tc.name: EGLDisplay001
 * @tc.desc: test EGLDisplay
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(EglManagerTest, EGLDisplay001, TestSize.Level1)
{
    auto &manager = EglManager::GetInstance();
    manager.Init();
    EXPECT_EQ(manager.GetConfig(EglManager::EGL_SUPPORT_VERSION, nullptr), nullptr);
}

} // namespace Rosen
} // namespace OHOS