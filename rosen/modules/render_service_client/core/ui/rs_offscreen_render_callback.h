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


#ifndef RS_OFFSCREEN_RENDER_CALLBACK_H
#define RS_OFFSCREEN_RENDER_CALLBACK_H

#include "platform/common/rs_log.h"
#include "transaction/rs_render_service_client.h"
#include "ui/rs_capture_callback.h"

namespace OHOS {
namespace Rosen {
class RSOffscreenRenderCallback: public SurfaceCaptureCallback, public CaptureCallback{
public:
    void OnSurfaceCapture(std::shared_ptr<Media::PixelMap> pixelmap) override
    {
        OutCall(pixelmap);
    }
};
} // Rosen
} // OHOS
#endif  // RS_OFFSCREEN_RENDER_CALLBACK_H