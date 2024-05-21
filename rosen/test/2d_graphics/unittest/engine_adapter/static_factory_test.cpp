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

#include <cstddef>
#include "gtest/gtest.h"
#include "engine_adapter/static_factory.h"
#include "utils/memory_stream.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class StaticFactoryTest : public testing::Test
{
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void StaticFactoryTest::SetUpTestCase() {}
void StaticFactoryTest::TearDownTestCase() {}
void StaticFactoryTest::SetUp() {}
void StaticFactoryTest::TearDown() {}

/**
 * @tc.name: MakeFromRSXform001
 * @tc.desc: Test StaticFactory
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(StaticFactoryTest, MakeFromRSXform001, TestSize.Level1)
{
    char text[10] = {0};
    RSXform xform[] = {{25, 36, 2, 5}, {7, 8, 9, 12}};
    Font font;
    StaticFactory::MakeFromRSXform(text, 10, xform, font, TextEncoding::UTF8);
}

/**
 * @tc.name: MakeFromStream001
 * @tc.desc: Test StaticFactory
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(StaticFactoryTest, MakeFromStream001, TestSize.Level1)
{
    char data[10] = {0};
    auto stream = std::make_unique<MemoryStream>(data, 10);
    ASSERT_TRUE(stream != nullptr);
    StaticFactory::MakeFromStream(std::move(stream), 0);
}

/**
 * @tc.name: MakeFromName001
 * @tc.desc: Test StaticFactory
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(StaticFactoryTest, MakeFromName001, TestSize.Level1)
{
    char familyName[] = "Arial";
    StaticFactory::MakeFromName(familyName, Drawing::FontStyle());
}

/**
 * @tc.name: DeserializeTypeface001
 * @tc.desc: Test StaticFactory
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(StaticFactoryTest, DeserializeTypeface001, TestSize.Level1)
{
    char data[10] = {0};
    StaticFactory::DeserializeTypeface(data, 10);
}

/**
 * @tc.name: AsBlendMode001
 * @tc.desc: Test StaticFactory
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(StaticFactoryTest, AsBlendMode001, TestSize.Level1)
{
    Brush brush;
    StaticFactory::AsBlendMode(brush);
}

/**
 * @tc.name: GetDrawingGlyphIDforTextBlob001
 * @tc.desc: Test StaticFactory
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(StaticFactoryTest, GetDrawingGlyphIDforTextBlob001, TestSize.Level1)
{
    std::vector<uint16_t> glyphId;
    char text[] = "hello";
    int byteLength = 5;
    Font font;
    std::shared_ptr<TextBlob> blob = TextBlob::MakeFromText(text,
                                                            byteLength, font, static_cast<TextEncoding>(TextEncoding::UTF8));
    StaticFactory::GetDrawingGlyphIDforTextBlob(blob.get(), glyphId);
}

/**
 * @tc.name: GetDrawingPointsForTextBlob001
 * @tc.desc: Test StaticFactory
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(StaticFactoryTest, GetDrawingPointsForTextBlob001, TestSize.Level1)
{
    char text[] = "hello";
    int byteLength = 5;
    Font font;
    std::shared_ptr<TextBlob> blob = TextBlob::MakeFromText(text,
                                                            byteLength, font, static_cast<TextEncoding>(TextEncoding::UTF8));
    std::vector<Point> points;
    StaticFactory::GetDrawingPointsForTextBlob(blob.get(), points);
}

/**
 * @tc.name: GetGroupParameters001
 * @tc.desc: Test StaticFactory
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(StaticFactoryTest, GetGroupParameters001, TestSize.Level1)
{
    DrawingAnimationType type = DrawingAnimationType::SCALE_TYPE;
    DrawingCommonSubType subType = DrawingCommonSubType::DOWN;
    StaticFactory::GetGroupParameters(type, 0, 0, subType);
}

/**
 * @tc.name: CreateEmpty001
 * @tc.desc: Test StaticFactory
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(StaticFactoryTest, CreateEmpty001, TestSize.Level1)
{
    StaticFactory::CreateEmpty();
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS