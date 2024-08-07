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

#ifndef SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_OPSWITCH
#define SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_OPSWITCH
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_rect.h>

#include "test_base.h"
#include "test_common.h"

class SubBasicGraphicsSpecialPerformanceCOpsWitch : public TestBase {
public:
    explicit SubBasicGraphicsSpecialPerformanceCOpsWitch(int type) : TestBase(type)
    {
        fileName_ = "SubBasicGraphicsSpecialPerformanceCOpsWitch";
    }
    ~SubBasicGraphicsSpecialPerformanceCOpsWitch() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SubBasicGraphicsSpecialPerformanceCOpsWitchPen : public TestBase {
public:
    explicit SubBasicGraphicsSpecialPerformanceCOpsWitchPen(int type) : TestBase(type)
    {
        fileName_ = "SubBasicGraphicsSpecialPerformanceCOpsWitchPen";
    }
    ~SubBasicGraphicsSpecialPerformanceCOpsWitchPen() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SubBasicGraphicsSpecialPerformanceCOpsWitchBrush : public TestBase {
public:
    explicit SubBasicGraphicsSpecialPerformanceCOpsWitchBrush(int type) : TestBase(type)
    {
        fileName_ = "SubBasicGraphicsSpecialPerformanceCOpsWitchBrush";
    }
    ~SubBasicGraphicsSpecialPerformanceCOpsWitchBrush() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SubBasicGraphicsSpecialPerformanceCLineAndPath : public TestBase {
public:
    explicit SubBasicGraphicsSpecialPerformanceCLineAndPath(int type) : TestBase(type)
    {
        fileName_ = "SubBasicGraphicsSpecialPerformanceCLineAndPath";
    }
    ~SubBasicGraphicsSpecialPerformanceCLineAndPath() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SubBasicGraphicsSpecialPerformanceCLineAndPathPen : public TestBase {
public:
    explicit SubBasicGraphicsSpecialPerformanceCLineAndPathPen(int type) : TestBase(type)
    {
        fileName_ = "SubBasicGraphicsSpecialPerformanceCLineAndPathPen";
    }
    ~SubBasicGraphicsSpecialPerformanceCLineAndPathPen() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class SubBasicGraphicsSpecialPerformanceCLineAndPathBrush : public TestBase {
public:
    explicit SubBasicGraphicsSpecialPerformanceCLineAndPathBrush(int type) : TestBase(type)
    {
        fileName_ = "SubBasicGraphicsSpecialPerformanceCLineAndPathBrush";
    }
    ~SubBasicGraphicsSpecialPerformanceCLineAndPathBrush() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

#endif // SUB_BASIC_GRAPHICS_SPECIAL_PERFORMANCE_C_OPSWITCH