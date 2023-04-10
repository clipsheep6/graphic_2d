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

#include "texgine_string.h"

namespace Texgine {
SkString *TexgineString::GetString()
{
    return string_.get();
}

void TexgineString::SetString(std::shared_ptr<SkString> string)
{
    string_ = string;
}

void TexgineString::SetString(const char *string)
{
    *string_ = string;
}

std::string TexgineString::ToString()
{
    return string_->c_str();
}
} // namespace Texgine
