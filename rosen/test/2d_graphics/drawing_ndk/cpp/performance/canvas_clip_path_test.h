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
#ifndef MYAPPLICATION_CANVAS_CLIP_PATH_TEST_H
#define MYAPPLICATION_CANVAS_CLIP_PATH_TEST_H

#include "test_base.h"
class CanvasClipPathLine : public TestBase {
public:
    explicit CanvasClipPathLine(int type) : TestBase(type)
    {
        fileName_ = "CanvasClipPathLine";
    };
    ~CanvasClipPathLine() override {};
protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
class CanvasClipPathLinePen : public TestBase {
public:
    explicit CanvasClipPathLinePen(int type) : TestBase(type)
    {
        fileName_ = "CanvasClipPathCurvePen";
    };
    ~CanvasClipPathLinePen() override {};
protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
class CanvasClipPathLineBrush : public TestBase {
public:
    explicit CanvasClipPathLineBrush(int type) : TestBase(type)
    {
        fileName_ = "CanvasClipPathLineBrush";
    };
    ~CanvasClipPathLineBrush() override {};
protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
class CanvasClipPathCurve : public TestBase {
public:
    explicit CanvasClipPathCurve(int type) : TestBase(type)
    {
        fileName_ = "CanvasClipPathCurve";
    };
    ~CanvasClipPathCurve() override {};
protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
class CanvasClipPathCurvePen : public TestBase {
public:
    explicit CanvasClipPathCurvePen(int type) : TestBase(type)
    {
        fileName_ = "CanvasClipPathCurvePen";
    };
    ~CanvasClipPathCurvePen() override {};
protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
class CanvasClipPathCurveBrush : public TestBase {
public:
    explicit CanvasClipPathCurveBrush(int type) : TestBase(type)
    {
        fileName_ = "CanvasClipPathCurveBrush";
    };
    ~CanvasClipPathCurveBrush() override {};
protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

#endif //MYAPPLICATION_CANVAS_CLIP_PATH_TEST_H
