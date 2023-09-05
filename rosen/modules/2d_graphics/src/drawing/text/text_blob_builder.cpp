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

#include "text/text_blob_builder.h"
#include "impl_factory.h"


namespace OHOS {
namespace Rosen {
namespace Drawing {
TextBlobBuilder::TextBlobBuilder() : impl_(ImplFactory::CreateTextBlobBuilderImpl()) {}

std::shared_ptr<RunBuffer> TextBlobBuilder::AllocRunPos(const Font& font, int count)
{
    return impl_->AllocRunPos(font, count);
}

std::shared_ptr<TextBlob> TextBlobBuilder::Make()
{
    return impl_->Make();
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
