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

#ifndef FILTER_TEST_H
#define FILTER_TEST_H

#include "pipeline_test_utils.h"

#include "draw/brush.h"
#include "draw/canvas.h"
#include "draw/color.h"
#include "draw/pen.h"
#include "effect/filter.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class FilterTest {
public:
    inline static FilterTest& GetInstance()
    {
        static FilterTest c;
        return c;
    }

    static void TestDrawFilter(Canvas& canvas, uint32_t width, uint32_t height);
    void FilterTestCase(uint32_t screenWidth, uint32_t screenHeight);

private:
    FilterTest() = default;
    virtual ~FilterTest() = default;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif