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

#ifndef TEXT_BLOB_BUILDER_H
#define TEXT_BLOB_BUILDER_H

#include "drawing/engine_adapter/impl_interface/text_blob_builder_impl.h"
#include "text/text_blob.h"
namespace OHOS {
namespace Rosen {
namespace Drawing {
class   TextBlobBuilder {
public:
    TextBlobBuilder() noexcept;
    virtual ~TextBlobBuilder() = default;
    virtual DrawingType GetDrawingType() const
    {
        return DrawingType::COMMON;
    }

    template<typename T>
    const std::shared_ptr<T> GetImpl() const
    {
        return textBlobBuilderImpl_->DowncastingTo<T>();
    }

    virtual std::shared_ptr<TextBlob> Make();

    virtual std::shared_ptr<TextBlobBuilderImpl::RunBuffer> AllocRunPos(const Font &font, int count);

    void InitTextBlobBuilderImpl(std::shared_ptr<TextBlobBuilderImpl> replaceTextBuilderImpl);

private:
    std::shared_ptr<TextBlobBuilderImpl> textBlobBuilderImpl_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif