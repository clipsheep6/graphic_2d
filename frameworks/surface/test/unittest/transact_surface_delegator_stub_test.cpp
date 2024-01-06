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
#include <securec.h>
#include <gtest/gtest.h>
#include <surface.h>
#include <consumer_surface.h>
#include <native_window.h>
#include "buffer_consumer_listener.h"
#include "sync_fence.h"
#include <message_option.h>
#include <message_parcel.h>
#include "transact_surface_delegator_stub.h"
#include "buffer_log.h"
#include "producer_surface_delegator.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class TransactSurfaceDelegatorStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    static inline sptr<TransactSurfaceDelegatorStub> tsd = nullptr;
    static inline sptr<ProducerSurfaceDelegator> surfaceDelegator = nullptr;
};

void TransactSurfaceDelegatorStubTest::SetUpTestCase()
{
    tsd = new TransactSurfaceDelegatorStub();
    surfaceDelegator = ProducerSurfaceDelegator::Create();
}

void TransactSurfaceDelegatorStubTest::TearDownTestCase()
{
    tsd = nullptr;
    surfaceDelegator = nullptr;
}

/*
* Function: SetClient
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetClient
*                  2. check ret
 */
HWTEST_F(TransactSurfaceDelegatorStubTest, SetClient001, Function | MediumTest | Level2)
{
    bool ret = tsd->SetClient(surfaceDelegator->AsObject());
    ASSERT_EQ(ret, true);

    ret = tsd->SetClient(surfaceDelegator->AsObject());
    ASSERT_EQ(ret, false);
}

/*
* Function: SendSelfProxy
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SendSelfProxy
*                  2. check ret
 */
HWTEST_F(TransactSurfaceDelegatorStubTest, SendSelfProxy001, Function | MediumTest | Level2)
{
    int ret = tsd->SendSelfProxy();
    ASSERT_EQ(ret, ERR_NONE);
}

/*
* Function: ReadNativeHandle
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call ReadNativeHandle
*                  2. check ret
 */
HWTEST_F(TransactSurfaceDelegatorStubTest, ReadNativeHandle001, Function | MediumTest | Level2)
{
    MessageParcel input;
    int32_t version = 3;
    int32_t numFds = 5;
    int32_t numInts = 6;
    input.WriteInt32(version);
    input.WriteInt32(numFds);
    input.WriteInt32(numInts);
    NativeHandleT* buffer = tsd->ReadNativeHandle(input);
    ASSERT_EQ(buffer->version, version);
    ASSERT_EQ(buffer->numFds, numFds);
    ASSERT_EQ(buffer->numInts, numInts);
}
} // namespace OHOS::Rosen