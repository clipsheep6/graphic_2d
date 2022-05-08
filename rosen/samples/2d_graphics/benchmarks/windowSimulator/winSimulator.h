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
#ifndef WINSIMULATOR_H
#define WINSIMULATOR_H
#include "include/core/SkCanvas.h"
#include "include/core/SkSurface.h"

class WinSimulator {
public:
    WinSimulator() {}
    int setWindowInfo(int width, int height);
    int onPaint(SkSurface *surface);
    int onFlush();
    void setMode(int mode, int wins);
    void useMultiThread(int multiThread);
    float getPaintTime();
private:
    int mWindowWidth;
    int mWindowHeight;
    int mDisplayMode;
    int mSubWindows;
    int mMultiThread = 0;
    float mFrameTime_paint = 0;
    double mDrawTime = 0;
    int mFrameCount = 0;
    };
#endif
