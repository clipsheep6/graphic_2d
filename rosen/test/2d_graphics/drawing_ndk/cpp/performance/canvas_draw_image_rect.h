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

#ifndef MYAPPLICATION_CANVAS_DRAW_IMAGE_RECT_H
#define MYAPPLICATION_CANVAS_DRAW_IMAGE_RECT_H

#include "test_base.h"
#include "test_common.h"
class OHCanvasDrawImageRect : public TestBase {
public:
    explicit OHCanvasDrawImageRect(int type) : TestBase(type)
    {
        fileName_ = "OHCanvasDrawImageRect";
    };
    ~OHCanvasDrawImageRect() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class OHCanvasDrawImageRectPen : public TestBase {
public:
    explicit OHCanvasDrawImageRectPen(int type) : TestBase(type)
    {
        fileName_ = "OHCanvasDrawImageRectPen";
    };
    ~OHCanvasDrawImageRectPen() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class OHCanvasDrawImageRectBrush : public TestBase {
public:
    explicit OHCanvasDrawImageRectBrush(int type) : TestBase(type)
    {
        fileName_ = "OHCanvasDrawImageRectBrush";
    };
    ~OHCanvasDrawImageRectBrush() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

#endif //MYAPPLICATION_CANVAS_DRAW_IMAGE_RECT_H
