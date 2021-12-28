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

#ifndef FRAMEWORKS_WMCLIENT_TEST_UNITTEST_SUBWINDOW_NORMAL_IMPL_TEST_H
#define FRAMEWORKS_WMCLIENT_TEST_UNITTEST_SUBWINDOW_NORMAL_IMPL_TEST_H

#include <gtest/gtest.h>
#include <window_manager.h>

namespace OHOS {
class SubwindowNormalImplTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;

private:
    static inline int32_t initRet = GSERROR_NOT_INIT;
    static inline sptr<Window> window = nullptr;
    static inline sptr<SubwindowOption> subwindowOption = nullptr;
};
} // namespace OHOS

#endif // FRAMEWORKS_WMCLIENT_TEST_UNITTEST_SUBWINDOW_NORMAL_IMPL_TEST_H
