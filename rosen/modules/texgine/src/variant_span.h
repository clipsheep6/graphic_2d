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

#ifndef ROSEN_MODULES_TEXGINE_SRC_VARIANT_SPAN_H
#define ROSEN_MODULES_TEXGINE_SRC_VARIANT_SPAN_H

#include <memory>
#include <optional>
#include <vector>

#include "texgine_canvas.h"
#include "texgine/font_providers.h"
#include "texgine/any_span.h"
#include "text_span.h"
#include "platform/common/rs_log.h"
#include "symbol_animation_config.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
enum class DumpType {
    NORMAL,
    DONT_RETURN,
};

class VariantSpan {
public:
    VariantSpan() noexcept(true) = default;
    VariantSpan(const std::shared_ptr<TextSpan> &ts) noexcept(true);
    VariantSpan(const std::shared_ptr<AnySpan> &as) noexcept(true);
    VariantSpan(std::shared_ptr<TextSpan> &&ts) noexcept(true);
    VariantSpan(std::shared_ptr<AnySpan> &&as) noexcept(true);
    VariantSpan(std::nullptr_t) noexcept(true);

    template<typename Type,
             typename = std::enable_if_t<std::is_convertible_v<Type, std::shared_ptr<AnySpan>>>>
    VariantSpan(Type type) noexcept(true)
        : VariantSpan(std::static_pointer_cast<AnySpan>(type)) {}

    std::shared_ptr<TextSpan> TryToTextSpan() const noexcept(false);
    std::shared_ptr<AnySpan> TryToAnySpan() const noexcept(false);

    double GetWidth() const noexcept(false);
    double GetHeight() const noexcept(false);
    size_t GetNumberOfCharGroup() const noexcept(false);
    std::vector<double> GetGlyphWidths() const noexcept(false);
    double GetVisibleWidth() const noexcept(false);
    size_t GetNumberOfChar() const noexcept(false);
    void Dump(const DumpType &dtype = DumpType::NORMAL) const noexcept(false);

    void SetTextStyle(const TextStyle &xs) noexcept(true);
    TextStyle &GetTextStyle() noexcept(true);
    const TextStyle &GetTextStyle() const noexcept(true);

    double GetOffsetX() const noexcept(true);
    double GetOffsetY() const noexcept(true);
    void AdjustOffsetX(double offset) noexcept(true);
    void AdjustOffsetY(double offset) noexcept(true);

    void Paint(TexgineCanvas &canvas, double offsetX, double offsetY) noexcept(false);
    void PaintShadow(TexgineCanvas &canvas, double offsetX, double offsetY) noexcept(false);

    bool IsRTL() const noexcept(false);
    bool IsHardBreak() const noexcept(false);

    double GetJustifyGap() const noexcept(true);
    void SetJustifyGap(double justifyGap) noexcept(true);

    bool HasBackgroundRect() const noexcept(true);
    RoundRectType GetRoundRectType() const noexcept(true);
    void SetRoundRectType(RoundRectType type) noexcept(true);

    void SetLineHeight(double lineHeight) noexcept(true);
    double GetLineHeight() const noexcept(true);

    void SetLineY(double lineY) noexcept(true);
    double GetLineY() const noexcept(true);

    operator bool() const noexcept(false);
    bool operator ==(std::nullptr_t) const noexcept(false);
    bool operator ==(const VariantSpan &rhs) const noexcept(false);
    bool operator !=(std::nullptr_t) const noexcept(false);
    bool operator !=(const VariantSpan &rhs) const noexcept(false);
    void SetAnimation(std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)> animationFunc) {
        if(animationFunc == nullptr){
            RS_LOGE("HmSymbol variant_span.h null ");

        }else{
            ts_->SetAnimation(animationFunc);
            RS_LOGD("HmSymbol variant_span.h success ");

        }
    }
private:
    void CheckPointer(bool nullable = false) const noexcept(false);

    std::shared_ptr<TextSpan> ts_ = nullptr;
    std::shared_ptr<AnySpan> as_ = nullptr;

    double offsetX_ = 0;
    double offsetY_ = 0;
    double justifyGap_ = 0;
    TextStyle xs_;
    RoundRectType roundRectType_ = RoundRectType::NONE;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_VARIANT_SPAN_H
