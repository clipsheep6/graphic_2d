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

#ifndef RENDER_SERVICE_PROFILER_H
#define RENDER_SERVICE_PROFILER_H

#include <string>

namespace OHOS {
class MessageParcel;
class MessageOption;
} // namespace OHOS

namespace OHOS::Rosen {

class RSRenderService;
class RSMainThread;
class RSRenderServiceConnection;
class RSTransactionData;

class RSProfiler {
public:
    static void Init(RSRenderService* renderService);

    static void MainThreadOnFrameBegin();
    static void MainThreadOnFrameEnd();
    static void MainThreadOnRenderBegin();
    static void MainThreadOnRenderEnd();
    static void MainThreadOnProcessCommand();

    static void RenderServiceOnCreateConnection(pid_t remotePid);
    static void RenderServiceConnectionOnRemoteRequest(RSRenderServiceConnection* connection, uint32_t code,
        MessageParcel& data, MessageParcel& reply, MessageOption& option);

    static void UnmarshalThreadOnRecvParcel(const MessageParcel* parcel, RSTransactionData* data);

    static RSRenderService* GetRenderService();
    static RSMainThread* GetRenderServiceThread();
    static const char* GetProcessNameByPID(int pid);

    static uint64_t TimePauseApply(uint64_t time);

    static bool IsRecording();
    static bool IsPlaying();
};

} // namespace OHOS::Rosen

#endif // RENDER_SERVICE_PROFILER_H