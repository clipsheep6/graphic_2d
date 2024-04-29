/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "common/log_common.h"
#include "testcasefactory.h"

#include "dm/aa_rect_modes.h"
#include "dm/blur_circles.h"
#include "dm/blur_large_rrects.h"
#include "dm/add_arc.h"
#include "dm/font_regen.h"
#include "dm/alpha_image.h"
#include "dm/shadowutils.h"
#include "dm/clipped_cubic.h"
#include "dm/circular_arcs.h"
#include "dm/largeclippedpath.h"
#include "dm/filltypes.h"
#include "dm/dashing.h"
#include "dm/path_interior.h"
#include "dm/strokes.h"
#include "dm/alpha_gradients.h"
#include "dm/convex_paths.h"
#include "dm/gradient_dirty_laundry.h"
#include "dm/fill_types_persp.h"
#include "dm/arcofzorro.h"
#include "dm/stroke_rect_shader.h"
#include "dm/luma_filter.h"
#include "dm/bugfix7792.h"
#include "dm/bitmaprect.h"
#include "dm/points_mask_filter.h"
#include "dm/clip_cubic.h"
#include "dm/points.h"
#include "dm/skbug_8955.h"
#include "dm/conic_paths.h"
#include "dm/newsurface.h"
#include "dm/drawbitmaprect.h"
#include "dm/onebadarc.h"
#include "dm/anisotropic.h"
#include "interface/canvas_test.h"
#include "interface/text_blob_test.h"
#include "interface/path_test.h"

namespace {
std::unordered_map<std::string, std::function<std::shared_ptr<TestBase>()>> FunctionalCpuMap =
    {
        // DM
        {"aarectmodes", []() -> std::shared_ptr<TestBase> { return std::make_shared<AARectModes>(); }},                                          // ok
        {"blurcircles", []() -> std::shared_ptr<TestBase> { return std::make_shared<BlurCircles>(); }},                                          // ok
        {"blur_large_rrects", []() -> std::shared_ptr<TestBase> { return std::make_shared<BlurLargeRrects>(); }},                                // ok
        {"addarc_meas", []() -> std::shared_ptr<TestBase> { return std::make_shared<AddArcMeas>(); }},                                           // ok
        {"addarc", []() -> std::shared_ptr<TestBase> { return std::make_shared<AddArc>(); }},                                                    // ok
        {"badapple", []() -> std::shared_ptr<TestBase> { return std::make_shared<BadApple>(); }},                                                // 字体相关问题，函数缺失
        {"alpha_image_alpha_tint", []() -> std::shared_ptr<TestBase> { return std::make_shared<AlphaImageAlphaTint>(); }},                       // ok
        {"shadowutils", []() -> std::shared_ptr<TestBase> { return std::make_shared<ShadowUtils>(ShadowUtils::K_NO_OCCLUDERS); }},               // maprect 接口计算不对
        {"shadowutils_occl", []() -> std::shared_ptr<TestBase> { return std::make_shared<ShadowUtils>(ShadowUtils::K_OCCLUDERS); }},             // maprect 接口计算不对
        {"shadowutils_gray", []() -> std::shared_ptr<TestBase> { return std::make_shared<ShadowUtils>(ShadowUtils::K_GRAY_SCALE); }},            // maprect 接口计算不对
        {"circular_arc_stroke_matrix", []() -> std::shared_ptr<TestBase> { return std::make_shared<CircularArcStrokeMatrix>(); }},               // ok
        {"clippedcubic", []() -> std::shared_ptr<TestBase> { return std::make_shared<ClippedCubic>(); }},                                        // ok
        {"largeclippedpath_winding", []() -> std::shared_ptr<TestBase> { return std::make_shared<LargeClippedPath>(PATH_FILL_TYPE_WINDING); }},  // ok
        {"largeclippedpath_evenodd", []() -> std::shared_ptr<TestBase> { return std::make_shared<LargeClippedPath>(PATH_FILL_TYPE_EVEN_ODD); }}, // ok
        {"filltypes", []() -> std::shared_ptr<TestBase> { return std::make_shared<FillType>(); }},                                               // ok
        {"dashing", []() -> std::shared_ptr<TestBase> { return std::make_shared<Dashing>(); }},                                                  // ok
        {"pathinterior", []() -> std::shared_ptr<TestBase> { return std::make_shared<PathInterior>(); }},                                        // ok
        {"dashing2", []() -> std::shared_ptr<TestBase> { return std::make_shared<Dashing2>(); }},                                                // ok
        {"alphagradients", []() -> std::shared_ptr<TestBase> { return std::make_shared<AlphaGradients>(); }},                                    // ok
        {"dashing4", []() -> std::shared_ptr<TestBase> { return std::make_shared<Dashing4>(); }},                                                // ok
        {"convexpaths", []() -> std::shared_ptr<TestBase> { return std::make_shared<ConvexPaths>(); }},                                          // ok
        {"gradient_dirty_laundry", []() -> std::shared_ptr<TestBase> { return std::make_shared<Gradients>(); }},                                 // 内部形状和颜色差异较大
        {"arcofzorro", []() -> std::shared_ptr<TestBase> { return std::make_shared<ArcOfZorro>(); }},                                            // ok
        {"stroke_rect_shader", []() -> std::shared_ptr<TestBase> { return std::make_shared<StrokeRectShader>(); }},                              // ok
        {"lumafilter", []() -> std::shared_ptr<TestBase> { return std::make_shared<LumaFilter>(); }},                                            // 字体粗细、位置有差异
        {"pointsmaskfilter", []() -> std::shared_ptr<TestBase> { return std::make_shared<PointsMaskFilter>(); }},                                // drawpoints argb参数存在部分偏差
        {"clipcubic", []() -> std::shared_ptr<TestBase> { return std::make_shared<ClipCubic>(); }},                                              // ok
        {"filltypespersp", []() -> std::shared_ptr<TestBase> { return std::make_shared<FillTypePersp>(); }},                                     // ok
        {"strokes_poly", []() -> std::shared_ptr<TestBase> { return std::make_shared<Strokes2>(); }},                                            // ok
        {"bug7792", []() -> std::shared_ptr<TestBase> { return std::make_shared<BugFix7792>(); }},                                               // ok
        {"points", []() -> std::shared_ptr<TestBase> { return std::make_shared<Points>(); }},                                                    // ok
        {"alpha_image", []() -> std::shared_ptr<TestBase> { return std::make_shared<AlphaImage>(); }},                                           // 第二个三角形虚化不对，
        {"conicpaths", []() -> std::shared_ptr<TestBase> { return std::make_shared<ConicPaths>(); }},                                            // //有部分线条cpu出图部分缺失,gpu正常出图，颜色为黑色
        {"onebadarc", []() -> std::shared_ptr<TestBase> { return std::make_shared<OneBadArc>(); }},                                              // 完全按照skia的逻辑所画出的图形和skia不一致
        {"skbug_8955", []() -> std::shared_ptr<TestBase> { return std::make_shared<SkBug_8955>(); }},                                            // font.textToGlyphs、font.getPos接口缺失
        {"surfacenew", []() -> std::shared_ptr<TestBase> { return std::make_shared<NewSurfaceGM>(); }},                                          // ok
        {"bitmaprect", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawBitmapRect2>(); }},                                       // OH_Drawing_CanvasDrawRect接口有问题内部逻辑并未用画笔而是用画刷
        {"bigbitmaprect", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawBitmapRect4>(false); }},                               // ok
        {"anisotropic_hq", []() -> std::shared_ptr<TestBase> { return std::make_shared<Anisotropic>(); }},                                       // 该用例OH_Drawing_SamplingOptionsCreate接口mode对应内容未开放,无法实现
};

std::unordered_map<std::string, std::function<std::shared_ptr<TestBase>()>>
    PerformanceCpuMap = {

        // interface canvas
        {"canvas_drawrect", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasDrawRect>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"canvas_rotate", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasRotate>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"Canvas_DrawImageRect", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasDrawImageRect>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"Canvas_Translate", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasTranslate>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"Canvas_Scale", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasScale>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"Canvas_Clear", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasClear>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"Canvas_SetMatrix", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasSetMatrix>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"Canvas_ResetMatrix", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasResetMatrix>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"Canvas_ReadPixels", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasReadPixels>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"Canvas_ReadPixelsToBitmap", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasReadPixelsToBitmap>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"Canvas_DrawImageRectWithSrc", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasDrawImageRectWithSrc>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"Canvas_DrawShadow", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasDrawShadow>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"Canvas_Skew", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasSkew>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"Canvas_DrawVertices", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasDrawVertices>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"canvas_drawbitmaprect", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasDrawBitmapRect>(TestBase::DRAW_STYLE_COMPLEX); }},   // 100次crash，10次515ms
        {"canvas_drawcircle", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasDrawCircle>(TestBase::DRAW_STYLE_COMPLEX); }},           // 1000次crash，100次2813ms
        {"canvas_drawoval", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasDrawOval>(TestBase::DRAW_STYLE_COMPLEX); }},               // 1000次crash，100次751ms
        {"canvas_drawarc", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasDrawArc>(TestBase::DRAW_STYLE_COMPLEX); }},                 // 1000次crash，100次902ms
        {"canvas_drawroundrect", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasDrawRoundRect>(TestBase::DRAW_STYLE_COMPLEX); }},     // 1000次crash，100次743ms
        {"canvas_drawtextblob", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasDrawTextBlob>(TestBase::DRAW_STYLE_COMPLEX); }},       // 1000次1784ms
        {"canvas_drawcliprect", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasClipRect>(TestBase::DRAW_STYLE_COMPLEX); }},           // 1000次968ms
        {"canvas_drawcliproundrect", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasClipRoundRect>(TestBase::DRAW_STYLE_COMPLEX); }}, // 1000次crash，100次1582ms
        {"canvas_drawclippath", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasClipPath>(TestBase::DRAW_STYLE_COMPLEX); }},           // 1000次26ms
        {"canvas_drawpath", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasDrawPath>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"canvas_drawbitmap", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasDrawBitmap>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"canvas_drawlayer", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasDrawLayer>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"canvas_drawpoints", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasDrawPoints>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"canvas_drawline", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasDrawLine>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"canvas_drawbackground", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasDrawBackground>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"canvas_drawregion", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasDrawRegion>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"canvas_restoretocount", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasRestoreToCount>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"canvas_save", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasSave>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"canvas_drawpixelmaprect", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasDrawPixelMapRect>(TestBase::DRAW_STYLE_COMPLEX); }}, // OH_PixelMap_CreatePixelMap传入参数napi_env env无法获取，无法实现

        // path
        {"Path_Create", []() -> std::shared_ptr<TestBase> { return std::make_shared<PathCreate>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"Path_LineTo", []() -> std::shared_ptr<TestBase> { return std::make_shared<PathLineTo>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"Path_ArcTo", []() -> std::shared_ptr<TestBase> { return std::make_shared<PathArcTo>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"Path_QuadTo", []() -> std::shared_ptr<TestBase> { return std::make_shared<PathQuadTo>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"Path_Copy", []() -> std::shared_ptr<TestBase> { return std::make_shared<PathCopy>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"Path_EffectCreateDashPathEffect", []() -> std::shared_ptr<TestBase> { return std::make_shared<PathEffectCreateDashPathEffect>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"Path_AddOvalWithInitialPoint", []() -> std::shared_ptr<TestBase> { return std::make_shared<PathAddOvalWithInitialPoint>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"Path_AddArc", []() -> std::shared_ptr<TestBase> { return std::make_shared<PathAddArc>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"Path_AddPathWithMatrixAndMode", []() -> std::shared_ptr<TestBase> { return std::make_shared<PathAddPathWithMatrixAndMode>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"Path_ConicTo", []() -> std::shared_ptr<TestBase> { return std::make_shared<PathConicTo>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"path_transformwithclip", []() -> std::shared_ptr<TestBase> { return std::make_shared<PathTransformWithPerspectiveClip>(TestBase::DRAW_STYLE_COMPLEX, false); }},
        {"path_setfilltype", []() -> std::shared_ptr<TestBase> { return std::make_shared<PathSetFillType>(TestBase::DRAW_STYLE_COMPLEX, OH_Drawing_PathFillType::PATH_FILL_TYPE_WINDING); }},
        {"path_getlength", []() -> std::shared_ptr<TestBase> { return std::make_shared<PathGetLength>(TestBase::DRAW_STYLE_COMPLEX, true); }},
        {"path_close", []() -> std::shared_ptr<TestBase> { return std::make_shared<PathClose>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"path_offset", []() -> std::shared_ptr<TestBase> { return std::make_shared<PathOffset>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"path_reset", []() -> std::shared_ptr<TestBase> { return std::make_shared<PathReset>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"path_transform", []() -> std::shared_ptr<TestBase> { return std::make_shared<PathTransform>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"path_addoval", []() -> std::shared_ptr<TestBase> { return std::make_shared<PathAddOval>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"path_contain", []() -> std::shared_ptr<TestBase> { return std::make_shared<PathContains>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"path_cubicto", []() -> std::shared_ptr<TestBase> { return std::make_shared<PathCubicTo>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"path_rmoveto", []() -> std::shared_ptr<TestBase> { return std::make_shared<PathRMoveTo>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"path_rlineto", []() -> std::shared_ptr<TestBase> { return std::make_shared<PathRLineTo>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"path_rquadto", []() -> std::shared_ptr<TestBase> { return std::make_shared<PathRQuadTo>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"path_rconicto", []() -> std::shared_ptr<TestBase> { return std::make_shared<PathRConicTo>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"path_rcubicto", []() -> std::shared_ptr<TestBase> { return std::make_shared<PathRCubicTo>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"path_addrect", []() -> std::shared_ptr<TestBase> { return std::make_shared<PathAddRect>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"path_addrect_withcorner", []() -> std::shared_ptr<TestBase> { return std::make_shared<PathAddRectWithInitialCorner>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"path_addroundrect", []() -> std::shared_ptr<TestBase> { return std::make_shared<PathAddRoundRect>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"path_addpath", []() -> std::shared_ptr<TestBase> { return std::make_shared<PathAddPath>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"path_addpathwithmode", []() -> std::shared_ptr<TestBase> { return std::make_shared<PathAddPathWithMode>(TestBase::DRAW_STYLE_COMPLEX, OH_Drawing_PathAddMode::PATH_ADD_MODE_APPEND); }},
        {"path_addpathwithoffsetmode", []() -> std::shared_ptr<TestBase> { return std::make_shared<PathAddPathWithOffsetAndMode>(TestBase::DRAW_STYLE_COMPLEX, OH_Drawing_PathAddMode::PATH_ADD_MODE_APPEND); }},

        // textblob
        {"textblob_createbuilder", []() -> std::shared_ptr<TestBase> { return std::make_shared<TextBlobBuilderCreate>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"textblob_createfrom_text", []() -> std::shared_ptr<TestBase> { return std::make_shared<TextBlobCreateFromText>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"textblob_createfrom_postext", []() -> std::shared_ptr<TestBase> { return std::make_shared<TextBlobCreateFromPosText>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"textblob_createfrom_string", []() -> std::shared_ptr<TestBase> { return std::make_shared<TextBlobCreateFromString>(TestBase::DRAW_STYLE_COMPLEX); }},
        {"textblob_getbounds", []() -> std::shared_ptr<TestBase> { return std::make_shared<TextBlobGetBounds>(TestBase::DRAW_STYLE_COMPLEX); }},

};
} // namespace

std::shared_ptr<TestBase> TestCaseFactory::GetFunctionCpuCase(std::string caseName) {
    if (FunctionalCpuMap.find(caseName) == FunctionalCpuMap.end()) {
        DRAWING_LOGE("caseName error");
        return nullptr;
    }

    return FunctionalCpuMap.find(caseName)->second();
}

std::shared_ptr<TestBase> TestCaseFactory::GetPerformanceCpuCase(std::string caseName) {
    if (PerformanceCpuMap.find(caseName) == PerformanceCpuMap.end()) {
        DRAWING_LOGE("caseName error");
        return nullptr;
    }

    return PerformanceCpuMap.find(caseName)->second();
}

std::unordered_map<std::string, std::function<std::shared_ptr<TestBase>()>> TestCaseFactory::GetFunctionCpuCaseAll() {
    return FunctionalCpuMap;
}
