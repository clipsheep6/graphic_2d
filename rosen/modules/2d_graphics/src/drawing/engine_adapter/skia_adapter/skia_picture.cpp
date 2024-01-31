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

#include "skia_picture.h"
#include "skia_adapter/skia_data.h"
#include "utils/data.h"
#include "utils/log.h"
namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaPicture::SkiaPicture() noexcept : skiaPicture_(nullptr) {}

const sk_sp<SkPicture> SkiaPicture::GetPicture() const
{
    return skiaPicture_;
}

std::shared_ptr<Data> SkiaPicture::Serialize() const
{
    if (skiaPicture_ == nullptr) {
        LOGE("SkiaPicture::Serialize, SkPicture is nullptr!");
        return nullptr;
    }

    auto skData = skiaPicture_->serialize();
    auto data = std::make_shared<Data>();
    data->GetImpl<SkiaData>()->SetSkData(skData);

    return data;
}

bool SkiaPicture::Deserialize(std::shared_ptr<Data> data)
{
    if (data == nullptr) {
        return false;
    }

    skiaPicture_ = SkPicture::MakeFromData(data->GetData(), data->GetSize());
    return skiaPicture_ != nullptr;
}

void SkiaPicture::SetSkPicture(const sk_sp<SkPicture>& skPicture)
{
    skiaPicture_ = skPicture;
}

std::shared_ptr<Picture> SkiaPicture::MakePlaceHolder(Rect cull)
{
    if (skiaPicture_ == nullptr) {
        LOGE("SkiaPicture::MakePlaceHolder, skiaPicture_ is nullptr!");
        return nullptr;
    }
    SkRect skCull = SkRect::MakeLTRB(cull.GetLeft(), cull.GetTop(), cull.GetRight(), cull.GetBottom());
    sk_sp<SkPicture> skPicture = skiaPicture_->MakePlaceholder(skCull);
    std::shared_ptr<Picture> pic = std::make_shared<Picture>();
    pic->GetImpl<SkiaPicture>()->SetSkPicture(skPicture);
    return pic;
}

void SkiaPicture::PlayBack(CoreCanvas* canvas)
{
    if (skiaPicture_ == nullptr) {
        LOGE("SkiaPicture::PlayBack, skiaPicture_ is nullptr!");
        return;
    }
    if (canvas == nullptr) {
        LOGE("SkiaPicture::PlayBack, canvas is nullptr!");
        return;
    }
    auto skCanvas = std::make_shared<SkCanvas>(canvas->GetWidth(), canvas->GetHeight());
    skiaPicture_->playback(skCanvas);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
