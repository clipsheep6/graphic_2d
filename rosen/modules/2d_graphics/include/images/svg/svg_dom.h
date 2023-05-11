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

#ifndef SVG_DOM_H
#define SVG_DOM_H

#include "drawing/engine_adapter/impl_interface/svg_dom_impl.h"
#include "drawing/engine_adapter/impl_factory.h"
#include "utils/size.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SVGDOM {
public:
    SVGDOM();
    virtual ~SVGDOM() {}

    void Render(Canvas& canvas);
    const Size ContainerSize() const;
private:
    std::shared_ptr<SVGDOMImpl> impl_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
