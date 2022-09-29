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

#include <gtest/gtest.h>

#include "measurer.h"
#include "texgine/font_providers.h"
#include "text_converter.h"

using namespace testing;

// #define DEBUG_MEASURER_TEST
#ifdef DEBUG_MEASURER_TEST
#define DEBUG_LOG() GTEST_LOG_(INFO)
#else
#define DEBUG_LOG() std::stringstream()
#endif

namespace Texgine {
namespace {
class MeasurerTest : public testing::Test {
};

TEST_F(MeasurerTest, MeasureText)
{
    auto providers = FontProviders::Create();
    auto fc = providers->GenerateFontCollection({});
    GTEST_ASSERT_NE(fc, nullptr);

    auto text = TextConverter::ToUTF16("啊123\u261d\U0001f3ff \u261d321");
    auto measurer = Measurer::Create(text, *fc);
    measurer->SetFontStyle({});
    measurer->SetLocale("zh_CN.utf-8");
    measurer->SetRTL(false);
    measurer->SetRange(0, text.size());
    CharGroups cgs;
    GTEST_ASSERT_EQ(measurer->Measure(cgs), 0);
    GTEST_ASSERT_EQ(cgs.GetNumberOfCharGroup(), 11u);

    for (const auto &cg : cgs) {
        std::stringstream ss;
        ss << "cg: " << "width=" << cg.GetWidth() << ", ";
        for (const auto &c : cg.chars_) {
            ss << "[0x" << std::hex << c << "]";
            GTEST_ASSERT_GT(c, 0);
        }
        DEBUG_LOG() << ss.str();

        GTEST_ASSERT_GT(cg.chars_.size(), 0u);
        GTEST_ASSERT_GT(cg.GetWidth(), 0);
    }
}
} // namespace
} // namespace Texgine
