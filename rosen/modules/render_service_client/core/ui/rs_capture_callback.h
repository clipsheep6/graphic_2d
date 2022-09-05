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


#ifndef RS_CAPTURE_CALLBACK_H
#define RS_CAPTURE_CALLBACK_H

#include "pixel_map.h"
#include "platform/common/rs_log.h"


namespace OHOS {
namespace Rosen {
class CaptureCallback {
public:
    CaptureCallback() = default;
    ~CaptureCallback() {};

    void Call(std::shared_ptr<Media::PixelMap> pixelmap)
    {
        if (!flag_) {
            pixelMap_ = pixelmap;
            flag_ = true;
        }    
    }
    bool IsReady()
    {
        return flag_;
    }
    std::shared_ptr<Media::PixelMap> FetchResult()
    {
        return pixelMap_;
    }
    void OutCall(std::shared_ptr<Media::PixelMap> pixelmap)
    {
        std::unique_lock <std::mutex> lock(mutex_);
        Call(pixelmap);
        conditionVariable_.notify_one();
    }
    std::shared_ptr<Media::PixelMap> GetResult(long timeOut)
    {
        std::unique_lock <std::mutex> lock(mutex_);
        if (!conditionVariable_.wait_for(lock, std::chrono::milliseconds(timeOut), [this] { return IsReady(); })) {
            ROSEN_LOGE("wait for %lu timeout", timeOut);
        }
        return FetchResult();
    }
private:
    std::shared_ptr<Media::PixelMap> pixelMap_ = nullptr;
    std::condition_variable conditionVariable_;
    std::mutex mutex_;
    bool flag_ = false;
};
} // Rosen
} // OHOS
#endif  // SURFACE_CAPTURE_FUTURE_H