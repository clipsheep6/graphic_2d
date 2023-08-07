/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "pipeline/rs_uni_ui_capture.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_root_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSUniUiCaptureTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUniUiCaptureTest::SetUpTestCase() {}
void RSUniUiCaptureTest::TearDownTestCase() {}
void RSUniUiCaptureTest::SetUp() {}
void RSUniUiCaptureTest::TearDown() {}

class RSRootRenderNodeMock : public RSRootRenderNode {
public:
    explicit RSRootRenderNodeMock(NodeId id, std::weak_ptr<RSContext> context = {})
        : RSRootRenderNode(id, context) {}
    MOCK_METHOD0(ShouldPaint, bool());
};

class RSCanvasRenderNodeMock : public RSCanvasRenderNode {
public:
    explicit RSCanvasRenderNodeMock(NodeId id, std::weak_ptr<RSContext> context = {})
        : RSCanvasRenderNode(id, context) {}
    MOCK_METHOD0(ShouldPaint, bool());
};

class RSEffectRenderNodeMock : public RSEffectRenderNode {
public:
    explicit RSEffectRenderNodeMock(NodeId id, std::weak_ptr<RSContext> context = {})
        : RSEffectRenderNode(id, context) {}
    MOCK_METHOD0(ShouldPaint, bool());
};

class RSSurfaceRenderNodeMock : public RSSurfaceRenderNode {
public:
    explicit RSSurfaceRenderNodeMock(NodeId id, std::weak_ptr<RSContext> context = {})
        : RSSurfaceRenderNode(id, context) {}
    MOCK_METHOD0(ShouldPaint, bool());
    MOCK_CONST_METHOD0(GetBuffer, sptr<SurfaceBuffer>());
};

/**
 * @tc.name: TakeLocalCapture001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSUniUiCaptureTest, TakeLocalCapture001, TestSize.Level1)
{
    NodeId nodeId = 0;
    float scaleX = 0.0;
    float scaleY = 0.0;
    RSUniUICapture rsUniUICapture(nodeId, scaleX, scaleY);
    EXPECT_EQ(nullptr, rsUniUICapture.TakeLocalCapture());
}

/**
 * @tc.name: SetCanvasTest
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSUniUiCaptureTest, SetCanvasTest, TestSize.Level1)
{
    NodeId nodeId = 0;
    float scaleX = 0.0;
    float scaleY = 0.0;
    RSUniUICapture::RSUniUICaptureVisitor rsUniUICaptureVisitor(nodeId, scaleX, scaleY);
    std::shared_ptr<RSRecordingCanvas> canvas = nullptr;
    rsUniUICaptureVisitor.SetCanvas(canvas);
}

/**
 * @tc.name: ProcessRootRenderNodeTest
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSUniUiCaptureTest, ProcessRootRenderNodeTest, TestSize.Level1)
{
    NodeId nodeId = 0;
    float scaleX = 0.0;
    float scaleY = 0.0;
    std::weak_ptr<RSContext> context;
    RSUniUICapture::RSUniUICaptureVisitor rsUniUICaptureVisitor(nodeId, scaleX, scaleY);
    RSRootRenderNode node(nodeId, context);
    rsUniUICaptureVisitor.ProcessRootRenderNode(node);
}

/**
 * @tc.name: ProcessCanvasRenderNodeTest
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSUniUiCaptureTest, ProcessCanvasRenderNodeTest, TestSize.Level1)
{
    NodeId nodeId = 0;
    float scaleX = 0.0;
    float scaleY = 0.0;
    std::weak_ptr<RSContext> context;
    RSUniUICapture::RSUniUICaptureVisitor rsUniUICaptureVisitor(nodeId, scaleX, scaleY);
    RSRootRenderNode node(nodeId, context);
    rsUniUICaptureVisitor.ProcessCanvasRenderNode(node);
}

/**
 * @tc.name: ProcessSurfaceRenderNodeTest
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSUniUiCaptureTest, ProcessSurfaceRenderNodeTest, TestSize.Level1)
{
    NodeId nodeId = 0;
    float scaleX = 0.0;
    float scaleY = 0.0;
    std::weak_ptr<RSContext> context;
    RSUniUICapture::RSUniUICaptureVisitor rsUniUICaptureVisitor(nodeId, scaleX, scaleY);
    RSSurfaceRenderNode node(nodeId, context);
    rsUniUICaptureVisitor.ProcessSurfaceRenderNode(node);
}

/**
 * @tc.name: ProcessEffectRenderNode001
 * @tc.desc: Test RSUniUiCapture.ProcessEffectRenderNode api
 * @tc.type:
 * @tc.require:
 */
HWTEST_F(RSUniUiCaptureTest, ProcessEffectRenderNode001, TestSize.Level1)
{
    NodeId id = 0;
    RSEffectRenderNode node(id);
    float scaleX = 0.0;
    float scaleY = 0.0;
    RSUniUICapture::RSUniUICaptureVisitor rsUniUICaptureVisitor(id, scaleX, scaleY);
    rsUniUICaptureVisitor.ProcessEffectRenderNode(node);
}

/**
 * @tc.name: CreateSurface
 * @tc.desc: Test RSUniUiCapture.CreateSurface api
 * @tc.type:
 * @tc.require:
 */
HWTEST_F(RSUniUiCaptureTest, CreateSurface, TestSize.Level1)
{
    NodeId nodeId = 0;
    float scaleX = 0.0;
    float scaleY = 0.0;
    RSUniUICapture rsUniUICapture(nodeId, scaleX, scaleY);
    Media::InitializationOptions opts;
    opts.size.width = 1;
    opts.size.height = 1;
    std::shared_ptr<Media::PixelMap> pixelmap = Media::PixelMap::Create(opts);

    auto surfacePtr = rsUniUICapture.CreateSurface(nullptr);
    EXPECT_EQ(surfacePtr, nullptr);

    surfacePtr = rsUniUICapture.CreateSurface(pixelmap);
    EXPECT_NE(surfacePtr, nullptr);
}

/**
 * @tc.name: PostTaskToRSRecord
 * @tc.desc: Test RSUniUiCapture.PostTaskToRSRecord api
 * @tc.type:
 * @tc.require:
 */
HWTEST_F(RSUniUiCaptureTest, PostTaskToRSRecord, TestSize.Level1)
{
    NodeId nodeId = 0;
    float scaleX = 0.0;
    float scaleY = 0.0;
    std::weak_ptr<RSContext> context;
    RSUniUICapture rsUniUICapture(nodeId, scaleX, scaleY);
    auto rsUniUICaptureVistorPtr = std::make_shared<RSUniUICapture::RSUniUICaptureVisitor>(nodeId, scaleX, scaleY);
    auto node = std::make_shared<RSRootRenderNode>(nodeId, context);
    auto canvas = std::make_shared<RSRecordingCanvas>(0, 0);
    rsUniUICaptureVistorPtr->SetCanvas(canvas);
    rsUniUICapture.PostTaskToRSRecord(canvas, node, rsUniUICaptureVistorPtr);
    EXPECT_NE(canvas, nullptr);
}

/**
 * @tc.name: ProcessRenderNode
 * @tc.desc: Test RSUniUiCapture.ProcessRenderNode api
 * @tc.type:
 * @tc.require:
 */
HWTEST_F(RSUniUiCaptureTest, ProcessRenderNode, TestSize.Level1)
{
    NodeId nodeId = 0;
    float scaleX = 0.0;
    float scaleY = 0.0;
    std::weak_ptr<RSContext> context;
    RSUniUICapture rsUniUICapture(nodeId, scaleX, scaleY);
    RSUniUICapture::RSUniUICaptureVisitor rsUniUICaptureVisitor(nodeId, scaleX, scaleY);

    RSRootRenderNodeMock rootNode(nodeId, context);
    rsUniUICaptureVisitor.ProcessRootRenderNode(rootNode);
    RSCanvasRenderNodeMock canvasNode(nodeId, context);
    rsUniUICaptureVisitor.ProcessCanvasRenderNode(canvasNode);
    RSEffectRenderNodeMock effectNode(nodeId, context);
    rsUniUICaptureVisitor.ProcessEffectRenderNode(effectNode);
    RSSurfaceRenderNodeMock surfaceNode(nodeId, context);
    rsUniUICaptureVisitor.ProcessSurfaceRenderNode(surfaceNode);

    ON_CALL(rootNode, ShouldPaint()).WillByDefault(Return(true));
    ON_CALL(canvasNode, ShouldPaint()).WillByDefault(Return(true));
    ON_CALL(effectNode, ShouldPaint()).WillByDefault(Return(true));
    ON_CALL(surfaceNode, ShouldPaint()).WillByDefault(Return(true));
    ON_CALL(surfaceNode, GetBuffer()).WillByDefault(Return(nullptr));

    auto canvas = std::make_shared<RSRecordingCanvas>(0, 0);
    EXPECT_NE(canvas, nullptr);
    rsUniUICaptureVisitor.SetCanvas(canvas);
    rsUniUICaptureVisitor.ProcessRootRenderNode(rootNode);
    rsUniUICaptureVisitor.ProcessCanvasRenderNode(canvasNode);
    rsUniUICaptureVisitor.ProcessEffectRenderNode(effectNode);
    rsUniUICaptureVisitor.ProcessSurfaceRenderNode(surfaceNode);
    rsUniUICaptureVisitor.ProcessSurfaceRenderNodeWithUni(surfaceNode);
    rsUniUICaptureVisitor.ProcessSurfaceViewWithoutUni(surfaceNode);
    rsUniUICaptureVisitor.ProcessSurfaceViewWithUni(surfaceNode);

    rsUniUICaptureVisitor.PrepareSurfaceRenderNode(surfaceNode);
    rsUniUICaptureVisitor.PrepareEffectRenderNode(effectNode);
    rsUniUICaptureVisitor.PrepareRootRenderNode(rootNode);
    rsUniUICaptureVisitor.PrepareCanvasRenderNode(canvasNode);
}
} // namespace OHOS::Rosen