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

#include "winSimulator.h"
#include "skpPainter.h"
#include "include/core/SkFont.h"

int WinSimulator::setWindowInfo(int width, int height) {
    mWindowWidth = width;
    mWindowHeight = height;
    return 0;
}

int WinSimulator::onPaint(SkSurface * surface) {
    static int frame = 1;
    std::string skp_file1 = "data/data_T1109";
    std::string skp_file2 = "data/data_T8347";
    std::string skp_file3 = "data/data_T8775";
    int windowCount = mSubWindows;
    int xOffset = 0;
    int yOffset = 0;
    int width = mWindowWidth;
    int height = mWindowHeight;

    if (frame > 0) {
        skp_file1 += "_" + std::to_string(frame);
    }
    if (frame > 0) {
        skp_file2 += "_" + std::to_string(frame);
    }
    if (frame > 0) {
        skp_file3 += "_" + std::to_string(frame);
    }
    frame++;
    if (frame > 1000) {
        frame = 1;
    }

    skpPainter painter1 = skpPainter(skp_file1);
    skpPainter painter2 = skpPainter(skp_file2);
    skpPainter painter3 = skpPainter(skp_file3);

    painter1.loadSkp(skp_file1);
    painter2.loadSkp(skp_file2);
    painter3.loadSkp(skp_file3);
    painter1.enableThread(mMultiThread);
    painter2.enableThread(mMultiThread);
    painter3.enableThread(mMultiThread);

    int subWinWidth = 0;
    int subWInHeight = 0;
    int row = 0;
    int cow = 0;
    if (mDisplayMode == 0) {
        subWinWidth = (float)width /windowCount;
        subWInHeight = height * subWinWidth / (float)width;
        row = 1;
        cow = windowCount;
        xOffset = subWinWidth;
        yOffset = 0;
    }

    int i = 0;
    for (i = 0; i < row; i++) {
        for(int l = 0; l < cow; l++) {
            painter2.onPaint(surface, xOffset * l, yOffset * i, subWinWidth, subWInHeight);
            if(i * cow + l >= windowCount) {
                break;
            }
        }
    }

    painter1.wait4Paint();
    painter2.wait4Paint();
    painter3.wait4Paint();

    mFrameTime_paint = 0;
    mFrameTime_paint += painter1.getPaintTime();
    mFrameTime_paint += painter2.getPaintTime();
    mFrameTime_paint += painter3.getPaintTime();
    std::cout <<"whole draw time = "<<mFrameTime_paint<<"ms"<<std::endl;

    mFrameCount++;
    mDrawTime += mFrameTime_paint;

    float avg_drawTime = mDrawTime / mFrameCount;

    std::string drawtime = "draw:";
    std::string avg_drawtime = "avg_draw:";
    std::string framecount = "frameCount:";
    std::string usMultiThread = "usMultiThread:";

    drawtime += std::to_string(mFrameTime_paint) + "ms";
    avg_drawtime += std::to_string(avg_drawTime) + "ms";
    framecount += std::to_string(mFrameCount);
    usMultiThread += std::to_string(mMultiThread);

    SkCanvas* canvas = surface->getCanvas();

    SkPaint paint;
    paint.setColor(SK_ColorGREEN);
    SkFont font;
    font.setSubpixel(true);
    font.setSize(20);

    canvas->save();
    canvas->translate(0,50);
    canvas->drawString(drawtime.c_str(), 1.0, 1.0, font, paint);
    canvas->translate(0,20);
    canvas->drawString(avg_drawtime.c_str(), 1.0, 1.0, font, paint);
    canvas->translate(0,20);
    canvas->drawString(framecount.c_str(), 1.0, 1.0, font, paint);
    canvas->translate(0,20);
    canvas->drawString(usMultiThread.c_str(), 1.0, 1.0, font, paint);
    canvas->restore();
    return 0;
}

int WinSimulator::onFlush() {
    return 0;
}

void WinSimulator::setMode(int mode, int wins) {
    mDisplayMode = mode;
    mSubWindows = wins;
}

float WinSimulator::getPaintTime() {
    return mFrameTime_paint;
}

void WinSimulator::useMultiThread(int multiThread)
{
    mMultiThread = multiThread;
}