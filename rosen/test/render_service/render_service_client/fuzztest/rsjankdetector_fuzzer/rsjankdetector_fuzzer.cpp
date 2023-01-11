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

#include "rsjankdetector_fuzzer.h"

#include <securec.h>

#include "jank_detector/rs_jank_detector.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t STR_LEN = 10;
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

/*
 * describe: get data from outside untrusted data(data_) which size is according to sizeof(T)
 * tips: only support basic type
 */
template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (g_data == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    size_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}

/*
 * get a string from data_
 */
std::string GetStringFromData(int strlen)
{
    char cstr[strlen];
    cstr[strlen - 1] = '\0';
    for (int i = 0; i < strlen - 1; i++) {
        cstr[i] = GetData<char>();
    }
    std::string str(cstr);
    return str;
}

bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    g_data = data;
    g_size = size;
    g_pos = 0;

    uint64_t refreshPeriod = GetData<uint64_t>();
    uint64_t startTimeStamp = GetData<uint64_t>();
    uint64_t endTimeStamp = GetData<uint64_t>();
    std::string abilityName = GetStringFromData(STR_LEN);
    uint64_t renderStartTimeStamp = GetData<uint64_t>();
    uint64_t renderEndTimeStamp = GetData<uint64_t>();

    std::shared_ptr<RSJankDetector> jankDetector = std::make_shared<RSJankDetector>();
    jankDetector->GetSysTimeNs();
    jankDetector->SetRefreshPeriod(refreshPeriod);
    jankDetector->UpdateUiDrawFrameMsg(startTimeStamp, endTimeStamp, abilityName);
    jankDetector->CalculateSkippedFrame(renderStartTimeStamp, renderEndTimeStamp);
  
    return true;
}
}// namespace Rosen
}// namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}

