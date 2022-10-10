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

#include "rsscreenmanager_fuzzer.h"

#include <securec.h>
#include <stddef.h>
#include <stdint.h>

#include <rs_screen_manager.h>
#include <surface.h>


namespace OHOS {
namespace Rosen {
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

    bool CreateVirtualScreenFuzzTest(const uint8_t* data, size_t size)
    {
        if (data == nullptr || size < 0) {
            return false;
        }

        // initialize
        data_ = data;
        size_ = size;
        pos = 0;

        // get data
        int32_t width = GetData<int32_t>();
        int32_t height = GetData<int32_t>();
        uint64_t id = GetData<uint64_t>();
        int32_t flags = GetData<int32_t>();

        // test
        sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
        if (screenManager->Init() == false) {
            return false;
        }
        sptr<Surface> cSurface = Surface::CreateSurfaceAsConsumer();
        sptr<IBufferProducer> producer = cSurface->GetProducer();
        sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(producer);
        ScreenId virtualScreenId = screenManager->CreateVirtualScreen("FUZZ", width, height, pSurface, static_cast<ScreenId>(id), flags);
        screenManager->RemoveVirtualScreen(virtualScreenId);

        return true;
    }
} // Rosen
} // OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::CreateVirtualScreenFuzzTest(data, size);
    return 0;
}

