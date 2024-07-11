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

#ifndef INTERFACE_BITMAP_TEST_H
#define INTERFACE_BITMAP_TEST_H
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_rect.h>

#include "test_base.h"
#include "test_common.h"

class ImageBuildFromBitmap : public TestBase {
public:
    explicit ImageBuildFromBitmap(int type) : TestBase(type)
    {
        fileName_ = "ImageBuildFromBitmap";
    };
    ~ImageBuildFromBitmap() override {};

protected:
    int rectWidth_ = 0;
    int rectHeight_ = 0;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class ImageIsOpaque : public TestBase {
public:
    explicit ImageIsOpaque(int type) : TestBase(type) { fileName_ = "ImageIsOpaque"; };
    ~ImageIsOpaque() override{};

protected:
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override;
};

class ImageCreateFromRaster : public TestBase {
public:
    explicit ImageCreateFromRaster(int type) : TestBase(type) { fileName_ = "ImageCreateFromRaster"; };
    ~ImageCreateFromRaster() override{};

protected:
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override;
};

class BitmapReadPixels : public TestBase {
public:
    explicit BitmapReadPixels(int type, OH_Drawing_ColorFormat ct, OH_Drawing_AlphaFormat at)
        : TestBase(type), fCT(ct), fAT(at)
    {
        fileName_ = "BitmapReadPixels";
    };
    ~BitmapReadPixels() override = default;

protected:
    OH_Drawing_ColorFormat fCT;
    OH_Drawing_AlphaFormat fAT;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class BitmapBuild : public TestBase {
public:
    explicit BitmapBuild(int type) : TestBase(type)
    {
        fileName_ = "BitmapBuild";
    };
    ~BitmapBuild() override {};

protected:
    int rectWidth_ = 0;
    int rectHeight_ = 0;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class BitmapCreateFromPixels : public TestBase {
public:
    explicit BitmapCreateFromPixels(int type) : TestBase(type)
    {
        fileName_ = "BitmapCreateFromPixels";
    };
    ~BitmapCreateFromPixels() override {};

protected:
    int rectWidth_ = 0;
    int rectHeight_ = 0;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class BitmapInstallPixels : public TestBase {
public:
    explicit BitmapInstallPixels(int type, OH_Drawing_ColorFormat ct, OH_Drawing_AlphaFormat at)
        : TestBase(type), fCT(ct), fAT(at) {
        fileName_ = "BitmapInstallPixels";
    };
    ~BitmapInstallPixels() override = default;

protected:
    OH_Drawing_ColorFormat fCT;
    OH_Drawing_AlphaFormat fAT;
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override;
};

class BitmapPeekPixels : public TestBase {
public:
    explicit BitmapPeekPixels(int type, OH_Drawing_ColorFormat ct, OH_Drawing_AlphaFormat at)
        : TestBase(type), fCT(ct), fAT(at) {
        fileName_ = "BitmapPeekPixels";
    };
    ~BitmapPeekPixels() override = default;

protected:
    OH_Drawing_ColorFormat fCT;
    OH_Drawing_AlphaFormat fAT;
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override;
};

class BitmapTryAllocPixels : public TestBase {
public:
    explicit BitmapTryAllocPixels(int type, OH_Drawing_ColorFormat ct, OH_Drawing_AlphaFormat at)
        : TestBase(type), fCT(ct), fAT(at) {
        fileName_ = "BitmapTryAllocPixels";
    };
    ~BitmapTryAllocPixels() override = default;

protected:
    OH_Drawing_ColorFormat fCT;
    OH_Drawing_AlphaFormat fAT;
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override;
};

class BitmapComputeByteSize : public TestBase {
public:
    explicit BitmapComputeByteSize(int type, OH_Drawing_ColorFormat ct, OH_Drawing_AlphaFormat at)
        : TestBase(type), fCT(ct), fAT(at) {
        fileName_ = "BitmapComputeByteSize";
    };
    ~BitmapComputeByteSize() override = default;

protected:
    OH_Drawing_ColorFormat fCT;
    OH_Drawing_AlphaFormat fAT;
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override;
};
#endif // INTERFACE_BITMAP_TEST_H