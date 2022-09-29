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

#ifndef ROSEN_TEST_TEXGINE_UNITTEST_MOCK_MOCK_SK_FONT_MGR_H
#define ROSEN_TEST_TEXGINE_UNITTEST_MOCK_MOCK_SK_FONT_MGR_H

#include <gmock/gmock.h>
#include <include/core/SkFontMgr.h>

namespace Texgine {
class MockSkFontMgr : public SkFontMgr {
public:
    MOCK_METHOD(int, onCountFamilies, (), (const, override));
    MOCK_METHOD(void, onGetFamilyName, (int index, SkString *familyName), (const, override));
    MOCK_METHOD(SkFontStyleSet *, onCreateStyleSet, (int index), (const, override));
    MOCK_METHOD(SkFontStyleSet *, onMatchFamily, (const char familyName[]), (const, override));
    MOCK_METHOD(SkTypeface *, onMatchFamilyStyle, (const char familyName[], const SkFontStyle &), (const, override));
    MOCK_METHOD(SkTypeface *, onMatchFamilyStyleCharacter, (const char familyName[], const SkFontStyle &,
                                                            const char *bcp47[], int bcp47Count,
                                                            SkUnichar character), (const, override));
    MOCK_METHOD(sk_sp<SkTypeface>, onMakeFromData, (sk_sp<SkData>, int ttcIndex), (const, override));
    MOCK_METHOD(sk_sp<SkTypeface>, onMakeFromStreamIndex, (std::unique_ptr<SkStreamAsset>,
                                                           int ttcIndex), (const, override));
    MOCK_METHOD(sk_sp<SkTypeface>, onMakeFromStreamArgs, (std::unique_ptr<SkStreamAsset>,
                                                          const SkFontArguments &), (const, override));
    MOCK_METHOD(sk_sp<SkTypeface>, onMakeFromFile, (const char path[], int ttcIndex), (const, override));
    MOCK_METHOD(sk_sp<SkTypeface>, onLegacyMakeTypeface, (const char familyName[], SkFontStyle), (const, override));
    MOCK_METHOD(SkTypeface *, onMatchFaceStyle, (const SkTypeface *, const SkFontStyle &), (const, override));
};
} // namespace Texgine

#endif // ROSEN_TEST_TEXGINE_UNITTEST_MOCK_MOCK_SK_FONT_MGR_H
