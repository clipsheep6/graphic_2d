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

#include "vsyncgenerator_fuzzer.h"

#include <securec.h>

#include "vsync_distributor.h"
#include "vsync_generator.h"
#include "vsync_controller.h"

namespace OHOS {
    namespace {
        const uint8_t* data_ = nullptr;
        size_t size_ = 0;
        size_t pos;
    }

    /*
    * describe: get data from outside untrusted data(data_) which size is according to sizeof(T)
    * tips: only support basic type
    */
    template<class T>
    T GetData()
    {
        T object {};
        size_t objectSize = sizeof(object);
        if (data_ == nullptr || objectSize > size_ - pos) {
            return object;
        }
        errno_t ret = memcpy_s(&object, objectSize, data_ + pos, objectSize);
        if (ret != EOK) {
            return {};
        }
        pos += objectSize;
        return object;
    }

    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        data_ = data;
        size_ = size;
        pos = 0;

        // get data
        int64_t period = GetData<int64_t>();
        int64_t phase = GetData<int64_t>();
        int64_t referenceTime = GetData<int64_t>();
        uint32_t generatorRefreshRate = GetData<uint32_t>();
        int64_t expectVsyncTime = GetData<int64_t>();

        // test
        sptr<Rosen::VSyncGenerator> vsyncGenerator = Rosen::CreateVSyncGenerator();
        vsyncGenerator->UpdateMode(period, phase, referenceTime);
        Rosen::VSyncGenerator::ListenerRefreshRateData listenerRefreshRates = {};
        Rosen::VSyncGenerator::ListenerPhaseOffsetData listenerPhaseOffset = {};
        vsyncGenerator->ChangeGeneratorRefreshRateModel(
            listenerRefreshRates, listenerPhaseOffset, generatorRefreshRate, expectVsyncTime);

        return true;
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}

