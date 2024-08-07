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

#ifndef MYAPPLICATION_CANVAS_DRAW_TEXT_BLOB_TEXT_H
#define MYAPPLICATION_CANVAS_DRAW_TEXT_BLOB_TEXT_H
#include "test_base.h"
#include "test_common.h"
class CanvasDrawTextBlobLong : public TestBase {
public:
    explicit CanvasDrawTextBlobLong(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawTextBlobLong";
    };
    ~CanvasDrawTextBlobLong() override {};
protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
class CanvasDrawTextBlobLongPen : public TestBase {
public:
    explicit CanvasDrawTextBlobLongPen(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawTextBlobLongPen";
    };
    ~CanvasDrawTextBlobLongPen() override {};
protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
class CanvasDrawTextBlobLongBrush : public TestBase {
public:
    explicit CanvasDrawTextBlobLongBrush(int type) : TestBase(type)
    {
        fileName_ = "CanvasClipPathLineBrush";
    };
    ~CanvasDrawTextBlobLongBrush() override {};
protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};


class CanvasDrawTextBlobMax : public TestBase {
public:
    explicit CanvasDrawTextBlobMax(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawTextBlobMax";
    };
    ~CanvasDrawTextBlobMax() override {};
protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
class CanvasDrawTextBlobMaxPen : public TestBase {
public:
    explicit CanvasDrawTextBlobMaxPen(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawTextBlobMaxPen";
    };
    ~CanvasDrawTextBlobMaxPen() override {};
protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
class CanvasDrawTextBlobMaxBrush : public TestBase {
public:
    explicit CanvasDrawTextBlobMaxBrush(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawTextBlobMaxBrush";
    };
    ~CanvasDrawTextBlobMaxBrush() override {};
protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
#endif //MYAPPLICATION_CANVAS_DRAW_TEXT_BLOB_TEXT_H
