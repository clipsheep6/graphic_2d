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

#include "skia_stream.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

SkiaMemoryStream::SkiaMemoryStream(): memoryStream_(std::make_unique<SkMemoryStream>())
{
}

SkiaMemoryStream::SkiaMemoryStream(std::unique_ptr<SkMemoryStream> stream): memoryStream_(std::move(stream))
{
}

std::unique_ptr<SkMemoryStream> SkiaMemoryStream::GetStream()
{
    return std::move(memoryStream_);
}

std::unique_ptr<SkiaMemoryStream> SkiaMemoryStream::MakeCopy(const void *data, size_t length)
{
    return std::make_unique<SkiaMemoryStream>(SkMemoryStream::MakeCopy(data, length));
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
