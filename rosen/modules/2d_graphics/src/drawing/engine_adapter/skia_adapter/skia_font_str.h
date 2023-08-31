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

#ifndef SKIA_FONT_STR_H
#define SKIA_FONT_STR_H

#include <memory>
#include <include/core/SkString.h>
#include "impl_interface/font_string_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaFontStr : public FontStringImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;
    SkiaFontStr() noexcept {};
    ~SkiaFontStr() override {}
    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    void SetString(const std::string &s) override;

    std::string ToString() const override;

    SkString *GetString() const;

    void SetSkString(const std::shared_ptr<SkString> string);

private:
    std::shared_ptr<SkString> skString_ = std::make_shared<SkString>();
};
} // Drawing
} // Rosen
} // OHOS

#endif
