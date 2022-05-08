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
#ifndef SKPPAINTER_H
#define SKPPAINTER_H
#include <thread>
#include <string>
#include <iostream>
#include "include/core/SkCanvas.h"
#include "include/core/SkPicture.h"
#include "include/core/SkSurface.h"

class SkCanvas;
class SkPictureRecorder;
class SkNWayCanvas;
class WinSimulator;
class skpPainter {
public:
    skpPainter(std::string file);
    int setPath(std::string file);
    int loadSkp(std::string file);
    int onPaint(SkSurface * surface, int letf, int top, int width, int height);
    int onFlush();
    void wait4Paint();
    void enableThread(int enable);
    float getPaintTime();
private:
    int doPaint(SkSurface * surface, int letf, int top, int width, int height);
private:
    std::string mPath;
    sk_sp<SkPicture> mPic;
    int mUseThread;
    std::unique_ptr<std::thread> mThread = nullptr;
    float mPaintTime = 0;
};
#endif