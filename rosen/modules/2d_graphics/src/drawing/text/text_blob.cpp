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

#include "text/text_blob.h"
#include "impl_factory.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
TextBlob::TextBlob() noexcept : textBlobImpl_(ImplFactory::CreateTextBlobImpl())
{}

void TextBlob::InitTextBlobImpl(std::shared_ptr<TextBlobImpl> replaceTextBlobImpl)
{
    textBlobImpl_ = move(replaceTextBlobImpl);
}


} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
