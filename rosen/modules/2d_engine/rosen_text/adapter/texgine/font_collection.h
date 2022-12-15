/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef ROSEN_TEXT_ADAPTER_TEXGINE_FONT_COLLECTION_H
#define ROSEN_TEXT_ADAPTER_TEXGINE_FONT_COLLECTION_H

#include "rosen_text/font_collection.h"

namespace Texgine {
class FontProviders;
class DynamicFontProvider;
} // namespace Texgine

namespace OHOS {
namespace Rosen {
namespace AdapterTexgine {
class FontCollection : public ::OHOS::Rosen::FontCollection {
public:
    FontCollection();
    std::unique_ptr<Texgine::FontProviders> Get();

    void DisableFallback() override;
    void DisableSystemFont() override;
    void LoadFont(const std::string &familyName, const uint8_t *data, size_t datalen) override;

private:
    std::unique_ptr<Texgine::FontProviders> fontProviders_ = nullptr;
    std::shared_ptr<Texgine::DynamicFontProvider> dfprovider_ = nullptr;
    bool disableSystemFont_ = false;
};
} // namespace AdapterTexgine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_TEXT_ADAPTER_TEXGINE_FONT_COLLECTION_H
