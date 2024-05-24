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

#include "rspropertymodifier_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "animation/rs_animation.h"
#include "transaction/rs_interfaces.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_ui_director.h"
#include "modifier/rs_property.h"
#include "modifier/rs_property_modifier.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

/*
 * describe: get data from outside untrusted data(g_data) which size is according to sizeof(T)
 * tips: only support basic type
 */
template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (g_data == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}

bool DoRSBoundsModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSBoundsSizeModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSBoundsSizeModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSBoundsPositionModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSBoundsPositionModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSFrameModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSFrameModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSPositionZModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSPositionZModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSSandBoxModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSSandBoxModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSPivotModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSPivotModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSPivotZModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSPivotZModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSQuaternionModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSQuaternionModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSRotationModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSRotationModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSRotationXModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSRotationXModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSRotationYModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSRotationYModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSCameraDistanceModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSCameraDistanceModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSScaleModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSScaleModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSSkewModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSSkewModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSPerspModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSPerspModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSTranslateModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSTranslateModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSTranslateZModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSTranslateZModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSCornerRadiusModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSCornerRadiusModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSAlphaModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSAlphaModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSAlphaOffscreenModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSAlphaOffscreenModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSEnvForegroundColorModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSEnvForegroundColorModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSEnvForegroundColorStrategyModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSEnvForegroundColorStrategyModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSForegroundColorModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSForegroundColorModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSUseShadowBatchingModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSUseShadowBatchingModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSBackgroundColorModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSBackgroundColorModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSBackgroundShaderModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSBackgroundShaderModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSBgImageModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSBgImageModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSBgImageInnerRectModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSBgImageInnerRectModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSBgImageWidthModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSBgImageWidthModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSBgImageHeightModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSBgImageHeightModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSBgImagePositionXModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSBgImagePositionXModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSBgImagePositionYModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSBgImagePositionYModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSBorderColorModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSBorderColorModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSBorderWidthModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSBorderWidthModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSBorderStyleModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSBorderStyleModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSOutlineColorModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSOutlineColorModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSOutlineWidthModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSOutlineWidthModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSOutlineStyleModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSOutlineStyleModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSOutlineRadiusModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSOutlineRadiusModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSForegroundEffectRadiusModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSForegroundEffectRadiusModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSFilterModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSFilterModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSBackgroundBlurRadiusModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSBackgroundBlurRadiusModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSBackgroundBlurSaturationModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSBackgroundBlurSaturationModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSBackgroundBlurBrightnessModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSBackgroundBlurBrightnessModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSBackgroundBlurMaskColorModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSBackgroundBlurMaskColorModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSBackgroundBlurColorModeModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSBackgroundBlurColorModeModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSBackgroundBlurRadiusXModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSBackgroundBlurRadiusXModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSBackgroundBlurRadiusYModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSBackgroundBlurRadiusYModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSForegroundBlurRadiusModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSForegroundBlurRadiusModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSForegroundBlurSaturationModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSForegroundBlurSaturationModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSForegroundBlurBrightnessModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSForegroundBlurBrightnessModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSForegroundBlurMaskColorModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSForegroundBlurMaskColorModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSForegroundBlurColorModeModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSForegroundBlurColorModeModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSForegroundBlurRadiusXModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSForegroundBlurRadiusXModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSForegroundBlurRadiusYModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSForegroundBlurRadiusYModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSLightUpEffectModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSLightUpEffectModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSDynamicLightUpRateModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSDynamicLightUpRateModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSDynamicLightUpDegreeModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSDynamicLightUpDegreeModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSFgBrightnessParamsModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSFgBrightnessParamsModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSFgBrightnessFractModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSFgBrightnessFractModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSBgBrightnessParamsModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSBgBrightnessParamsModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSBgBrightnessFractModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSBgBrightnessFractModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSEmitterUpdaterModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSEmitterUpdaterModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSParticleNoiseFieldsModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSParticleNoiseFieldsModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSDynamicDimDegreeModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSDynamicDimDegreeModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSLinearGradientBlurParaModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSLinearGradientBlurParaModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSMotionBlurParaModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSMotionBlurParaModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSBackgroundFilterModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSBackgroundFilterModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSFrameGravityModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSFrameGravityModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSClipRRectModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSClipRRectModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSClipBoundsModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSClipBoundsModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSClipToBoundsModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSClipToBoundsModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSClipToFrameModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSClipToFrameModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSVisibleModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSVisibleModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSSpherizeModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSSpherizeModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSShadowColorModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSShadowColorModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSShadowOffsetXModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSShadowOffsetXModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSShadowOffsetYModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSShadowOffsetYModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSShadowAlphaModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSShadowAlphaModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSShadowElevationModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSShadowElevationModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSShadowRadiusModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSShadowRadiusModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSShadowPathModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSShadowPathModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSShadowMaskModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSShadowMaskModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSShadowIsFilledModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSShadowIsFilledModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSGreyCoefModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSGreyCoefModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSMaskModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSMaskModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSPixelStretchModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSPixelStretchModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSPixelStretchPercentModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSPixelStretchPercentModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSPixelStretchTileModeModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSPixelStretchTileModeModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSGrayScaleModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSGrayScaleModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSLightIntensityModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSLightIntensityModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSLightColorModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSLightColorModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSIlluminatedTypeModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSIlluminatedTypeModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSLightPositionModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSLightPositionModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSIlluminatedBorderWidthModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSIlluminatedBorderWidthModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSBloomModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSBloomModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSBrightnessModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSBrightnessModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSContrastModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSContrastModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSSaturateModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSSaturateModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSSepiaModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSSepiaModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSInvertModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSInvertModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSAiInvertModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSAiInvertModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSSystemBarEffectModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSSystemBarEffectModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSHueRotateModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSHueRotateModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSColorBlendModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSColorBlendModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSUseEffectModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSUseEffectModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSColorBlendModeModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSColorBlendModeModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSColorBlendApplyTypeModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSColorBlendApplyTypeModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}

bool DoRSShadowColorStrategyModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    float value = GetData<float>();
    auto property = std::make_shared<RSAnimatableProperty<float>>(value);
    auto modifier = std::make_shared<RSShadowColorStrategyModifier>(property);
    modifier->GetModifierType();
    modifier->CreateRenderModifier();
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoRSBoundsModifier(data, size);
    OHOS::Rosen::DoRSBoundsPositionModifier(data, size);
    OHOS::Rosen::DoRSFrameModifier(data, size);
    OHOS::Rosen::DoRSPositionZModifier(data, size);
    OHOS::Rosen::DoRSSandBoxModifier(data, size);
    OHOS::Rosen::DoRSPivotModifier(data, size);
    OHOS::Rosen::DoRSPivotZModifier(data, size);
    OHOS::Rosen::DoRSQuaternionModifier(data, size);
    OHOS::Rosen::DoRSRotationModifier(data, size);
    OHOS::Rosen::DoRSRotationXModifier(data, size);
    OHOS::Rosen::DoRSRotationYModifier(data, size);
    OHOS::Rosen::DoRSCameraDistanceModifier(data, size);
    OHOS::Rosen::DoRSScaleModifier(data, size);
    OHOS::Rosen::DoRSSkewModifier(data, size);
    OHOS::Rosen::DoRSPerspModifier(data, size);
    OHOS::Rosen::DoRSTranslateModifier(data, size);
    OHOS::Rosen::DoRSTranslateZModifier(data, size);
    OHOS::Rosen::DoRSCornerRadiusModifier(data, size);
    OHOS::Rosen::DoRSAlphaModifier(data, size);
    OHOS::Rosen::DoRSAlphaOffscreenModifier(data, size);
    OHOS::Rosen::DoRSEnvForegroundColorModifier(data, size);
    OHOS::Rosen::DoRSEnvForegroundColorStrategyModifier(data, size);
    OHOS::Rosen::DoRSForegroundColorModifier(data, size);
    OHOS::Rosen::DoRSUseShadowBatchingModifier(data, size);
    OHOS::Rosen::DoRSBackgroundColorModifier(data, size);
    OHOS::Rosen::DoRSBackgroundShaderModifier(data, size);
    OHOS::Rosen::DoRSBgImageModifier(data, size);
    OHOS::Rosen::DoRSBgImageInnerRectModifier(data, size);
    OHOS::Rosen::DoRSBgImageWidthModifier(data, size);
    OHOS::Rosen::DoRSBgImageHeightModifier(data, size);
    OHOS::Rosen::DoRSBgImagePositionXModifier(data, size);
    OHOS::Rosen::DoRSBgImagePositionYModifier(data, size);
    OHOS::Rosen::DoRSBorderColorModifier(data, size);
    OHOS::Rosen::DoRSBorderWidthModifier(data, size);
    OHOS::Rosen::DoRSBorderStyleModifier(data, size);
    OHOS::Rosen::DoRSOutlineColorModifier(data, size);
    OHOS::Rosen::DoRSOutlineWidthModifier(data, size);
    OHOS::Rosen::DoRSOutlineStyleModifier(data, size);
    OHOS::Rosen::DoRSOutlineRadiusModifier(data, size);
    OHOS::Rosen::DoRSForegroundEffectRadiusModifier(data, size);
    OHOS::Rosen::DoRSFilterModifier(data, size);
    OHOS::Rosen::DoRSBackgroundBlurRadiusModifier(data, size);
    OHOS::Rosen::DoRSBackgroundBlurSaturationModifier(data, size);
    OHOS::Rosen::DoRSBackgroundBlurBrightnessModifier(data, size);
    OHOS::Rosen::DoRSBackgroundBlurMaskColorModifier(data, size);
    OHOS::Rosen::DoRSBackgroundBlurColorModeModifier(data, size);
    OHOS::Rosen::DoRSBackgroundBlurRadiusXModifier(data, size);
    OHOS::Rosen::DoRSBackgroundBlurRadiusYModifier(data, size);
    OHOS::Rosen::DoRSForegroundBlurRadiusModifier(data, size);
    OHOS::Rosen::DoRSForegroundBlurSaturationModifier(data, size);
    OHOS::Rosen::DoRSForegroundBlurBrightnessModifier(data, size);
    OHOS::Rosen::DoRSForegroundBlurMaskColorModifier(data, size);
    OHOS::Rosen::DoRSForegroundBlurColorModeModifier(data, size);
    OHOS::Rosen::DoRSForegroundBlurRadiusXModifier(data, size);
    OHOS::Rosen::DoRSForegroundBlurRadiusYModifier(data, size);
    OHOS::Rosen::DoRSLightUpEffectModifier(data, size);
    OHOS::Rosen::DoRSDynamicLightUpRateModifier(data, size);
    OHOS::Rosen::DoRSDynamicLightUpDegreeModifier(data, size);
    OHOS::Rosen::DoRSFgBrightnessParamsModifier(data, size);
    OHOS::Rosen::DoRSFgBrightnessFractModifier(data, size);
    OHOS::Rosen::DoRSBgBrightnessParamsModifier(data, size);
    OHOS::Rosen::DoRSBgBrightnessFractModifier(data, size);
    OHOS::Rosen::DoRSEmitterUpdaterModifier(data, size);
    OHOS::Rosen::DoRSParticleNoiseFieldsModifier(data, size);
    OHOS::Rosen::DoRSDynamicDimDegreeModifier(data, size);
    OHOS::Rosen::DoRSLinearGradientBlurParaModifier(data, size);
    OHOS::Rosen::DoRSMotionBlurParaModifier(data, size);
    OHOS::Rosen::DoRSBackgroundFilterModifier(data, size);
    OHOS::Rosen::DoRSFrameGravityModifier(data, size);
    OHOS::Rosen::DoRSClipRRectModifier(data, size);
    OHOS::Rosen::DoRSClipBoundsModifier(data, size);
    OHOS::Rosen::DoRSClipToBoundsModifier(data, size);
    OHOS::Rosen::DoRSClipToFrameModifier(data, size);
    OHOS::Rosen::DoRSVisibleModifier(data, size);
    OHOS::Rosen::DoRSSpherizeModifier(data, size);
    OHOS::Rosen::DoRSShadowColorModifier(data, size);
    OHOS::Rosen::DoRSShadowOffsetXModifier(data, size);
    OHOS::Rosen::DoRSShadowOffsetYModifier(data, size);
    OHOS::Rosen::DoRSShadowAlphaModifier(data, size);
    OHOS::Rosen::DoRSShadowElevationModifier(data, size);
    OHOS::Rosen::DoRSShadowRadiusModifier(data, size);
    OHOS::Rosen::DoRSShadowPathModifier(data, size);
    OHOS::Rosen::DoRSShadowMaskModifier(data, size);
    OHOS::Rosen::DoRSShadowIsFilledModifier(data, size);
    OHOS::Rosen::DoRSGreyCoefModifier(data, size);
    OHOS::Rosen::DoRSMaskModifier(data, size);
    OHOS::Rosen::DoRSPixelStretchModifier(data, size);
    OHOS::Rosen::DoRSPixelStretchPercentModifier(data, size);
    OHOS::Rosen::DoRSPixelStretchTileModeModifier(data, size);
    OHOS::Rosen::DoRSGrayScaleModifier(data, size);
    OHOS::Rosen::DoRSLightIntensityModifier(data, size);
    OHOS::Rosen::DoRSLightColorModifier(data, size);
    OHOS::Rosen::DoRSIlluminatedTypeModifier(data, size);
    OHOS::Rosen::DoRSLightPositionModifier(data, size);
    OHOS::Rosen::DoRSIlluminatedBorderWidthModifier(data, size);
    OHOS::Rosen::DoRSBloomModifier(data, size);
    OHOS::Rosen::DoRSBrightnessModifier(data, size);
    OHOS::Rosen::DoRSContrastModifier(data, size);
    OHOS::Rosen::DoRSSaturateModifier(data, size);
    OHOS::Rosen::DoRSSepiaModifier(data, size);
    OHOS::Rosen::DoRSInvertModifier(data, size);
    OHOS::Rosen::DoRSAiInvertModifier(data, size);
    OHOS::Rosen::DoRSSystemBarEffectModifier(data, size);
    OHOS::Rosen::DoRSHueRotateModifier(data, size);
    OHOS::Rosen::DoRSColorBlendModifier(data, size);
    OHOS::Rosen::DoRSUseEffectModifier(data, size);
    OHOS::Rosen::DoRSColorBlendModeModifier(data, size);
    OHOS::Rosen::DoRSColorBlendApplyTypeModifier(data, size);
    OHOS::Rosen::DoRSShadowColorStrategyModifier(data, size);
    return 0;
}

