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

#include "skia_text_blob.h"
#include "utils/log.h"
#include "skia_adapter/skia_data.h"
#include <include/core/SkSerialProcs.h>

namespace OHOS {
namespace Rosen {
namespace Drawing {

sk_sp<SkTextBlob> SkiaTextBlob::GetSkTextBlob() const
{
    return skTextBlob_;
}

void SkiaTextBlob::SetSkTextBlob(const sk_sp<SkTextBlob> skTextBlob)
{
    skTextBlob_ = skTextBlob;
}

std::shared_ptr<Data> SkiaTextBlob::Serialize() const
{
    if (skTextBlob_ == nullptr) {
        LOGE("SkiaTextBlob::Serialize, SkiaTextBlob is nullptr!");
        return nullptr;
    }
    SkSerialProcs skSerialProcs;
    auto skData = skTextBlob_->serialize(skSerialProcs);
    auto data = std::make_shared<Data>();
    data->GetImpl<SkiaData>()->SetSkData(skData);
    return data;
}

bool SkiaTextBlob::Deserialize(std::shared_ptr<Data> data)
{
    if (data == nullptr) {
        return false;
    }
    SkDeserialProcs skDeserialProcs;
    skTextBlob_ = SkTextBlob::Deserialize(data->GetData(), data->GetSize(),skDeserialProcs);
    return skTextBlob_ != nullptr;
}

} // Drawing
} // Rosen
} // OHOS
