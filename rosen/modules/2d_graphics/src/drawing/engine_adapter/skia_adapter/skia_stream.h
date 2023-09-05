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

#ifndef SKIA_STREAM_H
#define SKIA_STREAM_H

#include <include/core/SkStream.h>
#include "impl_interface/stream_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaMemoryStream : public StreamImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;
    SkiaMemoryStream();
    explicit SkiaMemoryStream(std::unique_ptr<SkMemoryStream> stream);
    ~SkiaMemoryStream() override = default;

    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    std::unique_ptr<SkMemoryStream> GetStream();

    static std::unique_ptr<SkiaMemoryStream> MakeCopy(const void *data, size_t length);

private:
    std::unique_ptr<SkMemoryStream> memoryStream_ = nullptr;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // SKIA_STREAM_H
