/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef TEXT_BLOB_H
#define TEXT_BLOB_H

#include "impl_interface/text_blob_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

enum class TextEncoding {
    kUTF8_TextEncoding,//utf-8，默认格式
    kUTF16_TextEncoding,
    kUTF32_TextEncoding,
    kGlyphID_TextEncoding
};

class TextBlob {
public:
    TextBlob();
    ~TextBlob() = default;

    template<typename T>
    const std::shared_ptr<T> GetImpl() const
    {
        return impl_->DowncastingTo<T>();
    }
    // std::shared_ptr<Data> Serialize();
    // bool Deserialize(std::shared_ptr<Data> data);

private:
    std::shared_ptr<TextBlobImpl> impl_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
