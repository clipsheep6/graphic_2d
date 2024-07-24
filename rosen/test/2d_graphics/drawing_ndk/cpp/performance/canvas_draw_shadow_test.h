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

#ifndef MYAPPLICATION_CANVAS_DRAW_SHADOW_TEST_H
#define MYAPPLICATION_CANVAS_DRAW_SHADOW_TEST_H
#include "test_base.h"
#include "test_common.h"

class CanvasDrawShadowLine : public TestBase {
public:
    explicit CanvasDrawShadowLine(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawShadowLine";
    };
    ~CanvasDrawShadowLine() override {};
protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
class CanvasDrawShadowLinePen : public TestBase {
public:
    explicit CanvasDrawShadowLinePen(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawShadowLinePen";
    };
    ~CanvasDrawShadowLinePen() override {};
protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
class CanvasDrawShadowLineBrush : public TestBase {
public:
    explicit CanvasDrawShadowLineBrush(int type) : TestBase(type)
    {
        fileName_ = "CanvasClipPathLineBrush";
    };
    ~CanvasDrawShadowLineBrush() override {};
protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class CanvasDrawShadowCurve : public TestBase {
public:
    explicit CanvasDrawShadowCurve(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawShadowCurve";
    };
    ~CanvasDrawShadowCurve() override {};
protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
class CanvasDrawShadowCurvePen : public TestBase {
public:
    explicit CanvasDrawShadowCurvePen(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawShadowCurvePen";
    };
    ~CanvasDrawShadowCurvePen() override {};
protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
class CanvasDrawShadowCurveBrush : public TestBase {
public:
    explicit CanvasDrawShadowCurveBrush(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawShadowCurveBrush";
    };
    ~CanvasDrawShadowCurveBrush() override {};
protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

#endif //MYAPPLICATION_CANVAS_DRAW_SHADOW_TEST_H
