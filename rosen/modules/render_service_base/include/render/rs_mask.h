/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_MASK_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_MASK_H

#include <memory>
#include "common/rs_macros.h"
#ifndef USE_ROSEN_DRAWING
#include "include/core/SkPaint.h"
#include "include/core/SkPath.h"
#include "include/core/SkPicture.h"
#else
#include "draw/brush.h"
#include "draw/path.h"
#include "image/picture.h"
#endif
#if defined(NEW_SKIA)
#include "modules/svg/include/SkSVGDOM.h"
#else
#include "experimental/svg/model/SkSVGDOM.h"
#endif
#include "transaction/rs_marshalling_helper.h"

namespace OHOS {
namespace Rosen {
enum class MaskType {
    NONE = 0,
    SVG,
    GRADIENT,
    PATH,
};

class RSB_EXPORT RSMask : public std::enable_shared_from_this<RSMask> {
public:
    RSMask();
    virtual ~RSMask();
#ifndef USE_ROSEN_DRAWING
    static std::shared_ptr<RSMask> CreateGradientMask(const SkPaint& maskPaint);
    static std::shared_ptr<RSMask> CreatePathMask(const SkPath& maskPath, const SkPaint& maskPaint);
#else
    static std::shared_ptr<RSMask> CreateGradientMask(const Drawing::Brush& maskBrush);
    static std::shared_ptr<RSMask> CreatePathMask(const Drawing::Path& maskPath, const Drawing::Brush& maskBrush);
#endif
    static std::shared_ptr<RSMask> CreateSVGMask(double x, double y, double scaleX, double scaleY,
        const sk_sp<SkSVGDOM>& svgDom);

    void SetSvgX(double x);
    double GetSvgX() const;
    void SetSvgY(double y);
    double GetSvgY() const;
    void SetScaleX(double scaleX);
    double GetScaleX() const;
    void SetScaleY(double scaleY);
    double GetScaleY() const;
#ifndef USE_ROSEN_DRAWING
    void SetMaskPath(const SkPath& path);
    SkPath GetMaskPath() const;
    void SetMaskPaint(const SkPaint& paint);
    SkPaint GetMaskPaint() const;
#else
    void SetMaskPath(const Drawing::Path& path);
    Drawing::Path GetMaskPath() const;
    void SetMaskBrush(const Drawing::Brush& brush);
    Drawing::Brush GetMaskBrush() const;
#endif
    void SetSvgDom(const sk_sp<SkSVGDOM>& svgDom);
    sk_sp<SkSVGDOM> GetSvgDom() const;
#ifndef USE_ROSEN_DRAWING
    sk_sp<SkPicture> GetSvgPicture() const;
#else
    std::shared_ptr<Drawing::Picture> GetSvgPicture() const;
#endif
    void SetMaskType(MaskType type);
    bool IsSvgMask() const;
    bool IsGradientMask() const;
    bool IsPathMask() const;

#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) const;
    [[nodiscard]] static RSMask* Unmarshalling(Parcel& parcel);
#endif

protected:
    RSMask(const RSMask&) = delete;
    RSMask(const RSMask&&) = delete;
    RSMask& operator=(const RSMask&) = delete;
    RSMask& operator=(const RSMask&&) = delete;

private:
    MaskType type_ = MaskType::NONE;
    double svgX_ = 0.0f;
    double svgY_ = 0.0f;
    double scaleX_ = 1.0f;
    double scaleY_ = 1.0f;
    sk_sp<SkSVGDOM> svgDom_;
#ifndef USE_ROSEN_DRAWING
    sk_sp<SkPicture> svgPicture_;
    SkPaint maskPaint_;
    SkPath maskPath_;
#else
    std::shared_ptr<Drawing::Picture> svgPicture_;
    Drawing::Brush maskBrush_;
    Drawing::Path maskPath_;
#endif

};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_MASK_H