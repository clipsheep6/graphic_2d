/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "bitmap_test.h"

using namespace OHOS::Media;

namespace OHOS {
namespace Rosen {
namespace Drawing {
    void BitmapTest::TestDrawBitmap(Canvas &canvas, uint32_t width, uint32_t height)
    {
        LOGI("+++++++ TestDrawBitmap");
        Bitmap bmp;
        BitmapFormat format {COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUYE};
        bmp.Build(200, 200, format); // bitmap width and height
        bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);

        Pen pen;
        pen.SetAntiAlias(true);
        pen.SetColor(Drawing::Color::COLOR_BLUE);
        pen.SetWidth(10); // The thickness of the pen is 10
        canvas.AttachPen(pen);
        canvas.DrawBitmap(bmp, 500, 500); // draw bitmap at (fx, fy)

        LOGI("------- TestDrawBitmap");
    }

    std::unique_ptr<PixelMap> ConstructPixmap()
    {
        uint32_t pixelMapWidth = 50;
        uint32_t pixelMapHeight = 50;
        std::unique_ptr<PixelMap> pixelMap = std::make_unique<PixelMap>();
        ImageInfo info;
        info.size.width = pixelMapWidth;
        info.size.height = pixelMapHeight;
        info.pixelFormat = Media::PixelFormat::RGBA_8888;
        info.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
        info.colorSpace = Media::ColorSpace::SRGB;
        pixelMap->SetImageInfo(info);
        LOGI("pixelMap info: width = %{public}d, height = %{public}d, pixelformat = %{public}d, "\
            "alphatype = %{public}d, colorspace = %{public}d", info.size.width, info.size.height,
            info.pixelFormat, info.alphaType, info.colorSpace);

        uint32_t rowDataSize = pixelMapWidth;
        uint32_t bufferSize = rowDataSize * pixelMapHeight;
        void *buffer = malloc(bufferSize);
        if (buffer == nullptr) {
            LOGE("alloc memory size:[%{public}d] error.", bufferSize);
            return nullptr;
        }
        char *ch = static_cast<char *>(buffer);
        for (unsigned int i = 0; i < bufferSize; i++) {
            *(ch++) = static_cast<char>(i);
        }

        pixelMap->SetPixelsAddr(buffer, nullptr, bufferSize, AllocatorType::HEAP_ALLOC, nullptr);

        return pixelMap;
    }

    void BitmapTest::TestDrawPixelmap(Canvas &canvas, uint32_t width, uint32_t height)
    {
        LOGI("+++++++ TestDrawPixelmap");
        std::unique_ptr<PixelMap> pixelmap = ConstructPixmap();

        Pen pen;
        pen.SetAntiAlias(true);
        pen.SetColor(Drawing::Color::COLOR_BLUE);
        pen.SetWidth(10); // The thickness of the pen is 10
        canvas.AttachPen(pen);

        Brush brush;
        brush.SetColor(Drawing::Color::COLOR_RED);
        canvas.AttachBrush(brush);

        canvas.DrawBitmap(*pixelmap.get(), 500, 500); // draw pixelmap at (fx, fy)
        LOGI("------- TestDrawPixelmap");
    }

    void BitmapTest::BitmapTestCase(uint32_t screenWidth, uint32_t screenHeight)
    {
        std::cout << "-------------------------------------------------------\n";
        std::cout << "Drawing module BitmapTestCase start.\n";

        auto surfaceNode = PipelineTestUtils::ToDrawSurface().CreateSurface();
        PipelineTestUtils::ToDrawSurface()
            .SetSurfaceNode(surfaceNode)
            .SetSurfaceNodeSize(screenWidth, screenHeight)
            .SetDraw(TestDrawBitmap)
            .Run();
        PipelineTestUtils::ToDrawSurface()
            .SetSurfaceNode(surfaceNode)
            .SetSurfaceNodeSize(screenWidth, screenHeight)
            .SetDraw(TestDrawPixelmap)
            .Run();
        std::cout << "Drawing module BitmapTestCase end.\n";
        std::cout << "-------------------------------------------------------\n";
    }
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS