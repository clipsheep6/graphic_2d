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
#include "testcase/rect_bench.h"
#include "testcase/path_bench.h"
#include "testcase/textblob_cache_bench.h"
#include "testcase/draw_bitmap_aa_bench.h"
#include "testcase/bulk_rect_bench.h"
#include "testcase/canvas_save_restore.h"
#include "testcase/shader_mf_bench.h"
#include "testcase/dash_line_bench.h"
#include "testcase/clip_strategy_bench.h"
#include "testcase/aa_clip_bench.h"
#include "testcase/circles_bench.h"
#include "testcase/nested_aa_clip_bench.h"
#include "testcase/hairline_path_bench.h"
#include "testcase/gradient_bench.h"
#include "testcase/xfermode_bench.h"
#include "testcase/skbench_addpathtest.h"
#include "testcase/canvas_matrix_bench.h"
#include "testcase/shadow_bench.h"
#include "testcase/read_pix_bench.h"
#include "testcase/bezier_bench.h"
#include "testcase/clear_bench.h"

namespace {
    std::unordered_map<std::string, std::function<std::shared_ptr<TestBase>()>> FunctionalCpuMap =
        {
            {"drawrect", []() -> std::shared_ptr<TestBase> { return std::make_shared<RectBench>(1); }}, // drawrect 随机颜色，随机位置
            {"drawtextblob", []() -> std::shared_ptr<TestBase> { return std::make_shared<TextBlobCachedBench>(); }}, // drawtextblob
            {"drawbitmapnoaa", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawBitmapAABench>(); }}, // drawbitmap
            {"drawbitmaprect", []() -> std::shared_ptr<TestBase> { return std::make_shared<BulkRectBench>(BulkRectBench::BITMAP_RECT); }}, // drawbitmaprect 排列平铺到整个页面
            {"saverestore8", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasSaveRestore>(); }}, //8*(save+concat+restore)+drawcolor
            {"savelayer", []() -> std::shared_ptr<TestBase> { return std::make_shared<ShaderMFBench>(); }}, // savelayer+canvasclear+restore
            {"drawline", []() -> std::shared_ptr<TestBase> { return std::make_shared<DashLineBench>(); }}, // drawline
            {"drawcircle", []() -> std::shared_ptr<TestBase> { return std::make_shared<ClipStrategyBench>(); }}, // savelayer+createpoint+drawcircle
            {"drawoval", []() -> std::shared_ptr<TestBase> { return std::make_shared<OvalBench>(1); }}, // drawOval
            {"drawroundrect", []() -> std::shared_ptr<TestBase> { return std::make_shared<RRectBench>(1); }}, // DrawRoundRect
            {"clippath", []() -> std::shared_ptr<TestBase> { return std::make_shared<AAClipBench>(true, true); }}, // save clippath drawrect restore
            {"cliprect", []() -> std::shared_ptr<TestBase> { return std::make_shared<AAClipBench>(false, true); }}, // save cliprect drawrect restore
            {"drawimagerect", []() -> std::shared_ptr<TestBase> { return std::make_shared<BulkRectBench>(BulkRectBench::IMAGE_RECT); }}, // drawimagerect
            {"drawpatharcto", []() -> std::shared_ptr<TestBase> { return std::make_shared<CirclesBench>(); }}, // patharcto*2 + pathclose + drawpath
            {"drawpathaddrect", []() -> std::shared_ptr<TestBase> { return std::make_shared<RectPathBench>(); }}, // drawpath, path由AddRect构造
            {"drawpathaddroundrect", []() -> std::shared_ptr<TestBase> { return std::make_shared<NestedAAClipBench>(); }},  // path由addrrrect构建，canvas clippath：在此背景上drawrect，随机alpha
            {"drawpathlineto", []() -> std::shared_ptr<TestBase> { return std::make_shared<TrianglePathBench>(); } }, // drawpath, path由lineto构造
            {"drawpathcubicto", []() -> std::shared_ptr<TestBase> { return std::make_shared<CubicPathBench>(); }}, // drawpath, path先由Cubic构造再进行transform
            {"drawshaderlinear_oval", []() -> std::shared_ptr<TestBase> { return std::make_shared<GradientBench>(true,GradientBench::LINEAR,2); }}, // 主循环drawoval, penseteffect（由linear构造）
            {"drawshaderradial_oval", []() -> std::shared_ptr<TestBase> { return std::make_shared<GradientBench>(true,GradientBench::RADIAL,2); }}, // 主循环drawoval, penseteffect（由radial构造）
            {"drawshadersweep_oval", []() -> std::shared_ptr<TestBase> { return std::make_shared<GradientBench>(true,GradientBench::SWEEP,2); }}, // 主循环drawoval, penseteffect（由sweep构造）
            {"drawshaderlinear_rect", []() -> std::shared_ptr<TestBase> { return std::make_shared<GradientBench>(false,GradientBench::LINEAR,2); }}, // 主循环drawrect, penseteffect（由linear构造）
            {"drawshaderradial_rect", []() -> std::shared_ptr<TestBase> { return std::make_shared<GradientBench>(false,GradientBench::RADIAL,2); }}, // 主循环drawrect, penseteffect（由radial构造）
            {"drawshadersweep_rect", []() -> std::shared_ptr<TestBase> { return std::make_shared<GradientBench>(false,GradientBench::SWEEP,2); }}, // 主循环drawrect, penseteffect（由sweep构造）
            {"drawtextblobcreate_text", []() -> std::shared_ptr<TestBase> { return std::make_shared<XfermodeBench>(1); }}, // DrawTextBlob, textblob由createformtext创建，每drawtextblob 1000次就重新创建一下textblob
            {"drawtextblobcreate_pos", []() -> std::shared_ptr<TestBase> { return std::make_shared<XfermodeBench>(2); }}, // DrawTextBlob, textblob由createformtextpos创建，每drawtextblob 1000次就重新创建一下textblob
            {"drawtextblobcreate_string", []() -> std::shared_ptr<TestBase> { return std::make_shared<XfermodeBench>(3); }}, // DrawTextBlob, textblob由createformstring创建，每drawtextblob 1000次就重新创建一下textblob
             //skbench_kaddbrush：创建一个矩阵对象将原路径矩阵变换后添加到当前路径中.PAINTBRUSH_RECT设置画刷给画布并随机画布颜色
            {"skbench_kaddbrush", []() -> std::shared_ptr<TestBase> { return std::make_shared<SkBench_AddPathTest>(kAdd_AddType, PAINTBRUSH_RECT); }},
             //skbench_kaddtransbrush：设置矩阵为单位矩阵并平移(dx, dy)后将原路径矩阵变换添加到当前路径中.PAINTBRUSH_RECT设置画刷给画布并随机画布颜色
            {"skbench_kaddtransbrush", []() -> std::shared_ptr<TestBase> { return std::make_shared<SkBench_AddPathTest>(kAddTrans_AddType, PAINTBRUSH_RECT); }},
             //skbench_kaddmatrixbrush：设置矩阵对象的参数后将原路径矩阵变换添加到当前路径中.PAINTBRUSH_RECT设置画刷给画布并随机画布颜色
            {"skbench_kaddmatrixbrush", []() -> std::shared_ptr<TestBase> { return std::make_shared<SkBench_AddPathTest>(kAddMatrix_AddType, PAINTBRUSH_RECT); }},
             //skbench_kadd：创建一个矩阵对象将原路径矩阵变换后添加到当前路径中.NO_BRUSH_RECT无画布使用画笔更清晰直观看路径变化
            {"skbench_kadd", []() -> std::shared_ptr<TestBase> { return std::make_shared<SkBench_AddPathTest>(kAdd_AddType, NO_BRUSH_RECT); }},
             //skbench_kaddtrans：设置矩阵为单位矩阵并平移(dx, dy)后将原路径矩阵变换添加到当前路径中.NO_BRUSH_RECT无画布使用画笔更清晰直观看路径变化
            {"skbench_kaddtrans", []() -> std::shared_ptr<TestBase> { return std::make_shared<SkBench_AddPathTest>(kAddTrans_AddType, NO_BRUSH_RECT); }},
             //skbench_kaddmatrix：设置矩阵对象的参数后将原路径矩阵变换添加到当前路径中.NO_BRUSH_RECT无画布使用画笔更清晰直观看路径变化
            {"skbench_kaddmatrix", []() -> std::shared_ptr<TestBase> { return std::make_shared<SkBench_AddPathTest>(kAddMatrix_AddType, NO_BRUSH_RECT); }},
            {"drawcanvasscale_translate", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasMatrixBench>(CanvasMatrixBench::CanvasType::Translate); }},
            {"drawcanvasscale_scale", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasMatrixBench>(CanvasMatrixBench::CanvasType::Scale); }},
            {"drawcanvasscale_matrix", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasMatrixBench>(CanvasMatrixBench::CanvasType::ConcatMatrix); }},
            {"drawcanvasdrawshadow_t_g", []() -> std::shared_ptr<TestBase> { return std::make_shared<ShadowBench>(true,true); }},
            {"drawcanvasdrawshadow_o_a", []() -> std::shared_ptr<TestBase> { return std::make_shared<ShadowBench>(false,false); }},
            {"drawcanvasdrawshadow_t_a", []() -> std::shared_ptr<TestBase> { return std::make_shared<ShadowBench>(true,false); }},
            {"drawcanvasdrawshadow_o_g", []() -> std::shared_ptr<TestBase> { return std::make_shared<ShadowBench>(false,true); }},
            {"drawcanvasreadpixels", []() -> std::shared_ptr<TestBase> { return std::make_shared<ReadPixBench>(false); }},
            {"drawcanvasreadpixels_tobitmap", []() -> std::shared_ptr<TestBase> { return std::make_shared<ReadPixBench>(true); }},
            {"drawpenrect_1_0_aa", []() -> std::shared_ptr<TestBase> { return std::make_shared<RectBench>(1,false,0,true); }},
            {"drawpenrect_1_4_aa", []() -> std::shared_ptr<TestBase> { return std::make_shared<RectBench>(1,false,4,true); }},
            {"drawpenrect_3_0_aa", []() -> std::shared_ptr<TestBase> { return std::make_shared<RectBench>(3,false,0,true); }},
            {"drawpenrect_3_4_aa", []() -> std::shared_ptr<TestBase> { return std::make_shared<RectBench>(3,false,4,true); }},
            {"drawpenrect_1_0_bw", []() -> std::shared_ptr<TestBase> { return std::make_shared<RectBench>(1,false,0,false); }},
            {"drawpenrect_1_4_bw", []() -> std::shared_ptr<TestBase> { return std::make_shared<RectBench>(1,false,4,false); }},
            {"drawpenrect_3_0_bw", []() -> std::shared_ptr<TestBase> { return std::make_shared<RectBench>(3,false,0,false); }},
            {"drawpenrect_3_4_bw", []() -> std::shared_ptr<TestBase> { return std::make_shared<RectBench>(3,false,4,false); }},
            {"drawpenrectsrcmode", []() -> std::shared_ptr<TestBase> { return std::make_shared<SrcModeRectBench>(); }},
            {"drawpenbeizer_quad_butt_round_2", []() -> std::shared_ptr<TestBase> { return std::make_shared<BezierBench>(OH_Drawing_PenLineCapStyle::LINE_FLAT_CAP,OH_Drawing_PenLineJoinStyle::LINE_ROUND_JOIN,2,BezierBench::DrawType::draw_quad); }},
            {"drawpenbeizer_quad_square_bevel_10", []() -> std::shared_ptr<TestBase> { return std::make_shared<BezierBench>(OH_Drawing_PenLineCapStyle::LINE_SQUARE_CAP,OH_Drawing_PenLineJoinStyle::LINE_BEVEL_JOIN,10,BezierBench::DrawType::draw_quad); }},
            {"drawpenbeizer_quad_round_miter_50", []() -> std::shared_ptr<TestBase> { return std::make_shared<BezierBench>(OH_Drawing_PenLineCapStyle::LINE_ROUND_CAP,OH_Drawing_PenLineJoinStyle::LINE_MITER_JOIN,50,BezierBench::DrawType::draw_quad); }},
            {"drawpenbeizer_cubic_butt_round_2", []() -> std::shared_ptr<TestBase> { return std::make_shared<BezierBench>(OH_Drawing_PenLineCapStyle::LINE_FLAT_CAP,OH_Drawing_PenLineJoinStyle::LINE_ROUND_JOIN,2,BezierBench::DrawType::draw_quad); }},
            {"drawpenbeizer_cubic_square_bevel_2", []() -> std::shared_ptr<TestBase> { return std::make_shared<BezierBench>(OH_Drawing_PenLineCapStyle::LINE_SQUARE_CAP,OH_Drawing_PenLineJoinStyle::LINE_BEVEL_JOIN,10,BezierBench::DrawType::draw_quad); }},
            {"drawpenbeizer_cubic_round_miter_2", []() -> std::shared_ptr<TestBase> { return std::make_shared<BezierBench>(OH_Drawing_PenLineCapStyle::LINE_ROUND_CAP,OH_Drawing_PenLineJoinStyle::LINE_MITER_JOIN,50,BezierBench::DrawType::draw_quad); }},
            {"clearbench", []() -> std::shared_ptr<TestBase> { return std::make_shared<ClearBench>(kPartial_ClearType); }},
            {"drawpathrotate_45", []() -> std::shared_ptr<TestBase> { return std::make_shared<RotatedRectBench>(45); }},
            {"drawpathrotate_60", []() -> std::shared_ptr<TestBase> { return std::make_shared<RotatedRectBench>(60); }},
    };

    std::unordered_map<std::string, std::function<std::shared_ptr<TestBase>()>>
        PerformanceCpuMap = {
            {"drawrect", []() -> std::shared_ptr<TestBase> { return std::make_shared<RectBench>(1); }}, // drawrect 随机颜色，随机位置
            {"drawtextblob", []() -> std::shared_ptr<TestBase> { return std::make_shared<TextBlobCachedBench>(); }}, // drawtextblob
            {"drawbitmapnoaa", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawBitmapAABench>(); }}, // drawbitmap
            {"drawbitmaprect", []() -> std::shared_ptr<TestBase> { return std::make_shared<BulkRectBench>(BulkRectBench::BITMAP_RECT); }}, // drawbitmaprect 排列平铺到整个页面
            {"saverestore8", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasSaveRestore>(); }}, //8*(save+concat+restore)+drawcolor
            {"savelayer", []() -> std::shared_ptr<TestBase> { return std::make_shared<ShaderMFBench>(); }}, // savelayer+canvasclear+restore
            {"drawline", []() -> std::shared_ptr<TestBase> { return std::make_shared<DashLineBench>(); }}, // drawline
            {"drawcircle", []() -> std::shared_ptr<TestBase> { return std::make_shared<ClipStrategyBench>(); }}, // savelayer+createpoint+drawcircle
            {"drawoval", []() -> std::shared_ptr<TestBase> { return std::make_shared<OvalBench>(1); }}, // drawOval
            {"drawroundrect", []() -> std::shared_ptr<TestBase> { return std::make_shared<RRectBench>(1); }}, // DrawRoundRect
            {"clippath", []() -> std::shared_ptr<TestBase> { return std::make_shared<AAClipBench>(true, true); }}, // save clippath drawrect restore
            {"cliprect", []() -> std::shared_ptr<TestBase> { return std::make_shared<AAClipBench>(false, true); }}, // save cliprect drawrect restore
            {"drawimagerect", []() -> std::shared_ptr<TestBase> { return std::make_shared<BulkRectBench>(BulkRectBench::IMAGE_RECT); }}, // drawimagerect
            {"drawpatharcto", []() -> std::shared_ptr<TestBase> { return std::make_shared<CirclesBench>(); }}, // patharcto*2 + pathclose + drawpath
            {"drawpathaddrect", []() -> std::shared_ptr<TestBase> { return std::make_shared<RectPathBench>(); }}, // drawpath, path由AddRect构造
            {"drawpathaddroundrect", []() -> std::shared_ptr<TestBase> { return std::make_shared<NestedAAClipBench>(); }},  // path由addrrrect构建，canvas clippath：在此背景上drawrect，随机alpha
            {"drawpathlineto", []() -> std::shared_ptr<TestBase> { return std::make_shared<TrianglePathBench>(); } }, // drawpath, path由lineto构造
            {"drawpathcubicto", []() -> std::shared_ptr<TestBase> { return std::make_shared<CubicPathBench>(); }}, // drawpath, path先由Cubic构造再进行transform
            {"drawshaderlinear_oval", []() -> std::shared_ptr<TestBase> { return std::make_shared<GradientBench>(true,GradientBench::LINEAR,2); }}, // 主循环drawoval, penseteffect（由linear构造）
            {"drawshaderradial_oval", []() -> std::shared_ptr<TestBase> { return std::make_shared<GradientBench>(true,GradientBench::RADIAL,2); }}, // 主循环drawoval, penseteffect（由radial构造）
            {"drawshadersweep_oval", []() -> std::shared_ptr<TestBase> { return std::make_shared<GradientBench>(true,GradientBench::SWEEP,2); }}, // 主循环drawoval, penseteffect（由sweep构造）
            {"drawshaderlinear_rect", []() -> std::shared_ptr<TestBase> { return std::make_shared<GradientBench>(false,GradientBench::LINEAR,2); }}, // 主循环drawrect, penseteffect（由linear构造）
            {"drawshaderradial_rect", []() -> std::shared_ptr<TestBase> { return std::make_shared<GradientBench>(false,GradientBench::RADIAL,2); }}, // 主循环drawrect, penseteffect（由radial构造）
            {"drawshadersweep_rect", []() -> std::shared_ptr<TestBase> { return std::make_shared<GradientBench>(false,GradientBench::SWEEP,2); }}, // 主循环drawrect, penseteffect（由sweep构造）
            {"drawtextblobcreate_text", []() -> std::shared_ptr<TestBase> { return std::make_shared<XfermodeBench>(1); }}, // DrawTextBlob, textblob由createformtext创建，每drawtextblob 1000次就重新创建一下textblob
            {"drawtextblobcreate_pos", []() -> std::shared_ptr<TestBase> { return std::make_shared<XfermodeBench>(2); }}, // DrawTextBlob, textblob由createformtextpos创建，每drawtextblob 1000次就重新创建一下textblob
            {"drawtextblobcreate_string", []() -> std::shared_ptr<TestBase> { return std::make_shared<XfermodeBench>(3); }}, // DrawTextBlob, textblob由createformstring创建，每drawtextblob 1000次就重新创建一下textblob
            {"drawcanvasscale_translate", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasMatrixBench>(CanvasMatrixBench::CanvasType::Translate); }},
            {"drawcanvasscale_scale", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasMatrixBench>(CanvasMatrixBench::CanvasType::Scale); }},
            {"drawcanvasscale_matrix", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasMatrixBench>(CanvasMatrixBench::CanvasType::ConcatMatrix); }},
            {"drawcanvasdrawshadow_t_g", []() -> std::shared_ptr<TestBase> { return std::make_shared<ShadowBench>(true,true); }},
            {"drawcanvasdrawshadow_o_a", []() -> std::shared_ptr<TestBase> { return std::make_shared<ShadowBench>(false,false); }},
            {"drawcanvasdrawshadow_t_a", []() -> std::shared_ptr<TestBase> { return std::make_shared<ShadowBench>(true,false); }},
            {"drawcanvasdrawshadow_o_g", []() -> std::shared_ptr<TestBase> { return std::make_shared<ShadowBench>(false,true); }},
            {"drawcanvasreadpixels", []() -> std::shared_ptr<TestBase> { return std::make_shared<ReadPixBench>(false); }},
            {"drawcanvasreadpixels_tobitmap", []() -> std::shared_ptr<TestBase> { return std::make_shared<ReadPixBench>(true); }},
            {"drawpenrect_1_0_aa", []() -> std::shared_ptr<TestBase> { return std::make_shared<RectBench>(1,false,0,true); }},
            {"drawpenrect_1_4_aa", []() -> std::shared_ptr<TestBase> { return std::make_shared<RectBench>(1,false,4,true); }},
            {"drawpenrect_3_0_aa", []() -> std::shared_ptr<TestBase> { return std::make_shared<RectBench>(3,false,0,true); }},
            {"drawpenrect_3_4_aa", []() -> std::shared_ptr<TestBase> { return std::make_shared<RectBench>(3,false,4,true); }},
            {"drawpenrect_1_0_bw", []() -> std::shared_ptr<TestBase> { return std::make_shared<RectBench>(1,false,0,false); }},
            {"drawpenrect_1_4_bw", []() -> std::shared_ptr<TestBase> { return std::make_shared<RectBench>(1,false,4,false); }},
            {"drawpenrect_3_0_bw", []() -> std::shared_ptr<TestBase> { return std::make_shared<RectBench>(3,false,0,false); }},
            {"drawpenrect_3_4_bw", []() -> std::shared_ptr<TestBase> { return std::make_shared<RectBench>(3,false,4,false); }},
            {"drawpenrectsrcmode", []() -> std::shared_ptr<TestBase> { return std::make_shared<SrcModeRectBench>(); }},
            {"drawpenbeizer_quad_butt_round_2", []() -> std::shared_ptr<TestBase> { return std::make_shared<BezierBench>(OH_Drawing_PenLineCapStyle::LINE_FLAT_CAP,OH_Drawing_PenLineJoinStyle::LINE_ROUND_JOIN,2,BezierBench::DrawType::draw_quad); }},
            {"drawpenbeizer_quad_square_bevel_10", []() -> std::shared_ptr<TestBase> { return std::make_shared<BezierBench>(OH_Drawing_PenLineCapStyle::LINE_SQUARE_CAP,OH_Drawing_PenLineJoinStyle::LINE_BEVEL_JOIN,10,BezierBench::DrawType::draw_quad); }},
            {"drawpenbeizer_quad_round_miter_50", []() -> std::shared_ptr<TestBase> { return std::make_shared<BezierBench>(OH_Drawing_PenLineCapStyle::LINE_ROUND_CAP,OH_Drawing_PenLineJoinStyle::LINE_MITER_JOIN,50,BezierBench::DrawType::draw_quad); }},
            {"drawpenbeizer_cubic_butt_round_2", []() -> std::shared_ptr<TestBase> { return std::make_shared<BezierBench>(OH_Drawing_PenLineCapStyle::LINE_FLAT_CAP,OH_Drawing_PenLineJoinStyle::LINE_ROUND_JOIN,2,BezierBench::DrawType::draw_quad); }},
            {"drawpenbeizer_cubic_square_bevel_2", []() -> std::shared_ptr<TestBase> { return std::make_shared<BezierBench>(OH_Drawing_PenLineCapStyle::LINE_SQUARE_CAP,OH_Drawing_PenLineJoinStyle::LINE_BEVEL_JOIN,10,BezierBench::DrawType::draw_quad); }},
            {"drawpenbeizer_cubic_round_miter_2", []() -> std::shared_ptr<TestBase> { return std::make_shared<BezierBench>(OH_Drawing_PenLineCapStyle::LINE_ROUND_CAP,OH_Drawing_PenLineJoinStyle::LINE_MITER_JOIN,50,BezierBench::DrawType::draw_quad); }},
            {"drawtextblobcreate_text", []() -> std::shared_ptr<TestBase> { return std::make_shared<XfermodeBench>(1); }}, // DrawTextBlob, textblob由createformtext创建
            {"drawtextblobcreate_pos", []() -> std::shared_ptr<TestBase> { return std::make_shared<XfermodeBench>(2); }}, // DrawTextBlob, textblob由createformtextpos创建
            {"drawtextblobcreate_string", []() -> std::shared_ptr<TestBase> { return std::make_shared<XfermodeBench>(3); }}, // DrawTextBlob, textblob由createformstring创建
             //skbench_kaddbrush：创建一个矩阵对象将原路径矩阵变换后添加到当前路径中.PAINTBRUSH_RECT设置画刷给画布并随机画布颜色
            {"skbench_kaddbrush", []() -> std::shared_ptr<TestBase> { return std::make_shared<SkBench_AddPathTest>(kAdd_AddType, PAINTBRUSH_RECT); }},
             //skbench_kaddtransbrush：设置矩阵为单位矩阵并平移(dx, dy)后将原路径矩阵变换添加到当前路径中.PAINTBRUSH_RECT设置画刷给画布并随机画布颜色
            {"skbench_kaddtransbrush", []() -> std::shared_ptr<TestBase> { return std::make_shared<SkBench_AddPathTest>(kAddTrans_AddType, PAINTBRUSH_RECT); }},
             //skbench_kaddmatrixbrush：设置矩阵对象的参数后将原路径矩阵变换添加到当前路径中.PAINTBRUSH_RECT设置画刷给画布并随机画布颜色
            {"skbench_kaddmatrixbrush", []() -> std::shared_ptr<TestBase> { return std::make_shared<SkBench_AddPathTest>(kAddMatrix_AddType, PAINTBRUSH_RECT); }},
             //skbench_kadd：创建一个矩阵对象将原路径矩阵变换后添加到当前路径中.NO_BRUSH_RECT无画布使用画笔更清晰直观看路径变化
            {"skbench_kadd", []() -> std::shared_ptr<TestBase> { return std::make_shared<SkBench_AddPathTest>(kAdd_AddType, NO_BRUSH_RECT); }},
             //skbench_kaddtrans：设置矩阵为单位矩阵并平移(dx, dy)后将原路径矩阵变换添加到当前路径中.NO_BRUSH_RECT无画布使用画笔更清晰直观看路径变化
            {"skbench_kaddtrans", []() -> std::shared_ptr<TestBase> { return std::make_shared<SkBench_AddPathTest>(kAddTrans_AddType, NO_BRUSH_RECT); }},
             //skbench_kaddmatrix：设置矩阵对象的参数后将原路径矩阵变换添加到当前路径中.NO_BRUSH_RECT无画布使用画笔更清晰直观看路径变化
            {"skbench_kaddmatrix", []() -> std::shared_ptr<TestBase> { return std::make_shared<SkBench_AddPathTest>(kAddMatrix_AddType, NO_BRUSH_RECT); }},
            {"clearbench", []() -> std::shared_ptr<TestBase> { return std::make_shared<ClearBench>(kPartial_ClearType); }},
            {"drawpathrotate_45", []() -> std::shared_ptr<TestBase> { return std::make_shared<RotatedRectBench>(45); }},
            {"drawpathrotate_60", []() -> std::shared_ptr<TestBase> { return std::make_shared<RotatedRectBench>(60); }},
    };
} // namespace


std::shared_ptr<TestBase> TestCaseFactory::GetFunctionCpuCase(std::string caseName)
{
    if (FunctionalCpuMap.find(caseName) == FunctionalCpuMap.end()) {
        DRAWING_LOGE("caseName error");
        return nullptr;
    }
    
    return FunctionalCpuMap.find(caseName)->second();
}

std::shared_ptr<TestBase> TestCaseFactory::GetPerformanceCpuCase(std::string caseName)
{
    if (PerformanceCpuMap.find(caseName) == PerformanceCpuMap.end()) {
        DRAWING_LOGE("caseName error");
        return nullptr;
    }
    
    return PerformanceCpuMap.find(caseName)->second();
}

std::unordered_map<std::string, std::function<std::shared_ptr<TestBase>()>> TestCaseFactory::GetFunctionCpuCaseAll()
{   
    return FunctionalCpuMap;
}
