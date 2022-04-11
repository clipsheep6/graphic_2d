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

#include "rs_window_animation_utils.h"

#include <js_runtime_utils.h>
#include <platform/common/rs_log.h>

namespace OHOS {
namespace Rosen {

using namespace AbilityRuntime;

const std::string RSWindowAnimationUtils::TAG = "RSWindowAnimationUtils";

NativeValue* RSWindowAnimationUtils::CreateJsWindowAnimationTarget(NativeEngine& engine,
    const RSWindowAnimationTarget& target)
{
    ROSEN_LOGI_WITH_TAG(RSWindowAnimationUtils::TAG, "CreateJsWindowAnimationTarget");
    auto objValue = engine.CreateObject();
    if (objValue == nullptr) {
        ROSEN_LOGE_WITH_TAG(RSWindowAnimationUtils::TAG, "Failed to create object!");
        return nullptr;
    }

    auto object = ConvertNativeValueTo<NativeObject>(objValue);

    if (object == nullptr) {
        ROSEN_LOGE_WITH_TAG(RSWindowAnimationUtils::TAG, "Failed to convert object!");
        return nullptr;
    }

    object->SetProperty("bundleName", CreateJsValue(engine, target.bundleName_));
    object->SetProperty("abilityName", CreateJsValue(engine, target.abilityName_));
    object->SetProperty("windowBounds", CreateJsRRect(engine, target.windowBounds_));
    object->SetProperty("windowId", CreateJsValue(engine, target.windowId_));
    object->SetProperty("surfaceNodeId",
        engine.CreateBigInt(target.surfaceNode_ != nullptr ? target.surfaceNode_->GetId() : -1));

    return objValue;
}

NativeValue* RSWindowAnimationUtils::CreateJsRRect(NativeEngine& engine, const RRect& rrect)
{
    ROSEN_LOGI_WITH_TAG(RSWindowAnimationUtils::TAG, "CreateJsRRect");
    auto objValue = engine.CreateObject();
    if (objValue == nullptr) {
        ROSEN_LOGE_WITH_TAG(RSWindowAnimationUtils::TAG, "Failed to create object!");
        return nullptr;
    }

    auto object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        ROSEN_LOGE_WITH_TAG(RSWindowAnimationUtils::TAG, "Failed to convert object!");
        return nullptr;
    }

    object->SetProperty("left", CreateJsValue(engine, rrect.rect_.left_));
    object->SetProperty("top", CreateJsValue(engine, rrect.rect_.top_));
    object->SetProperty("width", CreateJsValue(engine, rrect.rect_.width_));
    object->SetProperty("height", CreateJsValue(engine, rrect.rect_.height_));
    object->SetProperty("radiusX", CreateJsValue(engine, rrect.radius_[0].x_));
    object->SetProperty("radiusY", CreateJsValue(engine, rrect.radius_[0].y_));

    return objValue;
}

} // namespace Rosen
} // namespace OHOS
