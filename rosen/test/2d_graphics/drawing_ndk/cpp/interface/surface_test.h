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

#ifndef INTERFACE_SURFACE_TEST_H
#define INTERFACE_SURFACE_TEST_H
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_rect.h>

#include "test_base.h"
#include "test_common.h"

class SurfaceCreateFromGpuContext : public TestBase {
public:
    SurfaceCreateFromGpuContext(int type, bool budgeted) : TestBase(type), budgeted(budgeted)
    {
        fileName_ = "SurfaceCreateFromGpuContext";
    };
    ~SurfaceCreateFromGpuContext() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
    bool budgeted;
};

#endif // INTERFACE_SURFACE_TEST_H