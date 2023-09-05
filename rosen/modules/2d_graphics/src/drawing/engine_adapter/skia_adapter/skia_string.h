/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.. All rights reserved.
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

#ifndef SKIA_STRING_H
#define SKIA_STRING_H

#include <include/core/SkString.h>
#include "impl_interface/string_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaString : public StringImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;
    SkiaString() ;
    ~SkiaString() override = default;

    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    SkString *GetString() const;

    void SetString(const std::shared_ptr<SkString> string);

    void SetString(const std::string &s) override;

    std::string ToString() const override;

private:
    std::shared_ptr<SkString> string_ = std::make_shared<SkString>();
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // SKIA_STRING_H
