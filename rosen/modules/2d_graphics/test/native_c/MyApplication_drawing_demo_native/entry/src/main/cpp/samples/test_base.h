/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <string>
#include <bits/alltypes.h>
#include <native_drawing/drawing_canvas.h>

#ifndef DRAWING_DEMO_BASE_H
#define DRAWING_DEMO_BASE_H

#define DEFAULT_WIDTH 500
#define DEFAULT_HEIGHT 500
#define DEFAULT_BACKGROUND_COLOR 0xFFFFFFFF


class TestBase {
public:
    TestBase(){};
    virtual ~TestBase() = default;

    void Recording(OH_Drawing_Canvas* canvas);
    void SetFileName(std::string fileName);
    void GetSize(uint32_t& width, uint32_t& height);
    void GetBackgroundColor(uint32_t& color);

protected:
    virtual void OnRecording(OH_Drawing_Canvas* canvas) = 0;

    uint32_t width_ = DEFAULT_WIDTH;
    uint32_t height_ = DEFAULT_HEIGHT;
    uint32_t background_ = DEFAULT_BACKGROUND_COLOR;
    std::string fileName_ = "default";
};

#endif // DRAWING_DEMO_BASE_H