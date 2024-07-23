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

#ifndef TESTCASE_CANVAS_DRAWPOINTS_H
#define TESTCASE_CANVAS_DRAWPOINTS_H
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>

#include "test_base.h"
#include "test_common.h"

class CanvasDrawPointsPerformance : public TestBase {
public:
    explicit CanvasDrawPointsPerformance(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawPointsPerformance";
    };
    ~CanvasDrawPointsPerformance() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class CanvasDrawPointsPerformancePen : public TestBase {
public:
    explicit CanvasDrawPointsPerformancePen(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawPointsPerformancePen";
    };
    ~CanvasDrawPointsPerformancePen() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class CanvasDrawPointsPerformanceBrush : public TestBase {
public:
    explicit CanvasDrawPointsPerformanceBrush(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawPointsPerformanceBrush";
    };
    ~CanvasDrawPointsPerformanceBrush() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class CanvasDrawBrokenLinePerformance : public TestBase {
public:
    explicit CanvasDrawBrokenLinePerformance(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawBrokenLinePerformance";
    };
    ~CanvasDrawBrokenLinePerformance() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
class CanvasDrawBrokenLinePerformancePen : public TestBase {
public:
    explicit CanvasDrawBrokenLinePerformancePen(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawBrokenLinePerformancePen";
    };
    ~CanvasDrawBrokenLinePerformancePen() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
class CanvasDrawBrokenLinePerformanceBrush : public TestBase {
public:
    explicit CanvasDrawBrokenLinePerformanceBrush(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawBrokenLinePerformanceBrush";
    };
    ~CanvasDrawBrokenLinePerformanceBrush() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class CanvasDrawCurveLinePerformance : public TestBase {
public:
    explicit CanvasDrawCurveLinePerformance(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawCurveLinePerformance";
    };
    ~CanvasDrawCurveLinePerformance() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
class CanvasDrawCurveLinePerformancePen : public TestBase {
public:
    explicit CanvasDrawCurveLinePerformancePen(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawCurveLinePerformancePen";
    };
    ~CanvasDrawCurveLinePerformancePen() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
class CanvasDrawCurveLinePerformanceBrush : public TestBase {
public:
    explicit CanvasDrawCurveLinePerformanceBrush(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawCurveLinePerformanceBrush";
    };
    ~CanvasDrawCurveLinePerformanceBrush() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class CanvasDrawRegionPerformance : public TestBase {
public:
    explicit CanvasDrawRegionPerformance(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawRegionPerformance";
    };
    ~CanvasDrawRegionPerformance() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
class CanvasDrawRegionPerformancePen : public TestBase {
public:
    explicit CanvasDrawRegionPerformancePen(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawRegionPerformancePen";
    };
    ~CanvasDrawRegionPerformancePen() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
class CanvasDrawRegionPerformanceBrush : public TestBase {
public:
    explicit CanvasDrawRegionPerformanceBrush(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawRegionPerformanceBrush";
    };
    ~CanvasDrawRegionPerformanceBrush() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class CanvasDrawBitMapPerformance : public TestBase {
public:
    explicit CanvasDrawBitMapPerformance(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawBitMapPerformance";
    };
    ~CanvasDrawBitMapPerformance() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
class CanvasDrawBitMapPerformancePen : public TestBase {
public:
    explicit CanvasDrawBitMapPerformancePen(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawBitMapPerformancePen";
    };
    ~CanvasDrawBitMapPerformancePen() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
class CanvasDrawBitMapPerformanceBrush : public TestBase {
public:
    explicit CanvasDrawBitMapPerformanceBrush(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawBitMapPerformanceBrush";
    };
    ~CanvasDrawBitMapPerformanceBrush() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class CanvasDrawBitMapRectPerformance : public TestBase {
public:
    explicit CanvasDrawBitMapRectPerformance(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawBitMapRectPerformance";
    };
    ~CanvasDrawBitMapRectPerformance() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
class CanvasDrawBitMapRectPerformancePen : public TestBase {
public:
    explicit CanvasDrawBitMapRectPerformancePen(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawBitMapRectPerformancePen";
    };
    ~CanvasDrawBitMapRectPerformancePen() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
class CanvasDrawBitMapRectPerformanceBrush : public TestBase {
public:
    explicit CanvasDrawBitMapRectPerformanceBrush(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawBitMapRectPerformanceBrush";
    };
    ~CanvasDrawBitMapRectPerformanceBrush() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class CanvasDrawImagePerformance : public TestBase {
public:
    explicit CanvasDrawImagePerformance(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawImagePerformance";
    };
    ~CanvasDrawImagePerformance() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
class CanvasDrawImagePerformancePen : public TestBase {
public:
    explicit CanvasDrawImagePerformancePen(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawImagePerformancePen";
    };
    ~CanvasDrawImagePerformancePen() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
class CanvasDrawImagePerformanceBrush : public TestBase {
public:
    explicit CanvasDrawImagePerformanceBrush(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawImagePerformanceBrush";
    };
    ~CanvasDrawImagePerformanceBrush() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class CanvasDrawPiexlMapPerformance : public TestBase {   
public:
    explicit CanvasDrawPiexlMapPerformance(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawPiexlMapPerformance";
    };
    ~CanvasDrawPiexlMapPerformance() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
class CanvasDrawPiexlMapPerformancePen: public TestBase {   
public:
    explicit CanvasDrawPiexlMapPerformancePen(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawPiexlMapPerformancePen";
    };
    ~CanvasDrawPiexlMapPerformancePen() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
class CanvasDrawPiexlMapPerformanceBrush : public TestBase {   
public:
    explicit CanvasDrawPiexlMapPerformanceBrush(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawPiexlMapPerformanceBrush";
    };
    ~CanvasDrawPiexlMapPerformanceBrush() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
#endif
