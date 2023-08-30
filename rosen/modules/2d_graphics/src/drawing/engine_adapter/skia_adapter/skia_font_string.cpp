/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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


#include "skia_font_string.h"
namespace OHOS {
namespace Rosen {
namespace Drawing {
void SkiaFontString::SetString(const std::string &s)
{
    *skString_ = s.c_str();
}

void SkiaFontString::SetSkString(const std::shared_ptr<SkString> string)
{
    skString_ = string;
}

std::string SkiaFontString::ToString() const
{
    if (!skString_) {
        return "";
    }
    return skString_->c_str();
}

SkString *SkiaFontString::GetString() const
{
    return skString_.get();
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
