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

#include <cstdio>
#include <cstring>
#include <gtest/gtest.h>
extern "C" {
#include "usb_device_cdcacm_test.h"
}

using namespace std;
using namespace testing::ext;
namespace {
static struct AcmDevice *g_acmDevice = nullptr;
class UsbDeviceSdkIfTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void UsbDeviceSdkIfTest::SetUpTestCase()
{
}

void UsbDeviceSdkIfTest::TearDownTestCase()
{
}

void UsbDeviceSdkIfTest::SetUp()
{
}

void UsbDeviceSdkIfTest::TearDown()
{
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfCreateDevice001, TestSize.Level1)
{
    g_acmDevice = SetUpAcmDevice();
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->fnDev);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfRequestAsync001, TestSize.Level1)
{
    int ret;
    ret = UsbFnSubmitRequestAsync(nullptr);
    EXPECT_TRUE(ret != HDF_SUCCESS);
}

static void ReadComplete(uint8_t pipe, struct UsbFnRequest *req)
{
    EXPECT_NE(nullptr, req);
    if (req->actual) {
        uint8_t *data = (uint8_t *)req->buf;
        data[req->actual] = '\0';
        printf("receive [%d] bytes data: %s\n", req->actual, data);
        if (strcmp((const char *)data, "q") == 0 || \
            strcmp((const char *)data, "q\n") == 0) {
            g_acmDevice->submitExit = 1;
        }
    }
    g_acmDevice->submit = 1;
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfRequestAsync002, TestSize.Level1)
{
    struct UsbFnRequest *req = nullptr;
    int ret;
    int waitMs = 1000;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->dataIface.handle);
    printf("wait receiving data form host, please connect\n");
    req = UsbFnAllocRequest(g_acmDevice->dataIface.handle, g_acmDevice->dataOutPipe.id,
        g_acmDevice->dataOutPipe.maxPacketSize);
    EXPECT_NE(nullptr, req);
    req->complete = ReadComplete;
    req->context = g_acmDevice;
    g_acmDevice->submit = 0;
    while (1) {
        ret = UsbFnSubmitRequestAsync(req);
        if (ret != HDF_SUCCESS) {
            continue;
        }
        while(g_acmDevice->submit == 0) {
            OsalMSleep(waitMs);
        }
        EXPECT_TRUE(ret == HDF_SUCCESS);
        EXPECT_EQ(1, g_acmDevice->submit);
        g_acmDevice->submit = 0;
        if (req->actual > 0)
            break;
    }
    ret = UsbFnFreeRequest(req);
    EXPECT_TRUE(ret == HDF_SUCCESS);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfRequestAsync003, TestSize.Level1)
{
    struct UsbFnRequest *req = nullptr;
    int ret;
    int waitMs = WAIT_10MS;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->dataIface.handle);
    printf("recv data until 'q' exit\n");
    g_acmDevice->submitExit = 0;
    while (g_acmDevice->submitExit == 0) {
        req = UsbFnAllocRequest(g_acmDevice->dataIface.handle, g_acmDevice->dataOutPipe.id,
            g_acmDevice->dataOutPipe.maxPacketSize);
        EXPECT_NE(nullptr, req);
        req->complete = ReadComplete;
        req->context = g_acmDevice;
        g_acmDevice->submit = 0;
        ret = UsbFnSubmitRequestAsync(req);
        EXPECT_TRUE(ret == HDF_SUCCESS);
        EXPECT_EQ(0, g_acmDevice->submit);
        while(g_acmDevice->submit == 0) {
            OsalMSleep(waitMs);
        }
        g_acmDevice->submit = 0;
        ret = UsbFnFreeRequest(req);
        EXPECT_TRUE(ret == HDF_SUCCESS);
    }
}

static void WriteComplete(uint8_t pipe, struct UsbFnRequest *req)
{
    EXPECT_EQ(0, req->status);
    printf("write data ok\n");
    g_acmDevice->submit = 1;
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfRequestAsync004, TestSize.Level1)
{
    int ret;
    struct UsbFnRequest  *req = nullptr;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->dataIface.handle);
    req = UsbFnAllocRequest(g_acmDevice->dataIface.handle, g_acmDevice->dataInPipe.id,
        g_acmDevice->dataInPipe.maxPacketSize);
    EXPECT_NE(nullptr, req);
    req->complete = WriteComplete;
    req->context = g_acmDevice;
    g_acmDevice->submit = 0;
    printf("------send \"abc\" to host------\n");
    memcpy_s(req->buf, g_acmDevice->dataInPipe.maxPacketSize, "abc", strlen("abc"));
    req->length = strlen("abc");
    ret = UsbFnSubmitRequestAsync(req);
    EXPECT_TRUE(ret == HDF_SUCCESS);
    while(g_acmDevice->submit == 0) {
        OsalMSleep(1);
    }
    g_acmDevice->submit = 0;
    ret = UsbFnFreeRequest(req);
    EXPECT_TRUE(ret == HDF_SUCCESS);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfRequestAsync005, TestSize.Level1)
{
    int ret;
    int loopTime = TEST_TIMES;
    struct UsbFnRequest  *req = nullptr;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->dataIface.handle);
    printf("------send \"xyz\" 10 times to host------\n");
    while (loopTime--) {
        req = UsbFnAllocRequest(g_acmDevice->dataIface.handle, g_acmDevice->dataInPipe.id,
            g_acmDevice->dataInPipe.maxPacketSize);
        EXPECT_NE(nullptr, req);
        req->complete = WriteComplete;
        req->context = g_acmDevice;
        g_acmDevice->submit = 0;
        memcpy_s(req->buf, g_acmDevice->dataInPipe.maxPacketSize, "xyz", strlen("xyz"));
        req->length = strlen("xyz");
        ret = UsbFnSubmitRequestAsync(req);
        EXPECT_TRUE(ret == HDF_SUCCESS);
        while(g_acmDevice->submit == 0) {
            OsalMSleep(1);
        }
        g_acmDevice->submit = 0;
        ret = UsbFnFreeRequest(req);
        EXPECT_TRUE(ret == HDF_SUCCESS);
    }
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfRequestSync001, TestSize.Level1)
{
    int ret;
    ret = UsbFnSubmitRequestSync(nullptr, 0);
    EXPECT_TRUE(ret != HDF_SUCCESS);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfRequestSync002, TestSize.Level1)
{
    struct UsbFnRequest *req = nullptr;
    int ret;
    uint8_t *data = nullptr;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->dataIface.handle);
    printf("wait receiving data form host:\n");
    req = UsbFnAllocRequest(g_acmDevice->dataIface.handle, g_acmDevice->dataOutPipe.id,
        g_acmDevice->dataOutPipe.maxPacketSize);
    EXPECT_NE(nullptr, req);
    ret = UsbFnSubmitRequestSync(req, 0);
    EXPECT_TRUE(ret == 0);
    EXPECT_TRUE(req->actual > 0);
    EXPECT_TRUE(req->status == USB_REQUEST_COMPLETED);
    data = (uint8_t *)req->buf;
    data[req->actual] = '\0';
    ret = UsbFnFreeRequest(req);
    EXPECT_TRUE(ret == HDF_SUCCESS);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfRequestSync003, TestSize.Level1)
{
    struct UsbFnRequest *req = nullptr;
    int ret;
    int submitExit = 0;
    uint8_t *data;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->dataIface.handle);
    printf("receive data until 'q' exit\n");
    while (submitExit == 0){
        req = UsbFnAllocRequest(g_acmDevice->dataIface.handle, g_acmDevice->dataOutPipe.id,
        g_acmDevice->dataOutPipe.maxPacketSize);
        EXPECT_NE(nullptr, req);
        ret = UsbFnSubmitRequestSync(req, 0);
        EXPECT_TRUE(ret == 0);
        EXPECT_TRUE(req->actual > 0);
        EXPECT_TRUE(req->status == USB_REQUEST_COMPLETED);
        data = (uint8_t *)req->buf;
        data[req->actual] = '\0';
        if (strcmp((const char *)data, "q") == 0 || \
            strcmp((const char *)data, "q\n") == 0) {
            submitExit = 1;
        }
        printf("receive data from host: %s------\n", data);
        ret = UsbFnFreeRequest(req);
        EXPECT_TRUE(ret == HDF_SUCCESS);
    }
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfRequestSync004, TestSize.Level1)
{
    int ret;
    struct UsbFnRequest  *req = nullptr;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->dataIface.handle);
    req = UsbFnAllocRequest(g_acmDevice->dataIface.handle, g_acmDevice->dataInPipe.id,
        g_acmDevice->dataInPipe.maxPacketSize);
    EXPECT_NE(nullptr, req);
    printf("------send \"abc\" to host------\n");
    memcpy_s(req->buf, g_acmDevice->dataInPipe.maxPacketSize, "abc", strlen("abc"));
    req->length = strlen("abc");
    ret = UsbFnSubmitRequestSync(req, 0);
    EXPECT_TRUE(ret == 0);
    EXPECT_TRUE(req->actual == strlen("abc"));
    EXPECT_TRUE(req->status == USB_REQUEST_COMPLETED);
    ret = UsbFnFreeRequest(req);
    EXPECT_TRUE(ret == HDF_SUCCESS);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfRequestSync005, TestSize.Level1)
{
    int ret;
    int loopTime = TEST_TIMES;
    struct UsbFnRequest  *req = nullptr;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->dataIface.handle);
    printf("------send \"abcdefg\" 10 times to host------\n");
    while (loopTime--) {
        req = UsbFnAllocRequest(g_acmDevice->dataIface.handle, g_acmDevice->dataInPipe.id,
        g_acmDevice->dataInPipe.maxPacketSize);
        EXPECT_NE(nullptr, req);
        memcpy_s(req->buf, g_acmDevice->dataInPipe.maxPacketSize, "abcdefg", strlen("abcdefg"));
        req->length = strlen("abcdefg");
        ret = UsbFnSubmitRequestSync(req, 0);
        EXPECT_TRUE(ret == 0);
        EXPECT_TRUE(req->actual == strlen("abcdefg"));
        EXPECT_TRUE(req->status == USB_REQUEST_COMPLETED);
        ret = UsbFnFreeRequest(req);
        EXPECT_TRUE(ret == HDF_SUCCESS);
    }
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfRequestSync006, TestSize.Level1)
{
    struct UsbFnRequest *req = nullptr;
    int ret;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->dataIface.handle);
    printf("test sync timeout 5s:\n");
    req = UsbFnAllocRequest(g_acmDevice->dataIface.handle, g_acmDevice->dataOutPipe.id,
        g_acmDevice->dataOutPipe.maxPacketSize);
    EXPECT_NE(nullptr, req);
    ret = UsbFnSubmitRequestSync(req, SYNC_5000MS);
    EXPECT_TRUE(ret != 0);
    ret = UsbFnFreeRequest(req);
    EXPECT_TRUE(ret == HDF_SUCCESS);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfRequestSync007, TestSize.Level1)
{
    struct UsbFnRequest *req = nullptr;
    int ret;
    ret = UsbFnSubmitRequestSync(req, SYNC_5000MS);
    EXPECT_TRUE(ret != 0);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfGetReqStatus001, TestSize.Level1)
{
    int ret;
    UsbRequestStatus status;
    struct UsbFnRequest *notifyReq = nullptr;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->ctrlIface.handle);
    notifyReq = UsbFnAllocRequest(g_acmDevice->ctrlIface.handle, g_acmDevice->notifyPipe.id,
        sizeof(struct UsbCdcNotification));
    EXPECT_TRUE(notifyReq != nullptr);
    ret = UsbFnGetRequestStatus(notifyReq, &status);
    EXPECT_TRUE(ret == HDF_SUCCESS);
    EXPECT_TRUE(status >= USB_REQUEST_COMPLETED && status <= USB_REQUEST_OVERFLOW);
    ret = UsbFnFreeRequest(notifyReq);
    EXPECT_TRUE(ret == HDF_SUCCESS);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfGetReqStatus002, TestSize.Level1)
{
    int ret;
    UsbRequestStatus status;
    struct UsbFnRequest *notifyReq = nullptr;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->ctrlIface.handle);
    notifyReq = UsbFnAllocCtrlRequest(g_acmDevice->ctrlIface.handle, sizeof(struct UsbCdcNotification));
    EXPECT_TRUE(notifyReq != nullptr);
    ret = UsbFnGetRequestStatus(notifyReq, &status);
    EXPECT_TRUE(ret == HDF_SUCCESS);
    EXPECT_TRUE(status >= USB_REQUEST_COMPLETED && status <= USB_REQUEST_OVERFLOW);
    ret = UsbFnFreeRequest(notifyReq);
    EXPECT_TRUE(ret == HDF_SUCCESS);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfGetReqStatus003, TestSize.Level1)
{
    int ret;
    struct UsbFnRequest *notifyReq = nullptr;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->ctrlIface.handle);
    notifyReq = UsbFnAllocRequest(g_acmDevice->dataIface.handle, g_acmDevice->dataOutPipe.id,
        g_acmDevice->dataOutPipe.maxPacketSize);
    EXPECT_TRUE(notifyReq != nullptr);
    ret = UsbFnGetRequestStatus(notifyReq, nullptr);
    EXPECT_TRUE(ret != HDF_SUCCESS);
    ret = UsbFnFreeRequest(notifyReq);
    EXPECT_TRUE(ret == HDF_SUCCESS);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfGetReqStatus004, TestSize.Level1)
{
    int ret;
    UsbRequestStatus status;
    struct UsbFnRequest *notifyReq = nullptr;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->ctrlIface.handle);
    notifyReq = UsbFnAllocRequest(g_acmDevice->dataIface.handle, g_acmDevice->dataOutPipe.id,
        g_acmDevice->dataOutPipe.maxPacketSize);
    EXPECT_TRUE(notifyReq != nullptr);
    ret = UsbFnSubmitRequestAsync(notifyReq);
    EXPECT_TRUE(ret == HDF_SUCCESS);
    ret = UsbFnCancelRequest(notifyReq);
    EXPECT_TRUE(ret == HDF_SUCCESS);
    ret = UsbFnGetRequestStatus(notifyReq, &status);
    EXPECT_TRUE(ret == HDF_SUCCESS);
    EXPECT_TRUE(status >= USB_REQUEST_COMPLETED && status <= USB_REQUEST_OVERFLOW);
    ret = UsbFnFreeRequest(notifyReq);
    EXPECT_TRUE(ret == HDF_SUCCESS);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfGetReqStatus005, TestSize.Level1)
{
    int ret;
    UsbRequestStatus status;
    EXPECT_NE(nullptr, g_acmDevice);
    ret = UsbFnGetRequestStatus(nullptr, &status);
    EXPECT_TRUE(ret != HDF_SUCCESS);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfCancelReq001, TestSize.Level1)
{
    int ret;
    EXPECT_NE(nullptr, g_acmDevice);
    ret = UsbFnCancelRequest(nullptr);
    EXPECT_TRUE(ret != HDF_SUCCESS);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfCancelReq002, TestSize.Level1)
{
    int ret;
    struct UsbFnRequest *notifyReq = nullptr;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->ctrlIface.handle);
    notifyReq = UsbFnAllocRequest(g_acmDevice->ctrlIface.handle, g_acmDevice->notifyPipe.id,
        sizeof(struct UsbCdcNotification));
    EXPECT_TRUE(notifyReq != nullptr);
    ret = UsbFnSubmitRequestAsync(notifyReq);
    EXPECT_TRUE(ret == HDF_SUCCESS);
    ret = UsbFnCancelRequest(notifyReq);
    EXPECT_TRUE(ret == HDF_SUCCESS);
    ret = UsbFnFreeRequest(notifyReq);
    EXPECT_TRUE(ret == HDF_SUCCESS);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfCancelReq003, TestSize.Level1)
{
    int ret;
    struct UsbFnRequest  *req = nullptr;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->dataIface.handle);
    req = UsbFnAllocRequest(g_acmDevice->dataIface.handle, g_acmDevice->dataInPipe.id,
        g_acmDevice->dataInPipe.maxPacketSize);
    EXPECT_NE(nullptr, req);
    req->complete = WriteComplete;
    req->context = g_acmDevice;
    g_acmDevice->submit = 0;
    printf("------send \"abcdef\" to host------\n");
    memcpy_s(req->buf, g_acmDevice->dataInPipe.maxPacketSize, "abcdef", strlen("abcdef"));
    req->length = strlen("abcdef");
    ret = UsbFnSubmitRequestAsync(req);
    EXPECT_TRUE(ret == HDF_SUCCESS);
    while(g_acmDevice->submit == 0) {
        OsalMSleep(1);
    }
    g_acmDevice->submit = 0;
    ret = UsbFnCancelRequest(req);
    EXPECT_TRUE(ret != HDF_SUCCESS);
    ret = UsbFnFreeRequest(req);
    EXPECT_TRUE(ret == HDF_SUCCESS);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfCancelReq004, TestSize.Level1)
{
    int ret;
    struct UsbFnRequest *req = nullptr;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->dataIface.handle);
    req = UsbFnAllocRequest(g_acmDevice->dataIface.handle, g_acmDevice->dataOutPipe.id,
        g_acmDevice->dataOutPipe.maxPacketSize);
    EXPECT_TRUE(req != nullptr);
    ret = UsbFnSubmitRequestAsync(req);
    EXPECT_TRUE(ret == HDF_SUCCESS);
    ret = UsbFnCancelRequest(req);
    EXPECT_TRUE(ret == HDF_SUCCESS);
    ret = UsbFnFreeRequest(req);
    EXPECT_TRUE(ret == HDF_SUCCESS);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfCancelReq005, TestSize.Level1)
{
    int ret;
    struct UsbFnRequest *req = nullptr;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->dataIface.handle);
    req = UsbFnAllocRequest(g_acmDevice->dataIface.handle, g_acmDevice->dataInPipe.id,
        g_acmDevice->dataInPipe.maxPacketSize);
    EXPECT_TRUE(req != nullptr);
    ret = UsbFnCancelRequest(req);
    EXPECT_TRUE(ret != HDF_SUCCESS);
    ret = UsbFnFreeRequest(req);
    EXPECT_TRUE(ret == HDF_SUCCESS);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfCancelReq006, TestSize.Level1)
{
    int ret;
    struct UsbFnRequest *req = nullptr;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->dataIface.handle);
    req = UsbFnAllocRequest(g_acmDevice->dataIface.handle, g_acmDevice->dataOutPipe.id,
        g_acmDevice->dataOutPipe.maxPacketSize);
    EXPECT_TRUE(req != nullptr);
    ret = UsbFnCancelRequest(req);
    EXPECT_TRUE(ret != HDF_SUCCESS);
    ret = UsbFnFreeRequest(req);
    EXPECT_TRUE(ret == HDF_SUCCESS);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfCancelReq007, TestSize.Level1)
{
    int ret;
    struct UsbFnRequest *ctrlReq = nullptr;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->ctrlIface.handle);
    ctrlReq = UsbFnAllocCtrlRequest(g_acmDevice->ctrlIface.handle,
            sizeof(struct UsbCdcLineCoding) + sizeof(struct UsbCdcLineCoding));
    EXPECT_TRUE(ctrlReq != nullptr);
    ret = UsbFnCancelRequest(ctrlReq);
    EXPECT_TRUE(ret != HDF_SUCCESS);
    ret = UsbFnFreeRequest(ctrlReq);
    EXPECT_TRUE(ret == HDF_SUCCESS);
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfCancelReq008, TestSize.Level1)
{
    int ret;
    int count = 0;
    struct UsbFnRequest *req = nullptr;
    EXPECT_NE(nullptr, g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->dataIface.handle);
    for (count = 0; count < TEST_TIMES; count++) {
        req = UsbFnAllocRequest(g_acmDevice->dataIface.handle, g_acmDevice->dataOutPipe.id,
            g_acmDevice->dataOutPipe.maxPacketSize);
        EXPECT_TRUE(req != nullptr);
        ret = UsbFnSubmitRequestAsync(req);
        EXPECT_TRUE(ret == HDF_SUCCESS);
        ret = UsbFnCancelRequest(req);
        EXPECT_TRUE(ret == HDF_SUCCESS);
        ret = UsbFnFreeRequest(req);
        EXPECT_TRUE(ret == HDF_SUCCESS);
    }
}

HWTEST_F(UsbDeviceSdkIfTest, CheckDeviceSdkIfRemoveDevice002, TestSize.Level1)
{
    int ret;

    EXPECT_NE(nullptr, g_acmDevice);
    ReleaseAcmDevice(g_acmDevice);
    EXPECT_NE(nullptr, g_acmDevice->fnDev);
    ret = UsbFnRemoveDevice(g_acmDevice->fnDev);
    EXPECT_EQ(HDF_SUCCESS, ret);
    OsalMemFree(g_acmDevice);
}
}
