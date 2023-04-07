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

#ifndef ROSEN_TEST_TEXGINE_UNITTEST_MOCK_MOCK_BREAK_ITERATOR_H
#define ROSEN_TEST_TEXGINE_UNITTEST_MOCK_MOCK_BREAK_ITERATOR_H

#include <gmock/gmock.h>

#include <unicode/brkiter.h>

namespace Texgine {
class MockBreakIterator : public icu::BreakIterator {
public:
    UBool operator ==(const icu::BreakIterator &rhs) const override { return false; }
    MOCK_METHOD(BreakIterator *, clone, (), (const, override));
    MOCK_METHOD(UClassID, getDynamicClassID, (), (const, override));
    MOCK_METHOD(icu::CharacterIterator &, getText, (), (const, override));
    MOCK_METHOD(UText *, getUText, (UText *fillIn, UErrorCode &status), (const, override));
    MOCK_METHOD(void, setText, (const icu::UnicodeString &text), (override));
    MOCK_METHOD(void, setText, (UText *text, UErrorCode &status), (override));
    MOCK_METHOD(void, adoptText, (icu::CharacterIterator* it), (override));
    MOCK_METHOD(int32_t, first, (), (override));
    MOCK_METHOD(int32_t, last, (), (override));
    MOCK_METHOD(int32_t, previous, (), (override));
    MOCK_METHOD(int32_t, next, (), (override));
    MOCK_METHOD(int32_t, current, (), (const, override));
    MOCK_METHOD(int32_t, following, (int32_t offset), (override));
    MOCK_METHOD(int32_t, preceding, (int32_t offset), (override));
    MOCK_METHOD(UBool, isBoundary, (int32_t offset), (override));
    MOCK_METHOD(int32_t, next, (int32_t offset), (override));
    MOCK_METHOD(BreakIterator *, createBufferClone, (void *stackBuffer, int32_t &BufferSize,
                                                     UErrorCode &status), (override));
    MOCK_METHOD(BreakIterator &, refreshInputText, (UText *input, UErrorCode &status), (override));
};
}// namespace Texgine
#endif // ROSEN_TEST_TEXGINE_UNITTEST_MOCK_MOCK_BREAK_ITERATOR_H