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
#include <hb.h>
#include <include/core/SkTypeface.h>
#include "src/ports/SkFontMgr_custom.h"

#include "measurer_impl.h"
#include "param_test_macros.h"
#include "texgine_exception.h"

struct hb_buffer_t {};
struct hb_unicode_funcs_t {};
struct hb_font_t {};
struct hb_face_t {};

struct MockVars {
    size_t calledHBFontCreate_ = 0;
    std::unique_ptr<hb_buffer_t> retvalBufferCreate_ = std::make_unique<hb_buffer_t>();
    std::unique_ptr<hb_unicode_funcs_t> retvalUnicodeFuncsCreate_ = std::make_unique<hb_unicode_funcs_t>();
    size_t calledUnicodeScript_ = 0;
    std::vector<hb_script_t> retvalUnicodeScript_ = {HB_SCRIPT_INVALID};
    std::unique_ptr<hb_font_t> retvalFontCreate_ = std::make_unique<hb_font_t>();
    std::unique_ptr<hb_face_t> retvalFaceCreate_ = std::make_unique<hb_face_t>();
    std::vector<hb_glyph_info_t> retvalGetGlyphInfo_ = {{}};
    std::vector<hb_glyph_position_t> retvalGetGlyphPosition_ = {{}};
    int retvalGetUnitsPerEm_ = 1;
    bool retvalHBFeatureFromString_ = true;
    bool retvalIsWhitespace = false;
    bool retvalTypefaceHas_ = true;
    size_t retvalGetTableSize_ = 1;
    size_t retvalGetTableData_ = 1;

    std::shared_ptr<Texgine::Typeface> typeface_ =
        std::make_shared<Texgine::Typeface>(sk_make_sp<SkTypeface_Empty>());
} mockvars;

size_t SkTypeface::getTableSize(SkFontTableTag tag) const
{
    return mockvars.retvalGetTableSize_;
}

size_t SkTypeface::getTableData(SkFontTableTag tag, size_t offset, size_t length, void *data) const
{
    return mockvars.retvalGetTableData_;
}

int SkTypeface::getUnitsPerEm() const
{
    return mockvars.retvalGetUnitsPerEm_;
}

hb_blob_t *hb_blob_create(const char *, unsigned int, hb_memory_mode_t, void *, hb_destroy_func_t)
{
    return hb_blob_get_empty();
}

hb_unicode_funcs_t *hb_unicode_funcs_create(hb_unicode_funcs_t *)
{
    return mockvars.retvalUnicodeFuncsCreate_.get();
}

hb_script_t hb_unicode_script(hb_unicode_funcs_t *, hb_codepoint_t)
{
    assert(mockvars.calledUnicodeScript_ < mockvars.retvalUnicodeScript_.size());
    return mockvars.retvalUnicodeScript_[mockvars.calledUnicodeScript_++];
}

hb_bool_t hb_feature_from_string(const char *, int, hb_feature_t *)
{
    return mockvars.retvalHBFeatureFromString_;
}

hb_buffer_t *hb_buffer_create()
{
    return mockvars.retvalBufferCreate_.get();
}

void hb_buffer_destroy(hb_buffer_t *)
{
}

void hb_font_destroy (hb_font_t *)
{
}

void hb_face_destroy (hb_face_t *)
{
}

void hb_buffer_add_utf16(hb_buffer_t *, const uint16_t *, int32_t, uint32_t, int32_t)
{
}

void hb_buffer_set_direction(hb_buffer_t *, hb_direction_t)
{
}

void hb_buffer_set_unicode_funcs(hb_buffer_t *, hb_unicode_funcs_t)
{
}

hb_unicode_funcs_t *hb_icu_get_unicode_funcs()
{
    return nullptr;
}

void hb_buffer_set_script(hb_buffer_t *, hb_script_t)
{
}

void hb_buffer_set_language(hb_buffer_t *, hb_language_t)
{
}

hb_face_t *hb_face_create_for_tables(hb_reference_table_func_t, void *, hb_destroy_func_t)
{
    return mockvars.retvalFaceCreate_.get();
}

hb_font_t *hb_font_create(hb_face_t *face)
{
    mockvars.calledHBFontCreate_++;
    return mockvars.retvalFontCreate_.get();
}

void hb_shape(hb_font_t *, hb_buffer_t *, const hb_feature_t *, unsigned int)
{
}

hb_glyph_info_t *hb_buffer_get_glyph_infos(hb_buffer_t *buffer, unsigned int *length)
{
    *length = mockvars.retvalGetGlyphInfo_.size();
    return mockvars.retvalGetGlyphInfo_.data();
}

hb_glyph_position_t *hb_buffer_get_glyph_positions(hb_buffer_t *buffer, unsigned int *length)
{
    return mockvars.retvalGetGlyphPosition_.data();
}

U_CAPI UBool U_EXPORT2 u_isWhitespace(UChar32 c)
{
    return mockvars.retvalIsWhitespace;
}

namespace Texgine {
std::vector<Boundary> WordBreaker::GetBoundary(const std::vector<uint16_t> &u16str, bool)
{
    return {};
}

bool Typeface::Has(uint32_t ch)
{
    return mockvars.retvalTypefaceHas_;
}

std::shared_ptr<Typeface> FontCollection::GetTypefaceForChar(const uint32_t &ch,
    const std::string &locale, const FontStyles &style) const
{
    return mockvars.typeface_;
}

void InitMockVars(MockVars vars)
{
    mockvars = std::move(vars);
}

class MeasurerImplTest : public testing::Test {
public:
    MeasurerImplTest()
    {
        normalff_.SetFeature("ftag", 1);
    }

    std::vector<uint16_t> text_ = {};
    FontCollection fontCollection_ = std::vector<std::shared_ptr<FontStyleSet>>{};
    FontFeatures emptyff_;
    FontFeatures normalff_;
    CharGroups charGroups_;
};

// 过程测试
// 调用HbFaceReferenceTableSkTypeface函数
TEST_F(MeasurerImplTest, HbFaceReferenceTableSkTypeface)
{
    // path: typeface is nullptr
    EXPECT_EQ(HbFaceReferenceTableSkTypeface({}, {}, nullptr), nullptr);

    // path: tableSize is 0
    InitMockVars({.retvalGetTableSize_ = 0});
    EXPECT_EQ(HbFaceReferenceTableSkTypeface({}, {}, this), nullptr);

    // path: buffer is nullptr
    InitMockVars({.retvalGetTableSize_ = 100000000000});
    EXPECT_EQ(HbFaceReferenceTableSkTypeface({}, {}, this), nullptr);

    // path: tableSize != actualSize
    InitMockVars({.retvalGetTableSize_ = 2, .retvalGetTableData_ = 1});
    EXPECT_EQ(HbFaceReferenceTableSkTypeface({}, {}, this), nullptr);

    // path: normal
    InitMockVars({});
    EXPECT_NE(HbFaceReferenceTableSkTypeface({}, {}, this), nullptr);
}

// 过程测试
// 调用Create创建对象
// 判定不抛出异常
TEST_F(MeasurerImplTest, Create)
{
    EXPECT_NE(MeasurerImpl::Create({}, fontCollection_), nullptr);
}

// 控制shape返回1
TEST_F(MeasurerImplTest, Measure1)
{
    InitMockVars({.retvalBufferCreate_ = nullptr});
    text_ = {1};
    MeasurerImpl mi(text_, fontCollection_);
    mi.SetRange(0, 1);
    mi.SetFontFeatures(emptyff_);

    EXPECT_EQ(mi.Measure(charGroups_), 1);
    EXPECT_EQ(mockvars.calledHBFontCreate_, 0);
}

// 调用两次Measure方法，走找到缓存路径
TEST_F(MeasurerImplTest, Measure2)
{
    InitMockVars({});
    text_ = {2};
    MeasurerImpl mi(text_, fontCollection_);
    mi.SetRange(0, 1);
    mi.SetFontFeatures(emptyff_);

    EXPECT_EQ(mi.Measure(charGroups_), 0);
    EXPECT_EQ(mockvars.calledHBFontCreate_, 1);
    EXPECT_EQ(mi.Measure(charGroups_), 0);
    EXPECT_EQ(mockvars.calledHBFontCreate_, 1);
}

// 创建对象，传入非空FontFeatures
TEST_F(MeasurerImplTest, Measure3)
{
    InitMockVars({});
    text_ = {3};
    MeasurerImpl mi(text_, fontCollection_);
    mi.SetRange(0, 1);
    mi.SetFontFeatures(normalff_);

    EXPECT_EQ(mi.Measure(charGroups_), 0);
    EXPECT_EQ(mockvars.calledHBFontCreate_, 1);
}

// 过程测试
// 创建对象，控制GetTypefaceForChar返回nullptr
// 控制while循环1次
// 判定runs.size()为1
TEST_F(MeasurerImplTest, SeekTypeface1)
{
    InitMockVars({.typeface_ = nullptr});
    text_ = {'a'};
    MeasurerImpl mi(text_, fontCollection_);
    mi.SetRange(0, 1);

    std::list<struct MeasuringRun> runs;
    mi.SeekTypeface(runs);
    EXPECT_EQ(runs.size(), 1);
    EXPECT_EQ(runs.front().typeface, nullptr);
}

// 过程测试
// 创建对象，控制GetTypefaceForChar返回非空指针
// 控制while循环2次
// 判定runs.size()为1
TEST_F(MeasurerImplTest, SeekTypeface2)
{
    InitMockVars({});
    text_ = {'a', 'b'};
    MeasurerImpl mi(text_, fontCollection_);
    mi.SetRange(0, 2);

    std::list<struct MeasuringRun> runs;
    mi.SeekTypeface(runs);
    EXPECT_EQ(runs.size(), 1);
    EXPECT_EQ(runs.front().typeface, mockvars.typeface_);
}

// 过程测试
// 创建对象，控制GetTypefaceForChar返回非空指针
// 控制Has返回false，控制while循环2次
// 判定runs.size()为2
TEST_F(MeasurerImplTest, SeekTypeface3)
{
    InitMockVars({.retvalTypefaceHas_ = false});
    text_ = {'a', 'b'};
    MeasurerImpl mi(text_, fontCollection_);
    mi.SetRange(0, 2);
    std::list<struct MeasuringRun> runs;
    mi.SeekTypeface(runs);
    EXPECT_EQ(runs.size(), 2);
    EXPECT_EQ(runs.front().typeface, mockvars.typeface_);
}

// 创建对象，控制GetTypefaceForChar返回非空指针
// 控制Has返回false，控制while循环2次，第二次因text_.size()抛出异常
// 判定runs.size()为2
TEST_F(MeasurerImplTest, SeekTypeface4)
{
    InitMockVars({.retvalTypefaceHas_ = false});
    text_ = {'a'};
    MeasurerImpl mi(text_, fontCollection_);
    mi.SetRange(0, 2);
    std::list<struct MeasuringRun> runs;
    ASSERT_EXCEPTION(ExceptionType::ErrorStatus, mi.SeekTypeface(runs));
}

// 过程测试
// 创建对象，控制hb_unicode_funcs_create返回空指针
// 调用SeekScript方法，走第一个if路径
// 判定抛出APIFailed异常
TEST_F(MeasurerImplTest, SeekScript1)
{
    InitMockVars({.retvalUnicodeFuncsCreate_ = nullptr});
    MeasurerImpl mi(text_, fontCollection_);
    std::list<struct MeasuringRun> runs;
    ASSERT_EXCEPTION(ExceptionType::APIFailed, mi.SeekScript(runs));
}

// 过程测试
// 创建对象，控制while循环一次
// 调用SeekScript方法，走第二个if路径
// 判定runs.size()为1
TEST_F(MeasurerImplTest, SeekScript2)
{
    InitMockVars({.retvalUnicodeScript_ = {HB_SCRIPT_HAN}});
    MeasurerImpl mi(text_, fontCollection_);

    std::list<struct MeasuringRun> runs;
    runs.push_back({.start = 0, .end = 1});
    ASSERT_EXCEPTION(ExceptionType::ErrorStatus, mi.SeekScript(runs));
}

// 过程测试
// 创建对象，控制while循环一次
// 调用SeekScript方法，走第三个第四个if路径
// 判定runs.size()为1
TEST_F(MeasurerImplTest, SeekScript3)
{
    InitMockVars({.retvalUnicodeScript_ = {HB_SCRIPT_HAN}});
    text_ = {'a'};
    MeasurerImpl mi(text_, fontCollection_);

    std::list<struct MeasuringRun> runs;
    runs.push_back({.start = 0, .end = 1});
    EXPECT_NE(runs.front().script, HB_SCRIPT_HAN);
    mi.SeekScript(runs);
    EXPECT_EQ(runs.size(), 1);
    EXPECT_EQ(runs.front().script, HB_SCRIPT_HAN);
}

// 过程测试
// 创建对象，控制while循环一次
// 调用两次SeekScript方法，走第五个if路径
// 判定runs.size()为1
TEST_F(MeasurerImplTest, SeekScript4)
{
    InitMockVars({.retvalUnicodeScript_ = {HB_SCRIPT_INVALID}});
    text_ = {'a'};
    MeasurerImpl mi(text_, fontCollection_);

    std::list<struct MeasuringRun> runs;
    runs.push_back({.start = 0, .end = 1, .script = HB_SCRIPT_INHERITED});
    mi.SeekScript(runs);
    EXPECT_EQ(runs.size(), 1);
    EXPECT_EQ(runs.front().script, HB_SCRIPT_INVALID);

    InitMockVars({.retvalUnicodeScript_ = {HB_SCRIPT_INVALID}});
    runs.clear();
    runs.push_back({.start = 0, .end = 1, .script = HB_SCRIPT_COMMON});
    mi.SeekScript(runs);
    EXPECT_EQ(runs.size(), 1);
    EXPECT_EQ(runs.front().script, HB_SCRIPT_INVALID);
}

// 过程测试
// 创建对象，控制while循环一次
// 调用两次SeekScript方法，走第六个if路径
// 判定runs.size()为1
TEST_F(MeasurerImplTest, SeekScript5)
{
    InitMockVars({.retvalUnicodeScript_ = {HB_SCRIPT_HAN}});
    text_ = {'a'};
    MeasurerImpl mi(text_, fontCollection_);

    std::list<struct MeasuringRun> runs;
    runs.push_back({.start = 0, .end = 1, .script = HB_SCRIPT_INHERITED});
    mi.SeekScript(runs);
    EXPECT_EQ(runs.size(), 1);
    EXPECT_EQ(runs.front().script, HB_SCRIPT_HAN);

    InitMockVars({.retvalUnicodeScript_ = {HB_SCRIPT_HAN}});
    runs.clear();
    runs.push_back({.start = 0, .end = 1, .script = HB_SCRIPT_COMMON});
    mi.SeekScript(runs);
    EXPECT_EQ(runs.size(), 1);
    EXPECT_EQ(runs.front().script, HB_SCRIPT_HAN);
}

// 过程测试
// 创建对象，控制while循环一次
// 调用SeekScript方法，走第七个if路径
TEST_F(MeasurerImplTest, SeekScript6)
{
    InitMockVars({.retvalUnicodeScript_ = {HB_SCRIPT_COMMON}});
    text_ = {'a'};
    MeasurerImpl mi(text_, fontCollection_);

    std::list<struct MeasuringRun> runs;
    runs.push_back({.start = 0, .end = 1, .script = HB_SCRIPT_HAN});
    mi.SeekScript(runs);
    EXPECT_EQ(runs.size(), 1);
    EXPECT_EQ(runs.front().script, HB_SCRIPT_HAN);
}

// 过程测试
// 创建对象，控制while循环一次
// 调用SeekScript方法，走第八个if路径
// 判定runs.size()为2
TEST_F(MeasurerImplTest, SeekScript7)
{
    InitMockVars({.retvalUnicodeScript_ = {HB_SCRIPT_COMMON, HB_SCRIPT_LATIN, HB_SCRIPT_LATIN}});
    text_ = {'a', 'b'};
    MeasurerImpl mi(text_, fontCollection_);

    std::list<struct MeasuringRun> runs;
    runs.push_back({.start = 0, .end = 2, .script = HB_SCRIPT_HAN});
    mi.SeekScript(runs);
    EXPECT_EQ(runs.size(), 2);
    EXPECT_EQ(runs.front().script, HB_SCRIPT_HAN);
    auto it = runs.begin();
    it++;
    EXPECT_EQ(it->script, HB_SCRIPT_LATIN);
}

// 创建对象，控制while循环一次
// 调用SeekScript方法，走第九个if路径，触发异常
TEST_F(MeasurerImplTest, SeekScript8)
{
    InitMockVars({.retvalUnicodeScript_ = {HB_SCRIPT_LATIN}});
    text_ = {'a'};
    MeasurerImpl mi(text_, fontCollection_);

    std::list<struct MeasuringRun> runs;
    runs.push_back({.start = 0, .end = 1, .script = HB_SCRIPT_HAN});
    ASSERT_EXCEPTION(ExceptionType::ErrorStatus, mi.SeekScript(runs));
}

// 过程测试
// 调用Shape方法，控制不进入循环
// 判定返回0
TEST_F(MeasurerImplTest, Shape1)
{
    InitMockVars({});
    MeasurerImpl mi(text_, fontCollection_);

    std::list<struct MeasuringRun> runs;
    EXPECT_EQ(mi.Shape(charGroups_, runs, {}), 0);
    EXPECT_EQ(mockvars.calledHBFontCreate_, 0);
}

// 调用Shape方法，触发错误区间异常
TEST_F(MeasurerImplTest, Shape2)
{
    InitMockVars({});
    MeasurerImpl mi(text_, fontCollection_);

    std::list<struct MeasuringRun> runs;
    runs.push_back({.start = 0, .end = 0});
    ASSERT_EXCEPTION(ExceptionType::ErrorStatus, mi.Shape(charGroups_, runs, {}));
}

// 过程测试
// 调用Shape方法，控制run.typeface为空
// 判定返回1
TEST_F(MeasurerImplTest, Shape3)
{
    InitMockVars({});
    MeasurerImpl mi(text_, fontCollection_);

    std::list<struct MeasuringRun> runs;
    runs.push_back({.start = 0, .end = 1, .typeface = nullptr});
    EXPECT_EQ(mi.Shape(charGroups_, runs, {}), 1);
    EXPECT_EQ(mockvars.calledHBFontCreate_, 0);
}

// 过程测试
// 调用Shape方法，控制hb_buffer_create返回空指针
// 判定返回1
TEST_F(MeasurerImplTest, Shape4)
{
    InitMockVars({.retvalBufferCreate_ = nullptr});
    MeasurerImpl mi(text_, fontCollection_);

    std::list<struct MeasuringRun> runs;
    runs.push_back({.start = 0, .end = 1, .typeface = mockvars.typeface_});
    EXPECT_EQ(mi.Shape(charGroups_, runs, {}), 1);
    EXPECT_EQ(mockvars.calledHBFontCreate_, 0);
}

// 过程测试
// 调用Shape方法，控制hb_face_create_for_tables返回空指针
// 判定返回1
TEST_F(MeasurerImplTest, Shape5)
{
    InitMockVars({.retvalFaceCreate_ = nullptr});
    MeasurerImpl mi(text_, fontCollection_);

    std::list<struct MeasuringRun> runs;
    runs.push_back({.start = 0, .end = 1, .typeface = mockvars.typeface_});
    EXPECT_EQ(mi.Shape(charGroups_, runs, {}), 1);
    EXPECT_EQ(mockvars.calledHBFontCreate_, 0);
}

// 过程测试
// 调用Shape方法，控制hb_font_create返回空指针
// 判定返回1
TEST_F(MeasurerImplTest, Shape6)
{
    InitMockVars({.retvalFontCreate_ = nullptr});
    MeasurerImpl mi(text_, fontCollection_);

    std::list<struct MeasuringRun> runs;
    runs.push_back({.start = 0, .end = 1, .typeface = mockvars.typeface_});
    EXPECT_EQ(mi.Shape(charGroups_, runs, {}), 1);
    EXPECT_EQ(mockvars.calledHBFontCreate_, 1);
}

// 过程测试
// 调用Shape方法，控制hb_buffer_get_glyph_infos返回空指针
// 判定返回1
TEST_F(MeasurerImplTest, Shape7)
{
    InitMockVars({.retvalGetGlyphInfo_ = {}});
    MeasurerImpl mi(text_, fontCollection_);
    mi.SetFontFeatures(normalff_);

    std::list<struct MeasuringRun> runs;
    runs.push_back({.start = 0, .end = 1, .typeface = mockvars.typeface_});
    EXPECT_EQ(mi.Shape(charGroups_, runs, {}), 1);
    EXPECT_EQ(mockvars.calledHBFontCreate_, 1);
}

// 过程测试
// 调用Shape方法，控制hb_buffer_get_glyph_positions返回空指针
// 判定返回1
TEST_F(MeasurerImplTest, Shape8)
{
    InitMockVars({.retvalGetGlyphPosition_ = {}});
    MeasurerImpl mi(text_, fontCollection_);
    mi.SetFontFeatures(normalff_);

    std::list<struct MeasuringRun> runs;
    runs.push_back({.start = 0, .end = 1, .typeface = mockvars.typeface_});
    EXPECT_EQ(mi.Shape(charGroups_, runs, {}), 1);
    EXPECT_EQ(mockvars.calledHBFontCreate_, 1);
}

// 过程测试
// 调用Shape方法，控制getUnitsPerEm返回0
// 判定返回1
TEST_F(MeasurerImplTest, Shape9)
{
    InitMockVars({.retvalGetUnitsPerEm_ = 0});
    MeasurerImpl mi(text_, fontCollection_);
    mi.SetFontFeatures(normalff_);

    std::list<struct MeasuringRun> runs;
    runs.push_back({.start = 0, .end = 1, .typeface = mockvars.typeface_});
    EXPECT_EQ(mi.Shape(charGroups_, runs, {}), 1);
    EXPECT_EQ(mockvars.calledHBFontCreate_, 1);
}

// 过程测试
// 调用Shape方法,走for循环
// 控制u_isWhitespace返回true
// 判定返回0，charGroups.GetSize()为1
TEST_F(MeasurerImplTest, Shape10)
{
    InitMockVars({.retvalIsWhitespace = true});
    mockvars.retvalGetGlyphPosition_[0].x_advance = 10;
    text_ = {'a'};
    MeasurerImpl mi(text_, fontCollection_);
    mi.SetFontFeatures(normalff_);

    std::list<struct MeasuringRun> runs;
    runs.push_back({.start = 0, .end = 1, .typeface = mockvars.typeface_});
    EXPECT_EQ(mi.Shape(charGroups_, runs, {}), 0);
    EXPECT_EQ(charGroups_.GetSize(), 1);
    EXPECT_GT(charGroups_.Get(0).invisibleWidth_, charGroups_.Get(0).visibleWidth_);
}

// 过程测试
// 调用Shape方法，设置RTL为true，走if路径
// 判定返回0，charGroups.GetSize()为1
TEST_F(MeasurerImplTest, Shape11)
{
    InitMockVars({});
    text_ = {'a', 'b'};
    MeasurerImpl mi(text_, fontCollection_);
    mi.SetFontFeatures(normalff_);
    mi.SetRTL(true);

    std::list<struct MeasuringRun> runs;
    runs.push_back({.start = 0, .end = 1, .typeface = mockvars.typeface_});
    runs.push_back({.start = 1, .end = 2, .typeface = mockvars.typeface_});
    EXPECT_EQ(mi.Shape(charGroups_, runs, {}), 0);
    EXPECT_EQ(charGroups_.GetSize(), 2);
}

// spacing
TEST_F(MeasurerImplTest, Shape12)
{
    InitMockVars({.retvalGetGlyphInfo_ = {{.cluster = 0}, {.cluster = 1}},
                  .retvalGetGlyphPosition_ = {{}, {}}});
    text_ = {'a', 'b'};
    MeasurerImpl mi(text_, fontCollection_);
    mi.SetFontFeatures(normalff_);
    mi.SetSpacing(5, 10);

    std::list<struct MeasuringRun> runs;
    runs.push_back({.start = 0, .end = 2, .typeface = mockvars.typeface_});
    std::vector<Boundary> boundaries;
    boundaries.emplace_back(0, 2);
    EXPECT_EQ(mi.Shape(charGroups_, runs, boundaries), 0);
    EXPECT_EQ(charGroups_.GetSize(), 2);
    EXPECT_EQ(charGroups_.Get(0).invisibleWidth_, 5);
    EXPECT_EQ(charGroups_.Get(1).invisibleWidth_, 15);
}

// 过程测试
// 调用GenerateHBFeatures方法，第二个参数为空指针
// 判定features.size()为0
TEST_F(MeasurerImplTest, GenerateHBFeatures1)
{
    InitMockVars({});
    MeasurerImpl mi(text_, fontCollection_);
    std::vector<hb_feature_t> features;
    mi.GenerateHBFeatures(features, nullptr);
    EXPECT_EQ(features.size(), 0);
}

// 过程测试
// 调用GenerateHBFeatures方法，传入空FontFeatures
// 判定features.size()为0
TEST_F(MeasurerImplTest, GenerateHBFeatures2)
{
    InitMockVars({});
    MeasurerImpl mi(text_, fontCollection_);
    std::vector<hb_feature_t> features;
    mi.GenerateHBFeatures(features, &emptyff_);
    EXPECT_EQ(features.size(), 0);
}

// 过程测试
// 调用GenerateHBFeatures方法，传入非空FontFeatures
// 控制hb_feature_from_string返回false
// 判定features.size()为0，fontFeatures.size()为1
TEST_F(MeasurerImplTest, GenerateHBFeatures3)
{
    InitMockVars({.retvalHBFeatureFromString_ = false});
    MeasurerImpl mi(text_, fontCollection_);
    std::vector<hb_feature_t> features;
    mi.GenerateHBFeatures(features, &normalff_);
    EXPECT_EQ(features.size(), 0);
    EXPECT_EQ(normalff_.GetFeatures().size(), 1);
}

// 过程测试
// 调用GenerateHBFeatures方法，传入非空FontFeatures
// 判定features.size()为1，fontFeatures.size()为1
TEST_F(MeasurerImplTest, GenerateHBFeatures4)
{
    InitMockVars({});
    MeasurerImpl mi(text_, fontCollection_);
    std::vector<hb_feature_t> features;
    mi.GenerateHBFeatures(features, &normalff_);
    EXPECT_EQ(features.size(), 1);
    EXPECT_EQ(normalff_.GetFeatures().size(), 1);
}
} // namespace Texgine
