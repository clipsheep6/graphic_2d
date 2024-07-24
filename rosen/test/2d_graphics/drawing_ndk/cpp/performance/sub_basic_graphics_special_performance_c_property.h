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

#ifndef DRAWINGNDKTEST5_0_SUBBASICGRAPHICSSPECIALPERFORMANCECPROPERTY_H
#define DRAWINGNDKTEST5_0_SUBBASICGRAPHICSSPECIALPERFORMANCECPROPERTY_H
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_rect.h>

#include "test_base.h"
#include "test_common.h"

class PerformanceCCanvasDrawVerticesBrush : public TestBase {
public:
    explicit PerformanceCCanvasDrawVerticesBrush(int type) : TestBase(type)
    {
        fileName_ = "PerformanceCCanvasDrawVerticesBrush";
    }
    ~PerformanceCCanvasDrawVerticesBrush() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class PerformanceCCanvasReadPixels : public TestBase {
public:
    explicit PerformanceCCanvasReadPixels(int type) : TestBase(type)
    {
        fileName_ = "PerformanceCCanvasReadPixels";
    }
    ~PerformanceCCanvasReadPixels() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class PerformanceCCanvasReadPixelsPen : public TestBase {
public:
    explicit PerformanceCCanvasReadPixelsPen(int type) : TestBase(type)
    {
        fileName_ = "PerformanceCCanvasReadPixelsPen";
    }
    ~PerformanceCCanvasReadPixelsPen() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class PerformanceCCanvasReadPixelsBrush : public TestBase {
public:
    explicit PerformanceCCanvasReadPixelsBrush(int type) : TestBase(type)
    {
        fileName_ = "PerformanceCCanvasReadPixelsBrush";
    }
    ~PerformanceCCanvasReadPixelsBrush() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class PerformanceCCanvasReadPixelsToBitmap : public TestBase {
public:
    explicit PerformanceCCanvasReadPixelsToBitmap(int type) : TestBase(type)
    {
        fileName_ = "PerformanceCCanvasReadPixelsToBitmap";
    }
    ~PerformanceCCanvasReadPixelsToBitmap() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class PerformanceCCanvasReadPixelsToBitmapPen : public TestBase {
public:
    explicit PerformanceCCanvasReadPixelsToBitmapPen(int type) : TestBase(type)
    {
        fileName_ = "PerformanceCCanvasReadPixelsToBitmapPen";
    }
    ~PerformanceCCanvasReadPixelsToBitmapPen() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class PerformanceCCanvasReadPixelsToBitmapBrush : public TestBase {
public:
    explicit PerformanceCCanvasReadPixelsToBitmapBrush(int type) : TestBase(type)
    {
        fileName_ = "PerformanceCCanvasReadPixelsToBitmapBrush";
    }
    ~PerformanceCCanvasReadPixelsToBitmapBrush() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class PerformanceCCanvasLargeImageLoadDrawImageRectWithSrc : public TestBase {
public:
    explicit PerformanceCCanvasLargeImageLoadDrawImageRectWithSrc(int type) : TestBase(type)
    {
        fileName_ = "PerformanceCCanvasLargeImageLoadDrawImageRectWithSrc";
    }
    ~PerformanceCCanvasLargeImageLoadDrawImageRectWithSrc() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class PerformanceCCanvasLargeImageLoadDrawImageRectWithSrcPen : public TestBase {
public:
    explicit PerformanceCCanvasLargeImageLoadDrawImageRectWithSrcPen(int type) : TestBase(type)
    {
        fileName_ = "PerformanceCCanvasLargeImageLoadDrawImageRectWithSrcPen";
    }
    ~PerformanceCCanvasLargeImageLoadDrawImageRectWithSrcPen() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class PerformanceCCanvasLargeImageLoadDrawImageRectWithSrcBrush : public TestBase {
public:
    explicit PerformanceCCanvasLargeImageLoadDrawImageRectWithSrcBrush(int type) : TestBase(type)
    {
        fileName_ = "PerformanceCCanvasLargeImageLoadDrawImageRectWithSrcBrush";
    }
    ~PerformanceCCanvasLargeImageLoadDrawImageRectWithSrcBrush() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class PerformanceCCanvasLargeImageLoadDrawImageRect : public TestBase {
public:
    explicit PerformanceCCanvasLargeImageLoadDrawImageRect(int type) : TestBase(type)
    {
        fileName_ = "PerformanceCCanvasLargeImageLoadDrawImageRect";
    }
    ~PerformanceCCanvasLargeImageLoadDrawImageRect() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class PerformanceCCanvasLargeImageLoadDrawImageRectPen : public TestBase {
public:
    explicit PerformanceCCanvasLargeImageLoadDrawImageRectPen(int type) : TestBase(type)
    {
        fileName_ = "PerformanceCCanvasLargeImageLoadDrawImageRectPen";
    }
    ~PerformanceCCanvasLargeImageLoadDrawImageRectPen() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class PerformanceCCanvasLargeImageLoadDrawImageRectBrush : public TestBase {
public:
    explicit PerformanceCCanvasLargeImageLoadDrawImageRectBrush(int type) : TestBase(type)
    {
        fileName_ = "PerformanceCCanvasLargeImageLoadDrawImageRectBrush";
    }
    ~PerformanceCCanvasLargeImageLoadDrawImageRectBrush() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class PerformanceCCanvasLargeImageLoadReadPixels : public TestBase {
public:
    explicit PerformanceCCanvasLargeImageLoadReadPixels(int type) : TestBase(type)
    {
        fileName_ = "PerformanceCCanvasLargeImageLoadReadPixels";
    }
    ~PerformanceCCanvasLargeImageLoadReadPixels() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class PerformanceCCanvasLargeImageLoadReadPixelsPen : public TestBase {
public:
    explicit PerformanceCCanvasLargeImageLoadReadPixelsPen(int type) : TestBase(type)
    {
        fileName_ = "PerformanceCCanvasLargeImageLoadReadPixelsPen";
    }
    ~PerformanceCCanvasLargeImageLoadReadPixelsPen() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class PerformanceCCanvasLargeImageLoadReadPixelsBrush : public TestBase {
public:
    explicit PerformanceCCanvasLargeImageLoadReadPixelsBrush(int type) : TestBase(type)
    {
        fileName_ = "PerformanceCCanvasLargeImageLoadReadPixelsBrush";
    }
    ~PerformanceCCanvasLargeImageLoadReadPixelsBrush() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

#endif // DRAWINGNDKTEST5_0_SUBBASICGRAPHICSSPECIALPERFORMANCECPROPERTY_H
