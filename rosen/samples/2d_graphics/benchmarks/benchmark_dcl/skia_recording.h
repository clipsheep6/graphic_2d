/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef SKIA_RECORDING_H
#define SKIA_RECORDING_H

#include <string>

#include "include/core/SkCanvas.h"
#include "include/core/SkStream.h"
namespace OHOS {
namespace Rosen {
class SkiaRecording {
public:
    enum class SkiaCaptureMode {
        SingleFrame,
        MultiFrame,
        None,
    };
    bool GetCaptureEnabled();
    void InitConfigsFromParam();
    void SetupMultiFrame();
    SkCanvas*  BeginCapture(SkSurface* surface);
    void EndCapture();

private:
    bool captureEnabled_ = false;
    int captureFrameNum_ = 0;
    std::string captureFileName_ = "";
    SkiaCaptureMode captureMode_ = SkiaCaptureMode::None;

    // Should be decleared before other serializing member
    std::unique_ptr<SkSharingSerialContext> serialContext_;
    std::unique_ptr<SkFILEWStream> openMultiPicStream_;
    sk_sp<SkDocument> multiPic_;

    std::unique_ptr<SkPictureRecorder> recorder_;
    std::unique_ptr<SkNWayCanvas> nwayCanvas_;
};
}
}

#endif