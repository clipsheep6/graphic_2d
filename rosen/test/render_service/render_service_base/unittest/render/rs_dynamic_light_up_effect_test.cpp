/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"

#include "pipeline/rs_paint_filter_canvas.h"
#include "property/rs_properties_painter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSDynamicLightUpTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSDynamicLightUpTest::SetUpTestCase() {}
void RSDynamicLightUpTest::TearDownTestCase() {}
void RSDynamicLightUpTest::SetUp() {}
void RSDynamicLightUpTest::TearDown() {}

/**
 * @tc.name: RSDynamicLightUpTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSDynamicLightUpTest, RSDynamicLightUpTest001, TestSize.Level1)
{
    RSProperties properties;
    properties.SetDynamicLightUpRate(0.5);
    properties.SetDynamicLightUpDegree(0.5);
    SkCanvas skCanvas;
    RSPaintFilterCanvas canvas(&skCanvas);
    RSPropertiesPainter::DrawDynamicLightUp(properties, canvas);
}


} // namespace OHOS::Rosen
