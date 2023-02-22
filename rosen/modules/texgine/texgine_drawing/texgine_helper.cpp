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

#include "texgine_helper.h"

namespace Texgine {
std::shared_ptr<TexgineCanvas> TexgineHelper::StartRecording(const TexgineRect &rect)
{
    auto canvas = std::make_shared<TexgineCanvas>();
    canvas->SetCanvas(&helper_.StartRecording(*rect.GetRect()));
    return canvas;
}

std::shared_ptr<TexginePicture> TexgineHelper::FinishRecoding()
{
    auto picture = std::make_shared<TexginePicture>();
    picture->SetPicture(helper_.FinishRecoding());
    return picture;
}

std::shared_ptr<TexgineData> TexgineHelper::Marshal(const std::shared_ptr<TexginePicture> picture,
                                 std::map<std::string, std::string> &familyNameToPath)
{
    auto data = std::make_shared<TexgineData>();
    data->SetData(SkpHelper::Marshal(picture->GetPicture(), familyNameToPath));
    return data;
}
} // namespace Texgine
