/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#include "drawing_picture_recorder.h"

#include "drawing_canvas_utils.h"
#include "drawing_helper.h"
#include "image/picture_recorder.h"
#include "image/picture.h"
#include "utils/log.h"
#include "recording/recording_canvas.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

OH_Drawing_PictureRecorder* OH_Drawing_PictureRecorderCreate()
{
    return (OH_Drawing_PictureRecorder*)new PictureRecorder();
}

OH_Drawing_Canvas* OH_Drawing_BeginRecording(OH_Drawing_PictureRecorder* recorder, int32_t width, int32_t height)
{
    if (recorder == nullptr) {
        return nullptr;
    }
    PictureRecorder* pictureRecorder = reinterpret_cast<PictureRecorder*>(recorder);
    std::shared_ptr<RecordingCanvas> canvas = pictureRecorder->BeginRecording(width, height);
    return (OH_Drawing_Canvas*) (canvas.get());
}

OH_Drawing_Picture* OH_Drawing_FinishingRecording(OH_Drawing_PictureRecorder* recorder)
{
    if (recorder == nullptr) {
        return nullptr;
    }
    PictureRecorder* pictureRecorder = reinterpret_cast<PictureRecorder*>(recorder);
    NativeHandle<Picture>* blurDrawLooperHandle = new NativeHandle<Picture>;
    if (blurDrawLooperHandle == nullptr) {
        return nullptr;
    }
  
    blurDrawLooperHandle->value = pictureRecorder->FinishingRecording();
    if (blurDrawLooperHandle->value == nullptr) {
        delete blurDrawLooperHandle;
        return nullptr;
    }
    return Helper::CastTo<NativeHandle<Picture>*, OH_Drawing_Picture*>(blurDrawLooperHandle);
}

void OH_Drawing_PictureRecorderDestroy(OH_Drawing_PictureRecorder* recorder)
{
    delete reinterpret_cast<PictureRecorder*>(recorder);
}