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

#ifndef SKIA_SVG_DOM_H
#define SKIA_SVG_DOM_H

#include "experimental/svg/model/SkSVGDOM.h"

#include "impl_interface/svg_dom_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaSVGDOM : public SVGDOMImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;
    SkiaSVGDOM();
    ~SkiaSVGDOM() override {}

    void Render(Canvas& canvas) override;
    const Size ContainerSize() const override;

    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

private:
    sk_sp<SkSVGDOM> skSVGDOM_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
