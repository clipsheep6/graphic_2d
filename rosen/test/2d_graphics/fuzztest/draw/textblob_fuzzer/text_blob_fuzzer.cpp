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

#include "text_blob_fuzzer.h"
#include <cstddef>
#include <cstdint>
#include "get_object.h"
#include "text/text_blob.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

bool TextBlobFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    const char* str = "as";
    Font font;
    auto textblob = TextBlob::MakeFromText(str, strlen(str), font, TextEncoding::UTF8);
    textblob->Bounds();
    textblob->Serialize();
    textblob->Deserialize(nullptr, 0);

    std::vector<uint16_t> glyphIds = {};
    TextBlob::GetDrawingGlyphIDforTextBlob(textblob.get(), glyphIds);
    TextBlob::GetDrawingPathforTextBlob(0, textblob.get());

    return true;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::TextBlobFuzzTest(data, size);
    return 0;
}
