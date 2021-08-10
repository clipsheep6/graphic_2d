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

#include "vsync_module_impl_test.h"

#include <iservice_registry.h>
#include <system_ability_definition.h>
#include <unistd.h>

#include "vsync_module_impl.h"

namespace OHOS {
void VsyncHelperImplTest::SetUp()
{
}

void VsyncHelperImplTest::TearDown()
{
}

void VsyncHelperImplTest::SetUpTestCase()
{
}

void VsyncHelperImplTest::TearDownTestCase()
{
}

namespace {
HWTEST_F(VsyncHelperImplTest, Start1, testing::ext::TestSize.Level0)
{
    sptr<TestVsyncModuleImpl> vsyncModuleInstance = TestVsyncModuleImpl::GetInstance();
    sptr<MockDrmModule> mockInstance = new MockDrmModule();
    sptr<DrmModule> staticCall = mockInstance;
    auto origin = DrmModule::GetInstance();
    DrmModule::SetInstance(staticCall);
    EXPECT_CALL(*mockInstance, GetSystemAbilityManager())
                .Times(6).WillRepeatedly(testing::Return(nullptr));
    auto ret = vsyncModuleInstance->Start();
    ASSERT_EQ(ret, VSYNC_ERROR_SERVICE_NOT_FOUND);
    DrmModule::SetInstance(origin);
}

HWTEST_F(VsyncHelperImplTest, Start2, testing::ext::TestSize.Level0)
{
    auto origin = DrmModule::GetInstance();
    sptr<TestVsyncModuleImpl> vsyncModuleInstance = TestVsyncModuleImpl::GetInstance();
    sptr<MockDrmModule> mockInstance = new MockDrmModule();
    sptr<DrmModule> staticCall = mockInstance;
    DrmModule::SetInstance(staticCall);
    auto sm = origin->GetSystemAbilityManager();
    EXPECT_CALL(*mockInstance, GetSystemAbilityManager())
                .Times(1).WillRepeatedly(testing::Return(sm));
    EXPECT_CALL(*mockInstance, DrmOpen(testing::_, testing::_))
                .Times(1).WillRepeatedly(testing::Return(-1));

    auto ret = vsyncModuleInstance->Start();
    ASSERT_EQ(ret, VSYNC_ERROR_API_FAILED);

    mockInstance = nullptr;
    DrmModule::SetInstance(origin);
}

HWTEST_F(VsyncHelperImplTest, WaitNextVBlank, testing::ext::TestSize.Level0)
{
    auto origin = DrmModule::GetInstance();
    sptr<TestVsyncModuleImpl> vsyncModuleInstance = TestVsyncModuleImpl::GetInstance();
    sptr<MockDrmModule> mockInstance = new MockDrmModule();
    sptr<DrmModule> staticCall = mockInstance;
    DrmModule::SetInstance(staticCall);
    EXPECT_CALL(*mockInstance, DrmWaitBlank(testing::_, testing::_))
                .Times(1).WillRepeatedly(testing::Return(-1));
    auto ret = vsyncModuleInstance->WaitNextVBlank();
    ASSERT_EQ(ret, -1);
    DrmModule::SetInstance(origin);
}

HWTEST_F(VsyncHelperImplTest, Stop1, testing::ext::TestSize.Level0)
{
    sptr<TestVsyncModuleImpl> vsyncModuleInstance = TestVsyncModuleImpl::GetInstance();
    sptr<MockDrmModule> mockInstance = new MockDrmModule();
    sptr<DrmModule> staticCall = mockInstance;
    auto origin = DrmModule::GetInstance();
    DrmModule::SetInstance(staticCall);
    EXPECT_CALL(*mockInstance, GetSystemAbilityManager())
                .Times(1).WillRepeatedly(testing::Return(nullptr));
    vsyncModuleInstance->vsyncThreadRunning_ = false;
    vsyncModuleInstance->Stop();
    auto ret = vsyncModuleInstance->isRegisterSA_;
    ASSERT_EQ(ret, true);
    DrmModule::SetInstance(origin);
}

HWTEST_F(VsyncHelperImplTest, Stop2, testing::ext::TestSize.Level0)
{
    sptr<TestVsyncModuleImpl> vsyncModuleInstance = TestVsyncModuleImpl::GetInstance();
    sptr<MockDrmModule> mockInstance = new MockDrmModule();
    sptr<DrmModule> staticCall = mockInstance;
    auto origin = DrmModule::GetInstance();
    DrmModule::SetInstance(staticCall);
    EXPECT_CALL(*mockInstance, GetSystemAbilityManager())
                .Times(1).WillRepeatedly(testing::Return(nullptr));
    vsyncModuleInstance->vsyncThreadRunning_ = false;
    auto ret = vsyncModuleInstance->Stop();
    ASSERT_EQ(ret, VSYNC_ERROR_INVALID_OPERATING);
    DrmModule::SetInstance(origin);
}

HWTEST_F(VsyncHelperImplTest, Stop3, testing::ext::TestSize.Level0)
{
    sptr<TestVsyncModuleImpl> vsyncModuleInstance = TestVsyncModuleImpl::GetInstance();
    sptr<MockDrmModule> mockInstance = new MockDrmModule();
    sptr<DrmModule> staticCall = mockInstance;
    auto origin = DrmModule::GetInstance();
    DrmModule::SetInstance(staticCall);
    vsyncModuleInstance->vsyncThreadRunning_ = true;
    vsyncModuleInstance->vsyncThread_ = std::make_unique<std::thread>([](){});
    auto sm =SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_CALL(*mockInstance, GetSystemAbilityManager())
                .Times(1).WillRepeatedly(testing::Return(sm));
    EXPECT_CALL(*mockInstance, DrmClose(testing::_))
                .Times(1).WillRepeatedly(testing::Return(1));
    auto ret = vsyncModuleInstance->Stop();
    ASSERT_EQ(ret, VSYNC_ERROR_API_FAILED);
    DrmModule::SetInstance(origin);
}
} // namespace

sptr<TestVsyncModuleImpl> TestVsyncModuleImpl::GetInstance()
{
    if (instance == nullptr) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        if (instance == nullptr) {
            instance = new TestVsyncModuleImpl();
        }
    }
    return instance;
}

VsyncError TestVsyncModuleImpl::InitSA()
{
    return VsyncModuleImpl::InitSA(SYSTEMABILITYID);
}

void TestVsyncModuleImpl::VsyncMainThread()
{
}
} // namespace OHOS
