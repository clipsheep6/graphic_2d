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

#include <chrono>
#include "skpPainter.h"
#include "include/core/SkPictureRecorder.h"
#include "include/core/SkPicture.h"
#include "include/core/SkStream.h"

using namespace std::chrono;
skpPainter::skpPainter(std::string file)
    :mPath(file)
{
    mUseThread = 0;
}

int skpPainter::setPath(std::string file) {
    mPath = file;
    return 0;
}

int skpPainter::loadSkp(std::string file) {
    SkFILEStream input(mPath.c_str());
    if (!input.isValid()) {
        SkDebugf("Bad file path = %s", mPath.c_str());
        return -1;
    }
    mPic = SkPicture::MakeFromStream(&input);
    if (!mPic) {
        SkDebugf("Bad skp: load skp fail!");
        return -1;
    }
    return 0;
}

int skpPainter::onPaint(SkSurface * surface, int letf, int top, int width, int height) {
    if (mUseThread == 1) {
        if (mThread == nullptr) {
            mThread = std::make_unique<std::thread>(&skpPainter::doPaint, this, surface, letf, top, width, height);
        }
    } else {
        doPaint(surface, letf, top, width, height);
    }
    return 0;
}

int skpPainter::onFlush() {
    return 0;
}

void skpPainter::wait4Paint() {
    if (mUseThread == 1) {
        if (mThread != nullptr && mThread->joinable()) {
            mThread->join();
        }
        mThread = nullptr;
    }
}

int skpPainter::doPaint(SkSurface * surface, int letf, int top, int width, int height) {
    SkCanvas* canvas = surface->getCanvas();

    auto start = system_clock::now();

    SkRect skpWin = mPic->cullRect();
    int picWidth = skpWin.fRight - skpWin.fLeft;
    float scaleRatioX = 0;
    float scaleRatioY = 0;
    scaleRatioX = (float)width / picWidth;
    scaleRatioY = scaleRatioX;

    canvas->save();
    canvas->translate(letf, top);
    canvas->scale(SkFloatToScalar(scaleRatioX), SkFloatToScalar(scaleRatioY));
    canvas->drawPicture(mPic);
    canvas->restore();

    auto end = system_clock::now();
    auto duration = duration_cast<nanoseconds>(end - start);
    mPaintTime = duration.count() / 1000000.0;
    std::cout << mPath.c_str()<<":current draw time = "<<mPaintTime <<"ms"<<std::endl;

    return 0;
}

void skpPainter::enableThread(int enable) {
    mUseThread = enable;
}

float skpPainter::getPaintTime() {
    return mPaintTime;
}
