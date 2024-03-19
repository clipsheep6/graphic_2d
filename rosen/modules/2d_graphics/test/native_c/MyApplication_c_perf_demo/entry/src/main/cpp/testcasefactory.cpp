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
#include "testcase/draw_rect.h"
#include "testcase/draw_path.h"
#include "testcase/draw_textblob.h"
#include "testcase/draw_bitmap.h"
#include "testcase/draw_bitmaprect.h"
#include "testcase/canvas_save_restore.h"
#include "testcase/canvas_save_layer.h"
#include "testcase/draw_line.h"
#include "testcase/draw_circle.h"
#include "testcase/draw_oval.h"
#include "testcase/draw_round_rect.h"
#include "testcase/aa_clip_bench.h"
#include "testcase/draw_imagerect.h"
#include "testcase/draw_path_arc_to.h"
#include "testcase/draw_path_add_rect.h"
#include "testcase/draw_path_add_round_rect.h"
#include "testcase/draw_path_transform.h"
#include "testcase/draw_path_line_to.h"
#include "testcase/draw_shader_effect.h"
#include "testcase/draw_text_blob_create.h"

namespace {
    std::unordered_map<std::string, std::function<std::shared_ptr<TestBase>()>> FunctionalCpuMap =
        {
            {"drawrect", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawRect>(1); }},
            {"drawpath", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawPath>(); }},
            {"drawtextblob", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawTextBlob>(); }},
            {"drawbitmap", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawBitmap>(); }},
            {"drawbitmaprect", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawBitmapRect>(); }},
            {"saverestore8", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasSaveRestore>(); }},
            {"savelayer", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasSaveLayer>(); }},
            {"drawline", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawLine>(); }},
            {"drawcircle", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawCircle>(); }},
            {"drawoval", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawOval>(1); }},
            {"drawroundrect", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawRountRect>(1); }},
            {"clippath", []() -> std::shared_ptr<TestBase> { return std::make_shared<AaClipBench>(true, true); }},
            {"cliprect", []() -> std::shared_ptr<TestBase> { return std::make_shared<AaClipBench>(false, true); }},
            {"drawimagerect", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawImageRect>(); }},
            {"drawpatharcto", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawPathArcTo>(); }},
            {"drawpathaddrect", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawPathAddRect>(); }},
            {"drawpathaddroundrect", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawPathAddRoundRect>(); }},
            {"drawpathtransform", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawPathTransform>(); }},
            {"drawpathlineto", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawPathLineTo>(); } },
            {"drawshaderlinear_oval", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawShaderEffect>(true,0,2); }},
            {"drawshaderradial_oval", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawShaderEffect>(true,1,2); }},
            {"drawshadersweep_oval", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawShaderEffect>(true,2,2); }},
            {"drawshaderlinear_rect", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawShaderEffect>(false,0,2); }},
            {"drawshaderradial_rect", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawShaderEffect>(false,1,2); }},
            {"drawshadersweep_rect", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawShaderEffect>(false,2,2); }},
            {"drawtextblobcreate_text", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawTextBlobCreate>(1); }},
            {"drawtextblobcreate_pos", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawTextBlobCreate>(2); }},
            {"drawtextblobcreate_string", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawTextBlobCreate>(3); }},
    };

    std::unordered_map<std::string, std::function<std::shared_ptr<TestBase>()>>
        PerformanceCpuMap = {
            {"drawrect", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawRect>(1); }},
            {"drawpath", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawPath>(); }},
            {"drawtextblob", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawTextBlob>(); }},
            {"drawbitmap", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawBitmap>(); }},
            {"drawbitmaprect", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawBitmapRect>(); }},
            {"saverestore8", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasSaveRestore>(); }},
            {"savelayer", []() -> std::shared_ptr<TestBase> { return std::make_shared<CanvasSaveLayer>(); }},
            {"drawline", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawLine>(); }},
            {"drawcircle", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawCircle>(); }},
            {"drawoval", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawOval>(1); }},
            {"drawroundrect", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawRountRect>(1); }},
            {"clippath", []() -> std::shared_ptr<TestBase> { return std::make_shared<AaClipBench>(true, true); }},
            {"cliprect", []() -> std::shared_ptr<TestBase> { return std::make_shared<AaClipBench>(false, true); }},
            {"drawimagerect", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawImageRect>(); }},
            {"drawpatharcto", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawPathArcTo>(); }},
            {"drawpathaddrect", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawPathAddRect>(); }},
            {"drawpathaddroundrect", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawPathAddRoundRect>(); }},
            {"drawpathtransform", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawPathTransform>(); }},
            {"drawpathlineto", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawPathLineTo>(); } },
            {"drawshaderlinear_oval", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawShaderEffect>(true,0,2); }},
            {"drawshaderradial_oval", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawShaderEffect>(true,1,2); }},
            {"drawshadersweep_oval", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawShaderEffect>(true,2,2); }},
            {"drawshaderlinear_rect", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawShaderEffect>(false,0,2); }},
            {"drawshaderradial_rect", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawShaderEffect>(false,1,2); }},
            {"drawshadersweep_rect", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawShaderEffect>(false,2,2); }},
            {"drawtextblobcreate_text", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawTextBlobCreate>(1); }},
            {"drawtextblobcreate_pos", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawTextBlobCreate>(2); }},
            {"drawtextblobcreate_string", []() -> std::shared_ptr<TestBase> { return std::make_shared<DrawTextBlobCreate>(3); }},
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
