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
#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_CAPTURE_CALLBACK_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_CAPTURE_CALLBACK_H

#include "common/rs_common_def.h"

namespace OHOS {
namespace Media {
class PixelMap;
}
namespace Rosen {
class RS_EXPORT CaptureCallback {
public:
    CaptureCallback() = default;
    virtual ~CaptureCallback() {}
    virtual void OnLocalCapture(std::shared_ptr<Media::PixelMap> pixelmap) = 0;
};
} // Rosen
} // OHOS
#endif  // RENDER_SERVICE_CLIENT_CORE_UI_RS_CAPTURE_CALLBACK_H
