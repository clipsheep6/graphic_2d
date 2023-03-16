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

#include "texgine_data.h"

namespace Texgine {
sk_sp<SkData> TexgineData::GetData()
{
    return data_;
}

void TexgineData::SetData(sk_sp<SkData> data)
{
    data_ = data;
}

std::shared_ptr<TexgineData> TexgineData::MakeFromFileName(const char path[])
{
    auto data = std::make_shared<TexgineData>();
    data->SetData(SkData::MakeFromFileName(path));
    return data;
}
} // namespace Texgine
