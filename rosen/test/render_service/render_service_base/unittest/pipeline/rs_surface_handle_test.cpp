/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>

#include "pipeline/rs_context.h"
#include "pipeline/rs_surface_handler.h"
#include "pipeline/rs_surface_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSSurfaceHandlerTest : public testing::Test {
public:
    static inline NodeId id;
    static inline std::weak_ptr<RSContext> context = {};
};

/**
 * @tc.name: ConsumeAndUpdateBuffer01
 * @tc.desc: function test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceHandlerTest, ConsumeAndUpdateBuffer01, TestSize.Level2)
{
    RSSurfaceHandler::SurfaceBufferEntry buffer;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    auto& surfaceHandler = static_cast<RSSurfaceHandler&>(*(rsSurfaceRenderNode.get()));

    buffer.buffer = nullptr;
    buffer.timestamp = 100;
    surfaceHandler.ConsumeAndUpdateBuffer(buffer);
    ASSERT_EQ(surfaceHandler.IsCurrentFrameBufferConsumed(), false);
}

/**
 * @tc.name: CacheBuffer01
 * @tc.desc: function test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceHandlerTest, CacheBuffer01, TestSize.Level2)
{
    RSSurfaceHandler::SurfaceBufferEntry buffer;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    auto& surfaceHandler = static_cast<RSSurfaceHandler&>(*(rsSurfaceRenderNode.get()));

    buffer.buffer = nullptr;
    buffer.timestamp = 100;
    surfaceHandler.CacheBuffer(buffer);
    ASSERT_EQ(surfaceHandler.bufferCache_.size(), 1);
}

/**
 * @tc.name: GetBufferFromCache01
 * @tc.desc: function test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceHandlerTest, GetBufferFromCache01, TestSize.Level2)
{
    RSSurfaceHandler::SurfaceBufferEntry buffer1;
    RSSurfaceHandler::SurfaceBufferEntry buffer2;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    auto& surfaceHandler = static_cast<RSSurfaceHandler&>(*(rsSurfaceRenderNode.get()));

    buffer1.buffer = nullptr;
    buffer1.timestamp = 100;
    surfaceHandler.CacheBuffer(buffer1);

    buffer2.buffer = nullptr;
    buffer2.timestamp = 200;
    surfaceHandler.CacheBuffer(buffer2);

    uint64_t vsyncTimestamp = 50;
    ASSERT_EQ(surfaceHandler.GetBufferFromCache(vsyncTimestamp).timestamp, 0);
}

/**
 * @tc.name: GetBufferFromCache02
 * @tc.desc: function test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceHandlerTest, GetBufferFromCache02, TestSize.Level2)
{
    RSSurfaceHandler::SurfaceBufferEntry buffer1;
    RSSurfaceHandler::SurfaceBufferEntry buffer2;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    auto& surfaceHandler = static_cast<RSSurfaceHandler&>(*(rsSurfaceRenderNode.get()));

    buffer1.buffer = nullptr;
    buffer1.timestamp = 100;
    surfaceHandler.CacheBuffer(buffer1);

    buffer2.buffer = nullptr;
    buffer2.timestamp = 200;
    surfaceHandler.CacheBuffer(buffer2);

    uint64_t vsyncTimestamp = 150;
    ASSERT_EQ(surfaceHandler.GetBufferFromCache(vsyncTimestamp).timestamp, 100);
}

/**
 * @tc.name: GetBufferFromCache03
 * @tc.desc: function test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceHandlerTest, GetBufferFromCache03, TestSize.Level2)
{
    RSSurfaceHandler::SurfaceBufferEntry buffer1;
    RSSurfaceHandler::SurfaceBufferEntry buffer2;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(id, context);
    auto& surfaceHandler = static_cast<RSSurfaceHandler&>(*(rsSurfaceRenderNode.get()));

    buffer1.buffer = nullptr;
    buffer1.timestamp = 100;
    surfaceHandler.CacheBuffer(buffer1);

    buffer2.buffer = nullptr;
    buffer2.timestamp = 200;
    surfaceHandler.CacheBuffer(buffer2);

    uint64_t vsyncTimestamp = 300;
    ASSERT_EQ(surfaceHandler.GetBufferFromCache(vsyncTimestamp).timestamp, 200);
}

}
}