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
#include "bench/rect_bench.h"
#include "bench/path_bench.h"
#include "bench/textblob_cache_bench.h"
#include "bench/draw_bitmap_aa_bench.h"
#include "bench/bulk_rect_bench.h"
#include "bench/canvas_save_restore.h"
#include "bench/shader_mf_bench.h"
#include "bench/dash_line_bench.h"
#include "bench/clip_strategy_bench.h"
#include "bench/aa_clip_bench.h"
#include "bench/circles_bench.h"
#include "bench/nested_aa_clip_bench.h"
#include "bench/hairline_path_bench.h"
#include "bench/gradient_bench.h"
#include "bench/xfermode_bench.h"
#include "bench/blur_bench.h"
#include "bench/path_create_bench.h"
#include "bench/skbench_addpathtest.h"
#include "bench/canvas_matrix_bench.h"
#include "bench/shadow_bench.h"
#include "bench/read_pix_bench.h"
#include "bench/bezier_bench.h"
#include "bench/clear_bench.h"
#include "bench/big_path_bench.h"
#include "bench/hardstopgradientbench_scalenumcolors.h"
#include "dm/aa_rect_modes.h"
#include "dm/blur_circles_gm.h"
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
#include "dm/clip_cubic_gm.h"
#include "dm/points.h"
#include "dm/skbug_8955.h"
#include "dm/conic_paths.h"
#include "dm/newsurface.h"
#include "dm/drawbitmaprect.h"

#include "interface/canvas_test.h"
#include "dm/onebadarc.h"

namespace {
std::unordered_map<std::string, std::function<std::shared_ptr<TestBase>()>> FunctionalCpuMap =
    {
        {"drawrect", []() -> std::shared_ptr<TestBase> { return std::make_shared<RectBench>(1); }},                                                              // drawrect 随机颜色，随机位置
        {"drawtextblob", []() -> std::shared_ptr<TestBase> { return std::make_shared<TextBlobCachedBench>(true); }},                                             // drawtextblob
        {"drawbitmapnoaa", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawBitmapAABench>(); }},                                                 // drawbitmap
        {"drawbitmaprect", []() -> std::shared_ptr<TestBase> { return std::make_shared<BulkRectBench>(BulkRectBench::BITMAP_RECT); }},                           // drawbitmaprect 排列平铺到整个页面
        {"saverestore8", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasSaveRestore>(); }},                                                   // 8*(save+concat+restore)+drawcolor
        {"savelayer", []() -> std::shared_ptr<TestBase> { return std::make_shared<ShaderMFBench>(); }},                                                          // savelayer+canvasclear+restore
        {"drawdashline", []() -> std::shared_ptr<TestBase> { return std::make_shared<DashLineBench>(); }},                                                       // drawline
        {"drawcircle", []() -> std::shared_ptr<TestBase> { return std::make_shared<ClipStrategyBench>(); }},                                                     // savelayer+createpoint+drawcircle
        {"drawoval", []() -> std::shared_ptr<TestBase> { return std::make_shared<OvalBench>(1); }},                                                              // drawOval
        {"drawroundrect", []() -> std::shared_ptr<TestBase> { return std::make_shared<RRectBench>(1); }},                                                        // DrawRoundRect
        {"clippath", []() -> std::shared_ptr<TestBase> { return std::make_shared<AAClipBench>(true, true); }},                                                   // save clippath drawrect restore
        {"cliprect", []() -> std::shared_ptr<TestBase> { return std::make_shared<AAClipBench>(false, true); }},                                                  // save cliprect drawrect restore
        {"drawimagerect", []() -> std::shared_ptr<TestBase> { return std::make_shared<BulkRectBench>(BulkRectBench::IMAGE_RECT); }},                             // drawimagerect
        {"drawpatharcto", []() -> std::shared_ptr<TestBase> { return std::make_shared<CirclesBench>(); }},                                                       // patharcto*2 + pathclose + drawpath
        {"drawpathaddrect", []() -> std::shared_ptr<TestBase> { return std::make_shared<RectPathBench>(); }},                                                    // drawpath, path由AddRect构造
        {"drawpathaddroundrect", []() -> std::shared_ptr<TestBase> { return std::make_shared<NestedAAClipBench>(); }},                                           // path由addrrrect构建，canvas clippath：在此背景上drawrect，随机alpha
        {"drawpathlineto", []() -> std::shared_ptr<TestBase> { return std::make_shared<TrianglePathBench>(); }},                                                 // drawpath, path由lineto构造
        {"drawpathcubicto", []() -> std::shared_ptr<TestBase> { return std::make_shared<CubicPathBench>(); }},                                                   // drawpath, path先由Cubic构造再进行transform
        {"drawshaderlinear_oval", []() -> std::shared_ptr<TestBase> { return std::make_shared<GradientBench>(true, GradientBench::LINEAR, 2); }},                // 主循环drawoval, penseteffect（由linear构造）
        {"drawshaderradial_oval", []() -> std::shared_ptr<TestBase> { return std::make_shared<GradientBench>(true, GradientBench::RADIAL, 2); }},                // 主循环drawoval, penseteffect（由radial构造）
        {"drawshadersweep_oval", []() -> std::shared_ptr<TestBase> { return std::make_shared<GradientBench>(true, GradientBench::SWEEP, 2); }},                  // 主循环drawoval, penseteffect（由sweep构造）
        {"drawshaderlinear_rect", []() -> std::shared_ptr<TestBase> { return std::make_shared<GradientBench>(false, GradientBench::LINEAR, 2); }},               // 主循环drawrect, penseteffect（由linear构造）
        {"drawshaderradial_rect", []() -> std::shared_ptr<TestBase> { return std::make_shared<GradientBench>(false, GradientBench::RADIAL, 2); }},               // 主循环drawrect, penseteffect（由radial构造）
        {"drawshadersweep_rect", []() -> std::shared_ptr<TestBase> { return std::make_shared<GradientBench>(false, GradientBench::SWEEP, 2); }},                 // 主循环drawrect, penseteffect（由sweep构造）
        {"drawtextblobcreate_text", []() -> std::shared_ptr<TestBase> { return std::make_shared<XfermodeBench>(XfermodeBench::FromText); }},                     // DrawTextBlob, textblob由createformtext创建，每drawtextblob 1000次就重新创建一下textblob
        {"drawtextblobcreate_pos", []() -> std::shared_ptr<TestBase> { return std::make_shared<XfermodeBench>(XfermodeBench::FromPosText); }},                   // DrawTextBlob, textblob由createformtextpos创建，每drawtextblob 1000次就重新创建一下textblob
        {"drawtextblobcreate_string", []() -> std::shared_ptr<TestBase> { return std::make_shared<XfermodeBench>(XfermodeBench::FromString); }},                 // DrawTextBlob, textblob由createformstring创建，每drawtextblob 1000次就重新创建一下textblob
        {"maskfiltercreateblur_real_normal", []() -> std::shared_ptr<TestBase> { return std::make_shared<BlurBench>(BLUR_REAL, OH_Drawing_BlurType::NORMAL); }}, // blurbench,模糊偏差0.01，模糊类型normal
        {"drawpathreset", []() -> std::shared_ptr<TestBase> { return std::make_shared<PathCreateBench>(); }},                                                    // pathreset
        {"drawtextblobbuildercreate", []() -> std::shared_ptr<TestBase> { return std::make_shared<TextBlobCachedBench>(false); }},                               // drawtextblob BuilderCreate
        {"drawcanvasscale_translate", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasMatrixBench>(CanvasMatrixBench::CanvasType::Translate); }},
        {"drawcanvasscale_scale", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasMatrixBench>(CanvasMatrixBench::CanvasType::Scale); }},
        {"drawcanvasscale_matrix", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasMatrixBench>(CanvasMatrixBench::CanvasType::ConcatMatrix); }},
        {"drawcanvasdrawshadow_t_g", []() -> std::shared_ptr<TestBase> { return std::make_shared<ShadowBench>(true, true); }},
        {"drawcanvasdrawshadow_o_a", []() -> std::shared_ptr<TestBase> { return std::make_shared<ShadowBench>(false, false); }},
        {"drawcanvasdrawshadow_t_a", []() -> std::shared_ptr<TestBase> { return std::make_shared<ShadowBench>(true, false); }},
        {"drawcanvasdrawshadow_o_g", []() -> std::shared_ptr<TestBase> { return std::make_shared<ShadowBench>(false, true); }},
        {"drawcanvasreadpixels", []() -> std::shared_ptr<TestBase> { return std::make_shared<ReadPixBench>(false); }},
        {"drawcanvasreadpixels_tobitmap", []() -> std::shared_ptr<TestBase> { return std::make_shared<ReadPixBench>(true); }},
        {"drawpenrect_1_0_aa", []() -> std::shared_ptr<TestBase> { return std::make_shared<RectBench>(1, false, 0, true); }},
        {"drawpenrect_1_4_aa", []() -> std::shared_ptr<TestBase> { return std::make_shared<RectBench>(1, false, 4, true); }},
        {"drawpenrect_3_0_aa", []() -> std::shared_ptr<TestBase> { return std::make_shared<RectBench>(3, false, 0, true); }},
        {"drawpenrect_3_4_aa", []() -> std::shared_ptr<TestBase> { return std::make_shared<RectBench>(3, false, 4, true); }},
        {"drawpenrect_1_0_bw", []() -> std::shared_ptr<TestBase> { return std::make_shared<RectBench>(1, false, 0, false); }},
        {"drawpenrect_1_4_bw", []() -> std::shared_ptr<TestBase> { return std::make_shared<RectBench>(1, false, 4, false); }},
        {"drawpenrect_3_0_bw", []() -> std::shared_ptr<TestBase> { return std::make_shared<RectBench>(3, false, 0, false); }},
        {"drawpenrect_3_4_bw", []() -> std::shared_ptr<TestBase> { return std::make_shared<RectBench>(3, false, 4, false); }},
        {"drawpenrectsrcmode", []() -> std::shared_ptr<TestBase> { return std::make_shared<SrcModeRectBench>(); }},
        {"drawpenbeizer_quad_butt_round_2", []() -> std::shared_ptr<TestBase> { return std::make_shared<BezierBench>(OH_Drawing_PenLineCapStyle::LINE_FLAT_CAP, OH_Drawing_PenLineJoinStyle::LINE_ROUND_JOIN, 2, BezierBench::DrawType::draw_quad); }},
        {"drawpenbeizer_quad_square_bevel_10", []() -> std::shared_ptr<TestBase> { return std::make_shared<BezierBench>(OH_Drawing_PenLineCapStyle::LINE_SQUARE_CAP, OH_Drawing_PenLineJoinStyle::LINE_BEVEL_JOIN, 10, BezierBench::DrawType::draw_quad); }},
        {"drawpenbeizer_quad_round_miter_50", []() -> std::shared_ptr<TestBase> { return std::make_shared<BezierBench>(OH_Drawing_PenLineCapStyle::LINE_ROUND_CAP, OH_Drawing_PenLineJoinStyle::LINE_MITER_JOIN, 50, BezierBench::DrawType::draw_quad); }},
        {"drawpenbeizer_cubic_butt_round_2", []() -> std::shared_ptr<TestBase> { return std::make_shared<BezierBench>(OH_Drawing_PenLineCapStyle::LINE_FLAT_CAP, OH_Drawing_PenLineJoinStyle::LINE_ROUND_JOIN, 2, BezierBench::DrawType::draw_quad); }},
        {"drawpenbeizer_cubic_square_bevel_2", []() -> std::shared_ptr<TestBase> { return std::make_shared<BezierBench>(OH_Drawing_PenLineCapStyle::LINE_SQUARE_CAP, OH_Drawing_PenLineJoinStyle::LINE_BEVEL_JOIN, 10, BezierBench::DrawType::draw_quad); }},
        {"drawpenbeizer_cubic_round_miter_2", []() -> std::shared_ptr<TestBase> { return std::make_shared<BezierBench>(OH_Drawing_PenLineCapStyle::LINE_ROUND_CAP, OH_Drawing_PenLineJoinStyle::LINE_MITER_JOIN, 50, BezierBench::DrawType::draw_quad); }},
        {"clearbench", []() -> std::shared_ptr<TestBase> { return std::make_shared<ClearBench>(kPartial_ClearType); }},
        {"drawpathrotate_45", []() -> std::shared_ptr<TestBase> { return std::make_shared<RotatedRectBench>(45); }},
        {"drawpathrotate_60", []() -> std::shared_ptr<TestBase> { return std::make_shared<RotatedRectBench>(60); }},
        {"drawpathquadto", []() -> std::shared_ptr<TestBase> { return std::make_shared<LongCurvedPathBench>(); }}, // drawpath, path由PathQuadTo构造
        {"drawbigpath_left_round", []() -> std::shared_ptr<TestBase> { return std::make_shared<BigPathBench>(BigPathBench::Align::kLeft_Align, true); }},
        {"drawbigpath_middle_round", []() -> std::shared_ptr<TestBase> { return std::make_shared<BigPathBench>(BigPathBench::Align::kMiddle_Align, true); }},
        {"drawbigpath_right_round", []() -> std::shared_ptr<TestBase> { return std::make_shared<BigPathBench>(BigPathBench::Align::kRight_Align, true); }},
        {"drawbigpath_left_na", []() -> std::shared_ptr<TestBase> { return std::make_shared<BigPathBench>(BigPathBench::Align::kLeft_Align, false); }},
        {"drawbigpath_middle_na", []() -> std::shared_ptr<TestBase> { return std::make_shared<BigPathBench>(BigPathBench::Align::kMiddle_Align, false); }},
        {"drawbigpath_right_na", []() -> std::shared_ptr<TestBase> { return std::make_shared<BigPathBench>(BigPathBench::Align::kRight_Align, false); }},
        {"blitmaskbench_maskopaque", []() -> std::shared_ptr<TestBase> { return std::make_shared<BlitMaskBench>(BlitMaskBench::PointMode::kPoints_PointMode, BlitMaskBench::kMaskType::kMaskOpaque); }},
        {"blitmaskbench_maskblack", []() -> std::shared_ptr<TestBase> { return std::make_shared<BlitMaskBench>(BlitMaskBench::PointMode::kPoints_PointMode, BlitMaskBench::kMaskType::kMaskBlack); }},
        {"blitmaskbench_maskcolor", []() -> std::shared_ptr<TestBase> { return std::make_shared<BlitMaskBench>(BlitMaskBench::PointMode::kPoints_PointMode, BlitMaskBench::kMaskType::kMaskColor); }},
        {"blitmaskbench_maskshader", []() -> std::shared_ptr<TestBase> { return std::make_shared<BlitMaskBench>(BlitMaskBench::PointMode::kPoints_PointMode, BlitMaskBench::kMaskType::kMaskColor); }},

        // skbench_kadd：创建一个矩阵对象将原路径矩阵变换后添加到当前路径中.
        {"skbench_kadd", []() -> std::shared_ptr<TestBase> { return std::make_shared<SkBench_AddPathTest>(kAdd_AddType); }},
        // skbench_kaddtrans：设置矩阵为单位矩阵并平移(dx, dy)后将原路径矩阵变换添加到当前路径中.
        {"skbench_kaddtrans", []() -> std::shared_ptr<TestBase> { return std::make_shared<SkBench_AddPathTest>(kAddTrans_AddType); }},
        // skbench_kaddmatrix：设置矩阵对象的参数后将原路径矩阵变换添加到当前路径中.
        {"skbench_kaddmatrix", []() -> std::shared_ptr<TestBase> { return std::make_shared<SkBench_AddPathTest>(kAddMatrix_AddType); }},
        {"drawtextblobcreate_text_src", []() -> std::shared_ptr<TestBase> { return std::make_shared<XfermodeBench>(XfermodeBench::FromText, BLEND_MODE_SRC); }},
        {"drawtextblobcreate_text_clear", []() -> std::shared_ptr<TestBase> { return std::make_shared<XfermodeBench>(XfermodeBench::FromText, BLEND_MODE_CLEAR); }},
        {"drawtextblobcreate_text_color", []() -> std::shared_ptr<TestBase> { return std::make_shared<XfermodeBench>(XfermodeBench::FromText, BLEND_MODE_COLOR); }},
        {"drawtextblobcreate_text_diff", []() -> std::shared_ptr<TestBase> { return std::make_shared<XfermodeBench>(XfermodeBench::FromText, BLEND_MODE_DIFFERENCE); }},
        //HardStopGradientBench：kClamp、kRepeat、kMirror着色器效果平铺模式；4、25、100次数
        {"HardStopGradientBench_clamp_4", []() -> std::shared_ptr<TestBase> { return std::make_shared<HardStopGradientBench_ScaleNumColors>(HardStopGradientBench_ScaleNumColors::kClamp, 4);}}, 
        {"HardStopGradientBench_clamp_25", []() -> std::shared_ptr<TestBase> { return std::make_shared<HardStopGradientBench_ScaleNumColors>(HardStopGradientBench_ScaleNumColors::kClamp, 25);}},
        {"HardStopGradientBench_clamp_100", []() -> std::shared_ptr<TestBase> { return std::make_shared<HardStopGradientBench_ScaleNumColors>(HardStopGradientBench_ScaleNumColors::kClamp, 100);}},
        {"HardStopGradientBench_repeat_4", []() -> std::shared_ptr<TestBase> { return std::make_shared<HardStopGradientBench_ScaleNumColors>(HardStopGradientBench_ScaleNumColors::kRepeat, 4);}}, 
        {"HardStopGradientBench_repeat_25", []() -> std::shared_ptr<TestBase> { return std::make_shared<HardStopGradientBench_ScaleNumColors>(HardStopGradientBench_ScaleNumColors::kRepeat, 25);}},
        {"HardStopGradientBench_repeat_100", []() -> std::shared_ptr<TestBase> { return std::make_shared<HardStopGradientBench_ScaleNumColors>(HardStopGradientBench_ScaleNumColors::kRepeat, 100);}},
        {"HardStopGradientBench_mirror_4", []() -> std::shared_ptr<TestBase> { return std::make_shared<HardStopGradientBench_ScaleNumColors>(HardStopGradientBench_ScaleNumColors::kMirror, 4);}}, 
        {"HardStopGradientBench_mirror_25", []() -> std::shared_ptr<TestBase> { return std::make_shared<HardStopGradientBench_ScaleNumColors>(HardStopGradientBench_ScaleNumColors::kMirror, 25);}},
        {"HardStopGradientBench_mirror_100", []() -> std::shared_ptr<TestBase> { return std::make_shared<HardStopGradientBench_ScaleNumColors>(HardStopGradientBench_ScaleNumColors::kMirror, 100);}},

        // DM
        {"aarectmodes", []() -> std::shared_ptr<TestBase> { return std::make_shared<AARectModes>(); }},                                             // 缺少背景格子的渲染
        {"blurcircles", []() -> std::shared_ptr<TestBase> { return std::make_shared<BlurCirclesGM>(); }},                                           // ok
        {"blur_large_rrects", []() -> std::shared_ptr<TestBase> { return std::make_shared<BlurLargeRrects>(); }},                                   // ok
        {"addarc_meas", []() -> std::shared_ptr<TestBase> { return std::make_shared<AddArcMeas>(); }},                                              // ok
        {"addarc", []() -> std::shared_ptr<TestBase> { return std::make_shared<AddArc>(); }},                                                       // ok
        {"badapple", []() -> std::shared_ptr<TestBase> { return std::make_shared<BadApple>(); }},                                                   // 字体相关问题，函数缺失
        {"alpha_image_alpha_tint", []() -> std::shared_ptr<TestBase> { return std::make_shared<AlphaImage>(); }},                                   //  不是同一个图
        {"shadowutils", []() -> std::shared_ptr<TestBase> { return std::make_shared<ShadowUtils>(ShadowUtils::kDebugColorNoOccluders); }},          // maprect 接口计算不对
        {"shadowutils_occl", []() -> std::shared_ptr<TestBase> { return std::make_shared<ShadowUtils>(ShadowUtils::kDebugColorOccluders); }},       // maprect 接口计算不对
        {"shadowutils_gray", []() -> std::shared_ptr<TestBase> { return std::make_shared<ShadowUtils>(ShadowUtils::kGrayscale); }},                 // maprect 接口计算不对
        {"circular_arc_stroke_matrix", []() -> std::shared_ptr<TestBase> { return std::make_shared<CircularArcStrokeMatrix>(); }},                  // ok
        {"clippedcubic", []() -> std::shared_ptr<TestBase> { return std::make_shared<ClippedCubic>(); }},                                           // ok
        {"largeclippedpath_winding", []() -> std::shared_ptr<TestBase> { return std::make_shared<LargeClippedPath>(LargeClippedPath::kWinding); }}, // ok
        {"largeclippedpath_evenodd", []() -> std::shared_ptr<TestBase> { return std::make_shared<LargeClippedPath>(LargeClippedPath::kEvenOdd); }}, // ok
        {"filltypes", []() -> std::shared_ptr<TestBase> { return std::make_shared<FillTypeGM>(); }},                                                // ok
        {"dashing", []() -> std::shared_ptr<TestBase> { return std::make_shared<Dashing>(); }},                                                     // ok
        {"pathinterior", []() -> std::shared_ptr<TestBase> { return std::make_shared<PathInterior>(); }},                                           // ok
        {"dashing2", []() -> std::shared_ptr<TestBase> { return std::make_shared<Dashing2>(); }},                                                   // ok
        {"alphagradients", []() -> std::shared_ptr<TestBase> { return std::make_shared<AlphaGradients>(); }},                                       // ok
        {"dashing4", []() -> std::shared_ptr<TestBase> { return std::make_shared<Dashing4>(); }},                                                   // ok
        {"convexpaths", []() -> std::shared_ptr<TestBase> { return std::make_shared<ConvexPaths>(); }},                                             // ok
        {"gradient_dirty_laundry", []() -> std::shared_ptr<TestBase> { return std::make_shared<GradientsGM>(); }},                                  // 与原逻辑一样，只是色彩深浅存在差异
        {"arcofzorro", []() -> std::shared_ptr<TestBase> { return std::make_shared<ArcOfZorroGM>(); }},                                             // ok
        {"stroke_rect_shader", []() -> std::shared_ptr<TestBase> { return std::make_shared<StrokeRectShader>(); }},                                 // ok
        {"lumafilter", []() -> std::shared_ptr<TestBase> { return std::make_shared<LumaFilter>(); }},                                               // 字体粗细、位置有差异
        {"points_maskfilter", []() -> std::shared_ptr<TestBase> { return std::make_shared<PointsMaskFilter>(); }},                                   // drawpoints argb参数存在部分偏差
        {"clipcubic", []() -> std::shared_ptr<TestBase> { return std::make_shared<ClipCubicGm>(); }},                                               // ok
        {"filltypespersp", []() -> std::shared_ptr<TestBase> { return std::make_shared<FillTypePersp>(); }},                                       // ok
        {"strokes_poly", []() -> std::shared_ptr<TestBase> { return std::make_shared<Strokes2>(); }},                                               // ok
        {"bug7792", []() -> std::shared_ptr<TestBase> { return std::make_shared<BugFix7792>(); }},                                                  // ok

        {"points", []() -> std::shared_ptr<TestBase> { return std::make_shared<Points>(); }},          // ok
        {"alpha_image", []() -> std::shared_ptr<TestBase> { return std::make_shared<AlphaImage>(); }}, // 第二个三角形虚化不对，
        {"conicpaths", []() -> std::shared_ptr<TestBase> { return std::make_shared<ConicPaths>(); }},  // 有部分线条多余画出
        {"onebadarc", []() -> std::shared_ptr<TestBase> { return std::make_shared<OneBadArc>(); }},  // 完全按照skia的逻辑所画出的图形和skia不一致
        {"skbug_8955", []() -> std::shared_ptr<TestBase> { return std::make_shared<SkBug_8955>(); }},   //  font.textToGlyphs、font.getPos接口缺失
        {"bigbitmaprect", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawBitmapRect4>(false); }},   //代码完成，有crash，rect roundout 接口缺失
        {"surfacenew", []() -> std::shared_ptr<TestBase> { return std::make_shared<NewSurfaceGM>(); }},       //ok
        {"bitmaprect", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawBitmapRect2>(); }},     //OH_Drawing_CanvasDrawRect接口有问题内部逻辑并未用画笔而是用画刷

};

std::unordered_map<std::string, std::function<std::shared_ptr<TestBase>()>>
    PerformanceCpuMap = {
        {"drawrect", []() -> std::shared_ptr<TestBase> { return std::make_shared<RectBench>(1); }},                                                              // drawrect 随机颜色，随机位置
        {"drawtextblob", []() -> std::shared_ptr<TestBase> { return std::make_shared<TextBlobCachedBench>(true); }},                                             // drawtextblob
        {"drawbitmapnoaa", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawBitmapAABench>(); }},                                                 // drawbitmap
        {"drawbitmaprect", []() -> std::shared_ptr<TestBase> { return std::make_shared<BulkRectBench>(BulkRectBench::BITMAP_RECT); }},                           // drawbitmaprect 排列平铺到整个页面
        {"saverestore8", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasSaveRestore>(); }},                                                   // 8*(save+concat+restore)+drawcolor
        {"savelayer", []() -> std::shared_ptr<TestBase> { return std::make_shared<ShaderMFBench>(); }},                                                          // savelayer+canvasclear+restore
        {"drawdashline", []() -> std::shared_ptr<TestBase> { return std::make_shared<DashLineBench>(); }},                                                       // drawline
        {"drawcircle", []() -> std::shared_ptr<TestBase> { return std::make_shared<ClipStrategyBench>(); }},                                                     // savelayer+createpoint+drawcircle
        {"drawoval", []() -> std::shared_ptr<TestBase> { return std::make_shared<OvalBench>(1); }},                                                              // drawOval
        {"drawroundrect", []() -> std::shared_ptr<TestBase> { return std::make_shared<RRectBench>(1); }},                                                        // DrawRoundRect
        {"clippath", []() -> std::shared_ptr<TestBase> { return std::make_shared<AAClipBench>(true, true); }},                                                   // save clippath drawrect restore
        {"cliprect", []() -> std::shared_ptr<TestBase> { return std::make_shared<AAClipBench>(false, true); }},                                                  // save cliprect drawrect restore
        {"drawimagerect", []() -> std::shared_ptr<TestBase> { return std::make_shared<BulkRectBench>(BulkRectBench::IMAGE_RECT); }},                             // drawimagerect
        {"drawpatharcto", []() -> std::shared_ptr<TestBase> { return std::make_shared<CirclesBench>(); }},                                                       // patharcto*2 + pathclose + drawpath
        {"drawpathaddrect", []() -> std::shared_ptr<TestBase> { return std::make_shared<RectPathBench>(); }},                                                    // drawpath, path由AddRect构造
        {"drawpathaddroundrect", []() -> std::shared_ptr<TestBase> { return std::make_shared<NestedAAClipBench>(); }},                                           // path由addrrrect构建，canvas clippath：在此背景上drawrect，随机alpha
        {"drawpathlineto", []() -> std::shared_ptr<TestBase> { return std::make_shared<TrianglePathBench>(); }},                                                 // drawpath, path由lineto构造
        {"drawpathcubicto", []() -> std::shared_ptr<TestBase> { return std::make_shared<CubicPathBench>(); }},                                                   // drawpath, path先由Cubic构造再进行transform
        {"drawshaderlinear_oval", []() -> std::shared_ptr<TestBase> { return std::make_shared<GradientBench>(true, GradientBench::LINEAR, 2); }},                // 主循环drawoval, penseteffect（由linear构造）
        {"drawshaderradial_oval", []() -> std::shared_ptr<TestBase> { return std::make_shared<GradientBench>(true, GradientBench::RADIAL, 2); }},                // 主循环drawoval, penseteffect（由radial构造）
        {"drawshadersweep_oval", []() -> std::shared_ptr<TestBase> { return std::make_shared<GradientBench>(true, GradientBench::SWEEP, 2); }},                  // 主循环drawoval, penseteffect（由sweep构造）
        {"drawshaderlinear_rect", []() -> std::shared_ptr<TestBase> { return std::make_shared<GradientBench>(false, GradientBench::LINEAR, 2); }},               // 主循环drawrect, penseteffect（由linear构造）
        {"drawshaderradial_rect", []() -> std::shared_ptr<TestBase> { return std::make_shared<GradientBench>(false, GradientBench::RADIAL, 2); }},               // 主循环drawrect, penseteffect（由radial构造）
        {"drawshadersweep_rect", []() -> std::shared_ptr<TestBase> { return std::make_shared<GradientBench>(false, GradientBench::SWEEP, 2); }},                 // 主循环drawrect, penseteffect（由sweep构造）
        {"drawtextblobcreate_text", []() -> std::shared_ptr<TestBase> { return std::make_shared<XfermodeBench>(XfermodeBench::FromText); }},                     // DrawTextBlob, textblob由createformtext创建，每drawtextblob 1000次就重新创建一下textblob
        {"drawtextblobcreate_pos", []() -> std::shared_ptr<TestBase> { return std::make_shared<XfermodeBench>(XfermodeBench::FromPosText); }},                   // DrawTextBlob, textblob由createformtextpos创建，每drawtextblob 1000次就重新创建一下textblob
        {"drawtextblobcreate_string", []() -> std::shared_ptr<TestBase> { return std::make_shared<XfermodeBench>(XfermodeBench::FromString); }},                 // DrawTextBlob, textblob由createformstring创建，每drawtextblob 1000次就重新创建一下textblob
        {"maskfiltercreateblur_real_normal", []() -> std::shared_ptr<TestBase> { return std::make_shared<BlurBench>(BLUR_REAL, OH_Drawing_BlurType::NORMAL); }}, // blurbench,模糊偏差0.01，模糊类型normal
        {"drawpathreset", []() -> std::shared_ptr<TestBase> { return std::make_shared<PathCreateBench>(); }},                                                    // pathreset
        {"drawtextblobbuildercreate", []() -> std::shared_ptr<TestBase> { return std::make_shared<TextBlobCachedBench>(false); }},                               // drawtextblob BuilderCreate
        {"drawtextblobcreate_text", []() -> std::shared_ptr<TestBase> { return std::make_shared<XfermodeBench>(1); }},                                           // DrawTextBlob, textblob由createformtext创建，每drawtextblob 1000次就重新创建一下textblob
        {"drawtextblobcreate_pos", []() -> std::shared_ptr<TestBase> { return std::make_shared<XfermodeBench>(2); }},                                            // DrawTextBlob, textblob由createformtextpos创建，每drawtextblob 1000次就重新创建一下textblob
        {"drawtextblobcreate_string", []() -> std::shared_ptr<TestBase> { return std::make_shared<XfermodeBench>(3); }},                                         // DrawTextBlob, textblob由createformstring创建，每drawtextblob 1000次就重新创建一下textblob
        {"drawcanvasscale_translate", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasMatrixBench>(CanvasMatrixBench::CanvasType::Translate); }},
        {"drawcanvasscale_scale", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasMatrixBench>(CanvasMatrixBench::CanvasType::Scale); }},
        {"drawcanvasscale_matrix", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasMatrixBench>(CanvasMatrixBench::CanvasType::ConcatMatrix); }},
        {"drawcanvasdrawshadow_t_g", []() -> std::shared_ptr<TestBase> { return std::make_shared<ShadowBench>(true, true); }},
        {"drawcanvasdrawshadow_o_a", []() -> std::shared_ptr<TestBase> { return std::make_shared<ShadowBench>(false, false); }},
        {"drawcanvasdrawshadow_t_a", []() -> std::shared_ptr<TestBase> { return std::make_shared<ShadowBench>(true, false); }},
        {"drawcanvasdrawshadow_o_g", []() -> std::shared_ptr<TestBase> { return std::make_shared<ShadowBench>(false, true); }},
        {"drawcanvasreadpixels", []() -> std::shared_ptr<TestBase> { return std::make_shared<ReadPixBench>(false); }},
        {"drawcanvasreadpixels_tobitmap", []() -> std::shared_ptr<TestBase> { return std::make_shared<ReadPixBench>(true); }},
        {"drawpenrect_1_0_aa", []() -> std::shared_ptr<TestBase> { return std::make_shared<RectBench>(1, false, 0, true); }},
        {"drawpenrect_1_4_aa", []() -> std::shared_ptr<TestBase> { return std::make_shared<RectBench>(1, false, 4, true); }},
        {"drawpenrect_3_0_aa", []() -> std::shared_ptr<TestBase> { return std::make_shared<RectBench>(3, false, 0, true); }},
        {"drawpenrect_3_4_aa", []() -> std::shared_ptr<TestBase> { return std::make_shared<RectBench>(3, false, 4, true); }},
        {"drawpenrect_1_0_bw", []() -> std::shared_ptr<TestBase> { return std::make_shared<RectBench>(1, false, 0, false); }},
        {"drawpenrect_1_4_bw", []() -> std::shared_ptr<TestBase> { return std::make_shared<RectBench>(1, false, 4, false); }},
        {"drawpenrect_3_0_bw", []() -> std::shared_ptr<TestBase> { return std::make_shared<RectBench>(3, false, 0, false); }},
        {"drawpenrect_3_4_bw", []() -> std::shared_ptr<TestBase> { return std::make_shared<RectBench>(3, false, 4, false); }},
        {"drawpenrectsrcmode", []() -> std::shared_ptr<TestBase> { return std::make_shared<SrcModeRectBench>(); }},
        {"drawpenbeizer_quad_butt_round_2", []() -> std::shared_ptr<TestBase> { return std::make_shared<BezierBench>(OH_Drawing_PenLineCapStyle::LINE_FLAT_CAP, OH_Drawing_PenLineJoinStyle::LINE_ROUND_JOIN, 2, BezierBench::DrawType::draw_quad); }},
        {"drawpenbeizer_quad_square_bevel_10", []() -> std::shared_ptr<TestBase> { return std::make_shared<BezierBench>(OH_Drawing_PenLineCapStyle::LINE_SQUARE_CAP, OH_Drawing_PenLineJoinStyle::LINE_BEVEL_JOIN, 10, BezierBench::DrawType::draw_quad); }},
        {"drawpenbeizer_quad_round_miter_50", []() -> std::shared_ptr<TestBase> { return std::make_shared<BezierBench>(OH_Drawing_PenLineCapStyle::LINE_ROUND_CAP, OH_Drawing_PenLineJoinStyle::LINE_MITER_JOIN, 50, BezierBench::DrawType::draw_quad); }},
        {"drawpenbeizer_cubic_butt_round_2", []() -> std::shared_ptr<TestBase> { return std::make_shared<BezierBench>(OH_Drawing_PenLineCapStyle::LINE_FLAT_CAP, OH_Drawing_PenLineJoinStyle::LINE_ROUND_JOIN, 2, BezierBench::DrawType::draw_quad); }},
        {"drawpenbeizer_cubic_square_bevel_2", []() -> std::shared_ptr<TestBase> { return std::make_shared<BezierBench>(OH_Drawing_PenLineCapStyle::LINE_SQUARE_CAP, OH_Drawing_PenLineJoinStyle::LINE_BEVEL_JOIN, 10, BezierBench::DrawType::draw_quad); }},
        {"drawpenbeizer_cubic_round_miter_2", []() -> std::shared_ptr<TestBase> { return std::make_shared<BezierBench>(OH_Drawing_PenLineCapStyle::LINE_ROUND_CAP, OH_Drawing_PenLineJoinStyle::LINE_MITER_JOIN, 50, BezierBench::DrawType::draw_quad); }},
        {"clearbench", []() -> std::shared_ptr<TestBase> { return std::make_shared<ClearBench>(kPartial_ClearType); }},
        {"drawpathrotate_45", []() -> std::shared_ptr<TestBase> { return std::make_shared<RotatedRectBench>(45); }},
        {"drawpathrotate_60", []() -> std::shared_ptr<TestBase> { return std::make_shared<RotatedRectBench>(60); }},
        {"drawpathquadto", []() -> std::shared_ptr<TestBase> { return std::make_shared<LongCurvedPathBench>(); }}, // drawpath, path由PathQuadTo构造
        {"drawbigpath_left_round", []() -> std::shared_ptr<TestBase> { return std::make_shared<BigPathBench>(BigPathBench::Align::kLeft_Align, true); }},
        {"drawbigpath_middle_round", []() -> std::shared_ptr<TestBase> { return std::make_shared<BigPathBench>(BigPathBench::Align::kMiddle_Align, true); }},
        {"drawbigpath_right_round", []() -> std::shared_ptr<TestBase> { return std::make_shared<BigPathBench>(BigPathBench::Align::kRight_Align, true); }},
        {"drawbigpath_left_na", []() -> std::shared_ptr<TestBase> { return std::make_shared<BigPathBench>(BigPathBench::Align::kLeft_Align, false); }},
        {"drawbigpath_middle_na", []() -> std::shared_ptr<TestBase> { return std::make_shared<BigPathBench>(BigPathBench::Align::kMiddle_Align, false); }},
        {"drawbigpath_right_na", []() -> std::shared_ptr<TestBase> { return std::make_shared<BigPathBench>(BigPathBench::Align::kRight_Align, false); }},
        {"blitmaskbench_maskopaque", []() -> std::shared_ptr<TestBase> { return std::make_shared<BlitMaskBench>(BlitMaskBench::PointMode::kPoints_PointMode, BlitMaskBench::kMaskType::kMaskOpaque); }},
        {"blitmaskbench_maskblack", []() -> std::shared_ptr<TestBase> { return std::make_shared<BlitMaskBench>(BlitMaskBench::PointMode::kPoints_PointMode, BlitMaskBench::kMaskType::kMaskBlack); }},
        {"blitmaskbench_maskcolor", []() -> std::shared_ptr<TestBase> { return std::make_shared<BlitMaskBench>(BlitMaskBench::PointMode::kPoints_PointMode, BlitMaskBench::kMaskType::kMaskColor); }},
        {"blitmaskbench_maskshader", []() -> std::shared_ptr<TestBase> { return std::make_shared<BlitMaskBench>(BlitMaskBench::PointMode::kPoints_PointMode, BlitMaskBench::kMaskType::kMaskColor); }},
        // skbench_kadd：创建一个矩阵对象将原路径矩阵变换后添加到当前路径中.
        {"skbench_kadd", []() -> std::shared_ptr<TestBase> { return std::make_shared<SkBench_AddPathTest>(kAdd_AddType); }},
        // skbench_kaddtrans：设置矩阵为单位矩阵并平移(dx, dy)后将原路径矩阵变换添加到当前路径中.
        {"skbench_kaddtrans", []() -> std::shared_ptr<TestBase> { return std::make_shared<SkBench_AddPathTest>(kAddTrans_AddType); }},
        // skbench_kaddmatrix：设置矩阵对象的参数后将原路径矩阵变换添加到当前路径中.
        {"skbench_kaddmatrix", []() -> std::shared_ptr<TestBase> { return std::make_shared<SkBench_AddPathTest>(kAddMatrix_AddType); }},
        {"drawtextblobcreate_text_src", []() -> std::shared_ptr<TestBase> { return std::make_shared<XfermodeBench>(XfermodeBench::FromText, BLEND_MODE_SRC); }},
        {"drawtextblobcreate_text_clear", []() -> std::shared_ptr<TestBase> { return std::make_shared<XfermodeBench>(XfermodeBench::FromText, BLEND_MODE_CLEAR); }},
        {"drawtextblobcreate_text_color", []() -> std::shared_ptr<TestBase> { return std::make_shared<XfermodeBench>(XfermodeBench::FromText, BLEND_MODE_COLOR); }},
        {"drawtextblobcreate_text_diff", []() -> std::shared_ptr<TestBase> { return std::make_shared<XfermodeBench>(XfermodeBench::FromText, BLEND_MODE_DIFFERENCE); }},
        //HardStopGradientBench：kClamp、kRepeat、kMirror着色器效果平铺模式；4、25、100次数
        {"HardStopGradientBench_clamp_4", []() -> std::shared_ptr<TestBase> { return std::make_shared<HardStopGradientBench_ScaleNumColors>(HardStopGradientBench_ScaleNumColors::kClamp, 4);}}, 
        {"HardStopGradientBench_clamp_25", []() -> std::shared_ptr<TestBase> { return std::make_shared<HardStopGradientBench_ScaleNumColors>(HardStopGradientBench_ScaleNumColors::kClamp, 25);}},
        {"HardStopGradientBench_clamp_100", []() -> std::shared_ptr<TestBase> { return std::make_shared<HardStopGradientBench_ScaleNumColors>(HardStopGradientBench_ScaleNumColors::kClamp, 100);}},
        {"HardStopGradientBench_repeat_4", []() -> std::shared_ptr<TestBase> { return std::make_shared<HardStopGradientBench_ScaleNumColors>(HardStopGradientBench_ScaleNumColors::kRepeat, 4);}}, 
        {"HardStopGradientBench_repeat_25", []() -> std::shared_ptr<TestBase> { return std::make_shared<HardStopGradientBench_ScaleNumColors>(HardStopGradientBench_ScaleNumColors::kRepeat, 25);}},
        {"HardStopGradientBench_repeat_100", []() -> std::shared_ptr<TestBase> { return std::make_shared<HardStopGradientBench_ScaleNumColors>(HardStopGradientBench_ScaleNumColors::kRepeat, 100);}},
        {"HardStopGradientBench_mirror_4", []() -> std::shared_ptr<TestBase> { return std::make_shared<HardStopGradientBench_ScaleNumColors>(HardStopGradientBench_ScaleNumColors::kMirror, 4);}}, 
        {"HardStopGradientBench_mirror_25", []() -> std::shared_ptr<TestBase> { return std::make_shared<HardStopGradientBench_ScaleNumColors>(HardStopGradientBench_ScaleNumColors::kMirror, 25);}},
        {"HardStopGradientBench_mirror_100", []() -> std::shared_ptr<TestBase> { return std::make_shared<HardStopGradientBench_ScaleNumColors>(HardStopGradientBench_ScaleNumColors::kMirror, 100);}},
        {"canvas_drawrect", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasDrawRect>(TestBase::DRAW_STYLE_COMPLEX); }},

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
