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
#include <fstream>

#include "gtest/gtest.h"
#include "impl_factory.h"
#include "impl_interface/font_mgr_impl.h"

#include "text/font_mgr.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class FontMgrTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void FontMgrTest::SetUpTestCase() {}
void FontMgrTest::TearDownTestCase() {}
void FontMgrTest::SetUp() {}
void FontMgrTest::TearDown() {}

/**
 * @tc.name:LoadDynamicFont002
 * @tc.desc: Test LoadDynamicFont
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(FontMgrTest, LoadDynamicFont002, TestSize.Level1)
{
    std::shared_ptr<FontMgr> FontMgr = FontMgr::CreateDynamicFontMgr();
    std::vector<uint8_t> emptyFontData;
    Typeface* typeface = FontMgr->LoadDynamicFont("EmptyFont", emptyFontData.data(), emptyFontData.size());
    ASSERT_TRUE(typeface == nullptr);
}

/**
 *
 * @tc.name:MatchFamilyStyleCharacter001
 * @tc.desc: Test MatchFamilyStyleCharacter
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(FontMgrTest, MatchFamilyStyleCharacter001, TestSize.Level1)
{
    std::shared_ptr<FontMgr> FontMgr = FontMgr::CreateDefaultFontMgr();
    FontStyle fontStyle;
    const char* bcp47[] = { "en-US" };
    int bcp47Count = 1;
    int32_t character = 'A';
    Typeface* typeface = FontMgr->MatchFamilyStyleCharacter("serif", fontStyle, bcp47, bcp47Count, character);
    ASSERT_TRUE(typeface != nullptr);
}

/**
 * @tc.name:MatchFamily002
 * @tc.desc: Test MatchFamily
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(FontMgrTest, MatchFamily002, TestSize.Level1)
{
    std::shared_ptr<FontMgr> FontMgr = FontMgr::CreateDefaultFontMgr();
    const char* familyName = "serif";
    FontStyleSet* fontStyleSet = FontMgr->MatchFamily(familyName);
    ASSERT_TRUE(fontStyleSet != nullptr);
}

/**
 * @tc.name:CountFamilies001
 * @tc.desc: Test CountFamilies
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(FontMgrTest, CountFamilies001, TestSize.Level1)
{
    std::shared_ptr<FontMgr> FontMgr = FontMgr::CreateDefaultFontMgr();
    int familyCount = FontMgr->CountFamilies();
    ASSERT_GE(familyCount, 0);
}

/**
 * @tc.name:GetFamilyName001
 * @tc.desc: Test GetFamilyName
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(FontMgrTest, GetFamilyName001, TestSize.Level1)
{
    std::shared_ptr<FontMgr> FontMgr = FontMgr::CreateDefaultFontMgr();
    std::string familyName;
    FontMgr->GetFamilyName(0, familyName);
    ASSERT_TRUE(familyName.length() > 0);
}

/**
 * @tc.name:CreateStyleSet001
 * @tc.desc: Test CreateStyleSet
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(FontMgrTest, CreateStyleSet001, TestSize.Level1)
{
    std::shared_ptr<FontMgr> FontMgr = FontMgr::CreateDefaultFontMgr();
    ASSERT_TRUE(FontMgr != nullptr);
    FontStyleSet* fontStyleSet = FontMgr->CreateStyleSet(0);
    ASSERT_TRUE(fontStyleSet != nullptr);
}
/**
 * @tc.name:CheckFontValidity001
 * @tc.desc: Test CheckFontValidity
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(FontMgrTest, CheckFontValidity001, TestSize.Level1)
{
    std::shared_ptr<FontMgr> FontMgr = FontMgr::CreateDefaultFontMgr();
    ASSERT_TRUE(FontMgr != nullptr);

    std::vector<std::string> fullnameVec;
    int ret = FontMgr->CheckFontValidity(nullptr, fullnameVec);
    ASSERT_TRUE(ret != 0);

    // case 1: Checks if the font file is valid and returns an array with a fullname
    std::string fontPath = "/system/fonts/HarmonyOS_Sans.ttf";
    std::ifstream testFile1(fontPath);
    if (testFile1.is_open()) {
        testFile1.close();
        ret = FontMgr->CheckFontValidity(fontPath.c_str(), fullnameVec);
        ASSERT_TRUE(ret == 0 && fullnameVec.size() == 1);
    }
    fullnameVec.clear();
    // case 2: Checks if the font file is valid and returns an array with 5 fullnames
    fontPath = "/system/fonts/NotoSerifCJK-Regular.ttc";
    std::ifstream testFile2(fontPath);
    if (testFile2.is_open()) {
        testFile2.close();
        ret = FontMgr->CheckFontValidity(fontPath.c_str(), fullnameVec);
        ASSERT_TRUE(ret == 0 && fullnameVec.size() == 5);
    }
    fullnameVec.clear();
    // case 3: Checks if the font file is invalid and returns an error code
    fontPath = "/system/fonts/error_path.ttf";
    std::ifstream testFile3(fontPath);
    if (testFile3.is_open()) {
        testFile3.close();
    } else {
        ret = FontMgr->CheckFontValidity(fontPath.c_str(), fullnameVec);
        ASSERT_TRUE(ret == 1);
    }
}
/**
 * @tc.name:ParseInstallFont001
 * @tc.desc: Test ParseInstallFont
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(FontMgrTest, ParseInstallFont001, TestSize.Level1)
{
    std::shared_ptr<FontMgr> FontMgr = FontMgr::CreateInstallFontMgr();
    ASSERT_TRUE(FontMgr != nullptr);
    std::string fontPath = "/system/etc/install_fontconfig.json";
    std::ifstream configFile(fontPath);
    if (configFile.is_open()) {
        configFile.close();
        std::vector<std::string> fullPathVec;
        int ret = FontMgr->ParseInstallFontConfig(fontPath, fullPathVec);
        ASSERT_TRUE(ret == 0);
    }
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
