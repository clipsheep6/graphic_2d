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

#ifndef RS_RECORDING_THREAD
#define RS_RECORDING_THREAD


#include "pipeline/rs_draw_cmd_list.h"
#include "event_handler.h"

#include <sys/stat.h>
#include "rs_trace.h"

namespace OHOS::Rosen {
class RSB_EXPORT RSRecordingThread {
public:
    static RSRecordingThread& Instance();
    void Start();
    void PostTask(const std::function<void()> & task);
    bool CheckAndRecording();
    void FinishRecordingOneFrame();
    void RecordingToFile(const std::shared_ptr<DrawCmdList> & drawCmdList);
    [[nodiscard]] int GetCurDumpFrame() const {return curDumpFrame;}

    const static size_t RECORDING_PARCEL_MAX_CAPCITY = 234 * 1000 * 1024;
private:
    RSRecordingThread() = default;
    ~RSRecordingThread() = default;

    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
    std::string fileDir = "";
    bool isRecordingEnabled = false;
    int dumpFrameNum = 0;
    int curDumpFrame = 0;
};
} // namespace OHOS::Rosen

#endif // RS_RECORDING_THREAD