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
#ifndef Convex_Paths_H
#define Convex_Paths_H
#include "../test_base.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_mask_filter.h>
#include <native_drawing/drawing_filter.h>
#include "common/log_common.h"
#include "test_common.h"

#define SkIntToScalar(x) static_cast<SkScalar>(x)

class ConvexPaths : public TestBase {
  public:
    ConvexPaths() {
        bitmapWidth_ = 1200;
        bitmapHeight_ = 1100;
        fileName_ = "convexpaths";
    }
    ~ConvexPaths() = default;
    void MakePath();

  protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
};

#endif // Convex_Paths_H
