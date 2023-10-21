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

#include <memory>

#include "include/core/SkTextBlob.h"
#include "include/core/SkSerialProcs.h"

#include "impl_interface/text_blob_impl.h"
#include "text/text_blob.h"
#include "utils/data.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaTextBlob : public TextBlobImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;
    explicit SkiaTextBlob(sk_sp<SkTextBlob> skTextBlob);
    SkiaTextBlob() noexcept {};
    ~SkiaTextBlob() override = default;

    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    sk_sp<SkTextBlob> GetTextBlob() const;

    std::shared_ptr<Data> Serialize(const SkSerialProcs& procs) const;

    static std::shared_ptr<TextBlob> Deserialize(const void* data, size_t size, const SkDeserialProcs& procs);
    void SetSkTextBlob(sk_sp<SkTextBlob> textBlob);

private:
    sk_sp<SkTextBlob> skTextBlob_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif