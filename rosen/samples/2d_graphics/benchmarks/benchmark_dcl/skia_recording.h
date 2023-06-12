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

namespace OHOS {
namespace Rosen {
class SkiaRecording {
public:
    enum class SkiaCaptureMode {
        SingleFrame,
        MultiFrame,
        None,
    };
    void InitConfigsFromParam();
    void SetupMultiFrame();
    void BeginCapture();
    void EndCapture();

private:
    bool captureEnabled_ = false;
    int captureFrameNum_ = 0;
    std::string captureFileName_ = "";
    SkiaCaptureMode captureMode_ = SkiaCaptureMode::None;
};
}
}

#endif