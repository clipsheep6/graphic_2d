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

#include "skia_svg_dom.h"
#include "skia_canvas_enforcer.h"

#include "experimental/svg/model/SkSVGDOM.h"

#include "draw/canvas.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaSVGDOM::SkiaSVGDOM() : skSVGDOM_(sk_make_sp<SkSVGDOM>()) {}

void SkiaSVGDOM::Render(Canvas& canvas)
{
    if (skSVGDOM_ == nullptr) {
        LOGE("skSVGDOM is nullptr");
    }

    CoreCanvas::Type type = canvas.GetType();

    if (type == CoreCanvas::Type::COMMON) {
        auto skiaCanvasImpl = canvas.GetImpl<SkiaCanvas>();
        SkCanvas* skCanvas = skiaCanvasImpl->ExportSkCanvas();
        skSVGDOM_->render(skCanvas);
    } else if (type == CoreCanvas::Type::RECORDING) {
        auto skiaCanvasEnforcer = std::make_shared<SkiaCanvasEnforcer>(canvas);
        skSVGDOM_->render(skiaCanvasEnforcer.get());
    }
}

const Size SkiaSVGDOM::ContainerSize() const
{
    if (skSVGDOM_ == nullptr) {
        LOGE("skSVGDOM is nullptr");
    }

    auto skSize = skSVGDOM_->containerSize();
    return Size(skSize.fWidth, skSize.fHeight);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
