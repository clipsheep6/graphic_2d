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

#include "drawing_canvas_utils.h"
#include "drawing_helper.h"
#include "pipeline/rs_record_cmd_utils.h"
#include "recording/record_cmd.h"
#include "utils/log.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static RSRecordCmdUtils* CastToCmdUtils(OH_Drawing_RecordCmdUtils* cRecordCmdUtils)
{
    return reinterpret_cast<RSRecordCmdUtils*>(cRecordCmdUtils);
}

OH_Drawing_RecordCmdUtils* OH_Drawing_RecordCmdUtilsCreate()
{
    return (OH_Drawing_RecordCmdUtils*)new(std::nothrow) RSRecordCmdUtils();
}

void OH_Drawing_RecordCmdUtilsDestroy(OH_Drawing_RecordCmdUtils* recordCmdUtils)
{
    delete reinterpret_cast<RSRecordCmdUtils*>(recordCmdUtils);
}

OH_Drawing_Canvas* OH_Drawing_RecordCmdUtilsBeginRecording(OH_Drawing_RecordCmdUtils* cRecordCmdUtils,
    OH_Drawing_Rect* cBounds)
{
    if (cRecordCmdUtils == nullptr || cBounds == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return nullptr;
    }
    RSRecordCmdUtils* recordCmdUtils = CastToCmdUtils(cRecordCmdUtils);
    Drawing::Rect* bounds = reinterpret_cast<Drawing::Rect*>(cBounds);
    Drawing::Canvas* canvasPtr = recordCmdUtils->BeginRecording(*bounds);
    return (OH_Drawing_Canvas*) (canvasPtr);
}

OH_Drawing_RecordCmd* OH_Drawing_RecordCmdUtilsFinishingRecording(OH_Drawing_RecordCmdUtils* cRecordCmdUtils)
{
    if (cRecordCmdUtils == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return nullptr;
    }
    RSRecordCmdUtils* recordCmdUtils = CastToCmdUtils(cRecordCmdUtils);
    NativeHandle<RecordCmd>* recordCmdHandle = new(std::nothrow) NativeHandle<RecordCmd>;
    if (recordCmdHandle == nullptr) {
        return nullptr;
    }
    recordCmdHandle->value = recordCmdUtils->FinishRecording();
    if (recordCmdHandle->value == nullptr) {
        delete recordCmdHandle;
        return nullptr;
    }
    return Helper::CastTo<NativeHandle<RecordCmd>*, OH_Drawing_RecordCmd*>(recordCmdHandle);
}

void OH_Drawing_RecordCmdDestroy(OH_Drawing_RecordCmd* recordCmd)
{
    delete reinterpret_cast<RecordCmd*>(recordCmd);
}