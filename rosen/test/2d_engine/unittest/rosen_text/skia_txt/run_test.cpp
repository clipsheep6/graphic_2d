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

#include "gtest/gtest.h"
#include "font_collection.h"
#include "paragraph_builder.h"
#include "paragraph_style.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen;
using namespace OHOS::Rosen::Drawing;
using namespace OHOS::Rosen::SPText;

namespace txt {
class RunTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    static inline std::shared_ptr<Paragraph> paragraph_ = nullptr;
    static inline std::vector<std::unique_ptr<SPText::Run>> runs_;
};

void RunTest::SetUpTestCase()
{
    ParagraphStyle paragraphStyle;
    std::shared_ptr<FontCollection> fontCollection = std::make_shared<FontCollection>();
    if (!fontCollection) {
        std::cout << "RunTest::SetUpTestCase error fontCollection is nullptr" << std::endl;
        return;
    }
    fontCollection->SetupDefaultFontManager();
    std::shared_ptr<ParagraphBuilder> paragraphBuilder = ParagraphBuilder::Create(paragraphStyle, fontCollection);
    if (!paragraphBuilder) {
        std::cout << "RunTest::SetUpTestCase error paragraphBuilder is nullptr" << std::endl;
        return;
    }
    std::u16string text(u"RunTest");
    paragraphBuilder->AddText(text);
    paragraph_ = paragraphBuilder->Build();
    if (!paragraph_) {
        std::cout << "RunTest::SetUpTestCase error paragraph_ is nullptr" << std::endl;
        return;
    }
    // 50 just for test
    paragraph_->Layout(50);
    Canvas canvas;
    paragraph_->Paint(&canvas, 0.0, 0.0);
    std::vector<std::unique_ptr<SPText::TextLineBase>> textLineBases = paragraph_->GetTextLines();
    if (!textLineBases.size() || !textLineBases.at(0)) {
        std::cout << "RunTest::SetUpTestCase error textLineBases variable acquisition exception " << std::endl;
        return;
    }
    runs_ = textLineBases.at(0)->GetGlyphRuns();
    if (!runs_.size() || !runs_.at(0)) {
        std::cout << "RunTest::SetUpTestCase error runs_ variable acquisition exception" << std::endl;
    }
}

void RunTest::TearDownTestCase()
{
}

/*
 * @tc.name: RunTest001
 * @tc.desc: test for GetFont
 * @tc.type: FUNC
 */
HWTEST_F(RunTest, RunTest001, TestSize.Level1)
{
    EXPECT_EQ(paragraph_ != nullptr, true);
    EXPECT_EQ(runs_.size() != 0, true);
    EXPECT_EQ(runs_.at(0) != nullptr, true);
    EXPECT_EQ(runs_.at(0)->GetFont().GetSize() > 0, true);
}

/*
 * @tc.name: RunTest002
 * @tc.desc: test for GetGlyphCount
 * @tc.type: FUNC
 */
HWTEST_F(RunTest, RunTest002, TestSize.Level1)
{
    EXPECT_EQ(paragraph_ != nullptr, true);
    EXPECT_EQ(runs_.size() != 0, true);
    EXPECT_EQ(runs_.at(0) != nullptr, true);
    EXPECT_EQ(runs_.at(0)->GetGlyphCount() > 0, true);
}

/*
 * @tc.name: RunTest003
 * @tc.desc: test for GetGlyphs
 * @tc.type: FUNC
 */
HWTEST_F(RunTest, RunTest003, TestSize.Level1)
{
    EXPECT_EQ(paragraph_ != nullptr, true);
    EXPECT_EQ(runs_.size() != 0, true);
    EXPECT_EQ(runs_.at(0) != nullptr, true);
    EXPECT_EQ(runs_.at(0)->GetGlyphs().size() > 0, true);
}

/*
 * @tc.name: RunTest004
 * @tc.desc: test for GetPositions
 * @tc.type: FUNC
 */
HWTEST_F(RunTest, RunTest004, TestSize.Level1)
{
    EXPECT_EQ(paragraph_ != nullptr, true);
    EXPECT_EQ(runs_.size() != 0, true);
    EXPECT_EQ(runs_.at(0) != nullptr, true);
    EXPECT_EQ(runs_.at(0)->GetPositions().size() > 0, true);
}

/*
 * @tc.name: RunTest005
 * @tc.desc: test for GetOffsets
 * @tc.type: FUNC
 */
HWTEST_F(RunTest, RunTest005, TestSize.Level1)
{
    EXPECT_EQ(paragraph_ != nullptr, true);
    EXPECT_EQ(runs_.size() != 0, true);
    EXPECT_EQ(runs_.at(0) != nullptr, true);
    EXPECT_EQ(runs_.at(0)->GetOffsets().size() > 0, true);
}

/*
 * @tc.name: RunTest006
 * @tc.desc: test for Paint
 * @tc.type: FUNC
 */
HWTEST_F(RunTest, RunTest006, TestSize.Level1)
{
    EXPECT_EQ(paragraph_ != nullptr, true);
    EXPECT_EQ(runs_.size() != 0, true);
    EXPECT_EQ(runs_.at(0) != nullptr, true);
    Canvas canvas;
    runs_.at(0)->Paint(&canvas, 0.0, 0.0);
}
} // namespace txt