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

#include "skia_adapter/skia_data.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaTextBlob::SkiaTextBlob(sk_sp<SkTextBlob> skTextBlob) : skTextBlob_(skTextBlob) {}

sk_sp<SkTextBlob> SkiaTextBlob::GetTextBlob() const
{
    return skTextBlob_;
}

std::shared_ptr<Data> SkiaTextBlob::Serialize(const SkSerialProcs& procs) const
{
    if (!skTextBlob_) {
        LOGE("skTextBlob nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return nullptr;
    }
    auto skData = skTextBlob_->serialize(procs);
    auto data = std::make_shared<Data>();
    auto skiaData = data->GetImpl<SkiaData>();
    if (!skiaData) {
        LOGE("skiaData nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return nullptr;
    }
    skiaData->SetSkData(skData);
    return data;
}

std::shared_ptr<TextBlob> SkiaTextBlob::Deserialize(const void* data, size_t size, const SkDeserialProcs& procs)
{
    sk_sp<SkTextBlob> skTextBlob = SkTextBlob::Deserialize(data, size, procs);
    if (!skTextBlob) {
        LOGE("skTextBlob nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return nullptr;
    }
    std::shared_ptr<TextBlobImpl> textBlobImpl = std::make_shared<SkiaTextBlob>(skTextBlob);
    return std::make_shared<TextBlob>(textBlobImpl);
}

void SkiaTextBlob::SetSkTextBlob(const sk_sp<SkTextBlob> skTextBlob)
{
    skTextBlob_ = skTextBlob; 
}

void SkiaTextBlob::DataTransform(TextEngine::TexgineTextBlob &texgineTextBlob)
{
    if (texgineTextBlob.DetectionEffectiveness()) {
        skTextBlob_ = texgineTextBlob.GetTextBlob();
    }
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS