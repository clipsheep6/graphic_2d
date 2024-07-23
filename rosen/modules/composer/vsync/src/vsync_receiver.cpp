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

#include "vsync_receiver.h"
#include <memory>
#include <unistd.h>
#include <scoped_bytrace.h>
#include <fcntl.h>
#include <hitrace_meter.h>
#include "event_handler.h"
#include "graphic_common.h"
#include "res_sched_client.h"
#include "res_type.h"
#include "rs_frame_report_ext.h"
#include "vsync_log.h"
#include "sandbox_utils.h"
#include <rs_trace.h>

namespace OHOS {
namespace Rosen {
namespace {
constexpr int32_t INVALID_FD = -1;
}
void VSyncCallBackListener::OnReadable(int32_t fileDescriptor)
{
    HitracePerfScoped perfTrace(ScopedDebugTrace::isEnabled(), HITRACE_TAG_GRAPHIC_AGP, "OnReadablePerfCount");
    if (fileDescriptor < 0) {
        return;
    }
    // 3 is array size.
    int64_t data[3];
    int64_t now = 0;
    ssize_t ret = 0;
    ssize_t dataCount = 0;
    do {
        // only take the latest timestamp
        ret = read(fileDescriptor, data, sizeof(data));
        if (ret == 0) {
            return;
        }
        if (ret == -1) {
            if (errno == EINTR) {
                ret = 0;
                continue;
            }
        } else {
            dataCount += ret;
        }
    } while (ret != -1);

    VSyncCallback cb = nullptr;
    VSyncCallbackWithId cbWithId = nullptr;
    int64_t expectedEnd = 0;
    {
        std::lock_guard<std::mutex> locker(mtx_);
        cb = vsyncCallbacks_;
        cbWithId = vsyncCallbacksWithId_;
        RNVFlag_ = false;
        now = data[0];
        period_ = data[1];
        periodShared_ = data[1];
        timeStamp_ = data[0];
        timeStampShared_ = data[0];
        expectedEnd = CalculateExpectedEndLocked(now);
    }

    VLOGD("dataCount:%{public}d, cb == nullptr:%{public}d", dataCount, (cb == nullptr));
    // 1, 2: index of array data.
    RS_TRACE_NAME_FMT("ReceiveVsync dataCount: %ldbytes now: %ld expectedEnd: %ld vsyncId: %ld",
        dataCount, now, expectedEnd, data[2]); // data[2] is vsyncId
    if (dataCount > 0 && (cbWithId != nullptr || cb != nullptr)) {
        // data[2] is frameCount
        cbWithId != nullptr ? cbWithId(now, data[2], userData_) : cb(now, userData_);
    }
    if (OHOS::Rosen::RsFrameReportExt::GetInstance().GetEnable()) {
        OHOS::Rosen::RsFrameReportExt::GetInstance().ReceiveVSync();
    }
}

int64_t VSyncCallBackListener::CalculateExpectedEndLocked(int64_t now)
{
    int64_t expectedEnd = 0;
    if (now < period_ || now > INT64_MAX - period_) {
        RS_TRACE_NAME_FMT("invalid timestamps, now:%ld, period_:%ld", now, period_);
        VLOGE("invalid timestamps, now:" VPUBI64 ", period_:" VPUBI64, now, period_);
        period_ = 0;
        return 0;
    }
    expectedEnd = now + period_;
    // rs vsync offset is 5000000ns
    expectedEnd = (name_ == "rs") ? (expectedEnd + period_ - 5000000) : expectedEnd;
    return expectedEnd;
}

VSyncReceiver::VSyncReceiver(const sptr<IVSyncConnection>& conn,
    const sptr<IRemoteObject>& token,
    const std::shared_ptr<OHOS::AppExecFwk::EventHandler>& looper,
    const std::string& name)
    : connection_(conn), token_(token), looper_(looper),
    listener_(std::make_shared<VSyncCallBackListener>()),
    init_(false),
    fd_(INVALID_FD),
    name_(name)
{
};

VsyncError VSyncReceiver::Init()
{
    std::lock_guard<std::mutex> locker(initMutex_);
    if (init_) {
        return VSYNC_ERROR_OK;
    }
    if (connection_ == nullptr) {
        return VSYNC_ERROR_NULLPTR;
    }

    VsyncError ret = connection_->GetReceiveFd(fd_);
    if (ret != VSYNC_ERROR_OK) {
        return ret;
    }

    int32_t retVal = fcntl(fd_, F_SETFL, O_NONBLOCK); // set fd to NonBlock mode
    if (retVal != 0) {
        VLOGW("%{public}s fcntl set fd_:%{public}d NonBlock failed, retVal:%{public}d, errno:%{public}d",
            __func__, fd_, retVal, errno);
    }

    listener_->SetName(name_);

    if (looper_ == nullptr) {
        std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create(true);
        looper_ = std::make_shared<AppExecFwk::EventHandler>(runner);
        runner->Run();
        looper_->PostTask([this] { this->ThreadCreateNotify(); });
    }

    looper_->AddFileDescriptorListener(fd_, AppExecFwk::FILE_DESCRIPTOR_INPUT_EVENT, listener_, "vSyncTask");
    init_ = true;
    return VSYNC_ERROR_OK;
}

void VSyncReceiver::ThreadCreateNotify()
{
    int32_t pid = getprocpid();
    int32_t uid = getuid();
    int64_t tid = static_cast<uint32_t>(getproctid());
    VLOGI("vsync thread pid=%{public}d, tid=%{public}lld, uid=%{public}d.", pid, tid, uid);

    std::unordered_map<std::string, std::string> mapPayload;
    int64_t value = getproctid();
    mapPayload["pid"] = std::to_string(pid);
    mapPayload["uid"] = std::to_string(uid);
    mapPayload["tid"] = std::to_string(tid);
    OHOS::ResourceSchedule::ResSchedClient::GetInstance().ReportData(
        ResourceSchedule::ResType::RES_TYPE_REPORT_VSYNC_TID, value, mapPayload);
}

VSyncReceiver::~VSyncReceiver()
{
    if (fd_ != INVALID_FD) {
        looper_->RemoveFileDescriptorListener(fd_);
        close(fd_);
        fd_ = INVALID_FD;
        Destroy();
    }
}

VsyncError VSyncReceiver::RequestNextVSync(FrameCallback callback)
{
    return RequestNextVSync(callback, "unknown", 0);
}

VsyncError VSyncReceiver::RequestNextVSync(FrameCallback callback, const std::string &fromWhom, int64_t lastVSyncTS)
{
    std::lock_guard<std::mutex> locker(initMutex_);
    if (!init_) {
        VLOGE("%{public}s not init", __func__);
        return VSYNC_ERROR_NOT_INIT;
    }
    listener_->SetCallback(callback);
    listener_->SetRNVFlag(true);
    ScopedDebugTrace func("VSyncReceiver::RequestNextVSync:" + name_);
    if (OHOS::Rosen::RsFrameReportExt::GetInstance().GetEnable()) {
        OHOS::Rosen::RsFrameReportExt::GetInstance().RequestNextVSync();
    }
    return connection_->RequestNextVSync(fromWhom, lastVSyncTS);
}

VsyncError VSyncReceiver::SetVSyncRate(FrameCallback callback, int32_t rate)
{
    std::lock_guard<std::mutex> locker(initMutex_);
    if (!init_) {
        return VSYNC_ERROR_API_FAILED;
    }
    listener_->SetCallback(callback);
    return connection_->SetVSyncRate(rate);
}

/* 设置每帧回调 */
VsyncError VSyncReceiver::SetVsyncCallBackForEveryFrame(FrameCallback callback, bool isOpen)
{
    if (isOpen) {
        return SetVSyncRate(callback, 1);
    } else {
        return SetVSyncRate(callback, -1);
    }
}

VsyncError VSyncReceiver::GetVSyncPeriod(int64_t &period)
{
    int64_t timeStamp;
    return GetVSyncPeriodAndLastTimeStamp(period, timeStamp);
}

VsyncError VSyncReceiver::GetVSyncPeriodAndLastTimeStamp(int64_t &period, int64_t &timeStamp, bool isThreadShared)
{
    std::lock_guard<std::mutex> locker(initMutex_);
    if (!init_) {
        VLOGE("%{public}s not init", __func__);
        return VSYNC_ERROR_NOT_INIT;
    }
    if (isThreadShared == false) {
        int64_t periodNotShared = listener_->GetPeriod();
        int64_t timeStampNotShared = listener_->GetTimeStamp();
        if (periodNotShared == 0 || timeStampNotShared == 0) {
            VLOGD("%{public}s Hardware vsync is not available. please try again later!", __func__);
            return VSYNC_ERROR_UNKOWN;
        }
        period = periodNotShared;
        timeStamp = timeStampNotShared;
    } else {
        int64_t periodShared = listener_->GetPeriodShared();
        int64_t timeStampShared = listener_->GetTimeStampShared();
        if (periodShared == 0 || timeStampShared == 0) {
            VLOGD("%{public}s Hardware vsync is not available. please try again later!", __func__);
            return VSYNC_ERROR_UNKOWN;
        }
        period = periodShared;
        timeStamp = timeStampShared;
    }
    RS_TRACE_NAME_FMT("VSyncReceiver:period:%ld timeStamp:%ld isThreadShared:%d", period, timeStamp, isThreadShared);
    return VSYNC_ERROR_OK;
}

void VSyncReceiver::CloseVsyncReceiverFd()
{
    std::lock_guard<std::mutex> locker(initMutex_);
    if (looper_ != nullptr) {
        looper_->RemoveFileDescriptorListener(fd_);
        VLOGI("%{public}s looper remove fd listener, fd=%{public}d", __func__, fd_);
    }

    if (fd_ > 0) {
        close(fd_);
        fd_ = INVALID_FD;
    }
}

VsyncError VSyncReceiver::Destroy()
{
    std::lock_guard<std::mutex> locker(initMutex_);
    if (connection_ == nullptr) {
        return VSYNC_ERROR_API_FAILED;
    }
    return connection_->Destroy();
}

bool VSyncReceiver::IsRequestedNextVSync()
{
    std::lock_guard<std::mutex> locker(initMutex_);
    if (!init_) {
        return false;
    }
    return listener_->GetRNVFlag();
}

VsyncError VSyncReceiver::SetUiDvsyncSwitch(bool dvsyncSwitch)
{
    std::lock_guard<std::mutex> locker(initMutex_);
    if (!init_) {
        return VSYNC_ERROR_API_FAILED;
    }
    return connection_->SetUiDvsyncSwitch(dvsyncSwitch);
}
} // namespace Rosen
} // namespace OHOS
