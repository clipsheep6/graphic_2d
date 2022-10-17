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

#include "platform/common/rs_log.h"
#include "transaction/rs_render_service_client.h"
namespace OHOS {
namespace Media {
class PixelMap;
}
namespace Rosen {
class RS_EXPORT CaptureCallback {
public:
    CaptureCallback() = default;
    ~CaptureCallback() {};

    bool IsReady() const
    {
        return flag_;
    }
    void TriggerCallback(std::shared_ptr<Media::PixelMap> pixelmap)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (!flag_) {
            pixelMap_ = pixelmap;
            flag_ = true;
        }
        conditionVariable_.notify_one();
    }
    std::shared_ptr<Media::PixelMap> GetResult(long timeOut)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (!conditionVariable_.wait_for(lock, std::chrono::milliseconds(timeOut), [this] { return IsReady(); })) {
            ROSEN_LOGE("wait for %lu timeout", timeOut);
        }
        return pixelMap_;
    }
private:
    std::shared_ptr<Media::PixelMap> pixelMap_ = nullptr;
    std::mutex mutex_;
    std::condition_variable conditionVariable_;
    bool flag_ = false;
};

class RSOffscreenRenderCallback : public SurfaceCaptureCallback, public CaptureCallback {
public:
    void OnSurfaceCapture(std::shared_ptr<Media::PixelMap> pixelmap) override
    {
        TriggerCallback(pixelmap);
    }
};
} // Rosen
} // OHOS
#endif  // RS_CAPTURE_CALLBACK_H