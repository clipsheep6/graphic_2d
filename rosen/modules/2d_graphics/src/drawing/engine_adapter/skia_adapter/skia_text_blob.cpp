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

#include "skia_text_blob.h"

#include "src/core/SkReadBuffer.h"
#include "include/core/SkSerialProcs.h"
#include "src/core/SkTextBlobPriv.h"

#include "skia_data.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

SkiaTextBlob::SkiaTextBlob() {}

sk_sp<SkTextBlob> SkiaTextBlob::GetTextBlob() const
{
    return textBlob_;
}
void SkiaTextBlob::SetTextBlob(const sk_sp<SkTextBlob> textBlob)
{
    textBlob_ = textBlob;
}

std::shared_ptr<Data> SkiaTextBlob::Serialize() {

    if (textBlob_ == nullptr) {
         return nullptr;
    }
    SkSerialProcs proc;
    auto skData = textBlob_->serialize(proc);
    auto data = std::make_shared<Data>();
    data->GetImpl<SkiaData>()->SetSkData(skData);
    return data;
}

bool SkiaTextBlob::Deserialize(std::shared_ptr<Data> data) {
    
    if (data == nullptr) {
        return false;
    }
    SkDeserialProcs proc;

    SkReadBuffer buffer(data->GetData(), data->GetSize());
    buffer.setDeserialProcs(proc);
    return SkTextBlobPriv::MakeFromBuffer(buffer) != nullptr;
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
