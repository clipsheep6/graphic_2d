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

#include "text/memory_stream.h"
#include "impl_factory.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
MemoryStream::MemoryStream() noexcept : memoryStreamImpl_(ImplFactory::CreateMemoryStreamImpl())
{}

MemoryStream::MemoryStream(const void *data, size_t length) : MemoryStream()
{
    auto otherMemoryStream = memoryStreamImpl_->MakeCopy(data,length);
    ChangeInitMemoryStreamImpl(move(otherMemoryStream->memoryStreamImpl_));
}

void MemoryStream::ChangeInitMemoryStreamImpl(std::unique_ptr<MemoryStreamImpl> replaceMemoryStreamImpl)
{
    memoryStreamImpl_ = move(replaceMemoryStreamImpl);
}

std::unique_ptr<MemoryStream> MakeCopy(const void *data, size_t length)
{
    return std::make_unique<MemoryStream>(data, length);
}



} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
