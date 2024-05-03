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
#ifndef CONIC_PATHS_H
#define CONIC_PATHS_H
#include <bits/alltypes.h>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_mask_filter.h>
#include <native_drawing/drawing_rect.h>

#include "../test_base.h"
#include "test_common.h"

#include "common/log_common.h"

class ConicPaths : public TestBase {
public:
    ConicPaths()
    {
        bitmapWidth_ = 920;  // 920宽度
        bitmapHeight_ = 960; // 960高度
        fileName_ = "conicpaths";
    }
    ~ConicPaths() = default;
    void Makepath();

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
    std::vector<DrawRect> pathsBounds;
    std::vector<OH_Drawing_Path*> fPaths;
    //    const float w = sqrt(2.0f) / 2; // 2被除数
    void Makepath1();
    void Makepath2();
    void DrawSence(OH_Drawing_Canvas* canvas, OH_Drawing_Pen* pen, OH_Drawing_Brush* brush, uint8_t a, int p);
};

#endif // MyApplication_conic_paths_H
