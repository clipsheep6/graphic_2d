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

#ifndef ROSEN_TEST_TEXGINE_UNITTEST_MOCK_MOCK_SK_FONT_STYLE_SET_H
#define ROSEN_TEST_TEXGINE_UNITTEST_MOCK_MOCK_SK_FONT_STYLE_SET_H

#include <gmock/gmock.h>
#include <include/core/SkFontMgr.h>

namespace Texgine {
class MockSkFontStyleSet : public SkFontStyleSet {
public:
    MOCK_METHOD(int, count, (), (override));
    MOCK_METHOD(void, getStyle, (int, SkFontStyle *, SkString *), (override));
    MOCK_METHOD(SkTypeface *, createTypeface, (int index), (override));
    MOCK_METHOD(SkTypeface *, matchStyle, (const SkFontStyle &), (override));
};
} // namespace Texgine

#endif // ROSEN_TEST_TEXGINE_UNITTEST_MOCK_MOCK_SK_FONT_STYLE_SET_H
