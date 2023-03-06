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

#include "modifier/rs_render_modifier.h"
#include "modifier/rs_modifier_type.h"
#include <memory>
#include <unordered_map>
#include "pixel_map.h"

#include "modifier/rs_modifier_type.h"
#include "pipeline/rs_draw_cmd_list.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "property/rs_properties.h"
#include "property/rs_properties_def.h"
#include "property/rs_properties_painter.h"

namespace OHOS {
namespace Rosen {
namespace {
using ModifierUnmarshallingFunc = RSRenderModifier* (*)(Parcel& parcel);

#define DECLARE_ANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_TYPE, DELTA_OP, MODIFIER_TIER)        \
    { RSModifierType::MODIFIER_TYPE, [](Parcel& parcel) -> RSRenderModifier* {                          \
            std::shared_ptr<RSRenderAnimatableProperty<TYPE>> prop;                                     \
            if (!RSMarshallingHelper::Unmarshalling(parcel, prop)) {                                    \
                return nullptr;                                                                         \
            }                                                                                           \
            auto modifier = new RS##MODIFIER_NAME##RenderModifier(prop);                                \
            if (!modifier) {                                                                            \
                return nullptr;                                                                         \
            }                                                                                           \
            return modifier;                                                                            \
        },                                                                                              \
    },

#define DECLARE_NOANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_TYPE, MODIFIER_TIER)                \
    { RSModifierType::MODIFIER_TYPE, [](Parcel& parcel) -> RSRenderModifier* {                          \
            std::shared_ptr<RSRenderProperty<TYPE>> prop;                                               \
            if (!RSMarshallingHelper::Unmarshalling(parcel, prop)) {                                    \
                return nullptr;                                                                         \
            }                                                                                           \
            auto modifier = new RS##MODIFIER_NAME##RenderModifier(prop);                                \
            if (!modifier) {                                                                            \
                return nullptr;                                                                         \
            }                                                                                           \
            return modifier;                                                                            \
        },                                                                                              \
    },

static std::unordered_map<RSModifierType, ModifierUnmarshallingFunc> funcLUT = {
#include "modifier/rs_modifiers_def.in"
    { RSModifierType::EXTENDED, [](Parcel& parcel) -> RSRenderModifier* {
            std::shared_ptr<RSRenderProperty<std::shared_ptr<DrawCmdList>>> prop;
            int16_t type;
            bool hasOverlayBounds = false;
            if (!RSMarshallingHelper::Unmarshalling(parcel, prop) || !parcel.ReadInt16(type) ||
                !parcel.ReadBool(hasOverlayBounds)) {
                return nullptr;
            }
            RSDrawCmdListRenderModifier* modifier = new RSDrawCmdListRenderModifier(prop);
            modifier->SetType(static_cast<RSModifierType>(type));
            if (hasOverlayBounds) {
                // OVERLAY_STYLE
                int32_t left;
                int32_t top;
                int32_t width;
                int32_t height;
                if (!(parcel.ReadInt32(left) && parcel.ReadInt32(top) &&
                    parcel.ReadInt32(width) && parcel.ReadInt32(height))) {
                    return nullptr;
                }
                modifier->SetOverlayBounds(std::make_shared<RectI>(left, top, width, height));
            }
            return modifier;
        },
    },
    { RSModifierType::ENV_FOREGROUND_COLOR, [](Parcel& parcel) -> RSRenderModifier* {
            std::shared_ptr<RSRenderAnimatableProperty<Color>> prop;
            if (!RSMarshallingHelper::Unmarshalling(parcel, prop)) {
                return nullptr;
            }
            auto modifier = new RSEnvForegroundColorRenderModifier(prop);
            if (!modifier) {
                return nullptr;
            }
            return modifier;
        },
    },
    { RSModifierType::ENV_FOREGROUND_COLOR_STRATEGY, [](Parcel& parcel) -> RSRenderModifier* {
            std::shared_ptr<RSRenderProperty<ForegroundColorStrategyType>> prop;
            if (!RSMarshallingHelper::Unmarshalling(parcel, prop)) {
                return nullptr;
            }
            auto modifier = new RSEnvForegroundColorStrategyRenderModifier(prop);
            if (!modifier) {
                return nullptr;
            }
            return modifier;
        },
    },

};

#undef DECLARE_ANIMATABLE_MODIFIER
#undef DECLARE_NOANIMATABLE_MODIFIER
}

void RSDrawCmdListRenderModifier::Apply(RSModifierContext& context) const
{
    if (context.canvas_) {
        auto cmds = property_->Get();
        RSPropertiesPainter::DrawFrame(context.property_, *context.canvas_, cmds);
    }
}

void RSDrawCmdListRenderModifier::Update(const std::shared_ptr<RSRenderPropertyBase>& prop, bool isDelta)
{
    if (auto property = std::static_pointer_cast<RSRenderProperty<DrawCmdListPtr>>(prop)) {
        property_->Set(property->Get());
    }
}

bool RSDrawCmdListRenderModifier::Marshalling(Parcel& parcel)
{
    if (parcel.WriteInt16(static_cast<int16_t>(RSModifierType::EXTENDED)) &&
        RSMarshallingHelper::Marshalling(parcel, property_) && parcel.WriteInt16(static_cast<int16_t>(GetType())) &&
        parcel.WriteBool(overlayRect_ != nullptr)) {
        if (overlayRect_ != nullptr) {
            return parcel.WriteInt32(overlayRect_->GetLeft()) && parcel.WriteInt32(overlayRect_->GetTop()) &&
                parcel.WriteInt32(overlayRect_->GetWidth()) && parcel.WriteInt32(overlayRect_->GetHeight());
        }
        return true;
    }
    return false;
}

bool RSEnvForegroundColorRenderModifier::Marshalling(Parcel& parcel)
{
    auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<Color>>(property_);
    return parcel.WriteInt16(static_cast<int16_t>(RSModifierType::ENV_FOREGROUND_COLOR)) &&
            RSMarshallingHelper::Marshalling(parcel, renderProperty);
}

void RSEnvForegroundColorRenderModifier::Apply(RSModifierContext& context) const
{
    auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<Color>>(property_);
    context.canvas_->SetEnvForegroundColor(renderProperty->Get());
}

void RSEnvForegroundColorRenderModifier::Update(const std::shared_ptr<RSRenderPropertyBase>& prop, bool isDelta)
{
    if (auto property = std::static_pointer_cast<RSRenderAnimatableProperty<Color>>(prop)) {
        auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<Color>>(property_);
        renderProperty->Set(isDelta ? (renderProperty->Get() + property->Get()) : property->Get());
    }
}

bool RSEnvForegroundColorStrategyRenderModifier::Marshalling(Parcel& parcel)
{
    auto renderProperty = std::static_pointer_cast<RSRenderProperty<ForegroundColorStrategyType>>(property_);
    return parcel.WriteInt16(static_cast<short>(RSModifierType::ENV_FOREGROUND_COLOR_STRATEGY)) &&
            RSMarshallingHelper::Marshalling(parcel, renderProperty);
}


void RSEnvForegroundColorStrategyRenderModifier ::Apply(RSModifierContext& context) const
{
    auto renderProperty = std::static_pointer_cast<RSRenderProperty<ForegroundColorStrategyType>>(property_);
    switch (renderProperty->Get()) {
        case ForegroundColorStrategyType::INVERT_BACKGROUNDCOLOR: {
            // calculate the color by screebshot
            Color color = GetInvertBackgroundColor(context);
            context.canvas_->SetEnvForegroundColor(color);
            break;
        }
        default: {
            break;
        }
    }
}

Color RSEnvForegroundColorStrategyRenderModifier::GetInvertBackgroundColor(RSModifierContext& context) const
{
#ifdef ROSEN_OHOS
    auto imageSnapshot = context.canvas_->GetSurface()->makeImageSnapshot(context.canvas_->getDeviceClipBounds());
    if (imageSnapshot == nullptr) {
        RS_LOGI("RSRenderModifier::GetInvertBackgroundColor imageSnapshot null");
        return Color(0);
    }
    int pixmapWidth = context.property_.GetBoundsWidth();
    int pixmapHeight = context.property_.GetBoundsHeight();
    Media::InitializationOptions opts;
    opts.size.width = pixmapWidth;
    opts.size.height = pixmapHeight;
    std::unique_ptr<Media::PixelMap> pixelmap = Media::PixelMap::Create(opts);
    auto data = (uint8_t *)malloc(pixelmap->GetRowBytes() * pixelmap->GetHeight());
    if (data == nullptr) {
        RS_LOGE("RSRenderModifier::GetInvertBackgroundColor: data is nullptr");
        return Color(0);
    }
    SkImageInfo info = SkImageInfo::Make(pixelmap->GetWidth(), pixelmap->GetHeight(),
        kRGBA_8888_SkColorType, kPremul_SkAlphaType);
    if (!imageSnapshot->readPixels(info, data, pixelmap->GetRowBytes(), 0, 0)) {
        RS_LOGE("RSRenderModifier::Run: readPixels failed");
        free(data);
        data = nullptr;
        return Color(0);
    }
    pixelmap->SetPixelsAddr(data, nullptr, pixelmap->GetRowBytes() * pixelmap->GetHeight(),
        Media::AllocatorType::HEAP_ALLOC, nullptr);
    OHOS::Media::InitializationOptions options;
    options.alphaType = pixelmap->GetAlphaType();
    options.pixelFormat = pixelmap->GetPixelFormat();
    options.scaleMode = OHOS::Media::ScaleMode::FIT_TARGET_SIZE;
    options.size.width = 1;
    options.size.height = 1;
    options.editable = true;
    std::unique_ptr<Media::PixelMap> newPixelMap = Media::PixelMap::Create(*pixelmap.get(), options);
    uint32_t colorVal = 0;
    int x = 0;
    int y = 0;
    newPixelMap->GetARGB32Color(x, y, colorVal);
    uint32_t a = (colorVal >> 24) & 0xff;
    uint32_t r = 255 - ((colorVal >> 16) & 0xff);
    uint32_t g = 255 - ((colorVal >> 8) & 0xff);
    uint32_t b = 255 - ((colorVal >> 0) & 0xff);
    Color averageColor(r, g, b, a);
    return averageColor;
#else
    return Color(0);
#endif
}

void RSEnvForegroundColorStrategyRenderModifier::Update(const std::shared_ptr<RSRenderPropertyBase>& prop, bool isDelta)
{
    if (auto property = std::static_pointer_cast<RSRenderProperty<ForegroundColorStrategyType >>(prop)) {
        auto renderProperty = std::static_pointer_cast<RSRenderProperty<ForegroundColorStrategyType >>(property_);
        renderProperty->Set(property->Get());
    }
}

RSRenderModifier* RSRenderModifier::Unmarshalling(Parcel& parcel)
{
    int16_t type = 0;
    if (!parcel.ReadInt16(type)) {
        return nullptr;
    }
    auto it = funcLUT.find(static_cast<RSModifierType>(type));
    if (it == funcLUT.end()) {
        ROSEN_LOGE("RSRenderModifier Unmarshalling cannot find func in lut %d", type);
        return nullptr;
    }
    return it->second(parcel);
}

namespace {
template<typename T>
T Add(T a, T b)
{
    return a + b;
}
template<typename T>
T Multiply(T a, T b)
{
    return a * b;
}
template<typename T>
T Replace(T a, T b)
{
    return b;
}
} // namespace

#define DECLARE_ANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_TYPE, DELTA_OP, MODIFIER_TIER)                       \
    bool RS##MODIFIER_NAME##RenderModifier::Marshalling(Parcel& parcel)                                                \
    {                                                                                                                  \
        auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<TYPE>>(property_);                   \
        return parcel.WriteInt16(static_cast<int16_t>(RSModifierType::MODIFIER_TYPE)) &&                               \
               RSMarshallingHelper::Marshalling(parcel, renderProperty);                                               \
    }                                                                                                                  \
    void RS##MODIFIER_NAME##RenderModifier::Apply(RSModifierContext& context) const                                    \
    {                                                                                                                  \
        auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<TYPE>>(property_);                   \
        context.property_.Set##MODIFIER_NAME(DELTA_OP(context.property_.Get##MODIFIER_NAME(), renderProperty->Get())); \
    }                                                                                                                  \
    void RS##MODIFIER_NAME##RenderModifier::Update(const std::shared_ptr<RSRenderPropertyBase>& prop, bool isDelta)    \
    {                                                                                                                  \
        if (auto property = std::static_pointer_cast<RSRenderAnimatableProperty<TYPE>>(prop)) {                        \
            auto renderProperty = std::static_pointer_cast<RSRenderAnimatableProperty<TYPE>>(property_);               \
            renderProperty->Set(isDelta ? (renderProperty->Get() + property->Get()) : property->Get());                \
        }                                                                                                              \
    }

#define DECLARE_NOANIMATABLE_MODIFIER(MODIFIER_NAME, TYPE, MODIFIER_TYPE, MODIFIER_TIER)                            \
    bool RS##MODIFIER_NAME##RenderModifier::Marshalling(Parcel& parcel)                                             \
    {                                                                                                               \
        auto renderProperty = std::static_pointer_cast<RSRenderProperty<TYPE>>(property_);                          \
        return parcel.WriteInt16(static_cast<short>(RSModifierType::MODIFIER_TYPE)) &&                              \
               RSMarshallingHelper::Marshalling(parcel, renderProperty);                                            \
    }                                                                                                               \
    void RS##MODIFIER_NAME##RenderModifier::Apply(RSModifierContext& context) const                                 \
    {                                                                                                               \
        auto renderProperty = std::static_pointer_cast<RSRenderProperty<TYPE>>(property_);                          \
        context.property_.Set##MODIFIER_NAME(renderProperty->Get());                                                \
    }                                                                                                               \
    void RS##MODIFIER_NAME##RenderModifier::Update(const std::shared_ptr<RSRenderPropertyBase>& prop, bool isDelta) \
    {                                                                                                               \
        if (auto property = std::static_pointer_cast<RSRenderProperty<TYPE>>(prop)) {                               \
            auto renderProperty = std::static_pointer_cast<RSRenderProperty<TYPE>>(property_);                      \
            renderProperty->Set(property->Get());                                                                   \
        }                                                                                                           \
    }

#include "modifier/rs_modifiers_def.in"

#undef DECLARE_ANIMATABLE_MODIFIER
#undef DECLARE_NOANIMATABLE_MODIFIER
} // namespace Rosen
} // namespace OHOS
