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

#ifndef INTERFACE_TYPEFACE_TEST_H
#define INTERFACE_TYPEFACE_TEST_H
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_rect.h>

#include "test_base.h"
#include "test_common.h"

class TypefaceCreateDefault : public TestBase {
public:
    TypefaceCreateDefault(int type) : TestBase(type)
    {
        fileName_ = "TypefaceCreateDefault";
    }
    ~TypefaceCreateDefault() override {};

protected:
    int rectWidth_ = 0;
    int rectHeight_ = 0;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class TypefaceCreateFromFile : public TestBase {
public:
    TypefaceCreateFromFile(int type) : TestBase(type)
    {
        fileName_ = "TypefaceCreateFromFile";
    }
    ~TypefaceCreateFromFile() override {};

protected:
    int rectWidth_ = 0;
    int rectHeight_ = 0;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class TypefaceCreateFromStream : public TestBase {
public:
    TypefaceCreateFromStream(int type) : TestBase(type)
    {
        fileName_ = "TypefaceCreateFromStream";
    }
    ~TypefaceCreateFromStream() override {};

protected:
    int rectWidth_ = 0;
    int rectHeight_ = 0;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

#endif // INTERFACE_TYPEFACE_TEST_H