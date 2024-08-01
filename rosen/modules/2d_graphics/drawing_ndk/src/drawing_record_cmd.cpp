/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "drawing_record_cmd.h"

#include "drawing_helper.h"
#include "utils/log.h"
#include "recording/record_cmd.h"
#include "pipeline/rs_record_cmd_utils.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

OH_Drawing_RecordCmdUtils* OH_Drawing_RecordCmdUtilsCreate()
{
    return (OH_Drawing_RecordCmdUtils*)new RSRecordCmdUtils();
}

void OH_Drawing_RecordCmdUtilsDestroy(OH_Drawing_RecordCmdUtils* recordCmdUtils){
    delete reinterpret_cast<RSRecordCmdUtils*>(recordCmdUtils);
}

OH_Drawing_Canvas* OH_Drawing_RecordCmdUtilsBeginRecording(OH_Drawing_RecordCmdUtils* cRecordCmdUtils ,OH_Drawing_Rect* cBounds)
{
    if(cRecordCmdUtils == nullptr || cBounds == nullptr){
        return nullptr;
    }
    RSRecordCmdUtils* recordCmdUtils = reinterpret_cast<RSRecordCmdUtils*>(cRecordCmdUtils);
    Drawing::Rect& bounds = reinterpret_cast<Drawing::Rect&>(cBounds);
    Drawing::Canvas* canvasPtr = recordCmdUtils->BeginRecording(bounds);
    std::shared_ptr<Canvas> canvas(canvasPtr, [](Canvas* p) { delete p; });
    return (OH_Drawing_Canvas*) (canvas.get());
} 

OH_Drawing_RecordCmd* OH_Drawing_RecordCmdUtilsFinishingRecording(OH_Drawing_RecordCmdUtils* cRecordCmdUtils)
{
    if(cRecordCmdUtils == nullptr){
        return nullptr;
    }
    RSRecordCmdUtils* recordCmdUtils = reinterpret_cast<RSRecordCmdUtils*>(cRecordCmdUtils);
    NativeHandle<RecordCmd>* blurDrawLooperHandle = new NativeHandle<RecordCmd>;
    if (blurDrawLooperHandle == nullptr) {
        return nullptr;
    }
    blurDrawLooperHandle->value = recordCmdUtils->FinishRecording();
    if (blurDrawLooperHandle->value == nullptr) {
        delete blurDrawLooperHandle;
        return nullptr;
    }
    return Helper::CastTo<NativeHandle<RecordCmd>*, OH_Drawing_RecordCmd*>(blurDrawLooperHandle);
}

void OH_Drawing_RecordCmdDestroy(OH_Drawing_RecordCmd* recordCmd){
    delete reinterpret_cast<RecordCmd*>(recordCmd);
}