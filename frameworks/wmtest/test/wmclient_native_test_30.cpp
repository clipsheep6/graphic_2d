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

#include "wmclient_native_test_30.h"

#include <cstdio>
#include <securec.h>
#include <thread>

#include <display_type.h>
#include <iservice_registry.h>
#include <window_manager.h>
#include <surface.h>

#include "inative_test.h"
#include "native_test_class.h"
#include "util.h"

using namespace OHOS;

namespace {
class WMClientNativeTest30 : public INativeTest, public IBufferConsumerListenerClazz {
public:
    std::string GetDescription() const override
    {
        constexpr const char *desc = "attach/detach test";
        return desc;
    }

    std::string GetDomain() const override
    {
        constexpr const char *domain = "wmclient";
        return domain;
    }

    int32_t GetID() const override
    {
        constexpr int32_t id = 30;
        return id;
    }

    uint32_t GetLastTime() const override
    {
        constexpr uint32_t lastTime = LAST_TIME_FOREVER;
        return lastTime;
    }

    void Run(int32_t argc, const char **argv) override
    {
        auto initRet = WindowManager::GetInstance()->Init();
        if (initRet) {
            printf("init failed with %s\n", GSErrorStr(initRet).c_str());
            ExitTest();
            return;
        }

        pipe(pipeFd);
        pid_t pid = fork();
        if (pid < 0) {
            printf("%s fork failed", __func__);
            ExitTest();
            return;
        }

        if (pid == 0) {
            ChildProcess();
        } else {
            MainProcess();
        }
    }

    GSError OnReleaseBuffer(sptr<SurfaceBuffer> rbuffer)
    {
        auto sret = bq2->DetachBuffer(rbuffer);
        if (sret != GSERROR_OK) {
            printf("OnReleaseBuffer, Detach failed!\n");
            return GSERROR_NO_ENTRY;
        }

        do {
            sret = bq1->AttachBuffer(rbuffer);
        } while (sret != GSERROR_OK);

        auto ret = bq1->ReleaseBuffer(rbuffer, -1);
        if (ret != GSERROR_OK) {
            printf("release buffer failed!\n");
        }
        return GSERROR_OK;
    }

    virtual void OnBufferAvailable() override
    {
        int32_t flushFence;
        int64_t timestamp;
        Rect damage;
        auto sret = bq1->AcquireBuffer(sbuffer, flushFence, timestamp, damage);
        if (sret != GSERROR_OK) {
            printf("AcquireBuffer failed!\n");
            return;
        }

        sret = bq1->DetachBuffer(sbuffer);
        if (sret != GSERROR_OK) {
            printf("Detach buffer failed!\n");
            return;
        }

        if (sbuffer == nullptr) {
            printf("sbuffer is null\n");
        }

        do {
            sret = bq2->AttachBuffer(sbuffer);
        } while (sret != GSERROR_OK);

        if (sret != GSERROR_OK) {
            if (sret == GSERROR_NO_BUFFER) {
                printf("No Buffer!, %d\n", __LINE__);
                bq1->AttachBuffer(sbuffer);
                bq1->ReleaseBuffer(sbuffer, -1);
            } else {
                printf("Attach buffer failed!\n");
            }
            return;
        }

        BufferFlushConfig fconfig = {
            .damage = {
                .w = sbuffer->GetWidth(),
                .h = sbuffer->GetHeight(),
            }
        };

        sret = bq2->FlushBuffer(sbuffer, -1, fconfig);
        if (sret != GSERROR_OK) {
            printf("flush buffer failed\n");
        }
    }

private:
    void MainProcess()
    {
        window = NativeTestFactory::CreateWindow(WINDOW_TYPE_NORMAL);
        if (window == nullptr) {
            printf("%s window == nullptr\n", __func__);
            ExitTest();
            return;
        }
        window->SwitchTop();
        bq2 = window->GetSurface();
        auto func = [this](sptr<SurfaceBuffer> &buffer) {
            OnReleaseBuffer(buffer);
            return GSERROR_OK;
        };
        bq2->RegisterReleaseListener(func);
        if (bq2 == nullptr) {
            printf("%s bq2 == nullptr\n", __func__);
            ExitTest();
            return;
        }

        bq1 = Surface::CreateSurfaceAsConsumer("bq1");
        bq1->SetDefaultWidthAndHeight(window->GetWidth(), window->GetHeight());
        bq1->SetDefaultUsage(bq2->GetDefaultUsage());
        bq1->RegisterConsumerListener(this);
        auto producer = bq1->GetProducer();
        if (producer == nullptr) {
            printf("%s producer == nullptr\n", __func__);
            ExitTest();
            return;
        }

        auto producerObject = producer->AsObject();
        if (producerObject == nullptr) {
            printf("%s producerObject == nullptr\n", __func__);
            ExitTest();
            return;
        }

        auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (sam == nullptr) {
            printf("%s sam == nullptr\n", __func__);
            ExitTest();
            return;
        }

        sam->AddSystemAbility(SAID, producerObject);

        uint32_t msg = 0;
        write(pipeFd[1], &msg, sizeof(msg));

        sleep(0);

        char buf[10];
        read(pipeFd[0], buf, sizeof(buf));
        sam->RemoveSystemAbility(SAID);
        ExitTest();
    }

    void ChildProcess()
    {
        char buf[10];
        read(pipeFd[0], &buf, sizeof(buf));

        auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (sam == nullptr) {
            printf("%s main_process sam == nullptr\n", __func__);
            ExitTest();
            return;
        }

        auto robj = sam->GetSystemAbility(SAID);
        if (robj == nullptr) {
            printf("%s main_process robj == nullptr\n", __func__);
            ExitTest();
            return;
        }

        auto bp = iface_cast<IBufferProducer>(robj);
        if (bp == nullptr) {
            printf("%s main_process bp == nullptr\n", __func__);
            ExitTest();
            return;
        }

        ipcSurface = Surface::CreateSurfaceAsProducer(bp);
        if (ipcSurface == nullptr) {
            printf("%s main_process ipcSurface == nullptr\n", __func__);
            ExitTest();
            return;
        }
        windowSync = NativeTestSync::CreateSync(NativeTestDraw::FlushDraw, ipcSurface);

        constexpr uint32_t delayTime = 3000;
        PostTask(std::bind(&WMClientNativeTest30::ChildProcessAfter, this), delayTime);
    }

    void ChildProcessAfter()
    {
        char buf[10] = "end";
        write(pipeFd[1], buf, sizeof(buf));
        ExitTest();
        return;
    }

private:
    sptr<SurfaceBuffer> sbuffer = nullptr;
    sptr<Window> window = nullptr;
    sptr<NativeTestSync> windowSync = nullptr;
    int32_t pipeFd[2] = {};
    static inline constexpr uint32_t SAID = 4699;
    sptr<Surface> bq1 = nullptr;
    sptr<Surface> bq2 = nullptr;
    sptr<Surface> ipcSurface = nullptr;
} g_autoload;
}
