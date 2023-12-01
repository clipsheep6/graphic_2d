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

#ifndef ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_PICTURE_H
#define ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_PICTURE_H

#include <memory>

#ifndef USE_ROSEN_DRAWING
#include <include/core/SkPicture.h>
#else
#include "drawing.h"
#endif

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class TexginePicture {
public:
#ifndef USE_ROSEN_DRAWING
    sk_sp<SkPicture> GetPicture() const;
    void SetPicture(const sk_sp<SkPicture> picture);
#else
    std::shared_ptr<RSPicture> GetPicture() const;
    void SetPicture(const std::shared_ptr<RSPicture> picture);
#endif

private:
#ifndef USE_ROSEN_DRAWING
    sk_sp<SkPicture> picture_ = nullptr;
#else
    std::shared_ptr<RSPicture> picture_ = nullptr;
#endif
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_TEXGINE_DRAWING_TEXGINE_PICTURE_H
