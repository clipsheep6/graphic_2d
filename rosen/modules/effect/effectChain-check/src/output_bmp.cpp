/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "output_bmp.h"

OutputBMP::OutputBMP():Output()
{
}

void OutputBMP::DoProcess(ProcessDate& data)
{
    RenderOnScreen(data.srcTextureID);
    if (saveResult_) {
        SaveRenderResultToBMP(imageWidth_, imageHeight_);
        saveResult_ = false;
    }
    Filter* end = nullptr;
    AddNextFilter(end);
}

void OutputBMP::SaveRenderResultToBMP(int ImageWidth, int ImageHeight)
{
    if (!imageName_.empty()) {
        imageName_ += ".bmp";
        
        if (ImageWidth == -1 || imageHeight_ == -1) {
            GLint iViewport[4];
            glGetIntegerv(GL_VIEWPORT, iViewport);
            int widthPosition = 2;
            int heightPosition = 3;
            ImageWidth = iViewport[ widthPosition ];
            ImageHeight = iViewport[ heightPosition ];
        }

        RGBColor* ColorBuffer = new RGBColor[ImageWidth * ImageHeight];
        glReadPixels(0, 0, ImageWidth, ImageHeight, GL_BGR, GL_UNSIGNED_BYTE, ColorBuffer);
        WriteBMP(imageName_.c_str(), ColorBuffer, ImageWidth, ImageHeight);
        delete[] ColorBuffer;
    }
}

void OutputBMP::WriteBMP(const char* FileName, RGBColor* ColorBuffer, int ImageWidth, int ImageHeight)
{
    const int ColorBufferSize = ImageHeight * ImageWidth * sizeof(RGBColor);
    BITMAPFILEHEADER fileHeader;
    fileHeader.bfType = 0x4D42;
    fileHeader.bfReserved1 = 0;
    fileHeader.bfReserved2 = 0;
    fileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + ColorBufferSize;
    fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    BITMAPINFOHEADER bitmapHeader = { 0 };
    bitmapHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmapHeader.biHeight = ImageHeight;
    bitmapHeader.biWidth = ImageWidth;
    bitmapHeader.biPlanes = 1;
    bitmapHeader.biBitCount = 24;
    bitmapHeader.biSizeImage = ColorBufferSize;
    bitmapHeader.biCompression = 0;

    FILE* fp = fopen(FileName, "wb");
    if (fp) {
        size_t size = 1;
        size_t result = -1;
        result = fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), size, fp);
        if (result != size) {
            return;
        }
        result = fwrite(&bitmapHeader, sizeof(BITMAPINFOHEADER), size, fp);
        if (result != size) {
            return;
        }
        result = fwrite(ColorBuffer, ColorBufferSize, size, fp);
        if (result != size) {
            return;
        }
        fclose(fp);
    }
}

