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

#ifndef ROSEN_SYNC_REPORT_H
#define ROSEN_SYNC_REPORT_H

#include <cstdint>
#include <string>

namespace OHOS {
namespace Rosen {
using InitFunc = void (*)();
using SetFrameParamFunc = void(*)(int, int, int, int);
class SyncReport {
public:
    static SyncReport& GetInstance();
    int GetEnable();
    void MonitorGpuStart(uint32_t bufferCount, int flushEndTIme);
    void MonitorGpuEnd();
    void SetFrameParam(int requestId, int load, int schedFrameNum, int value);

private:
    SyncReport();
    ~SyncReport();
    void Init();
    bool LoadLibrary();
    void CloseLibrary();
    void *LoadSymbol(const char *symName);
    void *frameSchedHandle_ = nullptr;
    bool frameSchedSoLoaded_ = false;

    void* monitorGpuStartFunc_ = nullptr;
    void* monitorGpuEndFunc_ = nullptr;
    void* setFrameParamFunc_ = nullptr;
    InitFunc initFunc_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_SYNC_REPORT_H
