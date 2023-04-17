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

#include "benchmarks/rs_recording_thread.h"
#include "benchmarks/rs_recording_parcel.h"
#include "benchmarks/file_util.h"

#include "common/rs_thread_handler.h"
#include "platform/common/rs_system_properties.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen {
RSRecordingThread &RSRecordingThread::Instance()
{
    static RSRecordingThread instance;
    return instance;
}

void RSRecordingThread::Start()
{
    runner_ = AppExecFwk::EventRunner::Create("RRecordingThread");
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
}

void RSRecordingThread::PostTask(const std::function<void()> &task)
{
    if (handler_) {
        handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

bool RSRecordingThread::CheckAndRecording()
{
    if (!handler_) {
        RS_LOGE("RSRecordingThread::CheckAndRecording handler_ is nullptr");
        return false;
    }
    RSTaskMessage::RSTask task = [this]() {
        std::string line = "RSRecordingThread::CheckAndRecording curDumpFrame = " + std::to_string(curDumpFrame) +
            ", dumpFrameNum = " + std::to_string(dumpFrameNum);
        RS_LOGD(line.c_str());
        RS_TRACE_NAME(line);
        isRecordingEnabled = RSSystemProperties::GetRecordingEnabled();
        // init curDumpFrame
        if (isRecordingEnabled && curDumpFrame == 0) {
            dumpFrameNum = RSSystemProperties::GetDumpFrameNum();
            fileDir = RSSystemProperties::GetRecordingFile();
            if (access(fileDir.c_str(), F_OK) != 0) {
                mkdir(fileDir.c_str(), (S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH));
            }
        }
    };
    PostTask(task);
    return isRecordingEnabled;
}

void RSRecordingThread::FinishRecordingOneFrame() {
    std::string line = "RSRecordingThread::FinishRecordingOneFrame curDumpFrame = " + std::to_string(curDumpFrame) +
        ", dumpFrameNum = " + std::to_string(dumpFrameNum);
    RS_LOGD(line.c_str());
    RS_TRACE_NAME(line);
    if (curDumpFrame < dumpFrameNum) {
        curDumpFrame++;
    } else {
        isRecordingEnabled = false;
        curDumpFrame = -1;
        dumpFrameNum = 0;
        fileDir = "";
        RSSystemProperties::SetRecordingDisenabled();
        RS_LOGD("RSRecordingThread::FinishRecordingOneFrame isRecordingEnabled = false");
    }
}

void RSRecordingThread::RecordingToFile(const std::shared_ptr<DrawCmdList> & drawCmdList) {
    if (curDumpFrame < 0) return;
    int tmpCurDumpFrame = curDumpFrame;
    FinishRecordingOneFrame();
    RSTaskMessage::RSTask task = [this, drawCmdList, tmpCurDumpFrame]() {
        std::string line = "RSRecordingThread::RecordingToFile curDumpFrame = " + std::to_string(curDumpFrame) +
            ", dumpFrameNum = " + std::to_string(dumpFrameNum);
        RS_LOGD(line.c_str());
        RS_TRACE_NAME(line);
        // file name
        std::string file = fileDir + "/frame" + std::to_string(tmpCurDumpFrame) + ".txt";
        std::string file_ops = fileDir + "/ops_frame" + std::to_string(tmpCurDumpFrame) + ".txt";
        // get data
        RSRecordingParcel recordingParcel;
        recordingParcel.SetMaxCapacity(RECORDING_PARCEL_MAX_CAPCITY);
        recordingParcel.SetIsRecrodingToFile(true);
        drawCmdList->Marshalling(recordingParcel);
        size_t sz = recordingParcel.GetDataSize();
        uintptr_t buf = recordingParcel.GetData();

        WriteToFile(buf, sz, file);
    };
    PostTask(task);
}
}
