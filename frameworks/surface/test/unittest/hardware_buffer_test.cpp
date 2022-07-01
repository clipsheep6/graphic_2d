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
#include <gtest/gtest.h>
#include <iservice_registry.h>
#include <hardware_buffer.h>
#include <display_type.h>
#include "graphic_common_c.h"
//#include <surface_type.h>

using namespace testing;
using namespace testing::ext;
using namespace OHOS;

namespace OHOS::Rosen {
class HardwareBufferTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline OH_HardwareBuffer_Config config = {
        .width = 0x100,
        .height = 0x100,
        .format = PIXEL_FMT_RGBA_8888,
        .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
    };
    static inline OH_HardwareBuffer_Config checkConfig = {};
    static inline OH_HardwareBuffer* buffer = nullptr;
};

void HardwareBufferTest::SetUpTestCase()
{
    buffer = nullptr;
}

void HardwareBufferTest::TearDownTestCase()
{
    buffer = nullptr;
}

/*
* Function: OH_HardwareBuffer_HardwareBufferAlloc
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_HardwareBuffer_HardwareBufferAlloc by abnormal input
*                  2. check ret
*/
HWTEST_F(HardwareBufferTest, HardwareBufferAlloc001, Function | MediumTest | Level2)
{
    buffer = OH_HardwareBuffer_HardwareBufferAlloc(nullptr);
    ASSERT_EQ(buffer, nullptr);
}

/*
* Function: OH_HardwareBuffer_HardwareBufferAlloc
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_HardwareBuffer_HardwareBufferAlloc
*                  2. check ret
*/
HWTEST_F(HardwareBufferTest, HardwareBufferAlloc002, Function | MediumTest | Level2)
{
    buffer = OH_HardwareBuffer_HardwareBufferAlloc(&config);
    ASSERT_NE(buffer, nullptr);
}

/*
* Function: OH_HardwareBuffer_HardwareBufferGetSeqNum
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_HardwareBuffer_HardwareBufferGetSeqNum by abnormal input
*                  2. check ret
*/
HWTEST_F(HardwareBufferTest, HardwareBufferGetSeqNum001, Function | MediumTest | Level2)
{

    uint32_t id = OH_HardwareBuffer_HardwareBufferGetSeqNum(nullptr);
    ASSERT_EQ(id, GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: OH_HardwareBuffer_HardwareBufferGetSeqNum
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_HardwareBuffer_HardwareBufferGetSeqNum
*                  2. check ret
*/
HWTEST_F(HardwareBufferTest, HardwareBufferGetSeqNum002, Function | MediumTest | Level2)
{
    uint32_t id = OH_HardwareBuffer_HardwareBufferGetSeqNum(buffer);
    ASSERT_NE(id, GSERROR_INVALID_ARGUMENTS);
}

/*
* Function: OH_HardwareBuffer_GetHardwareBufferConfig
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_HardwareBuffer_GetHardwareBufferConfig
*                  2. check ret
 */
HWTEST_F(HardwareBufferTest, GetHardwareBufferConfig001, Function | MediumTest | Level2)
{
    OH_HardwareBuffer_GetHardwareBufferConfig(buffer, &checkConfig);
    ASSERT_NE(checkConfig, nullptr);
}

/*
* Function: OH_HardwareBuffer_GetHardwareBufferConfig
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_HardwareBuffer_GetHardwareBufferConfig by abnormal input
*                  2. check ret
*/
HWTEST_F(HardwareBufferTest, GetHardwareBufferConfig002, Function | MediumTest | Level2)
{
    OH_HardwareBuffer_GetHardwareBufferConfig(nullptr, &checkConfig);
    ASSERT_EQ(checkConfig, nullptr);
}

/*
* Function: OH_HardwareBuffer_HardwareBufferReference
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_HardwareBuffer_HardwareBufferReference by abnormal input
*                  2. check ret
*/
HWTEST_F(HardwareBufferTest, HardwareBufferReference001, Function | MediumTest | Level2)
{
    int32_t ret = OH_HardwareBuffer_HardwareBufferReference(nullptr);
    ASSERT_NE(ret, GSERROR_OK);
}

/*
* Function: OH_HardwareBuffer_HardwareBufferReference
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_HardwareBuffer_HardwareBufferReference
*                  2. check ret
*/
HWTEST_F(HardwareBufferTest, HardwareBufferReference002, Function | MediumTest | Level2)
{
    int32_t ret = OH_HardwareBuffer_HardwareBufferReference(buffer);
    ASSERT_EQ(ret, GSERROR_OK);
}

/*
* Function: OH_HardwareBuffer_HardwareBufferUnreference
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_HardwareBuffer_HardwareBufferUnreference by abnormal input
*                  2. check ret
*/
HWTEST_F(HardwareBufferTest, HardwareBufferUnreference001, Function | MediumTest | Level2)
{
    int32_t ret = OH_HardwareBuffer_HardwareBufferUnreference(nullptr);
    ASSERT_NE(ret, GSERROR_OK);
}

/*
* Function: OH_HardwareBuffer_HardwareBufferUnreference
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_HardwareBuffer_HardwareBufferUnreference
*                  2. check ret
*/
HWTEST_F(HardwareBufferTest, HardwareBufferUnreference002, Function | MediumTest | Level2)
{
    int32_t ret = OH_HardwareBuffer_HardwareBufferUnreference(buffer);
    ASSERT_EQ(ret, GSERROR_OK);
}

/*
* Function: OH_HardwareBuffer_HardwareBufferGetSeqNum and OH_HardwareBuffer_HardwareBufferUnreference
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_HardwareBuffer_HardwareBufferGetSeqNum
*                  2. call OH_HardwareBuffer_HardwareBufferUnreference
*                  3. OH_HardwareBuffer_HardwareBufferAlloc again
*                  2. check OH_HardwareBuffer_HardwareBufferGetSeqNum = oldSeq + 1
*/
HWTEST_F(HardwareBufferTest, HardwareBufferGetSeqNum003, Function | MediumTest | Level2)
{
    uint32_t oldSeq = OH_HardwareBuffer_HardwareBufferGetSeqNum(buffer);
    int32_t ret = OH_HardwareBuffer_HardwareBufferUnreference(buffer);
    ASSERT_EQ(ret, GSERROR_OK);
    ASSERT_EQ(buffer, nullptr);
    buffer = OH_HardwareBuffer_HardwareBufferAlloc(&config);
    ASSERT_EQ(oldSeq + 1, OH_HardwareBuffer_HardwareBufferGetSeqNum(buffer));
}

/*
* Function: OH_HardwareBuffer_HardwareBufferMap
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_HardwareBuffer_HardwareBufferMap by abnormal input
*                  2. check ret
*/
HWTEST_F(HardwareBufferTest, HardwareBufferMap001, Function | MediumTest | Level2)
{
    void *virAddr = nullptr;
    int32_t ret = OH_HardwareBuffer_HardwareBufferMap(nullptr, &virAddr);
    ASSERT_NE(ret, GSERROR_OK);

}

/*
* Function: OH_HardwareBuffer_HardwareBufferMap
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_HardwareBuffer_HardwareBufferMap
*                  2. check ret
*/
HWTEST_F(HardwareBufferTest, HardwareBufferMap002, Function | MediumTest | Level2)
{
    void *virAddr = nullptr;
    int32_t ret = OH_HardwareBuffer_HardwareBufferMap(buffer, &virAddr);
    ASSERT_EQ(ret, GSERROR_OK);
    ASSERT_NE(virAddr, nullptr);

}

/*
* Function: OH_HardwareBuffer_HardwareBufferUnmap
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_HardwareBuffer_HardwareBufferUnmap by abnormal input
*                  2. check ret
*/
HWTEST_F(HardwareBufferTest, HardwareBufferUnmap001, Function | MediumTest | Level2)
{
    int32_t ret = OH_HardwareBuffer_HardwareBufferUnmap(nullptr);
    ASSERT_NE(ret, GSERROR_OK);
}

/*
* Function: OH_HardwareBuffer_HardwareBufferUnmap and OH_HardwareBuffer_HardwareBufferUnreference
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call OH_HardwareBuffer_HardwareBufferUnmap
*                  2. check ret
*                  3. call OH_HardwareBuffer_HardwareBufferUnreference
*/
HWTEST_F(HardwareBufferTest, HardwareBufferUnmap002, Function | MediumTest | Level2)
{
    int32_t ret = OH_HardwareBuffer_HardwareBufferUnmap(buffer);
    ASSERT_EQ(ret, GSERROR_OK);
    ret = OH_HardwareBuffer_HardwareBufferUnreference(buffer);
    ASSERT_EQ(ret, GSERROR_OK);
}
}
