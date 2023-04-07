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

#include "measurer_impl.h"
#include "param_test_macros.h"
#include "texgine_exception.h"

using namespace testing;
using namespace testing::ext;

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
        std::make_shared<Texgine::Typeface>(std::make_shared<Texgine::TexgineTypeface>());
} measurerMockvars;

std::list<Texgine::MeasuringRun> mRuns = {};

hb_blob_t *hb_blob_create(const char *, unsigned int, hb_memory_mode_t, void *, hb_destroy_func_t)
{
    return hb_blob_get_empty();
}

hb_unicode_funcs_t *hb_unicode_funcs_create(hb_unicode_funcs_t *)
{
    return measurerMockvars.retvalUnicodeFuncsCreate_.get();
}

hb_script_t hb_unicode_script(hb_unicode_funcs_t *, hb_codepoint_t)
{
    assert(measurerMockvars.retvalUnicodeScript_.size() > measurerMockvars.calledUnicodeScript_);
    return measurerMockvars.retvalUnicodeScript_[measurerMockvars.calledUnicodeScript_++];
}

hb_bool_t hb_feature_from_string(const char *, int, hb_feature_t *)
{
    return measurerMockvars.retvalHBFeatureFromString_;
}

hb_buffer_t *hb_buffer_create()
{
    return measurerMockvars.retvalBufferCreate_.get();
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

void hb_buffer_set_unicode_funcs(hb_buffer_t *, hb_unicode_funcs_t*)
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
    return measurerMockvars.retvalFaceCreate_.get();
}

hb_font_t *hb_font_create(hb_face_t *face)
{
    measurerMockvars.calledHBFontCreate_++;
    return measurerMockvars.retvalFontCreate_.get();
}

void hb_shape(hb_font_t *, hb_buffer_t *, const hb_feature_t *, unsigned int)
{
}

hb_glyph_info_t *hb_buffer_get_glyph_infos(hb_buffer_t *buffer, unsigned int *length)
{
    *length = measurerMockvars.retvalGetGlyphInfo_.size();
    return measurerMockvars.retvalGetGlyphInfo_.data();
}

hb_glyph_position_t *hb_buffer_get_glyph_positions(hb_buffer_t *buffer, unsigned int *length)
{
    return measurerMockvars.retvalGetGlyphPosition_.data();
}

U_CAPI UBool U_EXPORT2 u_isWhitespace(UChar32 c)
{
    return measurerMockvars.retvalIsWhitespace;
}

namespace Texgine {
size_t TexgineTypeface::GetTableSize(uint32_t tag)
{
    return measurerMockvars.retvalGetTableSize_;
}

size_t TexgineTypeface::GetTableData(uint32_t tag, size_t offset, size_t length, void *data)
{
    return measurerMockvars.retvalGetTableData_;
}

int TexgineTypeface::GetUnitsPerEm()
{
    return measurerMockvars.retvalGetUnitsPerEm_;
}

std::vector<Boundary> WordBreaker::GetBoundary(const std::vector<uint16_t> &u16str, bool)
{
    return {};
}

bool Typeface::Has(uint32_t ch)
{
    return measurerMockvars.retvalTypefaceHas_;
}

std::shared_ptr<Typeface> FontCollection::GetTypefaceForChar(const uint32_t &ch,
    const FontStyles &style, const std::string &script, const std::string &locale) const
{
    return measurerMockvars.typeface_;
}

void InitMiMockVars(MockVars vars, std::list<struct MeasuringRun> runs)
{
    measurerMockvars = std::move(vars);
    mRuns.clear();
    mRuns.insert(mRuns.begin(), runs.begin(), runs.end());
}

class MeasurerImplTest : public testing::Test {
public:
    MeasurerImplTest()
    {
        normalff_.SetFeature("ftag", 1);
    }

    std::vector<uint16_t> text_ = {};
    FontCollection fontCollection_ = std::vector<std::shared_ptr<VariantFontStyleSet>>{};
    FontFeatures emptyff_;
    FontFeatures normalff_;
    CharGroups charGroups_;
};

// 过程测试
// 调用HbFaceReferenceTableTypeface函数
HWTEST_F(MeasurerImplTest, HbFaceReferenceTableTypeface, TestSize.Level1)
{
    // path: typeface is nullptr
    EXPECT_EQ(HbFaceReferenceTableTypeface({}, {}, nullptr), nullptr);

    // path: tableSize is 0
    InitMiMockVars({.retvalGetTableSize_ = 0}, {});
    EXPECT_EQ(HbFaceReferenceTableTypeface({}, {}, this), nullptr);

    // path: buffer is nullptr
    InitMiMockVars({.retvalGetTableSize_ = static_cast<size_t>(1000)}, {});
    EXPECT_EQ(HbFaceReferenceTableTypeface({}, {}, this), nullptr);

    // path: tableSize != actualSize
    InitMiMockVars({.retvalGetTableSize_ = 2, .retvalGetTableData_ = 1}, {});
    EXPECT_EQ(HbFaceReferenceTableTypeface({}, {}, this), nullptr);

    // path: normal
    InitMiMockVars({}, {});
    EXPECT_NE(HbFaceReferenceTableTypeface({}, {}, this), nullptr);
}

// 过程测试
// 调用Create创建对象
// 判定不抛出异常
HWTEST_F(MeasurerImplTest, Create, TestSize.Level1)
{
    EXPECT_NE(MeasurerImpl::Create({}, fontCollection_), nullptr);
}

// 控制shape返回1
HWTEST_F(MeasurerImplTest, Measure1, TestSize.Level1)
{
    InitMiMockVars({.retvalBufferCreate_ = nullptr}, {});
    text_ = {1};
    MeasurerImpl mi(text_, fontCollection_);
    mi.SetRange(0, 1);
    mi.SetFontFeatures(emptyff_);

    EXPECT_EQ(mi.Measure(charGroups_), 1);
    EXPECT_EQ(measurerMockvars.calledHBFontCreate_, 0);
}

// 调用两次Measure方法，走找到缓存路径
HWTEST_F(MeasurerImplTest, Measure2, TestSize.Level1)
{
    InitMiMockVars({}, {});
    text_ = {2};
    MeasurerImpl mi(text_, fontCollection_);
    mi.SetRange(0, 1);
    mi.SetFontFeatures(emptyff_);

    EXPECT_EQ(mi.Measure(charGroups_), 0);
    EXPECT_EQ(measurerMockvars.calledHBFontCreate_, 1);
    EXPECT_EQ(mi.Measure(charGroups_), 0);
    EXPECT_EQ(measurerMockvars.calledHBFontCreate_, 1);
}

// 创建对象，传入非空FontFeatures
HWTEST_F(MeasurerImplTest, Measure3, TestSize.Level1)
{
    InitMiMockVars({}, {});
    text_ = {3};
    MeasurerImpl mi(text_, fontCollection_);
    mi.SetRange(0, 1);
    mi.SetFontFeatures(normalff_);

    EXPECT_EQ(mi.Measure(charGroups_), 0);
    EXPECT_EQ(measurerMockvars.calledHBFontCreate_, 1);
}

// 过程测试
// 创建对象，控制GetTypefaceForChar返回nullptr
// 控制while循环1次
// 判定runs.size()为1
HWTEST_F(MeasurerImplTest, SeekTypeface1, TestSize.Level1)
{
    InitMiMockVars({.typeface_ = nullptr}, {{.start = 0, .end = 1}});
    text_ = {'a'};
    MeasurerImpl mi(text_, fontCollection_);
    mi.SetRange(0, 1);

    std::list<struct MeasuringRun> runs;
    mi.SeekTypeface(mRuns);
    EXPECT_EQ(mRuns.size(), 1);
    EXPECT_EQ(mRuns.front().typeface, nullptr);
}

// 过程测试
// 创建对象，控制GetTypefaceForChar返回非空指针
// 控制while循环2次
// 判定runs.size()为1
HWTEST_F(MeasurerImplTest, SeekTypeface2, TestSize.Level1)
{
    InitMiMockVars({}, {{.start = 0, .end = 2},});
    text_ = {'a', 'b'};
    MeasurerImpl mi(text_, fontCollection_);
    mi.SetRange(0, 2);

    mi.SeekTypeface(mRuns);
    EXPECT_EQ(mRuns.size(), 1);
    EXPECT_EQ(mRuns.front().typeface, measurerMockvars.typeface_);
}

// 过程测试
// 创建对象，控制GetTypefaceForChar返回非空指针
// 控制Has返回false，控制while循环2次
// 判定runs.size()为2
HWTEST_F(MeasurerImplTest, SeekTypeface3, TestSize.Level1)
{
    InitMiMockVars({.retvalTypefaceHas_ = false}, {{.start = 0, .end = 2},});
    text_ = {'a', 'b'};
    MeasurerImpl mi(text_, fontCollection_);
    mi.SetRange(0, 2);
    mi.SeekTypeface(mRuns);
    EXPECT_EQ(mRuns.size(), 2);
    EXPECT_EQ(mRuns.front().typeface, measurerMockvars.typeface_);
}

// 创建对象，控制GetTypefaceForChar返回非空指针
// 控制Has返回false，控制while循环2次，第二次因text_.size()抛出异常
// 判定runs.size()为2
HWTEST_F(MeasurerImplTest, SeekTypeface4, TestSize.Level1)
{
    InitMiMockVars({.retvalTypefaceHas_ = false}, {{.start = 0, .end = 2},});
    text_ = {'a'};
    MeasurerImpl mi(text_, fontCollection_);
    mi.SetRange(0, 2);
    ASSERT_EXCEPTION(ExceptionType::ErrorStatus, mi.SeekTypeface(mRuns));
}

// 过程测试
// 创建对象，控制hb_unicode_funcs_create返回空指针
// 调用SeekScript方法，走第一个if路径
// 判定抛出APIFailed异常
HWTEST_F(MeasurerImplTest, SeekScript1, TestSize.Level1)
{
    InitMiMockVars({.retvalUnicodeFuncsCreate_ = nullptr}, {});
    MeasurerImpl mi(text_, fontCollection_);
    ASSERT_EXCEPTION(ExceptionType::APIFailed, mi.SeekScript(mRuns));
}

// 过程测试
// 创建对象，控制while循环一次
// 调用SeekScript方法，走第二个if路径
// 判定runs.size()为1
HWTEST_F(MeasurerImplTest, SeekScript2, TestSize.Level1)
{
    InitMiMockVars({.retvalUnicodeScript_ = {HB_SCRIPT_HAN}}, {{.start = 0, .end = 1},});
    MeasurerImpl mi(text_, fontCollection_);

    ASSERT_EXCEPTION(ExceptionType::ErrorStatus, mi.SeekScript(mRuns));
}

// 过程测试
// 创建对象，控制while循环一次
// 调用SeekScript方法，走第三个第四个if路径
// 判定runs.size()为1
HWTEST_F(MeasurerImplTest, SeekScript3, TestSize.Level1)
{
    InitMiMockVars({.retvalUnicodeScript_ = {HB_SCRIPT_HAN}}, {});
    text_ = {'a'};
    MeasurerImpl mi(text_, fontCollection_);
    mi.SetRange(0, 1);

    EXPECT_NE(mRuns.front().script, HB_SCRIPT_HAN);
    mi.SeekScript(mRuns);
    EXPECT_EQ(mRuns.size(), 1);
    EXPECT_EQ(mRuns.front().script, HB_SCRIPT_HAN);
}

// 过程测试
// 创建对象，控制while循环一次
// 调用两次SeekScript方法，走第六个if路径
// 判定runs.size()为1
HWTEST_F(MeasurerImplTest, SeekScript4, TestSize.Level1)
{
    InitMiMockVars({.retvalUnicodeScript_ = {HB_SCRIPT_HAN, HB_SCRIPT_INHERITED}}, {});
    text_ = {'a', 'b'};
    MeasurerImpl mi(text_, fontCollection_);
    mi.SetRange(0, 2);

    mi.SeekScript(mRuns);
    EXPECT_EQ(mRuns.size(), 1);
    EXPECT_EQ(mRuns.front().script, HB_SCRIPT_HAN);

    InitMiMockVars({.retvalUnicodeScript_ = {HB_SCRIPT_HAN, HB_SCRIPT_COMMON}}, {});
    mi.SeekScript(mRuns);
    EXPECT_EQ(mRuns.size(), 1);
    EXPECT_EQ(mRuns.front().script, HB_SCRIPT_HAN);
}

// 创建对象，控制while循环一次
// 调用SeekScript方法，走第九个if路径，触发异常
HWTEST_F(MeasurerImplTest, SeekScript5, TestSize.Level1)
{
    InitMiMockVars({.retvalUnicodeScript_ = {HB_SCRIPT_LATIN}}, {});
    text_ = {'a'};
    MeasurerImpl mi(text_, fontCollection_);

    std::list<struct MeasuringRun> runs;
    runs.push_back({.start = 0, .end = 1, .script = HB_SCRIPT_HAN});
    ASSERT_EXCEPTION(ExceptionType::ErrorStatus, mi.SeekScript(runs));
}

// 过程测试
// 调用Shape方法，控制不进入循环
// 判定返回0
HWTEST_F(MeasurerImplTest, Shape1, TestSize.Level1)
{
    InitMiMockVars({}, {});
    MeasurerImpl mi(text_, fontCollection_);

    std::list<struct MeasuringRun> runs;
    EXPECT_EQ(mi.Shape(charGroups_, runs, {}), 0);
    EXPECT_EQ(measurerMockvars.calledHBFontCreate_, 0);
}

// 调用Shape方法，触发错误区间异常
HWTEST_F(MeasurerImplTest, Shape2, TestSize.Level1)
{
    InitMiMockVars({}, {});
    MeasurerImpl mi(text_, fontCollection_);

    std::list<struct MeasuringRun> runs;
    runs.push_back({.start = 0, .end = 0});
    ASSERT_EXCEPTION(ExceptionType::ErrorStatus, mi.Shape(charGroups_, runs, {}));
}

// 过程测试
// 调用Shape方法，控制run.typeface为空
// 判定返回1
HWTEST_F(MeasurerImplTest, Shape3, TestSize.Level1)
{
    InitMiMockVars({}, {});
    MeasurerImpl mi(text_, fontCollection_);

    std::list<struct MeasuringRun> runs;
    runs.push_back({.start = 0, .end = 1, .typeface = nullptr});
    EXPECT_EQ(mi.Shape(charGroups_, runs, {}), 1);
    EXPECT_EQ(measurerMockvars.calledHBFontCreate_, 0);
}

// 过程测试
// 调用Shape方法，控制hb_buffer_create返回空指针
// 判定返回1
HWTEST_F(MeasurerImplTest, Shape4, TestSize.Level1)
{
    InitMiMockVars({.retvalBufferCreate_ = nullptr}, {});
    MeasurerImpl mi(text_, fontCollection_);

    std::list<struct MeasuringRun> runs;
    runs.push_back({.start = 0, .end = 1, .typeface = measurerMockvars.typeface_});
    EXPECT_EQ(mi.Shape(charGroups_, runs, {}), 1);
    EXPECT_EQ(measurerMockvars.calledHBFontCreate_, 0);
}

// 过程测试
// 调用Shape方法，控制hb_face_create_for_tables返回空指针
// 判定返回1
HWTEST_F(MeasurerImplTest, Shape5, TestSize.Level1)
{
    InitMiMockVars({.retvalFaceCreate_ = nullptr}, {});
    MeasurerImpl mi(text_, fontCollection_);

    std::list<struct MeasuringRun> runs;
    runs.push_back({.start = 0, .end = 1, .typeface = measurerMockvars.typeface_});
    EXPECT_EQ(mi.Shape(charGroups_, runs, {}), 1);
    EXPECT_EQ(measurerMockvars.calledHBFontCreate_, 0);
}

// 过程测试
// 调用Shape方法，控制hb_font_create返回空指针
// 判定返回1
HWTEST_F(MeasurerImplTest, Shape6, TestSize.Level1)
{
    InitMiMockVars({.retvalFontCreate_ = nullptr}, {});
    MeasurerImpl mi(text_, fontCollection_);

    std::list<struct MeasuringRun> runs;
    runs.push_back({.start = 0, .end = 1, .typeface = measurerMockvars.typeface_});
    EXPECT_EQ(mi.Shape(charGroups_, runs, {}), 1);
    EXPECT_EQ(measurerMockvars.calledHBFontCreate_, 1);
}

// 过程测试
// 调用Shape方法，控制hb_buffer_get_glyph_infos返回空指针
// 判定返回1
HWTEST_F(MeasurerImplTest, Shape7, TestSize.Level1)
{
    InitMiMockVars({.retvalGetGlyphInfo_ = {}}, {});
    MeasurerImpl mi(text_, fontCollection_);
    mi.SetFontFeatures(normalff_);

    std::list<struct MeasuringRun> runs;
    runs.push_back({.start = 0, .end = 1, .typeface = measurerMockvars.typeface_});
    EXPECT_EQ(mi.Shape(charGroups_, runs, {}), 1);
    EXPECT_EQ(measurerMockvars.calledHBFontCreate_, 1);
}

// 过程测试
// 调用Shape方法，控制hb_buffer_get_glyph_positions返回空指针
// 判定返回1
HWTEST_F(MeasurerImplTest, Shape8, TestSize.Level1)
{
    InitMiMockVars({.retvalGetGlyphPosition_ = {}}, {});
    MeasurerImpl mi(text_, fontCollection_);
    mi.SetFontFeatures(normalff_);

    std::list<struct MeasuringRun> runs;
    runs.push_back({.start = 0, .end = 1, .typeface = measurerMockvars.typeface_});
    EXPECT_EQ(mi.Shape(charGroups_, runs, {}), 1);
    EXPECT_EQ(measurerMockvars.calledHBFontCreate_, 1);
}

// 过程测试
// 调用Shape方法，控制getUnitsPerEm返回0
// 判定返回1
HWTEST_F(MeasurerImplTest, Shape9, TestSize.Level1)
{
    InitMiMockVars({.retvalGetUnitsPerEm_ = 0}, {});
    MeasurerImpl mi(text_, fontCollection_);
    mi.SetFontFeatures(normalff_);

    std::list<struct MeasuringRun> runs;
    runs.push_back({.start = 0, .end = 1, .typeface = measurerMockvars.typeface_});
    EXPECT_EQ(mi.Shape(charGroups_, runs, {}), 1);
    EXPECT_EQ(measurerMockvars.calledHBFontCreate_, 1);
}

// 过程测试
// 调用Shape方法,走for循环
// 控制u_isWhitespace返回true
// 判定返回0，charGroups.GetSize()为1
HWTEST_F(MeasurerImplTest, Shape10, TestSize.Level1)
{
    InitMiMockVars({.retvalIsWhitespace = true}, {});
    measurerMockvars.retvalGetGlyphPosition_[0].x_advance = 10;
    text_ = {'a'};
    MeasurerImpl mi(text_, fontCollection_);
    mi.SetFontFeatures(normalff_);

    std::list<struct MeasuringRun> runs;
    runs.push_back({.start = 0, .end = 1, .typeface = measurerMockvars.typeface_});
    EXPECT_EQ(mi.Shape(charGroups_, runs, {}), 0);
    EXPECT_EQ(charGroups_.GetSize(), 1);
    EXPECT_GT(charGroups_.Get(0).invisibleWidth_, charGroups_.Get(0).visibleWidth_);
}

// 过程测试
// 调用Shape方法，设置RTL为true，走if路径
// 判定返回0，charGroups.GetSize()为1
HWTEST_F(MeasurerImplTest, Shape11, TestSize.Level1)
{
    InitMiMockVars({}, {});
    text_ = {'a', 'b'};
    MeasurerImpl mi(text_, fontCollection_);
    mi.SetFontFeatures(normalff_);
    mi.SetRTL(true);

    std::list<struct MeasuringRun> runs;
    runs.push_back({.start = 0, .end = 1, .typeface = measurerMockvars.typeface_});
    runs.push_back({.start = 1, .end = 2, .typeface = measurerMockvars.typeface_});
    EXPECT_EQ(mi.Shape(charGroups_, runs, {}), 0);
    EXPECT_EQ(charGroups_.GetSize(), 2);
}

// spacing
HWTEST_F(MeasurerImplTest, Shape12, TestSize.Level1)
{
    InitMiMockVars({.retvalGetGlyphInfo_ = {{.cluster = 0}, {.cluster = 1}},
                  .retvalGetGlyphPosition_ = {{}, {}}}, {});
    text_ = {'a', 'b'};
    MeasurerImpl mi(text_, fontCollection_);
    mi.SetFontFeatures(normalff_);
    mi.SetSpacing(5, 10);

    std::list<struct MeasuringRun> runs;
    runs.push_back({.start = 0, .end = 2, .typeface = measurerMockvars.typeface_});
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
HWTEST_F(MeasurerImplTest, GenerateHBFeatures1, TestSize.Level1)
{
    InitMiMockVars({}, {});
    MeasurerImpl mi(text_, fontCollection_);
    std::vector<hb_feature_t> features;
    mi.GenerateHBFeatures(features, nullptr);
    EXPECT_EQ(features.size(), 0);
}

// 过程测试
// 调用GenerateHBFeatures方法，传入空FontFeatures
// 判定features.size()为0
HWTEST_F(MeasurerImplTest, GenerateHBFeatures2, TestSize.Level1)
{
    InitMiMockVars({}, {});
    MeasurerImpl mi(text_, fontCollection_);
    std::vector<hb_feature_t> features;
    mi.GenerateHBFeatures(features, &emptyff_);
    EXPECT_EQ(features.size(), 0);
}

// 过程测试
// 调用GenerateHBFeatures方法，传入非空FontFeatures
// 控制hb_feature_from_string返回false
// 判定features.size()为0，fontFeatures.size()为1
HWTEST_F(MeasurerImplTest, GenerateHBFeatures3, TestSize.Level1)
{
    InitMiMockVars({.retvalHBFeatureFromString_ = false}, {});
    MeasurerImpl mi(text_, fontCollection_);
    std::vector<hb_feature_t> features;
    mi.GenerateHBFeatures(features, &normalff_);
    EXPECT_EQ(features.size(), 0);
    EXPECT_EQ(normalff_.GetFeatures().size(), 1);
}

// 过程测试
// 调用GenerateHBFeatures方法，传入非空FontFeatures
// 判定features.size()为1，fontFeatures.size()为1
HWTEST_F(MeasurerImplTest, GenerateHBFeatures4, TestSize.Level1)
{
    InitMiMockVars({}, {});
    MeasurerImpl mi(text_, fontCollection_);
    std::vector<hb_feature_t> features;
    mi.GenerateHBFeatures(features, &normalff_);
    EXPECT_EQ(features.size(), 1);
    EXPECT_EQ(normalff_.GetFeatures().size(), 1);
}
} // namespace Texgine
