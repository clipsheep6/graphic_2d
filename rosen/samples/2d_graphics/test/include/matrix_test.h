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

#ifndef MATRIX_TEST_H
#define MATRIX_TEST_H

#include "pipeline_test_utils.h"

#include "draw/brush.h"
#include "draw/canvas.h"
#include "draw/color.h"
#include "draw/path.h"
#include "draw/pen.h"
#include "utils/log.h"
#include "utils/matrix.h"
#include "utils/rect.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class MatrixTest {
public:
    inline static MatrixTest& GetInstance()
    {
        static MatrixTest c;
        return c;
    }

    static void TestMatrix(Canvas& canvas, uint32_t width, uint32_t height);
    void MatrixTestCase(uint32_t screenWidth, uint32_t screenHeight);

private:
    MatrixTest() = default;
    virtual ~MatrixTest() = default;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // MATRIX_TEST_H