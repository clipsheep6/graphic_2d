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

#ifndef SKIA_TEXT_BLOB_H
#define SKIA_TEXT_BLOB_H

#include <include/core/SkTextBlob.h>
#include "impl_interface/text_blob_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaTextBlob : public TextBlobImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;
    SkiaTextBlob() noexcept {};
    ~SkiaTextBlob() override {};
    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    /*
     * @brief Return the pointer of SkTextBlob to prepare the paint info
     */
    sk_sp<SkTextBlob> GetSkTextBlob() const;

    /*
     * @brief Sets SkTextBlob to TexgineTextBlob
     */
    void SetSkTextBlob(const sk_sp<SkTextBlob> textBlob);

    std::shared_ptr<Data> Serialize() const override;

    bool Deserialize(std::shared_ptr<Data> data) override;

private:
    sk_sp<SkTextBlob> skTextBlob_ = nullptr;
};
} // Drawing
} // Rosen
} // OHOS

#endif
