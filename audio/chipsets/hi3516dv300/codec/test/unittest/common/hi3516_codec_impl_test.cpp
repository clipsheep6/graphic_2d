/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hi3516_audio_driver_test.h"
#include <gtest/gtest.h>
#include "hdf_uhdf_test.h"

using namespace testing::ext;

namespace {
class Hi3516CodecImplTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void Hi3516CodecImplTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void Hi3516CodecImplTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void Hi3516CodecImplTest::SetUp()
{
}

void Hi3516CodecImplTest::TearDown()
{
}

HWTEST_F(Hi3516CodecImplTest, Hi3516CodecImplTest_CodecHalSysInit, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTCODECHALSYSINIT, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516CodecImplTest, Hi3516CodecImplTest_AcodecDeviceInit, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTACODECDEVICEININ, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516CodecImplTest, Hi3516CodecImplTest_AcodecHalReadReg, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTACODECHALREADREG, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516CodecImplTest, Hi3516CodecImplTest_AcodecHalWriteReg, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTACODECHALWRITEREG, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516CodecImplTest, Hi3516CodecImplTest_AcodecSetI2s1Fs, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTACODECSETI2S1FS, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516CodecImplTest, Hi3516CodecImplTest_AcodecSetI2s1FsInvalidRate, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTACODECSETI2S1FSINVALIDRATE, -1};
    EXPECT_EQ(-1, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516CodecImplTest, Hi3516CodecImplTest_AcodecSetI2s1DataWidth, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTACODECSETI2S1DATAWIDTH, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516CodecImplTest, Hi3516CodecImplTest_AcodecSetI2s1DataWidthInvalidWidth, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTACODECSETI2S1DATAWIDTHINVALIDBITWIDTH, -1};
    EXPECT_EQ(-1, HdfTestSendMsgToService(&msg));
}
}
