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

#include "usbd_transfer_test.h"
#include <iostream>
#include <vector>
#include "hdf_log.h"
#include "usb_param.h"
#include "usbd_client.h"

const int SLEEP_TIME = 3;

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::USB;
using namespace std;

struct UsbDev dev;

void UsbdTransferTest::SetUpTestCase(void)
{
    auto ret = UsbdClient::SetPortRole(1, 1, 1);
    sleep(SLEEP_TIME);
    HDF_LOGI("UsbdFunctionTest::[Device] %{public}d SetPortRole=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    if (ret != 0) {
        exit(0);
    }
    std::cout << "请连接设备，连接完后按回车键继续" << std::endl;
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        ;
    }
    HDF_LOGI("-----------Start UsbdTransferTest-----------");
    dev = {1, 2};
    ret = UsbdClient::OpenDevice(dev);
    HDF_LOGI("UsbdTransferTest:: %{public}d OpenDevice=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
}

void UsbdTransferTest::TearDownTestCase(void)
{
    dev = {1, 2};
    auto ret = UsbdClient::CloseDevice(dev);
    HDF_LOGI("UsbdTransferTest:: %{public}d Close=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    HDF_LOGI("-----------End UsbdTransferTest-----------");
}

void UsbdTransferTest::SetUp(void) {}

void UsbdTransferTest::TearDown(void) {}

void UsbdTransferTest::PrintBuffer(const char *charstr, const uint8_t *databuffer, uint32_t datalength)
{
    std::ostringstream oss;
    if (charstr == NULL || datalength == 0 || databuffer == nullptr) {
        return;
    }
    oss.str("");
    oss << charstr << " << 二进制数据流[" << datalength << "字节] >> :";
    for (uint32_t i = 0; i < datalength; ++i) {
        oss << " " << std::hex << (int)databuffer[i];
    }
    oss << "  -->  " << databuffer << std::endl;
    HDF_LOGI("%{public}s", oss.str().c_str());
}

/**
 * @tc.name: UsbdControlTransfer001
 * @tc.desc: Test functions to ControlTransfer(const UsbDev &dev, UsbCtrlTransfer &ctrl, std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdControlTransfer001, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdControlTransfer001 : ControlTransfer-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t buffer[255] = {0};
    uint32_t length = 255;
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    struct UsbCtrlTransfer ctrlparmas = {0b10000000, 8, 0, 0, 1000};
    auto ret = UsbdClient::ControlTransfer(dev, ctrlparmas, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer001 %{public}d length=%{public}d", __LINE__, bufferdata.size());
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer001 %{public}d ControlTransfer=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    PrintBuffer("控制传输", bufferdata.data(), bufferdata.size());
    HDF_LOGI("-----------Case End : UsbdControlTransfer001 : ControlTransfer-----------");
}

/**
 * @tc.name: UsbdControlTransfer002
 * @tc.desc: Test functions to ControlTransfer(const UsbDev &dev, UsbCtrlTransfer &ctrl, std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdControlTransfer002, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdControlTransfer002 : ControlTransfer-----------");
    dev.busNum = 255;
    dev.devAddr = 2;
    uint8_t buffer[255] = {0};
    uint32_t length = 255;
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    struct UsbCtrlTransfer ctrlparmas = {0b10000000, 8, 0, 0, 1000};
    auto ret = UsbdClient::ControlTransfer(dev, ctrlparmas, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer002 %{public}d length=%{public}d", __LINE__, bufferdata.size());
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer002 %{public}d ControlTransfer=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdControlTransfer002 : ControlTransfer-----------");
}

/**
 * @tc.name: UsbdControlTransfer003
 * @tc.desc: Test functions to ControlTransfer(const UsbDev &dev, UsbCtrlTransfer &ctrl, std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdControlTransfer003, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdControlTransfer003 : ControlTransfer-----------");
    dev.busNum = 1;
    dev.devAddr = 255;
    uint8_t buffer[255] = {0};
    uint32_t length = 255;
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    struct UsbCtrlTransfer ctrlparmas = {0b10000000, 8, 0, 0, 1000};
    auto ret = UsbdClient::ControlTransfer(dev, ctrlparmas, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer003 %{public}d length=%{public}d", __LINE__, bufferdata.size());
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer003 %{public}d ControlTransfer=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdControlTransfer003 : ControlTransfer-----------");
}

/**
 * @tc.name: UsbdControlTransfer004
 * @tc.desc: Test functions to ControlTransfer(const UsbDev &dev, UsbCtrlTransfer &ctrl, std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdControlTransfer004, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdControlTransfer004 : ControlTransfer-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint32_t length = 255;
    uint8_t buffer[255] = {0};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    struct UsbCtrlTransfer ctrlparmas = {0b10000000, 6, 0x100, 0, 1000};
    auto ret = UsbdClient::ControlTransfer(dev, ctrlparmas, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer004 %{public}d length=%{public}d", __LINE__, bufferdata.size());
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer004 %{public}d ControlTransfer=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    PrintBuffer("控制传输", bufferdata.data(), bufferdata.size());
    HDF_LOGI("-----------Case End : UsbdControlTransfer004 : ControlTransfer-----------");
}

/**
 * @tc.name: UsbdControlTransfer005
 * @tc.desc: Test functions to ControlTransfer(const UsbDev &dev, UsbCtrlTransfer &ctrl, std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdControlTransfer005, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdControlTransfer005 : ControlTransfer-----------");
    dev.busNum = 255;
    dev.devAddr = 2;
    uint32_t length = 255;
    uint8_t buffer[255] = {0};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    struct UsbCtrlTransfer ctrlparmas = {0b10000000, 6, 0x100, 0, 1000};
    auto ret = UsbdClient::ControlTransfer(dev, ctrlparmas, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer005 %{public}d length=%{public}d", __LINE__, bufferdata.size());
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer005 %{public}d ControlTransfer=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdControlTransfer005 : ControlTransfer-----------");
}

/**
 * @tc.name: UsbdControlTransfer006
 * @tc.desc: Test functions to ControlTransfer(const UsbDev &dev, UsbCtrlTransfer &ctrl, std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdControlTransfer006, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdControlTransfer006 : ControlTransfer-----------");
    dev.busNum = 1;
    dev.devAddr = 255;
    uint32_t length = 255;
    uint8_t buffer[255] = {0};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    struct UsbCtrlTransfer ctrlparmas = {0b10000000, 6, 0x100, 0, 1000};
    auto ret = UsbdClient::ControlTransfer(dev, ctrlparmas, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer006 %{public}d length=%{public}d", __LINE__, bufferdata.size());
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer006 %{public}d ControlTransfer=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdControlTransfer006 : ControlTransfer-----------");
}

/**
 * @tc.name: UsbdControlTransfer007
 * @tc.desc: Test functions to ControlTransfer(const UsbDev &dev, UsbCtrlTransfer &ctrl, std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdControlTransfer007, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdControlTransfer007 : ControlTransfer-----------");
    int32_t intercafeidex = 0;
    dev.busNum = 1;
    dev.devAddr = 2;
    uint32_t length = 255;
    uint8_t buffer[255] = {0};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    struct UsbCtrlTransfer ctrlparmas = {0b10000001, 0X0A, 0, intercafeidex, 1000};
    auto ret = UsbdClient::ControlTransfer(dev, ctrlparmas, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer007 %{public}d length=%{public}d", __LINE__, bufferdata.size());
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer007 %{public}d ControlTransfer=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    PrintBuffer("控制传输", bufferdata.data(), bufferdata.size());
    HDF_LOGI("-----------Case End : UsbdControlTransfer007 : ControlTransfer-----------");
}

/**
 * @tc.name: UsbdControlTransfer008
 * @tc.desc: Test functions to ControlTransfer(const UsbDev &dev, UsbCtrlTransfer &ctrl, std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdControlTransfer008, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdControlTransfer008 : ControlTransfer-----------");
    int32_t intercafeidex = 0;
    dev.busNum = 255;
    dev.devAddr = 2;
    uint32_t length = 255;
    uint8_t buffer[255] = {0};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    struct UsbCtrlTransfer ctrlparmas = {0b10000001, 0X0A, 0, intercafeidex, 1000};
    auto ret = UsbdClient::ControlTransfer(dev, ctrlparmas, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer008 %{public}d length=%{public}d", __LINE__, bufferdata.size());
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer008 %{public}d ControlTransfer=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdControlTransfer008 : ControlTransfer-----------");
}

/**
 * @tc.name: UsbdControlTransfer009
 * @tc.desc: Test functions to ControlTransfer(const UsbDev &dev, UsbCtrlTransfer &ctrl, std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdControlTransfer009, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdControlTransfer009 : ControlTransfer-----------");
    int32_t intercafeidex = 0;
    dev.busNum = 1;
    dev.devAddr = 255;
    uint32_t length = 255;
    uint8_t buffer[255] = {0};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    struct UsbCtrlTransfer ctrlparmas = {0b10000001, 0X0A, 0, intercafeidex, 1000};
    auto ret = UsbdClient::ControlTransfer(dev, ctrlparmas, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer009 %{public}d length=%{public}d", __LINE__, bufferdata.size());
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer009 %{public}d ControlTransfer=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdControlTransfer009 : ControlTransfer-----------");
}

/**
 * @tc.name: UsbdControlTransfer010
 * @tc.desc: Test functions to ControlTransfer(const UsbDev &dev, UsbCtrlTransfer &ctrl, std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdControlTransfer010, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdControlTransfer010 : ControlTransfer-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint32_t length = 255;
    uint8_t buffer[255] = {0};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    struct UsbCtrlTransfer ctrlparmas = {0b10000000, 0, 0, 0, 1000};
    auto ret = UsbdClient::ControlTransfer(dev, ctrlparmas, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer010 %{public}d length=%{public}d", __LINE__, bufferdata.size());
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer010 %{public}d ControlTransfer=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    PrintBuffer("控制传输", bufferdata.data(), bufferdata.size());
    HDF_LOGI("-----------Case Start : UsbdControlTransfer010 : ControlTransfer-----------");
}

/**
 * @tc.name: UsbdControlTransfer011
 * @tc.desc: Test functions to ControlTransfer(const UsbDev &dev, UsbCtrlTransfer &ctrl, std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdControlTransfer011, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdControlTransfer011 : ControlTransfer-----------");
    dev.busNum = 255;
    dev.devAddr = 2;
    uint32_t length = 255;
    uint8_t buffer[255] = {0};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    struct UsbCtrlTransfer ctrlparmas = {0b10000000, 0, 0, 0, 1000};
    auto ret = UsbdClient::ControlTransfer(dev, ctrlparmas, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer011 %{public}d length=%{public}d", __LINE__, bufferdata.size());
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer011 %{public}d ControlTransfer=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case Start : UsbdControlTransfer011 : ControlTransfer-----------");
}

/**
 * @tc.name: UsbdControlTransfer012
 * @tc.desc: Test functions to ControlTransfer(const UsbDev &dev, UsbCtrlTransfer &ctrl, std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdControlTransfer012, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdControlTransfer012 : ControlTransfer-----------");
    dev.busNum = 1;
    dev.devAddr = 255;
    uint32_t length = 255;
    uint8_t buffer[255] = {0};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    struct UsbCtrlTransfer ctrlparmas = {0b10000000, 0, 0, 0, 1000};
    auto ret = UsbdClient::ControlTransfer(dev, ctrlparmas, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer012 %{public}d length=%{public}d", __LINE__, bufferdata.size());
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer012 %{public}d ControlTransfer=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case Start : UsbdControlTransfer012 : ControlTransfer-----------");
}

/**
 * @tc.name: UsbdControlTransfer013
 * @tc.desc: Test functions to ControlTransfer(const UsbDev &dev, UsbCtrlTransfer &ctrl, std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdControlTransfer013, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdControlTransfer013 : ControlTransfer-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint32_t length = 255;
    uint8_t buffer[255] = {0};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    struct UsbCtrlTransfer ctrlparmas = {0b10000001, 0, 0, 0, 1000};
    auto ret = UsbdClient::ControlTransfer(dev, ctrlparmas, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer013 %{public}d length=%{public}d", __LINE__, bufferdata.size());
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer013 %{public}d ControlTransfer=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    PrintBuffer("控制传输", bufferdata.data(), bufferdata.size());
    HDF_LOGI("-----------Case End : UsbdControlTransfer013 : ControlTransfer-----------");
}

/**
 * @tc.name: UsbdControlTransfer014
 * @tc.desc: Test functions to ControlTransfer(const UsbDev &dev, UsbCtrlTransfer &ctrl, std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdControlTransfer014, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdControlTransfer014 : ControlTransfer-----------");
    dev.busNum = 255;
    dev.devAddr = 2;
    uint32_t length = 255;
    uint8_t buffer[255] = {0};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    struct UsbCtrlTransfer ctrlparmas = {0b10000001, 0, 0, 0, 1000};
    auto ret = UsbdClient::ControlTransfer(dev, ctrlparmas, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer014 %{public}d length=%{public}d", __LINE__, bufferdata.size());
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer014 %{public}d ControlTransfer=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdControlTransfer014 : ControlTransfer-----------");
}

/**
 * @tc.name: UsbdControlTransfer015
 * @tc.desc: Test functions to ControlTransfer(const UsbDev &dev, UsbCtrlTransfer &ctrl, std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdControlTransfer015, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdControlTransfer015 : ControlTransfer-----------");
    dev.busNum = 1;
    dev.devAddr = 255;
    uint32_t length = 255;
    uint8_t buffer[255] = {0};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    struct UsbCtrlTransfer ctrlparmas = {0b10000001, 0, 0, 0, 1000};
    auto ret = UsbdClient::ControlTransfer(dev, ctrlparmas, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer015 %{public}d length=%{public}d", __LINE__, bufferdata.size());
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer015 %{public}d ControlTransfer=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdControlTransfer015 : ControlTransfer-----------");
}

/**
 * @tc.name: UsbdControlTransfer016
 * @tc.desc: Test functions to ControlTransfer(const UsbDev &dev, UsbCtrlTransfer &ctrl, std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdControlTransfer016, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdControlTransfer016 : ControlTransfer-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint32_t length = 255;
    uint8_t buffer[255] = {0};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    struct UsbCtrlTransfer ctrlparmas = {0b10000010, 0, 0, 0, 1000};
    auto ret = UsbdClient::ControlTransfer(dev, ctrlparmas, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer016 %{public}d length=%{public}d", __LINE__, bufferdata.size());
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer016 %{public}d ControlTransfer=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    PrintBuffer("控制传输", bufferdata.data(), bufferdata.size());
    HDF_LOGI("-----------Case End : UsbdControlTransfer016 : ControlTransfer-----------");
}

/**
 * @tc.name: UsbdControlTransfer017
 * @tc.desc: Test functions to ControlTransfer(const UsbDev &dev, UsbCtrlTransfer &ctrl, std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdControlTransfer017, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdControlTransfer017 : ControlTransfer-----------");
    dev.busNum = 255;
    dev.devAddr = 2;
    uint32_t length = 255;
    uint8_t buffer[255] = {0};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    struct UsbCtrlTransfer ctrlparmas = {0b10000010, 0, 0, 0, 1000};
    auto ret = UsbdClient::ControlTransfer(dev, ctrlparmas, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer017 %{public}d length=%{public}d", __LINE__, bufferdata.size());
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer017 %{public}d ControlTransfer=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdControlTransfer017 : ControlTransfer-----------");
}

/**
 * @tc.name: UsbdControlTransfer018
 * @tc.desc: Test functions to ControlTransfer(const UsbDev &dev, UsbCtrlTransfer &ctrl, std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdControlTransfer018, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdControlTransfer018 : ControlTransfer-----------");
    dev.busNum = 1;
    dev.devAddr = 255;
    uint32_t length = 255;
    uint8_t buffer[255] = {0};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    struct UsbCtrlTransfer ctrlparmas = {0b10000010, 0, 0, 0, 1000};
    auto ret = UsbdClient::ControlTransfer(dev, ctrlparmas, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer018 %{public}d length=%{public}d", __LINE__, bufferdata.size());
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer018 %{public}d ControlTransfer=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdControlTransfer018 : ControlTransfer-----------");
}

/**
 * @tc.name: UsbdControlTransfer019
 * @tc.desc: Test functions to ControlTransfer(const UsbDev &dev, UsbCtrlTransfer &ctrl, std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdControlTransfer019, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdControlTransfer019 : ControlTransfer-----------");
    int idex = 4;
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer019 %{public}d idex=%{public}d", __LINE__, idex);
    dev.busNum = 1;
    dev.devAddr = 2;
    uint32_t length = 255;
    uint8_t buffer[255] = {};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    struct UsbCtrlTransfer ctrlparmas = {0b10000010, 0X0C, 0, 0, 1000};
    auto ret = UsbdClient::ControlTransfer(dev, ctrlparmas, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer019 %{public}d length=%{public}d", __LINE__, bufferdata.size());
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer019 %{public}d ControlTransfer=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    PrintBuffer("控制传输", bufferdata.data(), bufferdata.size());
    HDF_LOGI("-----------Case End : UsbdControlTransfer019 : ControlTransfer-----------");
}

/**
 * @tc.name: UsbdControlTransfer020
 * @tc.desc: Test functions to ControlTransfer(const UsbDev &dev, UsbCtrlTransfer &ctrl, std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdControlTransfer020, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdControlTransfer020 : ControlTransfer-----------");
    int idex = 4;
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer020 %{public}d idex=%{public}d", __LINE__, idex);
    dev.busNum = 255;
    dev.devAddr = 2;
    uint32_t length = 255;
    uint8_t buffer[255] = {};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    struct UsbCtrlTransfer ctrlparmas = {0b10000010, 0X0C, 0, 0, 1000};
    auto ret = UsbdClient::ControlTransfer(dev, ctrlparmas, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer020 %{public}d length=%{public}d", __LINE__, bufferdata.size());
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer020 %{public}d ControlTransfer=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdControlTransfer020 : ControlTransfer-----------");
}

/**
 * @tc.name: UsbdControlTransfer021
 * @tc.desc: Test functions to ControlTransfer(const UsbDev &dev, UsbCtrlTransfer &ctrl, std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdControlTransfer021, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdControlTransfer021 : ControlTransfer-----------");
    int idex = 4;
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer021 %{public}d idex=%{public}d", __LINE__, idex);
    dev.busNum = 1;
    dev.devAddr = 255;
    uint32_t length = 255;
    uint8_t buffer[255] = {};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    struct UsbCtrlTransfer ctrlparmas = {0b10000010, 0X0C, 0, 0, 1000};
    auto ret = UsbdClient::ControlTransfer(dev, ctrlparmas, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer021 %{public}d length=%{public}d", __LINE__, bufferdata.size());
    HDF_LOGI("UsbdTransferTest::UsbdControlTransfer021 %{public}d ControlTransfer=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdControlTransfer021 : ControlTransfer-----------");
}

/**
 * @tc.name: UsbdBulkTransferRead001
 * @tc.desc: Test functions to BulkTransferRead(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdBulkTransferRead001, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdBulkTransferRead001 : BulkTransferRead-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t interfaceid = 1;
    uint8_t pointid = 129;
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferRead001 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferRead001 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    uint8_t buffer[255] = {0};
    uint32_t length = 255;
    struct UsbPipe pipe = {interfaceid, pointid};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::BulkTransferRead(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferRead001 %{public}d UsbdBulkTransferRead=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    PrintBuffer("bulk read ", bufferdata.data(), bufferdata.size());
    HDF_LOGI("-----------Case End : UsbdBulkTransferRead001 : BulkTransferRead-----------");
}

/**
 * @tc.name: UsbdBulkTransferRead002
 * @tc.desc: Test functions to BulkTransferRead(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdBulkTransferRead002, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdBulkTransferRead002 : BulkTransferRead-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t interfaceid = 1;
    uint8_t pointid = 129;
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferRead002 %{public}d interfaceid=%{public}d", __LINE__, interfaceid);
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferRead002 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferRead002 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    dev.busNum = 222;
    uint32_t length = 100;
    uint8_t buffer[100] = {0};
    struct UsbPipe pipe = {interfaceid, pointid};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::BulkTransferRead(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferRead002 %{public}d UsbdBulkTransferRead=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdBulkTransferRead002 : BulkTransferRead-----------");
}

/**
 * @tc.name: UsbdBulkTransferRead003
 * @tc.desc: Test functions to BulkTransferRead(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdBulkTransferRead003, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdBulkTransferRead003 : BulkTransferRead-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t interfaceid = 1;
    uint8_t pointid = 129;
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferRead003 %{public}d interfaceid=%{public}d", __LINE__, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferRead003 %{public}d pionid=%{public}d", __LINE__, pointid);
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferRead003 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferRead003 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    dev.devAddr = 244;
    uint32_t length = 100;
    uint8_t buffer[100] = {0};
    struct UsbPipe pipe = {interfaceid, pointid};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::BulkTransferRead(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferRead003 %{public}d UsbdBulkTransferRead=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdBulkTransferRead003 : BulkTransferRead-----------");
}

/**
 * @tc.name: UsbdBulkTransferRead004
 * @tc.desc: Test functions to BulkTransferRead(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdBulkTransferRead004, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdBulkTransferRead004 : BulkTransferRead-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t interfaceid = 1;
    uint8_t pointid = 129;
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferRead004 %{public}d interfaceid=%{public}d", __LINE__, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferRead004 %{public}d pionid=%{public}d", __LINE__, pointid);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferRead004 %{public}d pionid=%{public}d", __LINE__, pointid);
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferRead004 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferRead004 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    uint8_t buffer[100] = {0};
    uint32_t length = 100;
    struct UsbPipe pipe = {interfaceid, pointid};
    pipe.interfaceId = 244;
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::BulkTransferRead(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferRead004 %{public}d UsbdBulkTransferRead=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdBulkTransferRead004 : BulkTransferRead-----------");
}

/**
 * @tc.name: UsbdBulkTransferRead005
 * @tc.desc: Test functions to BulkTransferRead(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdBulkTransferRead005, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdBulkTransferRead005 : BulkTransferRead-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t interfaceid = 1;
    uint8_t pointid = 129;
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferRead005 %{public}d interfaceid=%{public}d", __LINE__, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferRead005 %{public}d pionid=%{public}d", __LINE__, pointid);
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferRead005 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferRead005 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    uint32_t length = 100;
    uint8_t buffer[100] = {};
    struct UsbPipe pipe = {interfaceid, pointid};
    pipe.endpointId = 244;
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::BulkTransferRead(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferRead005 %{public}d UsbdBulkTransferRead=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdBulkTransferRead005 : BulkTransferRead-----------");
}

/**
 * @tc.name: UsbdBulkTransferWrite001
 * @tc.desc: Test functions to BulkTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdBulkTransferWrite001, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdBulkTransferWrite001 : BulkTransferWrite-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t interfaceid = 1;
    uint8_t pointid = 1;
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferWrite001 %{public}d interfaceid=%{public}d", __LINE__, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferWrite001 %{public}d pionid=%{public}d", __LINE__, pointid);
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferWrite001 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferWrite001 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    uint32_t length = 100;
    uint8_t buffer[100] = "hello world bulk writ01";
    struct UsbPipe pipe = {interfaceid, pointid};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::BulkTransferWrite(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferWrite001 %{public}d BulkTransferWrite=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    PrintBuffer("bulk write ", bufferdata.data(), bufferdata.size());
    HDF_LOGI("-----------Case End : UsbdBulkTransferWrite001 : BulkTransferWrite-----------");
}

/**
 * @tc.name: UsbdBulkTransferWrite002
 * @tc.desc: Test functions to BulkTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdBulkTransferWrite002, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdBulkTransferWrite002 : BulkTransferWrite-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t interfaceid = 1;
    uint8_t pointid = 1;
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferWrite002 %{public}d interfaceid=%{public}d", __LINE__, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferWrite002 %{public}d pionid=%{public}d", __LINE__, pointid);
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferWrite002 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferWrite002 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    struct UsbPipe pipe = {interfaceid, pointid};
    dev.busNum = 99;
    uint32_t length = 100;
    uint8_t buffer[100] = "hello world bulk writ02";
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::BulkTransferWrite(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferWrite002 %{public}d BulkTransferWrite=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdBulkTransferWrite002 : BulkTransferWrite-----------");
}

/**
 * @tc.name: UsbdBulkTransferWrite003
 * @tc.desc: Test functions to BulkTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdBulkTransferWrite003, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdBulkTransferWrite003 : BulkTransferWrite-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t interfaceid = 1;
    uint8_t pointid = 1;
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferWrite003 %{public}d interfaceid=%{public}d", __LINE__, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferWrite003 %{public}d pionid=%{public}d", __LINE__, pointid);
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferWrite003 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferWrite003 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    dev.devAddr = 244;
    uint32_t length = 100;
    uint8_t buffer[100] = "hello world bulk writ03";
    struct UsbPipe pipe = {interfaceid, pointid};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::BulkTransferWrite(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferWrite003 %{public}d BulkTransferWrite=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdBulkTransferWrite003 : BulkTransferWrite-----------");
}

/**
 * @tc.name: UsbdBulkTransferWrite004
 * @tc.desc: Test functions to BulkTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdBulkTransferWrite004, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdBulkTransferWrite004 : BulkTransferWrite-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t interfaceid = 1;
    uint8_t pointid = 1;
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferWrite004 %{public}d interfaceid=%{public}d", __LINE__, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferWrite004 %{public}d pionid=%{public}d", __LINE__, pointid);
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferWrite004 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferWrite004 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    struct UsbPipe pipe = {interfaceid, pointid};
    pipe.interfaceId = 255;
    uint32_t length = 100;
    uint8_t buffer[100] = "hello world bulk writ04";
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::BulkTransferWrite(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferWrite004 %{public}d BulkTransferWrite=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdBulkTransferWrite004 : BulkTransferWrite-----------");
}

/**
 * @tc.name: UsbdBulkTransferWrite005
 * @tc.desc: Test functions to BulkTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdBulkTransferWrite005, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdBulkTransferWrite005 : BulkTransferWrite-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t interfaceid = 1;
    uint8_t pointid = 1;
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferWrite005 %{public}d interfaceid=%{public}d", __LINE__, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferWrite005 %{public}d pionid=%{public}d", __LINE__, pointid);
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferWrite005 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferWrite005 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    struct UsbPipe pipe = {interfaceid, pointid};
    pipe.endpointId = 255;
    uint32_t length = 100;
    uint8_t buffer[100] = "hello world bulk writ05";
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::BulkTransferWrite(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferWrite005 %{public}d BulkTransferWrite=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdBulkTransferWrite005 : BulkTransferWrite-----------");
}

/**
 * @tc.name: UsbdBulkTransferWrite006
 * @tc.desc: Test functions to BulkTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdBulkTransferWrite006, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdBulkTransferWrite006 : BulkTransferWrite-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t interfaceid = 1;
    uint8_t pointid = 1;
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferWrite006 %{public}d interfaceid=%{public}d", __LINE__, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferWrite006 %{public}d pionid=%{public}d", __LINE__, pointid);
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferWrite006 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferWrite006 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    struct UsbPipe pipe = {interfaceid, pointid};
    dev.busNum = 99;
    dev.devAddr = 99;
    uint32_t length = 100;
    uint8_t buffer[100] = "hello world bulk writ06";
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::BulkTransferWrite(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferWrite006 %{public}d BulkTransferWrite=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdBulkTransferWrite006 : BulkTransferWrite-----------");
}

/**
 * @tc.name: UsbdBulkTransferWrite007
 * @tc.desc: Test functions to BulkTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdBulkTransferWrite007, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdBulkTransferWrite007 : BulkTransferWrite-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t interfaceid = 1;
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferWrite007 %{public}d interfaceid=%{public}d", __LINE__, interfaceid);
    uint8_t pointid = 99;
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferWrite007 %{public}d pionid=%{public}d", __LINE__, pointid);
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferWrite007 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferWrite007 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    struct UsbPipe pipe = {interfaceid, pointid};
    uint32_t length = 100;
    uint8_t buffer[100] = "hello world bulk writ07";
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::BulkTransferWrite(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferWrite007 %{public}d BulkTransferWrite=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdBulkTransferWrite007 : BulkTransferWrite-----------");
}

/**
 * @tc.name: UsbdBulkTransferWrite008
 * @tc.desc: Test functions to BulkTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdBulkTransferWrite008, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdBulkTransferWrite008 : BulkTransferWrite-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t interfaceid = 1;
    uint8_t pointid = 1;
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferWrite008 %{public}d interfaceid=%{public}d", __LINE__, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferWrite008 %{public}d pionid=%{public}d", __LINE__, pointid);
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferWrite008 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferWrite008 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    struct UsbPipe pipe = {interfaceid, pointid};
    uint32_t length = 100;
    uint8_t buffer[100] = "hello world bulk writ08";
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::BulkTransferWrite(dev, pipe, -1, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdBulkTransferWrite008 %{public}d BulkTransferWrite=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    PrintBuffer("bulk write ", bufferdata.data(), bufferdata.size());
    HDF_LOGI("-----------Case End : UsbdBulkTransferWrite008 : BulkTransferWrite-----------");
}

/**
 * @tc.name: InterruptTransferRead001
 * @tc.desc: Test functions to InterruptTransferRead(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdInterruptTransferRead001, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdInterruptTransferRead001 : InterruptTransferRead-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t interfaceid = 1;
    uint8_t pointid = 129;
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferRead001 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferRead001 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    uint8_t buffer[255] = {0};
    uint32_t length = 255;
    struct UsbPipe pipe = {interfaceid, pointid};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::InterruptTransferRead(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferRead001 %{public}d UsbdInterruptTransferRead=%{public}d", __LINE__,
             ret);
    ASSERT_TRUE(ret == 0);
    PrintBuffer("bulk read ", bufferdata.data(), bufferdata.size());
    HDF_LOGI("-----------Case End : UsbdInterruptTransferRead001 : InterruptTransferRead-----------");
}

/**
 * @tc.name: UsbdInterruptTransferRead002
 * @tc.desc: Test functions to InterruptTransferRead(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdInterruptTransferRead002, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdInterruptTransferRead002 : InterruptTransferRead-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t interfaceid = 1;
    uint8_t pointid = 129;
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferRead002 %{public}d interfaceid=%{public}d", __LINE__, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferRead002 %{public}d GetAddress=%{public}d", __LINE__, pointid);
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferRead002 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferRead002 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    dev.busNum = 222;
    uint32_t length = 100;
    uint8_t buffer[100] = {0};
    struct UsbPipe pipe = {interfaceid, pointid};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::InterruptTransferRead(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferRead002 %{public}d UsbdInterruptTransferRead=%{public}d", __LINE__,
             ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdInterruptTransferRead002 : InterruptTransferRead-----------");
}

/**
 * @tc.name: UsbdInterruptTransferRead003
 * @tc.desc: Test functions to InterruptTransferRead(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdInterruptTransferRead003, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdInterruptTransferRead003 : InterruptTransferRead-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t interfaceid = 1;
    uint8_t pointid = 129;
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferRead003 %{public}d interfaceid=%{public}d", __LINE__, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferRead003 %{public}d pionid=%{public}d", __LINE__, pointid);
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferRead003 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferRead003 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    dev.devAddr = 244;
    uint32_t length = 100;
    uint8_t buffer[100] = {0};
    struct UsbPipe pipe = {interfaceid, pointid};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::InterruptTransferRead(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferRead003 %{public}d UsbdInterruptTransferRead=%{public}d", __LINE__,
             ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdInterruptTransferRead003 : InterruptTransferRead-----------");
}

/**
 * @tc.name: UsbdInterruptTransferRead004
 * @tc.desc: Test functions to InterruptTransferRead(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdInterruptTransferRead004, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdInterruptTransferRead004 : InterruptTransferRead-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t interfaceid = 1;
    uint8_t pointid = 129;
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferRead004 %{public}d interfaceid=%{public}d", __LINE__, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferRead004 %{public}d pionid=%{public}d", __LINE__, pointid);
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferRead004 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferRead004 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    uint8_t buffer[100] = {0};
    uint32_t length = 100;
    struct UsbPipe pipe = {interfaceid, pointid};
    pipe.interfaceId = 244;
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::InterruptTransferRead(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferRead004 %{public}d UsbdInterruptTransferRead=%{public}d", __LINE__,
             ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdInterruptTransferRead004 : InterruptTransferRead-----------");
}

/**
 * @tc.name: UsbdInterruptTransferRead005
 * @tc.desc: Test functions to InterruptTransferRead(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdInterruptTransferRead005, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdInterruptTransferRead005 : InterruptTransferRead-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t interfaceid = 1;
    uint8_t pointid = 129;
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferRead005 %{public}d interfaceid=%{public}d", __LINE__, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferRead005 %{public}d pionid=%{public}d", __LINE__, pointid);
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferRead005 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferRead005 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    uint32_t length = 100;
    uint8_t buffer[100] = {};
    struct UsbPipe pipe = {interfaceid, pointid};
    pipe.endpointId = 244;
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::InterruptTransferRead(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferRead005 %{public}d UsbdInterruptTransferRead=%{public}d", __LINE__,
             ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdInterruptTransferRead005 : InterruptTransferRead-----------");
}

/**
 * @tc.name: UsbdInterruptTransferWrite001
 * @tc.desc: Test functions to InterruptTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdInterruptTransferWrite001, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdInterruptTransferWrite001 : InterruptTransferWrite-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t interfaceid = 1;
    uint8_t pointid = 1;
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferWrite001 %{public}d interfaceid=%{public}d", __LINE__,
             interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferWrite001 %{public}d pionid=%{public}d", __LINE__, pointid);
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferWrite001 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferWrite001 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    uint32_t length = 100;
    uint8_t buffer[100] = "hello world Interrupt writ01";
    struct UsbPipe pipe = {interfaceid, pointid};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::InterruptTransferWrite(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferWrite001 %{public}d InterruptTransferWrite=%{public}d", __LINE__,
             ret);
    ASSERT_TRUE(ret == 0);
    PrintBuffer("bulk write ", bufferdata.data(), bufferdata.size());
    HDF_LOGI("-----------Case End : UsbdInterruptTransferWrite001 : InterruptTransferWrite-----------");
}

/**
 * @tc.name: UsbdInterruptTransferWrite002
 * @tc.desc: Test functions to InterruptTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdInterruptTransferWrite002, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdInterruptTransferWrite002 : InterruptTransferWrite-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t interfaceid = 1;
    uint8_t pointid = 1;
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferWrite002 %{public}d interfaceid=%{public}d", __LINE__,
             interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferWrite002 %{public}d pionid=%{public}d", __LINE__, pointid);
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferWrite002 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferWrite002 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    struct UsbPipe pipe = {interfaceid, pointid};
    dev.busNum = 99;
    uint32_t length = 100;
    uint8_t buffer[100] = "hello world Interrupt writ02";
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::InterruptTransferWrite(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferWrite002 %{public}d InterruptTransferWrite=%{public}d", __LINE__,
             ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdInterruptTransferWrite002 : InterruptTransferWrite-----------");
}

/**
 * @tc.name: UsbdInterruptTransferWrite003
 * @tc.desc: Test functions to InterruptTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdInterruptTransferWrite003, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdInterruptTransferWrite003 : InterruptTransferWrite-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t interfaceid = 1;
    uint8_t pointid = 1;
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferWrite003 %{public}d interfaceid=%{public}d", __LINE__,
             interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferWrite003 %{public}d pionid=%{public}d", __LINE__, pointid);
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferWrite003 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferWrite003 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    dev.devAddr = 244;
    uint32_t length = 100;
    uint8_t buffer[100] = "hello world Interrupt writ03";
    struct UsbPipe pipe = {interfaceid, pointid};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::InterruptTransferWrite(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferWrite003 %{public}d InterruptTransferWrite=%{public}d", __LINE__,
             ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdInterruptTransferWrite003 : InterruptTransferWrite-----------");
}

/**
 * @tc.name: UsbdInterruptTransferWrite004
 * @tc.desc: Test functions to InterruptTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdInterruptTransferWrite004, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdInterruptTransferWrite004 : InterruptTransferWrite-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t interfaceid = 1;
    uint8_t pointid = 1;
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferWrite004 %{public}d interfaceid=%{public}d", __LINE__,
             interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferWrite004 %{public}d pionid=%{public}d", __LINE__, pointid);
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferWrite004 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferWrite004 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    struct UsbPipe pipe = {interfaceid, pointid};
    pipe.interfaceId = 255;
    uint32_t length = 100;
    uint8_t buffer[100] = "hello world Interrupt writ04";
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::InterruptTransferWrite(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferWrite004 %{public}d InterruptTransferWrite=%{public}d", __LINE__,
             ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdInterruptTransferWrite004 : InterruptTransferWrite-----------");
}

/**
 * @tc.name: UsbdInterruptTransferWrite005
 * @tc.desc: Test functions to InterruptTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdInterruptTransferWrite005, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdInterruptTransferWrite005 : InterruptTransferWrite-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t interfaceid = 1;
    uint8_t pointid = 1;
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferWrite005 %{public}d interfaceid=%{public}d", __LINE__,
             interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferWrite005 %{public}d pionid=%{public}d", __LINE__, pointid);
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferWrite005 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferWrite005 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    struct UsbPipe pipe = {interfaceid, pointid};
    pipe.endpointId = 255;
    uint32_t length = 100;
    uint8_t buffer[100] = "hello world Interrupt writ05";
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::InterruptTransferWrite(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferWrite005 %{public}d InterruptTransferWrite=%{public}d", __LINE__,
             ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdInterruptTransferWrite005 : InterruptTransferWrite-----------");
}

/**
 * @tc.name: UsbdInterruptTransferWrite006
 * @tc.desc: Test functions to InterruptTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdInterruptTransferWrite006, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdInterruptTransferWrite006 : InterruptTransferWrite-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t interfaceid = 1;
    uint8_t pointid = 1;
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferWrite006 %{public}d interfaceid=%{public}d", __LINE__,
             interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferWrite006 %{public}d pionid=%{public}d", __LINE__, pointid);
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferWrite006 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferWrite006 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    struct UsbPipe pipe = {interfaceid, pointid};
    dev.busNum = 99;
    dev.devAddr = 99;
    uint32_t length = 100;
    uint8_t buffer[100] = "hello world Interrupt writ06";
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::InterruptTransferWrite(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferWrite006 %{public}d InterruptTransferWrite=%{public}d", __LINE__,
             ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdInterruptTransferWrite006 : InterruptTransferWrite-----------");
}

/**
 * @tc.name: UsbdInterruptTransferWrite007
 * @tc.desc: Test functions to InterruptTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdInterruptTransferWrite007, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdInterruptTransferWrite007 : InterruptTransferWrite-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t interfaceid = 1;
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferWrite007 %{public}d interfaceid=%{public}d", __LINE__,
             interfaceid);
    uint8_t pointid = 99;
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferWrite007 %{public}d pionid=%{public}d", __LINE__, pointid);
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferWrite007 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferWrite007 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    struct UsbPipe pipe = {interfaceid, pointid};
    uint32_t length = 100;
    uint8_t buffer[100] = "hello world Interrupt writ07";
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::InterruptTransferWrite(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferWrite007 %{public}d InterruptTransferWrite=%{public}d", __LINE__,
             ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdInterruptTransferWrite007 : InterruptTransferWrite-----------");
}

/**
 * @tc.name: UsbdInterruptTransferWrite008
 * @tc.desc: Test functions to InterruptTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdInterruptTransferWrite008, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdInterruptTransferWrite008 : InterruptTransferWrite-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t interfaceid = 1;
    uint8_t pointid = 1;
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferWrite008 %{public}d interfaceid=%{public}d", __LINE__,
             interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferWrite008 %{public}d pionid=%{public}d", __LINE__, pointid);
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferWrite008 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferWrite008 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    struct UsbPipe pipe = {interfaceid, pointid};
    uint32_t length = 100;
    uint8_t buffer[100] = "hello world Interrupt writ08";
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::InterruptTransferWrite(dev, pipe, -1, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdInterruptTransferWrite008 %{public}d InterruptTransferWrite=%{public}d", __LINE__,
             ret);
    ASSERT_TRUE(ret == 0);
    PrintBuffer("bulk write ", bufferdata.data(), bufferdata.size());
    HDF_LOGI("-----------Case End : UsbdInterruptTransferWrite008 : InterruptTransferWrite-----------");
}

/**
 * @tc.name: UsbdIsoTransferRead001
 * @tc.desc: Test functions to IsoTransferRead(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdIsoTransferRead001, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdIsoTransferRead001 : IsoTransferRead-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t interfaceid = 1;
    uint8_t pointid = 129;
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferRead001 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferRead001 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    uint8_t buffer[255] = {0};
    uint32_t length = 255;
    struct UsbPipe pipe = {interfaceid, pointid};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::IsoTransferRead(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferRead001 %{public}d UsbdIsoTransferRead=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    PrintBuffer("bulk read ", bufferdata.data(), bufferdata.size());
    HDF_LOGI("-----------Case End : UsbdIsoTransferRead001 : IsoTransferRead-----------");
}

/**
 * @tc.name: UsbdIsoTransferRead002
 * @tc.desc: Test functions to IsoTransferRead(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdIsoTransferRead002, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdIsoTransferRead002 : IsoTransferRead-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t interfaceid = 1;
    uint8_t pointid = 129;
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferRead002 %{public}d interfaceid=%{public}d", __LINE__, interfaceid);
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferRead002 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferRead002 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    dev.busNum = 222;
    uint32_t length = 100;
    uint8_t buffer[100] = {0};
    struct UsbPipe pipe = {interfaceid, pointid};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::IsoTransferRead(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferRead002 %{public}d UsbdIsoTransferRead=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdIsoTransferRead002 : IsoTransferRead-----------");
}

/**
 * @tc.name: UsbdIsoTransferRead003
 * @tc.desc: Test functions to IsoTransferRead(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdIsoTransferRead003, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdIsoTransferRead003 : IsoTransferRead-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t interfaceid = 1;
    uint8_t pointid = 129;
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferRead003 %{public}d interfaceid=%{public}d", __LINE__, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferRead003 %{public}d pionid=%{public}d", __LINE__, pointid);
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferRead003 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferRead003 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    dev.devAddr = 244;
    uint32_t length = 100;
    uint8_t buffer[100] = {0};
    struct UsbPipe pipe = {interfaceid, pointid};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::IsoTransferRead(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferRead003 %{public}d UsbdIsoTransferRead=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdIsoTransferRead003 : IsoTransferRead-----------");
}

/**
 * @tc.name: UsbdIsoTransferRead004
 * @tc.desc: Test functions to IsoTransferRead(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdIsoTransferRead004, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdIsoTransferRead004 : IsoTransferRead-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t interfaceid = 1;
    uint8_t pointid = 129;
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferRead004 %{public}d interfaceid=%{public}d", __LINE__, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferRead004 %{public}d pionid=%{public}d", __LINE__, pointid);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferRead004 %{public}d pionid=%{public}d", __LINE__, pointid);
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferRead004 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferRead004 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    uint8_t buffer[100] = {0};
    uint32_t length = 100;
    struct UsbPipe pipe = {interfaceid, pointid};
    pipe.interfaceId = 244;
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::IsoTransferRead(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferRead004 %{public}d UsbdIsoTransferRead=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdIsoTransferRead004 : IsoTransferRead-----------");
}

/**
 * @tc.name: UsbdIsoTransferRead005
 * @tc.desc: Test functions to IsoTransferRead(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdIsoTransferRead005, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdIsoTransferRead005 : IsoTransferRead-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t interfaceid = 1;
    uint8_t pointid = 129;
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferRead005 %{public}d interfaceid=%{public}d", __LINE__, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferRead005 %{public}d pionid=%{public}d", __LINE__, pointid);
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferRead005 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferRead005 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    uint32_t length = 100;
    uint8_t buffer[100] = {};
    struct UsbPipe pipe = {interfaceid, pointid};
    pipe.endpointId = 244;
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::IsoTransferRead(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferRead005 %{public}d UsbdIsoTransferRead=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdIsoTransferRead005 : IsoTransferRead-----------");
}

/**
 * @tc.name: UsbdIsoTransferWrite001
 * @tc.desc: Test functions to IsoTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdIsoTransferWrite001, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdIsoTransferWrite001 : IsoTransferWrite-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t interfaceid = 1;
    uint8_t pointid = 1;
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferWrite001 %{public}d interfaceid=%{public}d", __LINE__, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferWrite001 %{public}d pionid=%{public}d", __LINE__, pointid);
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferWrite001 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferWrite001 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    uint32_t length = 100;
    uint8_t buffer[100] = "hello world Iso writ01";
    struct UsbPipe pipe = {interfaceid, pointid};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::IsoTransferWrite(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferWrite001 %{public}d IsoTransferWrite=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    PrintBuffer("bulk write ", bufferdata.data(), bufferdata.size());
    HDF_LOGI("-----------Case End : UsbdIsoTransferWrite001 : IsoTransferWrite-----------");
}

/**
 * @tc.name: UsbdIsoTransferWrite002
 * @tc.desc: Test functions to IsoTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdIsoTransferWrite002, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdIsoTransferWrite002 : IsoTransferWrite-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t interfaceid = 1;
    uint8_t pointid = 1;
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferWrite002 %{public}d interfaceid=%{public}d", __LINE__, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferWrite002 %{public}d pionid=%{public}d", __LINE__, pointid);
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferWrite002 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferWrite002 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    struct UsbPipe pipe = {interfaceid, pointid};
    dev.busNum = 99;
    uint32_t length = 100;
    uint8_t buffer[100] = "hello world Iso writ02";
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::IsoTransferWrite(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferWrite002 %{public}d IsoTransferWrite=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdIsoTransferWrite002 : IsoTransferWrite-----------");
}

/**
 * @tc.name: UsbdIsoTransferWrite003
 * @tc.desc: Test functions to IsoTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdIsoTransferWrite003, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdIsoTransferWrite003 : IsoTransferWrite-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t interfaceid = 1;
    uint8_t pointid = 1;
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferWrite003 %{public}d interfaceid=%{public}d", __LINE__, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferWrite003 %{public}d pionid=%{public}d", __LINE__, pointid);
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferWrite003 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferWrite003 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    dev.devAddr = 244;
    uint32_t length = 100;
    uint8_t buffer[100] = "hello world Iso writ03";
    struct UsbPipe pipe = {interfaceid, pointid};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::IsoTransferWrite(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferWrite003 %{public}d IsoTransferWrite=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdIsoTransferWrite003 : IsoTransferWrite-----------");
}

/**
 * @tc.name: UsbdIsoTransferWrite004
 * @tc.desc: Test functions to IsoTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdIsoTransferWrite004, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdIsoTransferWrite004 : IsoTransferWrite-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t interfaceid = 1;
    uint8_t pointid = 1;
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferWrite004 %{public}d interfaceid=%{public}d", __LINE__, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferWrite004 %{public}d pionid=%{public}d", __LINE__, pointid);
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferWrite004 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferWrite004 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    struct UsbPipe pipe = {interfaceid, pointid};
    pipe.interfaceId = 255;
    uint32_t length = 100;
    uint8_t buffer[100] = "hello world Iso writ04";
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::IsoTransferWrite(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferWrite004 %{public}d IsoTransferWrite=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdIsoTransferWrite004 : IsoTransferWrite-----------");
}

/**
 * @tc.name: UsbdIsoTransferWrite005
 * @tc.desc: Test functions to IsoTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdIsoTransferWrite005, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdIsoTransferWrite005 : IsoTransferWrite-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t interfaceid = 1;
    uint8_t pointid = 1;
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferWrite005 %{public}d interfaceid=%{public}d", __LINE__, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferWrite005 %{public}d pionid=%{public}d", __LINE__, pointid);
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferWrite005 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferWrite005 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    struct UsbPipe pipe = {interfaceid, pointid};
    pipe.endpointId = 255;
    uint32_t length = 100;
    uint8_t buffer[100] = "hello world Iso writ05";
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::IsoTransferWrite(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferWrite005 %{public}d IsoTransferWrite=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdIsoTransferWrite005 : IsoTransferWrite-----------");
}

/**
 * @tc.name: UsbdIsoTransferWrite006
 * @tc.desc: Test functions to IsoTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdIsoTransferWrite006, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdIsoTransferWrite006 : IsoTransferWrite-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t interfaceid = 1;
    uint8_t pointid = 1;
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferWrite006 %{public}d interfaceid=%{public}d", __LINE__, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferWrite006 %{public}d pionid=%{public}d", __LINE__, pointid);
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferWrite006 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferWrite006 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    struct UsbPipe pipe = {interfaceid, pointid};
    dev.busNum = 99;
    dev.devAddr = 99;
    uint32_t length = 100;
    uint8_t buffer[100] = "hello world Iso writ06";
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::IsoTransferWrite(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferWrite006 %{public}d IsoTransferWrite=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdIsoTransferWrite006 : IsoTransferWrite-----------");
}

/**
 * @tc.name: UsbdIsoTransferWrite007
 * @tc.desc: Test functions to IsoTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdIsoTransferWrite007, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdIsoTransferWrite007 : IsoTransferWrite-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t interfaceid = 1;
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferWrite007 %{public}d interfaceid=%{public}d", __LINE__, interfaceid);
    uint8_t pointid = 99;
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferWrite007 %{public}d pionid=%{public}d", __LINE__, pointid);
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferWrite007 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferWrite007 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    struct UsbPipe pipe = {interfaceid, pointid};
    uint32_t length = 100;
    uint8_t buffer[100] = "hello world Iso writ07";
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::IsoTransferWrite(dev, pipe, 1000, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferWrite007 %{public}d IsoTransferWrite=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdIsoTransferWrite007 : IsoTransferWrite-----------");
}

/**
 * @tc.name: UsbdIsoTransferWrite008
 * @tc.desc: Test functions to IsoTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
 * std::vector<uint8_t> &data);
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdIsoTransferWrite008, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdIsoTransferWrite008 : IsoTransferWrite-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t interfaceid = 1;
    uint8_t pointid = 1;
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferWrite008 %{public}d interfaceid=%{public}d", __LINE__, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferWrite008 %{public}d pionid=%{public}d", __LINE__, pointid);
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferWrite008 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferWrite008 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    struct UsbPipe pipe = {interfaceid, pointid};
    uint32_t length = 100;
    uint8_t buffer[100] = "hello world Iso writ08";
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::IsoTransferWrite(dev, pipe, -1, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdIsoTransferWrite008 %{public}d IsoTransferWrite=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    PrintBuffer("bulk write ", bufferdata.data(), bufferdata.size());
    HDF_LOGI("-----------Case End : UsbdIsoTransferWrite008 : IsoTransferWrite-----------");
}

/**
 * @tc.name: UsbdConfig001
 * @tc.desc: Test functions to SetConfig
 * @tc.desc: int32_t SetConfig(const UsbDev &dev, uint8_t configIndex);
 * @tc.desc: 正向测试：参数正确
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdSetConfig001, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdConfig001 : SetConfig-----------");
    uint8_t busNum = 1;
    uint8_t devAddr = 2;
    uint8_t configIndex = 1;
    HDF_LOGI("UsbdTransferTest::UsbdSetConfigConfig001 %{public}d GetBusNum=%{public}d GetDevAddr=%{public}d", __LINE__,
             1, 2);
    HDF_LOGI("UsbdTransferTest::UsbdSetConfigConfig001 %{public}d config.GetId=%{public}d", __LINE__, configIndex);
    struct UsbDev dev = {busNum, devAddr};
    auto ret = UsbdClient::SetConfig(dev, configIndex);
    HDF_LOGI("UsbdTransferTest::UsbdSetConfigConfig001 %{public}d SetConfig=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    HDF_LOGI("-----------Case End : UsbdConfig001 : SetConfig-----------");
}

/**
 * @tc.name: UsbdConfig002
 * @tc.desc: Test functions to SetConfig
 * @tc.desc: int32_t SetConfig(const UsbDev &dev, uint8_t configIndex);
 * @tc.desc: 反向测试：参数异常，busNum错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdSetConfig002, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdSetConfig002 : SetConfig-----------");
    uint8_t busNum = 222;
    uint8_t devAddr = 2;
    uint8_t configIndex = 1;
    dev = {busNum, devAddr};
    auto ret = UsbdClient::SetConfig(dev, configIndex);
    HDF_LOGI("UsbdTransferTest::UsbdSetConfig002 %{public}d SetConfig=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdConfig002 : SetConfig-----------");
}

/**
 * @tc.name: UsbdConfig003
 * @tc.desc: Test functions to SetConfig
 * @tc.desc: int32_t SetConfig(const UsbDev &dev, uint8_t configIndex);
 * @tc.desc: 反向测试：参数异常，devAddr错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdSetConfig003, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdConfig003 : SetConfig-----------");
    uint8_t busNum = 1;
    uint8_t devAddr = 222;
    uint8_t configIndex = 1;
    dev = {busNum, devAddr};
    auto ret = UsbdClient::SetConfig(dev, configIndex);
    HDF_LOGI("UsbdTransferTest::UsbdSetConfig003 %{public}d SetConfig=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdConfig003 : SetConfig-----------");
}

/**
 * @tc.name: UsbdConfig004
 * @tc.desc: Test functions to SetConfig
 * @tc.desc: int32_t SetConfig(const UsbDev &dev, uint8_t configIndex);
 * @tc.desc: 反向测试：参数异常，configIndex错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdSetConfig004, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdConfig004 : SetConfig-----------");
    uint8_t busNum = 1;
    uint8_t devAddr = 2;
    uint8_t configIndex = 222;
    dev = {busNum, devAddr};
    auto ret = UsbdClient::SetConfig(dev, configIndex);
    HDF_LOGI("UsbdTransferTest::UsbdSetConfig004 %{public}d SetConfig=%{public}d", __LINE__, ret);
    HDF_LOGI("UsbdTransferTest::UsbdSetConfig004 %{public}d configIndex=%{public}d", __LINE__, configIndex);
    ASSERT_TRUE(ret != 0);
    configIndex = 1;
    ret = UsbdClient::SetConfig(dev, configIndex);
    HDF_LOGI("UsbdTransferTest::UsbdSetConfig004 %{public}d SetConfig=%{public}d", __LINE__, ret);
    HDF_LOGI("UsbdTransferTest::UsbdSetConfig004 %{public}d configIndex=%{public}d", __LINE__, configIndex);
    ASSERT_TRUE(ret == 0);
    HDF_LOGI("-----------Case End : UsbdConfig004 : SetConfig-----------");
}

/**
 * @tc.name: UsbdConfig005
 * @tc.desc: Test functions to SetConfig
 * @tc.desc: int32_t SetConfig(const UsbDev &dev, uint8_t configIndex);
 * @tc.desc: 反向测试：参数异常，busNum、devAddr错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdSetConfig005, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdConfig005 : SetConfig-----------");
    uint8_t busNum = 222;
    uint8_t devAddr = 222;
    uint8_t configIndex = 1;
    dev = {busNum, devAddr};
    auto ret = UsbdClient::SetConfig(dev, configIndex);
    HDF_LOGI("UsbdTransferTest::UsbdSetConfig005 %{public}d SetConfig=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdConfig005 : SetConfig-----------");
}

/**
 * @tc.name: UsbdConfig006
 * @tc.desc: Test functions to SetConfig
 * @tc.desc: int32_t SetConfig(const UsbDev &dev, uint8_t configIndex);
 * @tc.desc: 反向测试：参数异常，busNum、configIndex错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdSetConfig006, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdConfig006 : SetConfig-----------");
    uint8_t busNum = 222;
    uint8_t devAddr = 2;
    uint8_t configIndex = 222;
    dev = {busNum, devAddr};
    auto ret = UsbdClient::SetConfig(dev, configIndex);
    HDF_LOGI("UsbdTransferTest::UsbdSetConfig006 %{public}d SetConfig=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdConfig006 : SetConfig-----------");
}

/**
 * @tc.name: UsbdConfig007
 * @tc.desc: Test functions to SetConfig
 * @tc.desc: int32_t SetConfig(const UsbDev &dev, uint8_t configIndex);
 * @tc.desc: 反向测试：devAddr、configIndex错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdSetConfig007, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdConfig007 : SetConfig-----------");
    uint8_t busNum = 1;
    uint8_t devAddr = 222;
    uint8_t configIndex = 222;
    dev = {busNum, devAddr};
    auto ret = UsbdClient::SetConfig(dev, configIndex);
    HDF_LOGI("UsbdTransferTest::UsbdSetConfig007 %{public}d SetConfig=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdConfig007 : SetConfig-----------");
}

/**
 * @tc.name: UsbdConfig008
 * @tc.desc: Test functions to SetConfig
 * @tc.desc: int32_t SetConfig(const UsbDev &dev, uint8_t configIndex);
 * @tc.desc: 反向测试：busNum、devAddr、configIndex错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdSetConfig008, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdConfig008 : SetConfig-----------");
    uint8_t busNum = 222;
    uint8_t devAddr = 222;
    uint8_t configIndex = 222;
    dev = {busNum, devAddr};
    auto ret = UsbdClient::SetConfig(dev, configIndex);
    HDF_LOGI("UsbdTransferTest::UsbdSetConfig008 %{public}d SetConfig=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdConfig008 : SetConfig-----------");
}

/**********************************************************************************************************/

/**
 * @tc.name: UsbdConfig001
 * @tc.desc: Test functions to GetConfig
 * @tc.desc: int32_t GetConfig(const UsbDev &dev, uint8_t &configIndex);
 * @tc.desc: 正向测试：参数正确
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdGetConfig001, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdConfig001 : GetConfig-----------");
    uint8_t busNum = 1;
    uint8_t devAddr = 2;
    uint8_t configIndex = 1;
    dev = {busNum, devAddr};
    auto ret = UsbdClient::GetConfig(dev, configIndex);
    HDF_LOGI("UsbdTransferTest::UsbdGetConfig001 %{public}d GetConfig=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    HDF_LOGI("-----------Case End : UsbdConfig001 : GetConfig-----------");
}

/**
 * @tc.name: UsbdConfig002
 * @tc.desc: Test functions to GetConfig
 * @tc.desc: int32_t GetConfig(const UsbDev &dev, uint8_t &configIndex);
 * @tc.desc: 反向测试：参数异常，busNum错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdGetConfig002, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdConfig002 : GetConfig-----------");
    uint8_t busNum = 222;
    uint8_t devAddr = 2;
    uint8_t configIndex = 1;
    dev = {busNum, devAddr};
    auto ret = UsbdClient::GetConfig(dev, configIndex);
    HDF_LOGI("UsbdTransferTest::UsbdGetConfig002 %{public}d GetConfig=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdConfig002 : GetConfig-----------");
}

/**
 * @tc.name: UsbdConfig003
 * @tc.desc: Test functions to GetConfig
 * @tc.desc: int32_t GetConfig(const UsbDev &dev, uint8_t &configIndex);
 * @tc.desc: 反向测试：参数异常，devAddr错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdGetConfig003, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdConfig003 : GetConfig-----------");
    uint8_t busNum = 1;
    uint8_t devAddr = 222;
    uint8_t configIndex = 1;
    dev = {busNum, devAddr};
    auto ret = UsbdClient::GetConfig(dev, configIndex);
    HDF_LOGI("UsbdTransferTest::UsbdGetConfig003 %{public}d GetConfig=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdConfig003 : GetConfig-----------");
}

/**
 * @tc.name: UsbdConfig004
 * @tc.desc: Test functions to GetConfig
 * @tc.desc: int32_t GetConfig(const UsbDev &dev, uint8_t &configIndex);
 * @tc.desc: 反向测试：参数异常，busNum、devAddr错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdGetConfig004, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdConfig004 : GetConfig-----------");
    uint8_t busNum = 222;
    uint8_t devAddr = 222;
    uint8_t configIndex = 1;
    dev = {busNum, devAddr};
    auto ret = UsbdClient::GetConfig(dev, configIndex);
    HDF_LOGI("UsbdTransferTest::UsbdGetConfig004 %{public}d GetConfig=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdConfig004 : GetConfig-----------");
}

/**
 * @tc.name: UsbdClaimInterface001
 * @tc.desc: Test functions to ClaimInterface
 * @tc.desc: int32_t  ClaimInterface(const UsbDev &dev, uint8_t interfaceid);
 * @tc.desc: 正向测试：参数正确
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdClaimInterface001, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdInterface001 : ClaimInterface-----------");
    uint8_t busNum = 1;
    uint8_t devAddr = 2;
    uint8_t interfaceid = 1;
    dev = {busNum, devAddr};
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdClaimInterface001 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdClaimInterface001 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    HDF_LOGI("-----------Case End : UsbdInterface001 : ClaimInterface-----------");
}

/**
 * @tc.name: UsbdClaimInterface002
 * @tc.desc: Test functions to ClaimInterface
 * @tc.desc: int32_t  ClaimInterface(const UsbDev &dev, uint8_t interfaceid);
 * @tc.desc: 反向测试：参数异常，busNum错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdClaimInterface002, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdInterface002 : ClaimInterface-----------");
    uint8_t busNum = 1;
    uint8_t devAddr = 2;
    uint8_t interfaceid = 1;
    dev = {busNum, devAddr};
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdClaimInterface002 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    dev.busNum = 20;
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdClaimInterface002 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdInterface002 : ClaimInterface-----------");
}

/**
 * @tc.name: UsbdClaimInterface003
 * @tc.desc: Test functions to ClaimInterface
 * @tc.desc: int32_t  ClaimInterface(const UsbDev &dev, uint8_t interfaceid);
 * @tc.desc: 反向测试：参数异常，devAddr错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdClaimInterface003, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdInterface003 : ClaimInterface-----------");
    uint8_t busNum = 1;
    uint8_t devAddr = 2;
    uint8_t interfaceid = 1;
    dev = {busNum, devAddr};
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdClaimInterface003 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    dev.devAddr = 255;
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdClaimInterface003 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdInterface003 : ClaimInterface-----------");
}

/**
 * @tc.name: UsbdClaimInterface004
 * @tc.desc: Test functions to ClaimInterface
 * @tc.desc: int32_t  ClaimInterface(const UsbDev &dev, uint8_t interfaceid);
 * @tc.desc: 反向测试：参数异常，interfaceid错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdClaimInterface004, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdInterface004 : ClaimInterface-----------");
    uint8_t busNum = 1;
    uint8_t devAddr = 2;
    uint8_t interfaceid = 1;
    dev = {busNum, devAddr};
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdClaimInterface004 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    interfaceid = 255;
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdClaimInterface004 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdInterface004 : ClaimInterface-----------");
}

/**
 * @tc.name: UsbdClaimInterface005
 * @tc.desc: Test functions to ClaimInterface
 * @tc.desc: int32_t  ClaimInterface(const UsbDev &dev, uint8_t interfaceid);
 * @tc.desc: 反向测试：参数异常，busNum、devAddr错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdClaimInterface005, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdInterface005 : ClaimInterface-----------");
    uint8_t busNum = 1;
    uint8_t devAddr = 2;
    uint8_t interfaceid = 1;
    dev = {busNum, devAddr};
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdClaimInterface005 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    dev.busNum = 255;
    dev.devAddr = 255;
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdClaimInterface005 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdInterface005 : ClaimInterface-----------");
}

/**
 * @tc.name: UsbdClaimInterface006
 * @tc.desc: Test functions to ClaimInterface
 * @tc.desc: int32_t  ClaimInterface(const UsbDev &dev, uint8_t interfaceid);
 * @tc.desc: 反向测试：参数异常，busNum、interfaceid错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdClaimInterface006, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdInterface006 : ClaimInterface-----------");
    uint8_t busNum = 1;
    uint8_t devAddr = 2;
    uint8_t interfaceid = 1;
    dev = {busNum, devAddr};
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdClaimInterface006 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    dev.busNum = 255;
    interfaceid = 255;
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdClaimInterface006 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdInterface006 : ClaimInterface-----------");
}

/**
 * @tc.name: UsbdClaimInterface007
 * @tc.desc: Test functions to ClaimInterface
 * @tc.desc: int32_t  ClaimInterface(const UsbDev &dev, uint8_t interfaceid);
 * @tc.desc: 反向测试：devAddr、interfaceid错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdClaimInterface007, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdInterface007 : ClaimInterface-----------");
    uint8_t busNum = 1;
    uint8_t devAddr = 2;
    uint8_t interfaceid = 1;
    dev = {busNum, devAddr};
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdClaimInterface007 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    dev.devAddr = 255;
    interfaceid = 255;
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdClaimInterface007 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdInterface007 : ClaimInterface-----------");
}

/**
 * @tc.name: UsbdClaimInterface008
 * @tc.desc: Test functions to ClaimInterface
 * @tc.desc: int32_t  ClaimInterface(const UsbDev &dev, uint8_t interfaceid);
 * @tc.desc: 反向测试：busNum、devAddr、interfaceid错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdClaimInterface008, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdInterface008 : ClaimInterface-----------");
    uint8_t busNum = 1;
    uint8_t devAddr = 2;
    uint8_t interfaceid = 1;
    dev = {busNum, devAddr};
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdClaimInterface008 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    dev.busNum = 255;
    dev.devAddr = 255;
    interfaceid = 255;
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdClaimInterface008 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdInterface008 : ClaimInterface-----------");
}

/**********************************************************************************************************/

/**
 * @tc.name: UsbdReleaseInterface001
 * @tc.desc: Test functions to ReleaseInterface
 * @tc.desc: int32_t ReleaseInterface(const UsbDev &dev, uint8_t interfaceid);
 * @tc.desc: 正向测试：参数正确
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdReleaseInterface001, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdInterface001 : ReleaseInterface-----------");
    uint8_t busNum = 1;
    uint8_t devAddr = 2;
    uint8_t interfaceid = 1;
    dev = {busNum, devAddr};
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdReleaseInterface001 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    HDF_LOGI("-----------Case End : UsbdInterface001 : ReleaseInterface-----------");
}

/**
 * @tc.name: UsbdReleaseInterface002
 * @tc.desc: Test functions to ReleaseInterface
 * @tc.desc: int32_t ReleaseInterface(const UsbDev &dev, uint8_t interfaceid);
 * @tc.desc: 反向测试：参数异常，busNum错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdReleaseInterface002, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdInterface002 : ReleaseInterface-----------");
    uint8_t busNum = 25;
    uint8_t devAddr = 2;
    uint8_t interfaceid = 1;
    dev = {busNum, devAddr};
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdReleaseInterface002 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdInterface002 : ReleaseInterface-----------");
}

/**
 * @tc.name: UsbdReleaseInterface003
 * @tc.desc: Test functions to ReleaseInterface
 * @tc.desc: int32_t ReleaseInterface(const UsbDev &dev, uint8_t interfaceid);
 * @tc.desc: 反向测试：参数异常，devAddr错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdReleaseInterface003, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdInterface003 : ReleaseInterface-----------");
    uint8_t busNum = 1;
    uint8_t devAddr = 25;
    uint8_t interfaceid = 1;
    dev = {busNum, devAddr};
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdReleaseInterface003 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdInterface003 : ReleaseInterface-----------");
}

/**
 * @tc.name: UsbdReleaseInterface004
 * @tc.desc: Test functions to ReleaseInterface
 * @tc.desc: int32_t ReleaseInterface(const UsbDev &dev, uint8_t interfaceid);
 * @tc.desc: 反向测试：参数异常，interfaceid错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdReleaseInterface004, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdInterface004 : ReleaseInterface-----------");
    uint8_t busNum = 1;
    uint8_t devAddr = 2;
    uint8_t interfaceid = 255;
    dev = {busNum, devAddr};
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdReleaseInterface004 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdInterface004 : ReleaseInterface-----------");
}

/**
 * @tc.name: UsbdReleaseInterface005
 * @tc.desc: Test functions to ReleaseInterface
 * @tc.desc: int32_t ReleaseInterface(const UsbDev &dev, uint8_t interfaceid);
 * @tc.desc: 反向测试：参数异常，busNum、devAddr错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdReleaseInterface005, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdInterface005 : ReleaseInterface-----------");
    uint8_t busNum = 25;
    uint8_t devAddr = 25;
    uint8_t interfaceid = 1;
    dev = {busNum, devAddr};
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdReleaseInterface005 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdInterface005 : ReleaseInterface-----------");
}

/**
 * @tc.name: UsbdReleaseInterface006
 * @tc.desc: Test functions to ReleaseInterface
 * @tc.desc: int32_t ReleaseInterface(const UsbDev &dev, uint8_t interfaceid);
 * @tc.desc: 反向测试：参数异常，busNum、interfaceid错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdReleaseInterface006, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdInterface006 : ReleaseInterface-----------");
    uint8_t busNum = 255;
    uint8_t devAddr = 2;
    int32_t interfaceid = 255;
    dev = {busNum, devAddr};
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdReleaseInterface006 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdInterface006 : ReleaseInterface-----------");
}

/**
 * @tc.name: UsbdReleaseInterface007
 * @tc.desc: Test functions to ReleaseInterface
 * @tc.desc: int32_t ReleaseInterface(const UsbDev &dev, uint8_t interfaceid);
 * @tc.desc: 反向测试：devAddr、interfaceid错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdReleaseInterface007, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdInterface007 : ReleaseInterface-----------");
    uint8_t busNum = 1;
    uint8_t devAddr = 255;
    int32_t interfaceid = 255;
    dev = {busNum, devAddr};
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdReleaseInterface007 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdInterface007 : ReleaseInterface-----------");
}

/**
 * @tc.name: UsbdReleaseInterface008
 * @tc.desc: Test functions to ReleaseInterface
 * @tc.desc: int32_t ReleaseInterface(const UsbDev &dev, uint8_t interfaceid);
 * @tc.desc: 反向测试：busNum、devAddr、interfaceid错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdReleaseInterface008, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdInterface008 : ReleaseInterface-----------");
    uint8_t busNum = 255;
    uint8_t devAddr = 255;
    int32_t interfaceid = 255;
    dev = {busNum, devAddr};
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdReleaseInterface008 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdInterface008 : ReleaseInterface-----------");
}

/**********************************************************************************************************/

/**
 * @tc.name: UsbdSetInterface001
 * @tc.desc: Test functions to SetInterface
 * @tc.desc: int32_t SetInterface(const UsbDev &dev, uint8_t interfaceid, uint8_t altIndex);
 * @tc.desc: 正向测试：参数正确
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdSetInterface001, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdInterface001 : SetInterface-----------");
    uint8_t busNum = 1;
    uint8_t devAddr = 2;
    uint8_t interfaceid = 1;
    uint8_t altIndex = 0;

    dev = {busNum, devAddr};
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdClaimInterface001 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);

    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdClaimInterface001 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::SetInterface(dev, interfaceid, altIndex);
    HDF_LOGI("UsbdTransferTest::UsbdSetInterface001 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    HDF_LOGI("-----------Case End : UsbdInterface001 : SetInterface-----------");
}

/**
 * @tc.name: UsbdSetInterface002
 * @tc.desc: Test functions to SetInterface
 * @tc.desc: int32_t SetInterface(const UsbDev &dev, uint8_t interfaceid, uint8_t altIndex);
 * @tc.desc: 反向测试：参数异常，busNum错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdSetInterface002, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdInterface002 : SetInterface-----------");
    uint8_t busNum = 1;
    uint8_t devAddr = 2;
    uint8_t interfaceid = 1;

    uint8_t altIndex = 0;
    dev = {busNum, devAddr};

    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdSetInterface002 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);

    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdSetInterface002 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    dev.busNum = 222;
    ret = UsbdClient::SetInterface(dev, interfaceid, altIndex);
    HDF_LOGI("UsbdTransferTest::UsbdSetInterface002 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdInterface002 : SetInterface-----------");
}

/**
 * @tc.name: UsbdSetInterface003
 * @tc.desc: Test functions to SetInterface
 * @tc.desc: int32_t SetInterface(const UsbDev &dev, uint8_t interfaceid, uint8_t altIndex);
 * @tc.desc: 反向测试：参数异常，devAddr错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdSetInterface003, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdInterface003 : SetInterface-----------");
    uint8_t busNum = 1;
    uint8_t devAddr = 2;
    uint8_t interfaceid = 1;
    uint8_t altIndex = 0;
    dev = {busNum, devAddr};
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdSetInterface003 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);

    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdSetInterface003 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    dev.devAddr = 222;
    ret = UsbdClient::SetInterface(dev, interfaceid, altIndex);
    HDF_LOGI("UsbdTransferTest::UsbdSetInterface003 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdInterface003 : SetInterface-----------");
}

/**
 * @tc.name: UsbdSetInterface004
 * @tc.desc: Test functions to SetInterface
 * @tc.desc: int32_t SetInterface(const UsbDev &dev, uint8_t interfaceid, uint8_t altIndex);
 * @tc.desc: 反向测试：参数异常，interfaceid错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdSetInterface004, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdInterface004 : SetInterface-----------");
    uint8_t busNum = 1;
    uint8_t devAddr = 2;
    uint8_t interfaceid = 1;
    uint8_t altIndex = 222;
    dev = {busNum, devAddr};
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdSetInterface004 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdSetInterface004 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    interfaceid = 222;
    ret = UsbdClient::SetInterface(dev, interfaceid, altIndex);
    HDF_LOGI("UsbdTransferTest::UsbdSetInterface004 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdInterface004 : SetInterface-----------");
}

/**
 * @tc.name: UsbdSetInterface005
 * @tc.desc: Test functions to SetInterface
 * @tc.desc: int32_t SetInterface(const UsbDev &dev, uint8_t interfaceid, uint8_t altIndex);
 * @tc.desc: 反向测试：参数异常，busNum、devAddr错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdSetInterface005, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdInterface005 : SetInterface-----------");
    uint8_t devAddr = 2;
    uint8_t busNum = 1;
    uint8_t interfaceid = 1;
    uint8_t altIndex = 0;

    dev = {busNum, devAddr};
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdSetInterface005 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);

    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdSetInterface005 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    dev.busNum = 233;
    dev.devAddr = 233;
    ret = UsbdClient::SetInterface(dev, interfaceid, altIndex);
    ;
    HDF_LOGI("UsbdTransferTest::UsbdSetInterface005 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdInterface005 : SetInterface-----------");
}

/**
 * @tc.name: UsbdSetInterface006
 * @tc.desc: Test functions to SetInterface
 * @tc.desc: int32_t SetInterface(const UsbDev &dev, uint8_t interfaceid, uint8_t altIndex);
 * @tc.desc: 反向测试：参数异常，busNum、interfaceid错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdSetInterface006, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdInterface006 : SetInterface-----------");
    uint8_t devAddr = 2;
    int32_t interfaceid = 1;
    uint8_t busNum = 1;
    uint8_t altIndex = 1;
    dev = {busNum, devAddr};
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdSetInterface006 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdSetInterface006 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    dev.busNum = 224;
    interfaceid = 224;
    ret = UsbdClient::SetInterface(dev, interfaceid, altIndex);
    HDF_LOGI("UsbdTransferTest::UsbdSetInterface006 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdInterface006 : SetInterface-----------");
}

/**
 * @tc.name: UsbdSetInterface007
 * @tc.desc: Test functions to SetInterface
 * @tc.desc: int32_t SetInterface(const UsbDev &dev, uint8_t interfaceid, uint8_t altIndex);
 * @tc.desc: 反向测试：devAddr、interfaceid错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdSetInterface007, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdInterface007 : SetInterface-----------");
    uint8_t busNum = 1;
    uint8_t devAddr = 2;
    int32_t interfaceid = 1;
    uint8_t altIndex = 225;
    dev = {busNum, devAddr};
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdSetInterface007 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdSetInterface007 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    dev.devAddr = 225;
    interfaceid = 225;
    ret = UsbdClient::SetInterface(dev, interfaceid, altIndex);
    HDF_LOGI("UsbdTransferTest::UsbdSetInterface007 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdInterface007 : SetInterface-----------");
}

/**
 * @tc.name: UsbdSetInterface008
 * @tc.desc: Test functions to SetInterface
 * @tc.desc: int32_t SetInterface(const UsbDev &dev, uint8_t interfaceid, uint8_t altIndex);
 * @tc.desc: 反向测试：busNum、devAddr、interfaceid错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdSetInterface008, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdInterface008 : SetInterface-----------");
    uint8_t altIndex = 225;
    uint8_t busNum = 1;
    uint8_t devAddr = 2;
    int32_t interfaceid = 1;
    dev = {busNum, devAddr};
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdSetInterface008 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdSetInterface008 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    dev.busNum = 225;
    dev.devAddr = 225;
    interfaceid = 225;
    ret = UsbdClient::SetInterface(dev, interfaceid, altIndex);
    HDF_LOGI("UsbdTransferTest::UsbdSetInterface008 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdInterface008 : SetInterface-----------");
}

/**
 * @tc.name: UsbdDescriptor001
 * @tc.desc: Test functions to GetDeviceDescriptor
 * @tc.desc: int32_t GetDeviceDescriptor(const UsbDev &dev, std::vector<uint8_t> &decriptor);
 * @tc.desc: 正向测试：参数正确
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdGetDeviceDescriptor001, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdDescriptor001 : GetDeviceDescriptor-----------");
    uint8_t busNum = 1;
    uint8_t devAddr = 2;
    uint32_t length = 255;
    uint8_t buffer[255] = {0};
    dev = {busNum, devAddr};
    std::vector<uint8_t> devdata(buffer, buffer + length);
    auto ret = UsbdClient::GetDeviceDescriptor(dev, devdata);
    HDF_LOGI("UsbdTransferTest::UsbdGetDeviceDescriptor001 %{public}d ret=%{public}d", __LINE__, ret);
    HDF_LOGI("UsbdTransferTest::UsbdGetDeviceDescriptor001 %{public}d length=%{public}d buffer=%{public}d", __LINE__,
             devdata.size(), sizeof(devdata));
    ASSERT_TRUE(ret == 0);
    HDF_LOGI("-----------Case End : UsbdDescriptor001 : GetDeviceDescriptor-----------");
}

/**
 * @tc.name: UsbdDescriptor002
 * @tc.desc: Test functions to GetDeviceDescriptor
 * @tc.desc: int32_t GetDeviceDescriptor(const UsbDev &dev, std::vector<uint8_t> &decriptor);
 * @tc.desc: 反向测试：参数异常，busNum错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdGetDeviceDescriptor002, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdDescriptor002 : GetDeviceDescriptor-----------");
    uint8_t busNum = 222;
    uint8_t devAddr = 2;
    uint8_t buffer[255] = {0};
    uint32_t length = 255;
    dev = {busNum, devAddr};
    std::vector<uint8_t> devdata(buffer, buffer + length);
    auto ret = UsbdClient::GetDeviceDescriptor(dev, devdata);
    HDF_LOGI("UsbdTransferTest::UsbdGetDeviceDescriptor002 %{public}d length=%{public}d buffer=%{public}d", __LINE__,
             devdata.size(), sizeof(devdata));
    HDF_LOGI("UsbdTransferTest::UsbdGetDeviceDescriptor002 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdDescriptor002 : GetDeviceDescriptor-----------");
}

/**
 * @tc.name: UsbdDescriptor003
 * @tc.desc: Test functions to GetDeviceDescriptor
 * @tc.desc: int32_t GetDeviceDescriptor(const UsbDev &dev, std::vector<uint8_t> &decriptor);
 * @tc.desc: 反向测试：参数异常，devAddr错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdGetDeviceDescriptor003, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdDescriptor003 : GetDeviceDescriptor-----------");
    uint8_t busNum = 1;
    uint8_t devAddr = 233;
    uint8_t buffer[255] = {0};
    uint32_t length = 255;
    dev = {busNum, devAddr};
    std::vector<uint8_t> devdata(buffer, buffer + length);
    auto ret = UsbdClient::GetDeviceDescriptor(dev, devdata);
    HDF_LOGI("UsbdTransferTest::UsbdGetDeviceDescriptor003 %{public}d length=%{public}d buffer=%{public}d", __LINE__,
             devdata.size(), sizeof(devdata));
    HDF_LOGI("UsbdTransferTest::UsbdGetDeviceDescriptor003 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdDescriptor003 : GetDeviceDescriptor-----------");
}

/**
 * @tc.name: UsbdDescriptor004
 * @tc.desc: Test functions to GetDeviceDescriptor
 * @tc.desc: int32_t GetDeviceDescriptor(const UsbDev &dev, std::vector<uint8_t> &decriptor);
 * @tc.desc: 反向测试：参数异常，length错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdGetDeviceDescriptor004, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdDescriptor004 : GetDeviceDescriptor-----------");
    uint8_t busNum = 1;
    uint8_t devAddr = 2;
    uint8_t buffer[255] = {};
    uint32_t length = 0;
    dev = {busNum, devAddr};
    std::vector<uint8_t> devdata(buffer, buffer + length);
    auto ret = UsbdClient::GetDeviceDescriptor(dev, devdata);
    HDF_LOGI("UsbdTransferTest::UsbdGetDeviceDescriptor004 %{public}d length=%{public}d buffer=%{public}d", __LINE__,
             devdata.size(), sizeof(devdata));
    HDF_LOGI("UsbdTransferTest::UsbdGetDeviceDescriptor004 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    HDF_LOGI("-----------Case End : UsbdDescriptor004 : GetDeviceDescriptor-----------");
}

/**
 * @tc.name: UsbdDescriptor005
 * @tc.desc: Test functions to GetDeviceDescriptor
 * @tc.desc: int32_t GetDeviceDescriptor(const UsbDev &dev, std::vector<uint8_t> &decriptor);
 * @tc.desc: 反向测试：参数异常，busNum、devAddr错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdGetDeviceDescriptor005, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdDescriptor005 : GetDeviceDescriptor-----------");
    uint8_t busNum = 99;
    uint8_t devAddr = 99;
    uint8_t buffer[255] = {};
    uint32_t length = 255;
    dev = {busNum, devAddr};
    std::vector<uint8_t> devdata(buffer, buffer + length);
    auto ret = UsbdClient::GetDeviceDescriptor(dev, devdata);
    HDF_LOGI("UsbdTransferTest::UsbdGetDeviceDescriptor005 %{public}d length=%{public}d buffer=%{public}d", __LINE__,
             devdata.size(), sizeof(devdata));
    HDF_LOGI("UsbdTransferTest::UsbdGetDeviceDescriptor005 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdDescriptor005 : GetDeviceDescriptor-----------");
}

/**
 * @tc.name: UsbdDescriptor006
 * @tc.desc: Test functions to GetDeviceDescriptor
 * @tc.desc: int32_t GetDeviceDescriptor(const UsbDev &dev, std::vector<uint8_t> &decriptor);
 * @tc.desc: 反向测试：参数异常，busNum、length错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdGetDeviceDescriptor006, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdDescriptor006 : GetDeviceDescriptor-----------");
    uint8_t busNum = 222;
    uint8_t devAddr = 2;
    uint8_t buffer[255] = {};
    uint32_t length = 0;
    dev = {busNum, devAddr};
    std::vector<uint8_t> devdata(buffer, buffer + length);
    auto ret = UsbdClient::GetDeviceDescriptor(dev, devdata);
    HDF_LOGI("UsbdTransferTest::UsbdGetDeviceDescriptor006 %{public}d length=%{public}d buffer=%{public}d", __LINE__,
             devdata.size(), sizeof(devdata));
    HDF_LOGI("UsbdTransferTest::UsbdGetDeviceDescriptor006 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdDescriptor006 : GetDeviceDescriptor-----------");
}

/**
 * @tc.name: UsbdDescriptor007
 * @tc.desc: Test functions to GetDeviceDescriptor
 * @tc.desc: int32_t GetDeviceDescriptor(const UsbDev &dev, std::vector<uint8_t> &decriptor);
 * @tc.desc: 反向测试：devAddr、length错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdGetDeviceDescriptor007, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdDescriptor007 : GetDeviceDescriptor-----------");
    uint8_t busNum = 1;
    uint8_t devAddr = 222;
    uint8_t buffer[] = {};
    uint32_t length = 0;
    dev = {busNum, devAddr};
    std::vector<uint8_t> devdata(buffer, buffer + length);
    auto ret = UsbdClient::GetDeviceDescriptor(dev, devdata);
    HDF_LOGI("UsbdTransferTest::UsbdGetDeviceDescriptor007 %{public}d length=%{public}d buffer=%{public}d", __LINE__,
             devdata.size(), sizeof(devdata));
    HDF_LOGI("UsbdTransferTest::UsbdGetDeviceDescriptor007 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdDescriptor007 : GetDeviceDescriptor-----------");
}

/**
 * @tc.name: UsbdDescriptor008
 * @tc.desc: Test functions to GetDeviceDescriptor
 * @tc.desc: int32_t GetDeviceDescriptor(const UsbDev &dev, std::vector<uint8_t> &decriptor);
 * @tc.desc: 反向测试：busNum、devAddr、length错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdGetDeviceDescriptor008, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdDescriptor008 : GetDeviceDescriptor-----------");
    uint8_t busNum = 233;
    uint8_t devAddr = 234;
    uint8_t buffer[] = {};
    uint32_t length = 0;
    dev = {busNum, devAddr};
    std::vector<uint8_t> devdata(buffer, buffer + length);
    auto ret = UsbdClient::GetDeviceDescriptor(dev, devdata);
    HDF_LOGI("UsbdTransferTest::UsbdGetDeviceDescriptor008 %{public}d length=%{public}d buffer=%{public}d", __LINE__,
             devdata.size(), sizeof(devdata));
    HDF_LOGI("UsbdTransferTest::UsbdGetDeviceDescriptor008 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdDescriptor008 : GetDeviceDescriptor-----------");
}

/**********************************************************************************************************/

/**
 * @tc.name: UsbdDescriptor001
 * @tc.desc: Test functions to GetStringDescriptor
 * @tc.desc: int32_t GetStringDescriptor(const UsbDev &dev, uint8_t descId, std::vector<uint8_t> &decriptor);
 * @tc.desc: 正向测试：参数正确
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdGetStringDescriptor001, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdDescriptor001 : GetStringDescriptor-----------");
    uint8_t busNum = 1;
    uint8_t devAddr = 2;
    uint8_t stringId = 0;
    uint8_t buffer[255] = {0};
    uint32_t length = 255;
    dev = {busNum, devAddr};
    std::vector<uint8_t> devdata(buffer, buffer + length);
    auto ret = UsbdClient::GetStringDescriptor(dev, stringId, devdata);
    HDF_LOGI("UsbdTransferTest::UsbdGetStringDescriptor001 %{public}d ret=%{public}d", __LINE__, ret);
    HDF_LOGI("UsbdTransferTest::UsbdGetStringDescriptor001 %{public}d length=%{public}d buffer=%{public}d", __LINE__,
             devdata.size(), sizeof(devdata));
    ASSERT_TRUE(ret == 0);
    HDF_LOGI("-----------Case End : UsbdDescriptor001 : GetStringDescriptor-----------");
}

/**
 * @tc.name: UsbdDescriptor002
 * @tc.desc: Test functions to GetStringDescriptor
 * @tc.desc: int32_t GetStringDescriptor(const UsbDev &dev, uint8_t descId, std::vector<uint8_t> &decriptor);
 * @tc.desc: 反向测试：参数异常，busNum错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdGetStringDescriptor002, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdDescriptor002 : GetStringDescriptor-----------");
    uint8_t busNum = 1;
    uint8_t devAddr = 2;
    uint8_t stringId = 1;
    uint8_t buffer[255] = {0};
    uint32_t length = 255;
    dev = {busNum, devAddr};
    std::vector<uint8_t> devdata(buffer, buffer + length);
    auto ret = UsbdClient::GetStringDescriptor(dev, stringId, devdata);
    HDF_LOGI("UsbdTransferTest::UsbdGetStringDescriptor002 %{public}d length=%{public}d buffer=%{public}d", __LINE__,
             devdata.size(), sizeof(devdata));
    HDF_LOGI("UsbdTransferTest::UsbdGetStringDescriptor002 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    HDF_LOGI("-----------Case End : UsbdDescriptor002 : GetStringDescriptor-----------");
}

/**
 * @tc.name: UsbdDescriptor003
 * @tc.desc: Test functions to GetStringDescriptor
 * @tc.desc: int32_t GetStringDescriptor(const UsbDev &dev, uint8_t descId, std::vector<uint8_t> &decriptor);
 * @tc.desc: 反向测试：参数异常，stringId错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdGetStringDescriptor003, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdDescriptor003 : GetStringDescriptor-----------");
    uint8_t busNum = 1;
    uint8_t devAddr = 2;
    uint8_t stringId = 222;
    uint8_t buffer[255] = {0};
    uint32_t length = 255;
    dev = {busNum, devAddr};
    std::vector<uint8_t> devdata(buffer, buffer + length);
    auto ret = UsbdClient::GetStringDescriptor(dev, stringId, devdata);
    HDF_LOGI("UsbdTransferTest::UsbdGetStringDescriptor003 %{public}d length=%{public}d buffer=%{public}d", __LINE__,
             devdata.size(), sizeof(devdata));
    HDF_LOGI("UsbdTransferTest::UsbdGetStringDescriptor003 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    HDF_LOGI("-----------Case End : UsbdDescriptor003 : GetStringDescriptor-----------");
}

/**
 * @tc.name: UsbdDescriptor004
 * @tc.desc: Test functions to GetStringDescriptor
 * @tc.desc: int32_t GetStringDescriptor(const UsbDev &dev, uint8_t descId, std::vector<uint8_t> &decriptor);
 * @tc.desc: 反向测试：参数异常，devAddr错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdGetStringDescriptor004, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdDescriptor004 : GetStringDescriptor-----------");
    uint8_t busNum = 1;
    uint8_t devAddr = 255;
    uint8_t stringId = 0;
    uint8_t buffer[255] = {0};
    uint32_t length = 8;
    dev = {busNum, devAddr};
    std::vector<uint8_t> devdata(buffer, buffer + length);
    auto ret = UsbdClient::GetStringDescriptor(dev, stringId, devdata);
    HDF_LOGI("UsbdTransferTest::UsbdGetStringDescriptor004 %{public}d length=%{public}d buffer=%{public}d", __LINE__,
             devdata.size(), sizeof(devdata));
    HDF_LOGI("UsbdTransferTest::UsbdGetStringDescriptor004 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdDescriptor004 : GetStringDescriptor-----------");
}

/**
 * @tc.name: UsbdDescriptor005
 * @tc.desc: Test functions to GetStringDescriptor
 * @tc.desc: int32_t GetStringDescriptor(const UsbDev &dev, uint8_t descId, std::vector<uint8_t> &decriptor);
 * @tc.desc: 反向测试：参数异常，busNum、devAddr错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdGetStringDescriptor005, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdDescriptor005 : GetStringDescriptor-----------");
    uint8_t busNum = 222;
    uint8_t devAddr = 222;
    uint8_t stringId = 0;
    uint8_t buffer[255] = {0};
    uint32_t length = 8;
    dev = {busNum, devAddr};
    std::vector<uint8_t> devdata(buffer, buffer + length);
    auto ret = UsbdClient::GetStringDescriptor(dev, stringId, devdata);
    HDF_LOGI("UsbdTransferTest::UsbdGetStringDescriptor005 %{public}d length=%{public}d buffer=%{public}d", __LINE__,
             devdata.size(), sizeof(devdata));
    HDF_LOGI("UsbdTransferTest::UsbdGetStringDescriptor005 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdDescriptor005 : GetStringDescriptor-----------");
}

/**
 * @tc.name: UsbdDescriptor006
 * @tc.desc: Test functions to GetStringDescriptor
 * @tc.desc: int32_t GetStringDescriptor(const UsbDev &dev, uint8_t descId, std::vector<uint8_t> &decriptor);
 * @tc.desc: 反向测试：参数异常，busNum错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdGetStringDescriptor006, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdDescriptor006 : GetStringDescriptor-----------");
    uint8_t busNum = 222;
    uint8_t devAddr = 2;
    uint8_t stringId = 0;
    uint8_t buffer[255] = {0};
    uint32_t length = 255;
    dev = {busNum, devAddr};
    std::vector<uint8_t> devdata(buffer, buffer + length);
    auto ret = UsbdClient::GetStringDescriptor(dev, stringId, devdata);
    HDF_LOGI("UsbdTransferTest::UsbdGetStringDescriptor006 %{public}d length=%{public}d buffer=%{public}d", __LINE__,
             devdata.size(), sizeof(devdata));
    HDF_LOGI("UsbdTransferTest::UsbdGetStringDescriptor006 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdDescriptor006 : GetStringDescriptor-----------");
}

/**
 * @tc.name: UsbdDescriptor007
 * @tc.desc: Test functions to GetStringDescriptor
 * @tc.desc: int32_t GetStringDescriptor(const UsbDev &dev, uint8_t descId, std::vector<uint8_t> &decriptor);
 * @tc.desc: 反向测试：devAddr、stringID错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdGetStringDescriptor007, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdDescriptor007 : GetStringDescriptor-----------");
    uint8_t busNum = 1;
    uint8_t devAddr = 222;
    uint8_t stringId = 233;
    uint8_t buffer[255] = {0};
    uint32_t length = 255;
    dev = {busNum, devAddr};
    std::vector<uint8_t> devdata(buffer, buffer + length);
    auto ret = UsbdClient::GetStringDescriptor(dev, stringId, devdata);
    HDF_LOGI("UsbdTransferTest::UsbdGetStringDescriptor007 %{public}d length=%{public}d buffer=%{public}d", __LINE__,
             devdata.size(), sizeof(devdata));
    HDF_LOGI("UsbdTransferTest::UsbdGetStringDescriptor007 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdDescriptor007 : GetStringDescriptor-----------");
}

/**
 * @tc.name: UsbdDescriptor008
 * @tc.desc: Test functions to GetStringDescriptor
 * @tc.desc: int32_t GetStringDescriptor(const UsbDev &dev, uint8_t descId, std::vector<uint8_t> &decriptor);
 * @tc.desc: 反向测试：busNum、devAddr、length错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdGetStringDescriptor008, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdDescriptor008 : GetStringDescriptor-----------");
    uint8_t busNum = 222;
    uint8_t devAddr = 222;
    uint8_t stringId = 222;
    uint8_t buffer[255] = {0};
    uint32_t length = 255;
    dev = {busNum, devAddr};
    std::vector<uint8_t> devdata(buffer, buffer + length);
    auto ret = UsbdClient::GetStringDescriptor(dev, stringId, devdata);
    HDF_LOGI("UsbdTransferTest::UsbdGetStringDescriptor008 %{public}d length=%{public}d buffer=%{public}d", __LINE__,
             devdata.size(), sizeof(devdata));
    HDF_LOGI("UsbdTransferTest::UsbdGetStringDescriptor008 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdDescriptor008 : GetStringDescriptor-----------");
}

/**********************************************************************************************************/

/**
 * @tc.name: UsbdDescriptor001
 * @tc.desc: Test functions to GetConfigDescriptor
 * @tc.desc: int32_t GetConfigDescriptor(const UsbDev &dev, uint8_t descId, std::vector<uint8_t> &decriptor);
 * @tc.desc: 正向测试：参数正确
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdGetConfigDescriptor001, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdDescriptor001 : GetConfigDescriptor-----------");
    uint8_t busNum = 1;
    uint8_t devAddr = 2;
    uint8_t configId = 0;
    uint8_t buffer[255] = {};
    uint32_t length = 255;
    dev = {busNum, devAddr};
    std::vector<uint8_t> devdata(buffer, buffer + length);
    auto ret = UsbdClient::GetConfigDescriptor(dev, configId, devdata);
    HDF_LOGI("UsbdTransferTest::UsbdGetConfigDescriptor001 %{public}d ret=%{public}d", __LINE__, ret);
    HDF_LOGI("UsbdTransferTest::UsbdGetConfigDescriptor001 %{public}d length=%{public}d buffer=%{public}d", __LINE__,
             devdata.size(), sizeof(devdata));
    ASSERT_TRUE(ret == 0);
    HDF_LOGI("-----------Case End : UsbdDescriptor001 : GetConfigDescriptor-----------");
}

/**
 * @tc.name: UsbdDescriptor002
 * @tc.desc: Test functions to GetConfigDescriptor
 * @tc.desc: int32_t GetConfigDescriptor(const UsbDev &dev, uint8_t descId, std::vector<uint8_t> &decriptor);
 * @tc.desc: 反向测试：参数异常，busNum错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdGetConfigDescriptor002, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdDescriptor002 : GetConfigDescriptor-----------");
    uint8_t busNum = 222;
    uint8_t devAddr = 2;
    uint8_t configId = 1;
    uint8_t buffer[255] = {};
    uint32_t length = 255;
    dev = {busNum, devAddr};
    std::vector<uint8_t> devdata(buffer, buffer + length);
    auto ret = UsbdClient::GetConfigDescriptor(dev, configId, devdata);
    HDF_LOGI("UsbdTransferTest::UsbdGetConfigDescriptor002 %{public}d length=%{public}d buffer=%{public}d", __LINE__,
             devdata.size(), sizeof(devdata));
    HDF_LOGI("UsbdTransferTest::UsbdGetConfigDescriptor002 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdDescriptor002 : GetConfigDescriptor-----------");
}

/**
 * @tc.name: UsbdDescriptor003
 * @tc.desc: Test functions to GetConfigDescriptor
 * @tc.desc: int32_t GetConfigDescriptor(const UsbDev &dev, uint8_t descId, std::vector<uint8_t> &decriptor);
 * @tc.desc: 反向测试：参数异常，devAddr错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdGetConfigDescriptor003, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdDescriptor003 : GetConfigDescriptor-----------");
    uint8_t busNum = 1;
    uint8_t devAddr = 222;
    uint8_t configId = 1;
    uint8_t buffer[255] = {};
    uint32_t length = 255;
    dev = {busNum, devAddr};
    std::vector<uint8_t> devdata(buffer, buffer + length);
    auto ret = UsbdClient::GetConfigDescriptor(dev, configId, devdata);
    HDF_LOGI("UsbdTransferTest::UsbdGetConfigDescriptor003 %{public}d length=%{public}d buffer=%{public}d", __LINE__,
             devdata.size(), sizeof(devdata));
    HDF_LOGI("UsbdTransferTest::UsbdGetConfigDescriptor003 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdDescriptor003 : GetConfigDescriptor-----------");
}

/**
 * @tc.name: UsbdDescriptor004
 * @tc.desc: Test functions to GetConfigDescriptor
 * @tc.desc: int32_t GetConfigDescriptor(const UsbDev &dev, uint8_t descId, std::vector<uint8_t> &decriptor);
 * @tc.desc: 反向测试：参数异常，configId
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdGetConfigDescriptor004, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdDescriptor004 : GetConfigDescriptor-----------");
    uint8_t busNum = 1;
    uint8_t devAddr = 2;
    uint8_t configId = 1;
    uint8_t buffer[255] = {};
    uint32_t length = 255;
    dev = {busNum, devAddr};
    std::vector<uint8_t> devdata(buffer, buffer + length);
    auto ret = UsbdClient::GetConfigDescriptor(dev, configId, devdata);
    HDF_LOGI("UsbdTransferTest::UsbdGetConfigDescriptor004 %{public}d length=%{public}d buffer=%{public}d", __LINE__,
             devdata.size(), sizeof(devdata));
    HDF_LOGI("UsbdTransferTest::UsbdGetConfigDescriptor004 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    HDF_LOGI("-----------Case End : UsbdDescriptor004 : GetConfigDescriptor-----------");
}

/**
 * @tc.name: UsbdDescriptor005
 * @tc.desc: Test functions to GetConfigDescriptor
 * @tc.desc: int32_t GetConfigDescriptor(const UsbDev &dev, uint8_t descId, std::vector<uint8_t> &decriptor);
 * @tc.desc: 反向测试：参数异常，busNum、devAddr错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdGetConfigDescriptor005, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdDescriptor005 : GetConfigDescriptor-----------");
    uint8_t busNum = 222;
    uint8_t devAddr = 222;
    uint8_t configId = 1;
    uint8_t buffer[255] = {};
    uint32_t length = 255;
    dev = {busNum, devAddr};
    std::vector<uint8_t> devdata(buffer, buffer + length);
    auto ret = UsbdClient::GetConfigDescriptor(dev, configId, devdata);
    HDF_LOGI("UsbdTransferTest::UsbdGetConfigDescriptor005 %{public}d length=%{public}d buffer=%{public}d", __LINE__,
             devdata.size(), sizeof(devdata));
    HDF_LOGI("UsbdTransferTest::UsbdGetConfigDescriptor005 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdDescriptor005 : GetConfigDescriptor-----------");
}

/**
 * @tc.name: UsbdDescriptor006
 * @tc.desc: Test functions to GetConfigDescriptor
 * @tc.desc: int32_t GetConfigDescriptor(const UsbDev &dev, uint8_t descId, std::vector<uint8_t> &decriptor);
 * @tc.desc: 反向测试：参数异常，busNum、configId错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdGetConfigDescriptor006, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdDescriptor006 : GetConfigDescriptor-----------");
    uint8_t busNum = 222;
    uint8_t devAddr = 2;
    uint8_t configId = 222;
    uint8_t buffer[255] = {0};
    uint32_t length = 255;
    dev = {busNum, devAddr};
    std::vector<uint8_t> devdata(buffer, buffer + length);
    auto ret = UsbdClient::GetConfigDescriptor(dev, configId, devdata);
    HDF_LOGI("UsbdTransferTest::UsbdGetConfigDescriptor006 %{public}d length=%{public}d buffer=%{public}d", __LINE__,
             devdata.size(), sizeof(devdata));
    HDF_LOGI("UsbdTransferTest::UsbdGetConfigDescriptor006 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdDescriptor006 : GetConfigDescriptor-----------");
}

/**
 * @tc.name: UsbdDescriptor007
 * @tc.desc: Test functions to GetConfigDescriptor
 * @tc.desc: int32_t GetConfigDescriptor(const UsbDev &dev, uint8_t descId, std::vector<uint8_t> &decriptor);
 * @tc.desc: 反向测试：devAddr、configId错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdGetConfigDescriptor007, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdDescriptor007 : GetConfigDescriptor-----------");
    uint8_t busNum = 1;
    uint8_t devAddr = 222;
    uint8_t configId = 222;
    uint8_t buffer[255] = {0};
    uint32_t length = 255;
    dev = {busNum, devAddr};
    std::vector<uint8_t> devdata(buffer, buffer + length);
    auto ret = UsbdClient::GetConfigDescriptor(dev, configId, devdata);
    HDF_LOGI("UsbdTransferTest::UsbdGetConfigDescriptor007 %{public}d length=%{public}d buffer=%{public}d", __LINE__,
             devdata.size(), sizeof(devdata));
    HDF_LOGI("UsbdTransferTest::UsbdGetConfigDescriptor007 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdDescriptor007 : GetConfigDescriptor-----------");
}

/**
 * @tc.name: UsbdDescriptor008
 * @tc.desc: Test functions to GetConfigDescriptor
 * @tc.desc: int32_t GetConfigDescriptor(const UsbDev &dev, uint8_t descId, std::vector<uint8_t> &decriptor);
 * @tc.desc: 反向测试：busNum、devAddr、configId错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdGetConfigDescriptor008, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdDescriptor008 : GetConfigDescriptor-----------");
    uint8_t busNum = 222;
    uint8_t devAddr = 222;
    uint8_t configId = 222;
    uint8_t buffer[255] = {0};
    uint32_t length = 255;
    dev = {busNum, devAddr};
    std::vector<uint8_t> devdata(buffer, buffer + length);
    auto ret = UsbdClient::GetConfigDescriptor(dev, configId, devdata);
    HDF_LOGI("UsbdTransferTest::UsbdGetConfigDescriptor008 %{public}d length=%{public}d buffer=%{public}d", __LINE__,
             devdata.size(), sizeof(devdata));
    HDF_LOGI("UsbdTransferTest::UsbdGetConfigDescriptor008 %{public}d ret=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdDescriptor008 : GetConfigDescriptor-----------");
}

/**
 * @tc.name: UsbdRequest001
 * @tc.desc: Test functions to RequestQueue
 * @tc.desc: int32_t RequestQueue(const UsbDev &dev, const UsbPipe &pipe, std::vector<uint8_t> &clientData,
        std::vector<uint8_t> &buffer);
 * @tc.desc: 正向测试：参数正确
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdRequestQueue001, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdRequest001 : RequestQueue-----------");
    uint8_t pointid = 129;
    uint8_t interfaceid = 1;
    dev.busNum = 1;
    dev.devAddr = 2;
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestQueue001 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestQueue001 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    uint8_t tag[1000] = "queue read";
    uint8_t buffer[255] = {0};
    uint32_t length = 255;
    struct UsbPipe pipe = {interfaceid, pointid};
    std::vector<uint8_t> clientdata = {tag, tag + 10};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::RequestQueue(dev, pipe, clientdata, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdRequestQueue001 %{public}d interfaceid=%{public}d pointid=%{public}d", __LINE__,
             interfaceid, pointid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestQueue001 %{public}d RequestQueue=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    HDF_LOGI("-----------Case End : UsbdRequest001 : RequestQueue-----------");
}

/**
 * @tc.name: UsbdRequest002
 * @tc.desc: Test functions to RequestQueue
 * @tc.desc: int32_t RequestQueue(const UsbDev &dev, const UsbPipe &pipe, std::vector<uint8_t> &clientData,
        std::vector<uint8_t> &buffer);
 * @tc.desc: 反向测试：参数异常，busNum、devAddr错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdRequestQueue002, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdRequest002 : RequestQueue-----------");
    dev.devAddr = 2;
    dev.busNum = 1;
    uint8_t pointid = 129;
    uint8_t interfaceid = 1;
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestQueue002 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestQueue002 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    uint8_t tag[1000] = "queue read";
    uint8_t buffer[255] = {0};
    uint32_t length = 255;
    dev = {222, 222};
    struct UsbPipe pipe = {interfaceid, pointid};
    std::vector<uint8_t> clientdata = {tag, tag + 10};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::RequestQueue(dev, pipe, clientdata, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdRequestQueue002 %{public}d interfaceid=%{public}d pointid=%{public}d", __LINE__,
             interfaceid, pointid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestQueue002 %{public}d RequestQueue=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case Start : UsbdRequest002 : RequestQueue-----------");
}

/**
 * @tc.name: UsbdRequest003
 * @tc.desc: Test functions to RequestQueue
 * @tc.desc: int32_t RequestQueue(const UsbDev &dev, const UsbPipe &pipe, std::vector<uint8_t> &clientData,
        std::vector<uint8_t> &buffer);
 * @tc.desc: 反向测试：参数异常，devAddr错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdRequestQueue003, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdRequest003 : RequestQueue-----------");
    uint8_t pointid = 129;
    uint8_t interfaceid = 1;
    dev.busNum = 1;
    uint8_t buffer[255] = {0};
    dev.devAddr = 2;
    uint32_t length = 255;
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestQueue003 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestQueue003 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    uint8_t tag[1000] = "queue read";
    dev.devAddr = 222;
    struct UsbPipe pipe = {interfaceid, pointid};
    std::vector<uint8_t> clientdata = {tag, tag + 10};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::RequestQueue(dev, pipe, clientdata, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdRequestQueue003 %{public}d interfaceid=%{public}d pointid=%{public}d", __LINE__,
             interfaceid, pointid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestQueue003 %{public}d RequestQueue=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdRequest003 : RequestQueue-----------");
}

/**
 * @tc.name: UsbdRequest004
 * @tc.desc: Test functions to RequestQueue
 * @tc.desc: int32_t RequestQueue(const UsbDev &dev, const UsbPipe &pipe, std::vector<uint8_t> &clientData,
        std::vector<uint8_t> &buffer);
 * @tc.desc: 反向测试：参数异常，busNum、configIndex错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdRequestQueue004, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdRequest004 : RequestQueue-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t pointid = 129;
    uint8_t interfaceid = 1;
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestQueue004 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestQueue004 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    uint8_t tag[1000] = "queue read";
    interfaceid = 222;
    dev.busNum = 222;
    uint8_t buffer[255] = {0};
    uint32_t length = 255;
    struct UsbPipe pipe = {interfaceid, pointid};
    std::vector<uint8_t> clientdata = {tag, tag + 10};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::RequestQueue(dev, pipe, clientdata, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdRequestQueue004 %{public}d interfaceid=%{public}d pointid=%{public}d", __LINE__,
             interfaceid, pointid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestQueue004 %{public}d RequestQueue=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdRequest004 : RequestQueue-----------");
}

/**
 * @tc.name: UsbdRequest005
 * @tc.desc: Test functions to RequestQueue
 * @tc.desc: int32_t RequestQueue(const UsbDev &dev, const UsbPipe &pipe, std::vector<uint8_t> &clientData,
        std::vector<uint8_t> &buffer);
 * @tc.desc: 反向测试：参数异常，busNum错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdRequestQueue005, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdRequest005 : RequestQueue-----------");
    uint8_t buffer[255] = {};
    uint32_t length = 255;
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t pointid = 129;
    uint8_t interfaceid = 1;
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestQueue005 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestQueue005 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    uint8_t tag[1000] = "queue read";
    dev.busNum = 222;
    struct UsbPipe pipe = {interfaceid, pointid};
    std::vector<uint8_t> clientdata = {tag, tag + 10};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::RequestQueue(dev, pipe, clientdata, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdRequestQueue005 %{public}d RequestQueue=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdRequest005 : RequestQueue-----------");
}

/**
 * @tc.name: UsbdRequest006
 * @tc.desc: Test functions to RequestQueue
 * @tc.desc: int32_t RequestQueue(const UsbDev &dev, const UsbPipe &pipe, std::vector<uint8_t> &clientData,
        std::vector<uint8_t> &buffer);
 * @tc.desc: 反向测试：参数异常，busNum、interfaceId、pointid错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdRequestQueue006, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdRequest006 : RequestQueue-----------");
    dev.busNum = 1;
    uint8_t buffer[255] = {};
    dev.devAddr = 2;
    uint8_t pointid = 129;
    uint8_t interfaceid = 1;
    uint32_t length = 255;
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestQueue006 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestQueue006 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    uint8_t tag[1000] = "queue read";
    dev.busNum = 222;
    interfaceid = 222;
    pointid = 222;
    struct UsbPipe pipe = {interfaceid, pointid};
    std::vector<uint8_t> clientdata = {tag, tag + 10};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::RequestQueue(dev, pipe, clientdata, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdRequestQueue006 %{public}d RequestQueue=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdRequest006 : RequestQueue-----------");
}

/**
 * @tc.name: UsbdRequest007
 * @tc.desc: Test functions to RequestQueue
 * @tc.desc: int32_t RequestQueue(const UsbDev &dev, const UsbPipe &pipe, std::vector<uint8_t> &clientData,
        std::vector<uint8_t> &buffer);
 * @tc.desc: 正向测试
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdRequestQueue007, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdRequest007 : RequestQueue-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t buffer[255] = "request 007";
    uint32_t length = 255;
    uint8_t pointid = 1;
    uint8_t interfaceid = 1;
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestQueue007 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestQueue007 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    uint8_t tag[1000] = "queue write";
    struct UsbPipe pipe = {interfaceid, pointid};
    std::vector<uint8_t> clientdata = {tag, tag + 11};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::RequestQueue(dev, pipe, clientdata, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdRequestQueue007 %{public}d interfaceid=%{public}d pointid=%{public}d", __LINE__,
             interfaceid, pointid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestQueue007 %{public}d RequestQueue=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    UsbdTransferTest::PrintBuffer("RequestWait:clientData", clientdata.data(), clientdata.size());
    UsbdTransferTest::PrintBuffer("RequestWait:buffer", bufferdata.data(), bufferdata.size());
    HDF_LOGI("-----------Case End : UsbdRequest007 : RequestQueue-----------");
}

/**
 * @tc.name: UsbdRequest008
 * @tc.desc: Test functions to RequestQueue
 * @tc.desc: int32_t RequestQueue(const UsbDev &dev, const UsbPipe &pipe, std::vector<uint8_t> &clientData,
        std::vector<uint8_t> &buffer);
 * @tc.desc: 反向测试：interfaceId错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdRequestQueue008, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdRequest008 : RequestQueue-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t buffer[255] = "reuquest008";
    uint32_t length = 255;
    uint8_t pointid = 1;
    uint8_t interfaceid = 1;
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestQueue008 %{public}d interfaceid=%{public}d pointid=%{public}d", __LINE__,
             interfaceid, pointid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestQueue008 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestQueue008 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    uint8_t tag[1000] = "queue write";
    interfaceid = 222;
    struct UsbPipe pipe = {interfaceid, pointid};
    std::vector<uint8_t> clientdata = {tag, tag + 11};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::RequestQueue(dev, pipe, clientdata, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdRequestQueue008 %{public}d RequestQueue=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdRequest008 : RequestQueue-----------");
}

/**
 * @tc.name: UsbdRequest009
 * @tc.desc: Test functions to RequestQueue
 * @tc.desc: int32_t RequestQueue(const UsbDev &dev, const UsbPipe &pipe, std::vector<uint8_t> &clientData,
        std::vector<uint8_t> &buffer);
 * @tc.desc: 反向测试：interfaceId、poinid错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdRequestQueue009, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdRequest009 : RequestQueue-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t buffer[255] = "request 009";
    uint32_t length = 255;
    uint8_t pointid = 1;
    uint8_t interfaceid = 1;
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestQueue009 %{public}d interfaceid=%{public}d pointid=%{public}d", __LINE__,
             interfaceid, pointid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestQueue009 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestQueue009 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    uint8_t tag[1000] = "queue write";
    interfaceid = 222;
    pointid = 222;
    struct UsbPipe pipe = {interfaceid, pointid};
    std::vector<uint8_t> clientdata = {tag, tag + 11};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::RequestQueue(dev, pipe, clientdata, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdRequestQueue009 %{public}d RequestQueue=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdRequest009 : RequestQueue-----------");
}

/**********************************************************************************************************/

/**
 * @tc.name: UsbdRequest001
 * @tc.desc: Test functions to RequestWait
 * @tc.desc: int32_t RequestWait(const UsbDev &dev, std::vector<uint8_t> &clientData, std::vector<uint8_t> &buffer,
 * int32_t timeout);
 * @tc.desc: 正向测试：参数正确
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdRequestWait001, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdRequest001 : RequestWait-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t pointid = 129;
    uint8_t interfaceid = 1;
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestWait001 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestWait001 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    uint8_t buffer[255] = {};
    uint32_t length = 255;
    uint8_t tag[1000] = "queue read";
    struct UsbPipe pipe = {interfaceid, pointid};
    std::vector<uint8_t> clientdata = {tag, tag + 10};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::RequestQueue(dev, pipe, clientdata, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdRequestWait001 %{public}d RequestQueue=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    uint8_t *clientObj = new uint8_t[10];
    std::vector<uint8_t> waitdata = {clientObj, clientObj + 10};
    ret = UsbdClient::RequestWait(dev, waitdata, bufferdata, 10000);
    HDF_LOGI("UsbdTransferTest::UsbdRequestWait001 %{public}d RequestWait=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    UsbdTransferTest::PrintBuffer("RequestWait:clientData", clientdata.data(), clientdata.size());
    UsbdTransferTest::PrintBuffer("RequestWait:buffer", bufferdata.data(), bufferdata.size());
    delete[] clientObj;
    clientObj = nullptr;
    HDF_LOGI("-----------Case End : UsbdRequest001 : RequestWait-----------");
}

/**
 * @tc.name: UsbdRequest002
 * @tc.desc: Test functions to RequestWait
 * @tc.desc: int32_t RequestWait(const UsbDev &dev, std::vector<uint8_t> &clientData, std::vector<uint8_t> &buffer,
 * int32_t timeout);
 * @tc.desc: 反向测试：busNum错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdRequestWait002, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdRequest002 : RequestWait-----------");
    uint8_t pointid = 129;
    uint8_t interfaceid = 1;
    dev.busNum = 1;
    dev.devAddr = 2;
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestWait002 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestWait002 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    uint8_t buffer[255] = {};
    uint32_t length = 255;
    uint8_t tag[1000] = "queue read";
    std::vector<uint8_t> clientdata = {tag, tag + 10};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    struct UsbPipe pipe = {interfaceid, pointid};
    ret = UsbdClient::RequestQueue(dev, pipe, clientdata, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdRequestWait002 %{public}d RequestQueue=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    dev.busNum = 222;
    uint8_t *clientObj = new uint8_t[10];
    std::vector<uint8_t> waitdata = {clientObj, clientObj + 10};
    ret = UsbdClient::RequestWait(dev, waitdata, bufferdata, 10000);
    HDF_LOGI("UsbdTransferTest::UsbdRequestWait002 %{public}d RequestWait=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    UsbdTransferTest::PrintBuffer("RequestWait:clientData", clientdata.data(), clientdata.size());
    UsbdTransferTest::PrintBuffer("RequestWait:buffer", bufferdata.data(), bufferdata.size());
    delete[] clientObj;
    clientObj = nullptr;
    HDF_LOGI("-----------Case End : UsbdRequest002 : RequestWait-----------");
}

/**
 * @tc.name: UsbdRequest003
 * @tc.desc: Test functions to RequestWait
 * @tc.desc: int32_t RequestWait(const UsbDev &dev, std::vector<uint8_t> &clientData, std::vector<uint8_t> &buffer,
 * int32_t timeout);
 * @tc.desc: 反向测试：devAddr错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdRequestWait003, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdRequest003 : RequestWait-----------");
    uint8_t pointid = 129;
    uint8_t interfaceid = 1;
    dev.busNum = 1;
    dev.devAddr = 2;
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestWait003 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestWait003 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    uint8_t tag[1000] = "queue read";
    uint8_t buffer[255] = {};
    uint32_t length = 255;
    std::vector<uint8_t> clientdata = {tag, tag + 10};
    struct UsbPipe pipe = {interfaceid, pointid};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::RequestQueue(dev, pipe, clientdata, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdRequestWait003 %{public}d RequestQueue=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    uint8_t *clientObj = new uint8_t[10];
    dev.devAddr = 222;
    std::vector<uint8_t> waitdata = {clientObj, clientObj + 10};
    ret = UsbdClient::RequestWait(dev, waitdata, bufferdata, 10000);
    HDF_LOGI("UsbdTransferTest::UsbdRequestWait003 %{public}d RequestWait=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    UsbdTransferTest::PrintBuffer("RequestWait:buffer", bufferdata.data(), bufferdata.size());
    UsbdTransferTest::PrintBuffer("RequestWait:clientData", clientdata.data(), clientdata.size());
    delete[] clientObj;
    clientObj = nullptr;
    HDF_LOGI("-----------Case End : UsbdRequest003 : RequestWait-----------");
}

/**
 * @tc.name: UsbdRequest004
 * @tc.desc: Test functions to RequestWait
 * @tc.desc: int32_t RequestWait(const UsbDev &dev, std::vector<uint8_t> &clientData, std::vector<uint8_t> &buffer,
 * int32_t timeout);
 * @tc.desc: 反向测试：timeout错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdRequestWait004, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdRequest004 : RequestWait-----------");
    uint8_t pointid = 129;
    uint8_t interfaceid = 1;
    dev.busNum = 1;
    dev.devAddr = 2;
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestWait004 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestWait004 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    uint8_t buffer[255] = {};
    uint8_t tag[1000] = "queue read";
    uint32_t length = 255;
    std::vector<uint8_t> clientdata = {tag, tag + 10};
    struct UsbPipe pipe = {interfaceid, pointid};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::RequestQueue(dev, pipe, clientdata, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdRequestWait004 %{public}d RequestQueue=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    uint8_t *clientObj = new uint8_t[10];
    std::vector<uint8_t> waitdata = {clientObj, clientObj + 10};
    ret = UsbdClient::RequestWait(dev, waitdata, bufferdata, -10000);
    HDF_LOGI("UsbdTransferTest::UsbdRequestWait004 %{public}d RequestWait=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    UsbdTransferTest::PrintBuffer("RequestWait:clientData", clientdata.data(), clientdata.size());
    UsbdTransferTest::PrintBuffer("RequestWait:buffer", bufferdata.data(), bufferdata.size());
    delete[] clientObj;
    clientObj = nullptr;
    HDF_LOGI("-----------Case End : UsbdRequest004 : RequestWait-----------");
}

/**
 * @tc.name: UsbdRequest005
 * @tc.desc: Test functions to RequestWait
 * @tc.desc: int32_t RequestWait(const UsbDev &dev, std::vector<uint8_t> &clientData, std::vector<uint8_t> &buffer,
 * int32_t timeout);
 * @tc.desc: 反向测试：busNum、devAddr错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdRequestWait005, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdRequest005 : RequestWait-----------");
    uint8_t pointid = 129;
    uint8_t interfaceid = 1;
    dev.busNum = 1;
    dev.devAddr = 2;
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestWait005 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestWait005 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    uint32_t length = 255;
    uint8_t tag[1000] = "queue read";
    uint8_t buffer[255] = {};
    struct UsbPipe pipe = {interfaceid, pointid};
    std::vector<uint8_t> clientdata = {tag, tag + 10};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::RequestQueue(dev, pipe, clientdata, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdRequestWait005 %{public}d RequestQueue=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    uint8_t *clientObj = new uint8_t[10];
    std::vector<uint8_t> waitdata = {clientObj, clientObj + 10};
    dev.devAddr = 255;
    dev.busNum = 255;
    ret = UsbdClient::RequestWait(dev, waitdata, bufferdata, 10000);
    HDF_LOGI("UsbdTransferTest::UsbdRequestWait005 %{public}d RequestWait=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    UsbdTransferTest::PrintBuffer("RequestWait:clientData", (const uint8_t *)waitdata.data(), waitdata.size());
    UsbdTransferTest::PrintBuffer("RequestWait:buffer", bufferdata.data(), bufferdata.size());
    delete[] clientObj;
    clientObj = nullptr;
    HDF_LOGI("-----------Case End : UsbdRequest005 : RequestWait-----------");
}

/**********************************************************************************************************/

/**
 * @tc.name: UsbdRequest001
 * @tc.desc: Test functions to RequestCancel
 * @tc.desc: int32_t RequestCancel(const UsbDev &dev, const UsbPipe &pipe);
 * @tc.desc: 正向测试：参数正确
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdRequestCancel001, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdRequest001 : RequestCancel-----------");
    uint8_t pointid = 129;
    uint8_t interfaceid = 1;
    uint8_t tag[1000] = "queue read";
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t buffer[255] = "request001";
    uint32_t length = 255;
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestCancel001 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    EXPECT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestCancel001 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    EXPECT_TRUE(ret == 0);
    struct UsbPipe pipe = {interfaceid, pointid};
    std::vector<uint8_t> clientdata = {tag, tag + 10};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::RequestQueue(dev, pipe, clientdata, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdRequestCancel001 %{public}d RequestQueue=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::RequestCancel(dev, pipe);
    HDF_LOGI("UsbdTransferTest::UsbdRequestQueue001 %{public}d interfaceid=%{public}d pointid=%{public}d", __LINE__,
             interfaceid, pointid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestCancel001 %{public}d RequestCancel=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    HDF_LOGI("-----------Case End : UsbdRequest001 : RequestCancel-----------");
}

/**
 * @tc.name: UsbdRequest002
 * @tc.desc: Test functions to RequestCancel
 * @tc.desc: int32_t RequestCancel(const UsbDev &dev, const UsbPipe &pipe);
 * @tc.desc: 反向测试：参数异常，busNum错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdRequestCancel002, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdRequest002 : RequestCancel-----------");
    uint8_t tag[1000] = "queue read";
    dev.busNum = 1;
    dev.devAddr = 2;
    uint32_t length = 255;
    uint8_t pointid = 129;
    uint8_t interfaceid = 1;
    uint8_t buffer[255] = "request002";
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestCancel002 %{public}d interfaceid=%{public}d pointid=%{public}d", __LINE__,
             interfaceid, pointid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestCancel002 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    EXPECT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestCancel002 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    EXPECT_TRUE(ret == 0);
    struct UsbPipe pipe = {interfaceid, pointid};
    std::vector<uint8_t> clientdata = {tag, tag + 10};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ;
    ret = UsbdClient::RequestQueue(dev, pipe, clientdata, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdRequestCancel002 %{public}d RequestQueue=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    dev.busNum = 222;
    ret = UsbdClient::RequestCancel(dev, pipe);
    HDF_LOGI("UsbdTransferTest::UsbdRequestCancel002 %{public}d RequestCancel=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdRequest002 : RequestCancel-----------");
}

/**
 * @tc.name: UsbdRequest003
 * @tc.desc: Test functions to RequestCancel
 * @tc.desc: int32_t RequestCancel(const UsbDev &dev, const UsbPipe &pipe);
 * @tc.desc: 反向测试：参数异常，devAddr错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdRequestCancel003, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdRequest003 : RequestCancel-----------");
    uint8_t tag[1000] = "queue read";
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t buffer[255] = "request003";
    uint32_t length = 255;
    uint8_t pointid = 129;
    uint8_t interfaceid = 1;
    HDF_LOGI("UsbdTransferTest::UsbdRequestCancel003 %{public}d interfaceid=%{public}d pointid=%{public}d", __LINE__,
             interfaceid, pointid);
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestCancel003 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    EXPECT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestCancel003 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    EXPECT_TRUE(ret == 0);
    std::vector<uint8_t> clientdata = {tag, tag + 10};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    struct UsbPipe pipe = {interfaceid, pointid};
    ret = UsbdClient::RequestQueue(dev, pipe, clientdata, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdRequestCancel003 %{public}d RequestQueue=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    dev.devAddr = 222;
    ret = UsbdClient::RequestCancel(dev, pipe);
    HDF_LOGI("UsbdTransferTest::UsbdRequestCancel003 %{public}d RequestCancel=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdRequest003 : RequestCancel-----------");
}

/**
 * @tc.name: UsbdRequest004
 * @tc.desc: Test functions to RequestCancel
 * @tc.desc: int32_t RequestCancel(const UsbDev &dev, const UsbPipe &pipe);
 * @tc.desc: 反向测试：参数异常，interfaceid错误 结果正常？？？
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdRequestCancel004, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdRequest004 : RequestCancel-----------");
    uint8_t tag[1000] = "queue read";
    dev.busNum = 1;
    dev.devAddr = 2;
    uint32_t length = 255;
    uint8_t buffer[255] = "request004";
    uint8_t pointid = 129;
    uint8_t interfaceid = 1;
    HDF_LOGI("UsbdTransferTest::UsbdRequestCancel004 %{public}d interfaceid=%{public}d pointid=%{public}d", __LINE__,
             interfaceid, pointid);
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestCancel004 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    EXPECT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestCancel004 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    EXPECT_TRUE(ret == 0);
    struct UsbPipe pipe = {interfaceid, pointid};
    std::vector<uint8_t> clientdata = {tag, tag + 10};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::RequestQueue(dev, pipe, clientdata, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdRequestCancel004 %{public}d RequestQueue=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    pipe.interfaceId = 222;
    pipe.endpointId = 222;
    ret = UsbdClient::RequestCancel(dev, pipe);
    HDF_LOGI("UsbdTransferTest::UsbdRequestCancel004 %{public}d RequestCancel=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    HDF_LOGI("-----------Case End : UsbdRequest004 : RequestCancel-----------");
}

/**
 * @tc.name: UsbdRequest005
 * @tc.desc: Test functions to RequestCancel
 * @tc.desc: int32_t RequestCancel(const UsbDev &dev, const UsbPipe &pipe);
 * @tc.desc: 正向测试
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdRequestCancel005, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdRequest005 : RequestCancel-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t buffer[255] = "request005";
    uint32_t length = 255;
    uint8_t pointid = 1;
    uint8_t interfaceid = 1;
    HDF_LOGI("UsbdTransferTest::UsbdRequestCancel005 %{public}d interfaceid=%{public}d pointid=%{public}d", __LINE__,
             interfaceid, pointid);
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestCancel005 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    EXPECT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestCancel005 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    EXPECT_TRUE(ret == 0);
    struct UsbPipe pipe = {interfaceid, pointid};
    uint8_t tag[1000] = "queue Write";
    std::vector<uint8_t> clientdata = {tag, tag + 11};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::RequestQueue(dev, pipe, clientdata, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdRequestCancel005 %{public}d RequestQueue=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    ret = UsbdClient::RequestCancel(dev, pipe);
    HDF_LOGI("UsbdTransferTest::UsbdRequestCancel005 %{public}d RequestCancel=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    HDF_LOGI("-----------Case End : UsbdRequest005 : RequestCancel-----------");
}

/**
 * @tc.name: UsbdRequest006
 * @tc.desc: Test functions to RequestCancel
 * @tc.desc: int32_t RequestCancel(const UsbDev &dev, const UsbPipe &pipe);
 * @tc.desc: 反向测试：参数异常，busNum、interfaceid错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdRequestCancel006, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdRequest006 : RequestCancel-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t buffer[255] = "request006";
    uint32_t length = 255;
    uint8_t pointid = 1;
    uint8_t interfaceid = 1;
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestCancel006 %{public}d interfaceid=%{public}d pointid=%{public}d", __LINE__,
             interfaceid, pointid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestCancel006 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    EXPECT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestCancel006 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    EXPECT_TRUE(ret == 0);
    uint8_t tag[1000] = "queue Write";
    std::vector<uint8_t> clientdata = {tag, tag + 11};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    struct UsbPipe pipe = {interfaceid, pointid};
    ret = UsbdClient::RequestQueue(dev, pipe, clientdata, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdRequestCancel006 %{public}d RequestQueue=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    dev.busNum = 224;
    pipe.interfaceId = 224;
    ret = UsbdClient::RequestCancel(dev, pipe);
    HDF_LOGI("UsbdTransferTest::UsbdRequestCancel006 %{public}d RequestCancel=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdRequest006 : RequestCancel-----------");
}

/**
 * @tc.name: UsbdRequest007
 * @tc.desc: Test functions to RequestCancel
 * @tc.desc: int32_t RequestCancel(const UsbDev &dev, const UsbPipe &pipe);
 * @tc.desc: 反向测试：参数异常，devAddr、interfaceid错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdRequestCancel007, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdRequest007 : RequestCancel-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t buffer[255] = "request007";
    uint32_t length = 255;
    uint8_t pointid = 1;
    uint8_t interfaceid = 1;
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestCancel007 %{public}d interfaceid=%{public}d pointid=%{public}d", __LINE__,
             interfaceid, pointid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestCancel007 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    EXPECT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestCancel007 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    EXPECT_TRUE(ret == 0);
    struct UsbPipe pipe = {interfaceid, pointid};
    uint8_t tag[1000] = "queue Write";
    std::vector<uint8_t> clientdata = {tag, tag + 11};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::RequestQueue(dev, pipe, clientdata, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdRequestCancel007 %{public}d RequestQueue=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    dev.devAddr = 224;
    pipe.interfaceId = 224;
    ret = UsbdClient::RequestCancel(dev, pipe);
    HDF_LOGI("UsbdTransferTest::UsbdRequestCancel007 %{public}d RequestCancel=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdRequest007 : RequestCancel-----------");
}

/**
 * @tc.name: UsbdRequest008
 * @tc.desc: Test functions to RequestCancel
 * @tc.desc: int32_t RequestCancel(const UsbDev &dev, const UsbPipe &pipe);
 * @tc.desc: 反向测试：参数异常，busNum、devAddr、interfaceid错误
 * @tc.type: FUNC
 */
HWTEST_F(UsbdTransferTest, UsbdRequestCancel008, TestSize.Level1)
{
    HDF_LOGI("-----------Case Start : UsbdRequest008 : RequestCancel-----------");
    dev.busNum = 1;
    dev.devAddr = 2;
    uint8_t buffer[255] = "request008";
    uint8_t pointid = 1;
    uint8_t interfaceid = 1;
    uint32_t length = 255;
    auto ret = UsbdClient::ReleaseInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestCancel008 %{public}d interfaceid=%{public}d pointid=%{public}d", __LINE__,
             interfaceid, pointid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestCancel008 %{public}d ReleaseInterface=%{public}d", __LINE__, ret);
    EXPECT_TRUE(ret == 0);
    ret = UsbdClient::ClaimInterface(dev, interfaceid);
    HDF_LOGI("UsbdTransferTest::UsbdRequestCancel008 %{public}d ClaimInterface=%{public}d", __LINE__, ret);
    EXPECT_TRUE(ret == 0);
    uint8_t tag[1000] = "queue Write";
    struct UsbPipe pipe = {interfaceid, pointid};
    std::vector<uint8_t> clientdata = {tag, tag + 11};
    std::vector<uint8_t> bufferdata = {buffer, buffer + length};
    ret = UsbdClient::RequestQueue(dev, pipe, clientdata, bufferdata);
    HDF_LOGI("UsbdTransferTest::UsbdRequestCancel008 %{public}d RequestQueue=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret == 0);
    dev.devAddr = 222;
    pipe.interfaceId = 222;
    dev.busNum = 222;
    ret = UsbdClient::RequestCancel(dev, pipe);
    HDF_LOGI("UsbdTransferTest::UsbdRequestCancel008 %{public}d RequestCancel=%{public}d", __LINE__, ret);
    ASSERT_TRUE(ret != 0);
    HDF_LOGI("-----------Case End : UsbdRequest008 : RequestCancel-----------");
}
