/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "fontmgr_fuzzer.h"
#include <cstddef>
#include <cstdint>
#include <iostream>
#include "get_object.h"
#include "text/font_mgr.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
namespace {
//constexpr size_t STR_LEN = 10;
static int32_t testNum = 0;
} // namespace



bool FontMgrFuzzTest(const uint8_t* data, size_t size)
{
    std::cout << " FontMgrFuzzTest 01 testNum = " << testNum << std::endl;
    if (data == nullptr || !size) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    //std::cout << " FontMgrFuzzTest 02 "  << std::endl;

    //std::shared_ptr<FontMgr> fongMgr = FontMgr::CreateDefaultFontMgr();
    std::shared_ptr<FontMgr> fongMgr = FontMgr::CreateDynamicFontMgr();
    if (!fongMgr) {
        return false;
    }

    std::string familyName((const char*)data, size);

    //std::string familyName = GetStringFromData(STR_LEN);
    // std::cout << " FontMgrFuzzTest 03  "  << "  data = " << (void*)data <<
    //     "  g_data = " << (void*)g_data << "  g_size = " << g_size  << "  g_pos = " << g_pos  << "  size = " << size  << 
    //     "  familyName = " << familyName << 
    //     std::endl;
    //fongMgr->LoadDynamicFont(familyName, data, size);
    FontStyle fontStyle;
    // std::cout << " FontMgrFuzzTest 04  "  << "  data = " << (void*)data <<
    //     "  g_data = " << (void*)g_data << "  g_size = " << g_size  << "  g_pos = " << g_pos  << "  size = " << size  << 
    //     "  familyName = " << familyName << 
    //     std::endl;
    Typeface* typeface = fongMgr->MatchFamilyStyle(familyName.c_str(), fontStyle);
    //std::cout << " FontMgrFuzzTest 05 "  << std::endl;
    if (typeface) {
        std::cout << " FontMgrFuzzTest 09  testNum = " << testNum << std::endl;
        testNum++;
        return true;
    } else {
        std::cout << " FontMgrFuzzTest 10  testNum = " << testNum << std::endl;
        testNum++;
        return false;
    }


}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::FontMgrFuzzTest(data, size);
    return 0;
}
