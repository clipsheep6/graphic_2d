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
#include <parameter.h>
#include <parameters.h>

#include "gtest/gtest.h"
#include "limit_number.h"
#include "rs_test_util.h"

#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_render_engine.h"
#include "pipeline/rs_uni_render_engine.h"
#include "platform/common/rs_innovation.h"
#include "platform/common/rs_system_properties.h"
#if defined(ACCESSIBILITY_ENABLE)
#include "accessibility_config.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
constexpr uint64_t REFRESH_PERIOD = 16666667;
constexpr uint64_t SKIP_COMMAND_FREQ_LIMIT = 30;
constexpr uint32_t MULTI_WINDOW_PERF_START_NUM = 2;
constexpr uint32_t MULTI_WINDOW_PERF_END_NUM = 4;
class RSMainThreadPipelineTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static void* CreateParallelSyncSignal(uint32_t count);
};

void RSMainThreadPipelineTest::SetUpTestCase()
{
    RSTestUtil::InitRenderNodeGC();
}
void RSMainThreadPipelineTest::TearDownTestCase() {}
void RSMainThreadPipelineTest::SetUp() {}
void RSMainThreadPipelineTest::TearDown() {}
void* RSMainThreadPipelineTest::CreateParallelSyncSignal(uint32_t count)
{
    (void)(count);
    return nullptr;
}

/**
 * @tc.name: Start001
 * @tc.desc: Test RSMainThreadPipelineTest.Start
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadPipelineTest, Start001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->Start();
}

/**
 * @tc.name: Start002
 * @tc.desc: Test RSMainThreadPipelineTest.Start
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadPipelineTest, Start002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->runner_ = nullptr;
    mainThread->Start();
}

/**
 * @tc.name: ProcessCommand
 * @tc.desc: Test RSMainThreadPipelineTest.ProcessCommand
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadPipelineTest, ProcessCommand, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = false;
    mainThread->ProcessCommand();
    mainThread->isUniRender_ = true;
    mainThread->lastAnimateTimestamp_ = 0;
    mainThread->timestamp_ = REFRESH_PERIOD + 1;
    mainThread->context_->purgeType_ = RSContext::PurgeType::GENTLY;
    mainThread->ProcessCommand();
    mainThread->context_->purgeType_ = RSContext::PurgeType::STRONGLY;
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: WaitUtilUniRenderFinished
 * @tc.desc: Test RSMainThreadPipelineTest.WaitUtilUniRenderFinished
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadPipelineTest, WaitUtilUniRenderFinished, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->NotifyUniRenderFinish();
    mainThread->WaitUtilUniRenderFinished();
    ASSERT_EQ(mainThread->uniRenderFinished_, true);
}

/**
 * @tc.name: ProcessCommandForDividedRender001
 * @tc.desc: Test RSMainThreadPipelineTest.ProcessCommandForDividedRender
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadPipelineTest, ProcessCommandForDividedRender001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->ProcessCommandForDividedRender();
}

/**
 * @tc.name: CalcOcclusion
 * @tc.desc: Test RSMainThreadPipelineTest.CalcOcclusion, doWindowAnimate_ is false, isUniRender_ is true
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadPipelineTest, CalcOcclusion, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->doWindowAnimate_ = false;
    mainThread->isUniRender_ = true;
    mainThread->CalcOcclusion();
}

/**
 * @tc.name: Animate001
 * @tc.desc: Test RSMainThreadPipelineTest.Animate, doWindowAnimate_ is false
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadPipelineTest, Animate001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->doWindowAnimate_ = false;
    mainThread->Animate(0);
}

/**
 * @tc.name: Animate002
 * @tc.desc: Test RSMainThreadPipelineTest.Animate, doWindowAnimate_ is true
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadPipelineTest, Animate002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->doWindowAnimate_ = true;
    mainThread->Animate(0);
}

/**
 * @tc.name: PostSyncTask
 * @tc.desc: Test PostSyncTask when handler is null or not
 * @tc.type: FUNC
 * @tc.require: issueI6R34I
 */
HWTEST_F(RSMainThreadPipelineTest, PostSyncTask, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    RSTaskMessage::RSTask task = []() -> void { return; };
    mainThread->PostSyncTask(task);
}

/**
 * @tc.name: ProcessCommandForUniRender
 * @tc.desc: ProcessCommandForUniRender test with invalid data
 * @tc.type: FUNC
 * @tc.require: issueI7A39J
 */
HWTEST_F(RSMainThreadPipelineTest, ProcessCommandForUniRender, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_EQ(mainThread->effectiveTransactionDataIndexMap_.empty(), true);

    // // let lastindex same as timeout index and test timeout case at first
    mainThread->transactionDataLastWaitTime_[0] = 0;
    mainThread->timestamp_ = REFRESH_PERIOD * SKIP_COMMAND_FREQ_LIMIT + 1;
    mainThread->effectiveTransactionDataIndexMap_[0].first = 0;
    if (mainThread->rsVSyncDistributor_ == nullptr) {
        auto vsyncGenerator = CreateVSyncGenerator();
        auto vsyncController = new VSyncController(vsyncGenerator, 0);
        mainThread->rsVSyncDistributor_ = new VSyncDistributor(vsyncController, "rs");
        vsyncGenerator->SetRSDistributor(mainThread->rsVSyncDistributor_);
    }
    // default data with index 0
    auto data = std::make_unique<RSTransactionData>();
    ASSERT_NE(data, nullptr);
    data->SetIndex(1);
    mainThread->effectiveTransactionDataIndexMap_[0].second.emplace_back(std::move(data));
    data = std::make_unique<RSTransactionData>();
    ASSERT_NE(data, nullptr);
    data->SetIndex(3);
    mainThread->effectiveTransactionDataIndexMap_[0].second.emplace_back(std::move(data));
    data = std::make_unique<RSTransactionData>();
    ASSERT_NE(data, nullptr);
    data->SetIndex(2);
    mainThread->effectiveTransactionDataIndexMap_[0].second.emplace_back(std::move(data));
    // empty data
    mainThread->effectiveTransactionDataIndexMap_[0].second.emplace_back(nullptr);
    mainThread->ProcessCommandForUniRender();
}

/**
 * @tc.name: DoParallelComposition
 * @tc.desc: DoParallelComposition test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadPipelineTest, DoParallelComposition, TestSize.Level1)
{
    NodeId nodeId = 1;
    std::weak_ptr<RSContext> context = {};
    auto node = std::make_shared<RSBaseRenderNode>(nodeId, context);
    auto childNode = std::make_shared<RSBaseRenderNode>(nodeId + 1, context);
    int index = 0;
    node->SetIsOnTheTree(true);
    node->AddChild(childNode, index);
    ASSERT_EQ(static_cast<int>(node->GetChildrenCount()), 1);
    ASSERT_TRUE(childNode->IsOnTheTree());

    auto mainThread = RSMainThread::Instance();
    RSInnovation::_s_createParallelSyncSignal = (void*)RSMainThreadPipelineTest::CreateParallelSyncSignal;
    if (RSInnovation::GetParallelCompositionEnabled(mainThread->isUniRender_)) {
        mainThread->DoParallelComposition(node);
    }
}

/**
 * @tc.name: ConsumeAndUpdateAllNodes001
 * @tc.desc: ConsumeAndUpdateAllNodes test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadPipelineTest, ConsumeAndUpdateAllNodes001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    bool isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = false;
    mainThread->ConsumeAndUpdateAllNodes();
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: ConsumeAndUpdateAllNodes002
 * @tc.desc: ConsumeAndUpdateAllNodes test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadPipelineTest, ConsumeAndUpdateAllNodes002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    bool isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = false;
    mainThread->ConsumeAndUpdateAllNodes();
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: WaitUntilUnmarshallingTaskFinished001
 * @tc.desc: WaitUntilUnmarshallingTaskFinished test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadPipelineTest, WaitUntilUnmarshallingTaskFinished001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    bool isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = false;
    mainThread->WaitUntilUnmarshallingTaskFinished();
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: UniRender001
 * @tc.desc: UniRender test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadPipelineTest, UniRender001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto& uniRenderThread = RSUniRenderThread::Instance();
    uniRenderThread.uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
    mainThread->renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
    // prepare nodes
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    const std::shared_ptr<RSBaseRenderNode> rootNode = context->GetGlobalRootRenderNode();
    NodeId id = 1;
    RSDisplayNodeConfig config;
    auto childDisplayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    rootNode->AddChild(childDisplayNode, 0);
    rootNode->InitRenderParams();
    childDisplayNode->InitRenderParams();
    mainThread->UniRender(rootNode);
}

/**
 * @tc.name: UniRender002
 * @tc.desc: UniRender test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadPipelineTest, UniRender002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto& uniRenderThread = RSUniRenderThread::Instance();
    uniRenderThread.uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
    mainThread->renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
    // prepare nodes
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    const std::shared_ptr<RSBaseRenderNode> rootNode = context->GetGlobalRootRenderNode();
    NodeId id = 1;
    RSDisplayNodeConfig config;
    auto childDisplayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    rootNode->AddChild(childDisplayNode, 0);
    // prepare status
    bool doDirectComposition = mainThread->doDirectComposition_;
    bool isDirty = mainThread->isDirty_;
    bool isAccessibilityConfigChanged = mainThread->isAccessibilityConfigChanged_;
    bool isCachedSurfaceUpdated = mainThread->isCachedSurfaceUpdated_;
    bool isHardwareEnabledBufferUpdated = mainThread->isHardwareEnabledBufferUpdated_;
    mainThread->doDirectComposition_ = true;
    mainThread->isDirty_ = false;
    mainThread->isAccessibilityConfigChanged_ = false;
    mainThread->isCachedSurfaceUpdated_ = false;
    mainThread->isHardwareEnabledBufferUpdated_ = true;
    rootNode->InitRenderParams();
    childDisplayNode->InitRenderParams();
    mainThread->UniRender(rootNode);
    // status recover
    mainThread->doDirectComposition_ = doDirectComposition;
    mainThread->isDirty_ = isDirty;
    mainThread->isAccessibilityConfigChanged_ = isAccessibilityConfigChanged;
    mainThread->isCachedSurfaceUpdated_ = isCachedSurfaceUpdated;
    mainThread->isHardwareEnabledBufferUpdated_ = isHardwareEnabledBufferUpdated;
}

/**
 * @tc.name: Render
 * @tc.desc: Render test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadPipelineTest, Render, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto& uniRenderThread = RSUniRenderThread::Instance();
    uniRenderThread.uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
    mainThread->renderThreadParams_ = std::make_unique<RSRenderThreadParams>();
    const std::shared_ptr<RSBaseRenderNode> rootNode = mainThread->context_->globalRootRenderNode_;
    NodeId id = 1;
    RSDisplayNodeConfig config;
    auto childDisplayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    rootNode->AddChild(childDisplayNode);
    bool isUniRender = mainThread->isUniRender_;
    mainThread->runner_ = AppExecFwk::EventRunner::Create(false);
    mainThread->handler_ = std::make_shared<AppExecFwk::EventHandler>(mainThread->runner_);
    // uni render
    mainThread->isUniRender_ = true;
    rootNode->InitRenderParams();
    childDisplayNode->InitRenderParams();
    mainThread->Render();
    mainThread->runner_ = nullptr;
    mainThread->handler_ = nullptr;
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: CalcOcclusionImplementation
 * @tc.desc: CalcOcclusionImplementation test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadPipelineTest, CalcOcclusionImplementation, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    RSDisplayNodeConfig config;
    NodeId displayNodeId = 0;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(displayNodeId, config, mainThread->context_);

    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;
    NodeId surfaceNodeId = 0;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(surfaceNodeId, mainThread->context_);
    curAllSurfaces.emplace_back(nullptr);
    curAllSurfaces.emplace_back(surfaceNode);
    VisibleData dstCurVisVec;
    std::map<NodeId, RSVisibleLevel> dstPidVisMap;
    mainThread->CalcOcclusionImplementation(displayNode, curAllSurfaces, dstCurVisVec, dstPidVisMap);
}

/**
 * @tc.name: CalcOcclusion002
 * @tc.desc: CalcOcclusion Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadPipelineTest, CalcOcclusion002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    // prepare states
    bool doWindowAnimate = mainThread->doWindowAnimate_;
    mainThread->doWindowAnimate_ = false;
    auto globalRootRenderNode = mainThread->context_->globalRootRenderNode_;
    // run with nullptr
    mainThread->context_->globalRootRenderNode_ = nullptr;
    mainThread->CalcOcclusion();
    // run with one child node
    auto node1 = std::make_shared<RSRenderNode>(0, true);
    auto node2 = std::make_shared<RSRenderNode>(1, true);
    node1->AddChild(node2);
    mainThread->context_->globalRootRenderNode_ = node1;
    mainThread->CalcOcclusion();
    // run with more than one node
    auto node3 = std::make_shared<RSRenderNode>(0, true);
    RSDisplayNodeConfig config1;
    auto node4 = std::make_shared<RSDisplayRenderNode>(1, config1);
    RSSurfaceRenderNodeConfig config2;
    auto node5 = std::make_shared<RSSurfaceRenderNode>(config2);
    node3->AddChild(node4);
    node4->curAllSurfaces_.push_back(nullptr);
    node4->curAllSurfaces_.push_back(node5);
    mainThread->context_->globalRootRenderNode_ = node3;
    mainThread->CalcOcclusion();
    // recover
    mainThread->doWindowAnimate_ = doWindowAnimate;
    mainThread->context_->globalRootRenderNode_ = globalRootRenderNode;
}

/**
 * @tc.name: Aniamte
 * @tc.desc: Aniamte Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadPipelineTest, Aniamte, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    NodeId id = 0;
    auto node = std::make_shared<RSRenderNode>(id);
    mainThread->context_->RegisterAnimatingRenderNode(node);
    mainThread->Animate(mainThread->timestamp_);
}

/**
 * @tc.name: ConsumeAndUpdateAllNodes003
 * @tc.desc: ConsumeAndUpdateAllNodes003 Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadPipelineTest, ConsumeAndUpdateAllNodes003, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    bool isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = true;
    // prepare nodemap
    mainThread->context_->GetMutableNodeMap().renderNodeMap_.clear();
    mainThread->context_->GetMutableNodeMap().surfaceNodeMap_.clear();
    mainThread->context_->GetMutableNodeMap().surfaceNodeMap_.emplace(1, nullptr);
    RSSurfaceRenderNodeConfig config;
    config.id = 2;
    auto node1 = std::make_shared<RSSurfaceRenderNode>(config);
    node1->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node1);
    config.id = 3;
    auto node2 = std::make_shared<RSSurfaceRenderNode>(config);
    node2->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node2);
    config.id = 4;
    auto node3 = std::make_shared<RSSurfaceRenderNode>(config);
    node3->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    node3->isHardwareEnabledNode_ = true;
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node3);
    mainThread->ConsumeAndUpdateAllNodes();
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: ReleaseAllNodesBuffer
 * @tc.desc: ReleaseAllNodesBuffer Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadPipelineTest, ReleaseAllNodesBuffer, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    bool isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = true;
    // prepare nodemap
    mainThread->context_->GetMutableNodeMap().renderNodeMap_.clear();
    mainThread->context_->GetMutableNodeMap().surfaceNodeMap_.clear();
    mainThread->context_->GetMutableNodeMap().surfaceNodeMap_.emplace(1, nullptr);
    RSSurfaceRenderNodeConfig config;
    config.id = 2;
    auto node2 = std::make_shared<RSSurfaceRenderNode>(config);
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node2);
    config.id = 3;
    auto node3 = std::make_shared<RSSurfaceRenderNode>(config);
    node3->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    node3->isHardwareEnabledNode_ = true;
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node3);
    config.id = 4;
    auto node4 = std::make_shared<RSSurfaceRenderNode>(config);
    node4->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    node4->isHardwareEnabledNode_ = true;
    node4->isLastFrameHardwareEnabled_ = true;
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node4);
    config.id = 5;
    auto node5 = std::make_shared<RSSurfaceRenderNode>(config);
    node5->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    node5->isHardwareEnabledNode_ = true;
    node5->isLastFrameHardwareEnabled_ = true;
    node5->isCurrentFrameHardwareEnabled_ = false;
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node5);
    mainThread->ReleaseAllNodesBuffer();
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: PostTask001
 * @tc.desc: PostTask Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadPipelineTest, PostTask001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->runner_ = AppExecFwk::EventRunner::Create(false);
    mainThread->handler_ = std::make_shared<AppExecFwk::EventHandler>(mainThread->runner_);
    RSTaskMessage::RSTask task = []() -> void { return; };
    mainThread->PostTask(task);
    mainThread->runner_ = nullptr;
    mainThread->handler_ = nullptr;
}

/**
 * @tc.name: PostTask002
 * @tc.desc: PostTask Test with IMMEDIATE Priority
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadPipelineTest, PostTask002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->runner_ = AppExecFwk::EventRunner::Create(false);
    mainThread->handler_ = std::make_shared<AppExecFwk::EventHandler>(mainThread->runner_);
    RSTaskMessage::RSTask task = []() -> void { return; };
    std::string name = "test";
    int64_t delayTime = 0;
    AppExecFwk::EventQueue::Priority priority = AppExecFwk::EventQueue::Priority::IMMEDIATE;
    mainThread->PostTask(task, name, delayTime, priority);
    mainThread->runner_ = nullptr;
    mainThread->handler_ = nullptr;
}

/**
 * @tc.name: RemoveTask
 * @tc.desc: RemoveTask Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadPipelineTest, RemoveTask, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->runner_ = AppExecFwk::EventRunner::Create(false);
    mainThread->handler_ = std::make_shared<AppExecFwk::EventHandler>(mainThread->runner_);
    std::string name = "test";
    mainThread->RemoveTask(name);
    mainThread->runner_ = nullptr;
    mainThread->handler_ = nullptr;
}

/**
 * @tc.name: PostSyncTask002
 * @tc.desc: PostSyncTask Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadPipelineTest, PostSyncTask002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->runner_ = AppExecFwk::EventRunner::Create(false);
    mainThread->handler_ = std::make_shared<AppExecFwk::EventHandler>(mainThread->runner_);
    RSTaskMessage::RSTask task = []() -> void { return; };
    mainThread->PostSyncTask(task);
    mainThread->runner_ = nullptr;
    mainThread->handler_ = nullptr;
}

/**
 * @tc.name: PerfAfterAnim001
 * @tc.desc: PerfAfterAnim Test, not UniRender, needRequestNextVsync
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadPipelineTest, PerfAfterAnim001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = false;
    bool needRequestNextVsync = true;
    mainThread->PerfAfterAnim(needRequestNextVsync);
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: PerfAfterAnim002
 * @tc.desc: PerfAfterAnim Test, UniRender, needRequestNextVsync
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadPipelineTest, PerfAfterAnim002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = true;
    bool needRequestNextVsync = true;
    mainThread->PerfAfterAnim(needRequestNextVsync);
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: PerfAfterAnim003
 * @tc.desc: PerfAfterAnim Test, UniRender, not needRequestNextVsync
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadPipelineTest, PerfAfterAnim003, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = true;
    bool needRequestNextVsync = false;
    mainThread->PerfAfterAnim(needRequestNextVsync);
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: ForceRefreshForUni001
 * @tc.desc: ForceRefreshForUni Test, UniRender
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadPipelineTest, ForceRefreshForUni001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = true;
    mainThread->ForceRefreshForUni();
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: ForceRefreshForUni002
 * @tc.desc: ForceRefreshForUni Test, without UniRender
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadPipelineTest, ForceRefreshForUni002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = false;
    mainThread->ForceRefreshForUni();
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: PerfForBlurIfNeeded
 * @tc.desc: PerfForBlurIfNeeded Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadPipelineTest, PerfForBlurIfNeeded, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->runner_ = AppExecFwk::EventRunner::Create(false);
    mainThread->handler_ = std::make_shared<AppExecFwk::EventHandler>(mainThread->runner_);
    mainThread->PerfForBlurIfNeeded();
    mainThread->runner_ = nullptr;
    mainThread->handler_ = nullptr;
}

/**
 * @tc.name: PerfMultiWindow001
 * @tc.desc: PerfMultiWindow Test, not unirender
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadPipelineTest, PerfMultiWindow001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = false;
    mainThread->PerfMultiWindow();
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: PerfMultiWindow002
 * @tc.desc: PerfMultiWindow Test, unirender
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadPipelineTest, PerfMultiWindow002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = true;
    auto appWindowNum = mainThread->appWindowNum_;
    mainThread->appWindowNum_ = MULTI_WINDOW_PERF_START_NUM - 1;
    mainThread->PerfMultiWindow();
    mainThread->appWindowNum_ = MULTI_WINDOW_PERF_START_NUM;
    mainThread->PerfMultiWindow();
    mainThread->appWindowNum_ = MULTI_WINDOW_PERF_END_NUM + 1;
    mainThread->PerfMultiWindow();
    mainThread->isUniRender_ = isUniRender;
    mainThread->appWindowNum_ = appWindowNum;
}

/**
 * @tc.name: RenderFrameStart
 * @tc.desc: RenderFrameStart Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadPipelineTest, RenderFrameStart, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->RenderFrameStart(mainThread->timestamp_);
}

/**
 * @tc.name: CalcOcclusionImplementation001
 * @tc.desc: calculate occlusion when surfaces do not overlap
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSMainThreadPipelineTest, CalcOcclusionImplementation001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;

    NodeId idBottom = 0;
    auto nodeBottom = std::make_shared<RSSurfaceRenderNode>(idBottom, mainThread->context_);
    nodeBottom->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(idBottom);
    RectI rectBottom = RectI(0, 0, 100, 100);
    nodeBottom->oldDirtyInSurface_ = rectBottom;
    nodeBottom->SetDstRect(rectBottom);
    nodeBottom->opaqueRegion_ = Occlusion::Region(rectBottom);

    NodeId idTop = 1;
    auto nodeTop = std::make_shared<RSSurfaceRenderNode>(idTop, mainThread->context_);
    nodeTop->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(idTop);
    RectI rectTop = RectI(100, 100, 100, 100);
    nodeTop->oldDirtyInSurface_ = rectTop;
    nodeTop->SetDstRect(rectTop);
    nodeTop->opaqueRegion_ = Occlusion::Region(rectTop);

    curAllSurfaces.emplace_back(nodeBottom);
    curAllSurfaces.emplace_back(nodeTop);
    VisibleData dstCurVisVec;
    std::map<NodeId, RSVisibleLevel> dstPidVisMap;
    mainThread->CalcOcclusionImplementation(nullptr, curAllSurfaces, dstCurVisVec, dstPidVisMap);
    ASSERT_EQ(nodeBottom->GetOcclusionVisible(), true);
    ASSERT_EQ(nodeTop->GetOcclusionVisible(), true);
    ASSERT_EQ(nodeBottom->GetVisibleRegion().Size(), 1);
    ASSERT_EQ(nodeTop->GetVisibleRegion().Size(), 1);
}

/**
 * @tc.name: CalcOcclusionImplementation002
 * @tc.desc: calculate occlusion when surfaces partially overlap
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSMainThreadPipelineTest, CalcOcclusionImplementation002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;

    NodeId idBottom = 0;
    auto nodeBottom = std::make_shared<RSSurfaceRenderNode>(idBottom, mainThread->context_);
    nodeBottom->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(idBottom);
    RectI rectBottom = RectI(0, 0, 100, 100);
    nodeBottom->oldDirtyInSurface_ = rectBottom;
    nodeBottom->SetDstRect(rectBottom);
    nodeBottom->opaqueRegion_ = Occlusion::Region(rectBottom);

    NodeId idTop = 1;
    auto nodeTop = std::make_shared<RSSurfaceRenderNode>(idTop, mainThread->context_);
    nodeTop->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(idTop);
    RectI rectTop = RectI(50, 50, 100, 100);
    nodeTop->oldDirtyInSurface_ = rectTop;
    nodeTop->SetDstRect(rectTop);
    nodeTop->opaqueRegion_ = Occlusion::Region(rectTop);

    curAllSurfaces.emplace_back(nodeBottom);
    curAllSurfaces.emplace_back(nodeTop);
    VisibleData dstCurVisVec;
    std::map<NodeId, RSVisibleLevel> dstPidVisMap;
    mainThread->CalcOcclusionImplementation(nullptr, curAllSurfaces, dstCurVisVec, dstPidVisMap);
    ASSERT_EQ(nodeBottom->GetOcclusionVisible(), true);
    ASSERT_EQ(nodeTop->GetOcclusionVisible(), true);
    ASSERT_EQ(nodeBottom->GetVisibleRegion().Size(), 2);
    ASSERT_EQ(nodeTop->GetVisibleRegion().Size(), 1);
}

/**
 * @tc.name: CalcOcclusionImplementation003
 * @tc.desc: calculate occlusion when the bottom node is occluded completely
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSMainThreadPipelineTest, CalcOcclusionImplementation003, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;

    NodeId idBottom = 0;
    auto nodeBottom = std::make_shared<RSSurfaceRenderNode>(idBottom, mainThread->context_);
    nodeBottom->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(idBottom);
    RectI rectBottom = RectI(0, 0, 100, 100);
    nodeBottom->oldDirtyInSurface_ = rectBottom;
    nodeBottom->SetDstRect(rectBottom);
    nodeBottom->opaqueRegion_ = Occlusion::Region(rectBottom);

    NodeId idTop = 1;
    auto nodeTop = std::make_shared<RSSurfaceRenderNode>(idTop, mainThread->context_);
    nodeTop->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(idTop);
    RectI rectTop = RectI(0, 0, 100, 100);
    nodeTop->oldDirtyInSurface_ = rectTop;
    nodeTop->SetDstRect(rectTop);
    nodeTop->opaqueRegion_ = Occlusion::Region(rectTop);

    curAllSurfaces.emplace_back(nodeBottom);
    curAllSurfaces.emplace_back(nodeTop);
    VisibleData dstCurVisVec;
    std::map<NodeId, RSVisibleLevel> dstPidVisMap;
    mainThread->CalcOcclusionImplementation(nullptr, curAllSurfaces, dstCurVisVec, dstPidVisMap);
    ASSERT_EQ(nodeBottom->GetOcclusionVisible(), false);
    ASSERT_EQ(nodeTop->GetOcclusionVisible(), true);
    ASSERT_EQ(nodeBottom->GetVisibleRegion().Size(), 0);
    ASSERT_EQ(nodeTop->GetVisibleRegion().Size(), 1);
}

/**
 * @tc.name: CalcOcclusionImplementation004
 * @tc.desc: calculate occlusion when the bottom node is occluded completely, and the top node is transparent
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSMainThreadPipelineTest, CalcOcclusionImplementation004, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;

    NodeId idBottom = 0;
    auto nodeBottom = std::make_shared<RSSurfaceRenderNode>(idBottom, mainThread->context_);
    nodeBottom->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(idBottom);
    RectI rectBottom = RectI(0, 0, 100, 100);
    nodeBottom->oldDirtyInSurface_ = rectBottom;
    nodeBottom->SetDstRect(rectBottom);
    nodeBottom->opaqueRegion_ = Occlusion::Region(rectBottom);

    NodeId idTop = 1;
    auto nodeTop = std::make_shared<RSSurfaceRenderNode>(idTop, mainThread->context_);
    nodeTop->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(idTop);
    RectI rectTop = RectI(0, 0, 100, 100);
    nodeTop->oldDirtyInSurface_ = rectTop;
    nodeTop->SetDstRect(rectTop);
    // The top node is transparent
    nodeTop->SetGlobalAlpha(0.0f);

    curAllSurfaces.emplace_back(nodeBottom);
    curAllSurfaces.emplace_back(nodeTop);
    VisibleData dstCurVisVec;
    std::map<NodeId, RSVisibleLevel> dstPidVisMap;
    mainThread->CalcOcclusionImplementation(nullptr, curAllSurfaces, dstCurVisVec, dstPidVisMap);
    ASSERT_EQ(nodeBottom->GetOcclusionVisible(), true);
    ASSERT_EQ(nodeTop->GetOcclusionVisible(), true);
    ASSERT_EQ(nodeBottom->GetVisibleRegion().Size(), 1);
    ASSERT_EQ(nodeTop->GetVisibleRegion().Size(), 1);
}

/**
 * @tc.name: CalcOcclusionImplementation005
 * @tc.desc: calculate occlusion when the bottom node is occluded completely,
 * And the top node is transparent and filter cache valid
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSMainThreadPipelineTest, CalcOcclusionImplementation005, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;

    NodeId idBottom = 0;
    auto nodeBottom = std::make_shared<RSSurfaceRenderNode>(idBottom, mainThread->context_);
    nodeBottom->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(idBottom);
    RectI rectBottom = RectI(0, 0, 100, 100);
    nodeBottom->oldDirtyInSurface_ = rectBottom;
    nodeBottom->SetDstRect(rectBottom);
    nodeBottom->opaqueRegion_ = Occlusion::Region(rectBottom);

    NodeId idTop = 1;
    auto nodeTop = std::make_shared<RSSurfaceRenderNode>(idTop, mainThread->context_);
    nodeTop->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(idTop);
    RectI rectTop = RectI(0, 0, 100, 100);
    nodeTop->oldDirtyInSurface_ = rectTop;
    nodeTop->SetDstRect(rectTop);
    // The top node is transparent
    nodeTop->SetGlobalAlpha(0.0f);
    nodeTop->isFilterCacheValidForOcclusion_ = true;

    curAllSurfaces.emplace_back(nodeBottom);
    curAllSurfaces.emplace_back(nodeTop);
    VisibleData dstCurVisVec;
    std::map<NodeId, RSVisibleLevel> dstPidVisMap;
    mainThread->CalcOcclusionImplementation(nullptr, curAllSurfaces, dstCurVisVec, dstPidVisMap);
    ASSERT_EQ(nodeBottom->GetOcclusionVisible(), false);
    ASSERT_EQ(nodeTop->GetOcclusionVisible(), true);
    ASSERT_EQ(nodeBottom->GetVisibleRegion().Size(), 0);
    ASSERT_EQ(nodeTop->GetVisibleRegion().Size(), 1);
}

/**
 * @tc.name: DoDirectComposition
 * @tc.desc: Test DoDirectComposition
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSMainThreadPipelineTest, DoDirectComposition, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    NodeId rootId = 0;
    auto rootNode = std::make_shared<RSBaseRenderNode>(rootId);
    NodeId displayId = 1;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(displayId, config);
    rootNode->AddChild(displayNode);
    mainThread->DoDirectComposition(rootNode, false);
}
} // namespace OHOS::Rosen
