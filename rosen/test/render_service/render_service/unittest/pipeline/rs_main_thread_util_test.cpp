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
constexpr int32_t SIMI_VISIBLE_RATE = 2;
constexpr int32_t SYSTEM_ANIMATED_SCENES_RATE = 2;
constexpr int32_t INVISBLE_WINDOW_RATE = 10;
constexpr int32_t DEFAULT_RATE = 1;
constexpr int32_t INVALID_VALUE = -1;
constexpr ScreenId DEFAULT_DISPLAY_SCREEN_ID = 0;
class RSMainThreadUtilTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSMainThreadUtilTest::SetUpTestCase()
{
    RSTestUtil::InitRenderNodeGC();
}
void RSMainThreadUtilTest::TearDownTestCase() {}
void RSMainThreadUtilTest::SetUp() {}
void RSMainThreadUtilTest::TearDown() {}

/**
 * @tc.name: RsEventParamDump
 * @tc.desc: Test RSMainThreadUtilTest.RsEventParamDump
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadUtilTest, RsEventParamDump, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    std::string str = "";
    mainThread->RsEventParamDump(str);
    ASSERT_TRUE(str.empty());
}

/**
 * @tc.name: RemoveRSEventDetector001
 * @tc.desc: Test RSMainThreadUtilTest.RemoveRSEventDetector, with init
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadUtilTest, RemoveRSEventDetector001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->InitRSEventDetector();
    mainThread->RemoveRSEventDetector();
}

/**
 * @tc.name: RemoveRSEventDetector002
 * @tc.desc: Test RSMainThreadUtilTest.RemoveRSEventDetector, without init
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadUtilTest, RemoveRSEventDetector002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->RemoveRSEventDetector();
}

/**
 * @tc.name: InitRSEventDetector
 * @tc.desc: Test RSMainThreadUtilTest.InitRSEventDetector, without init
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadUtilTest, InitRSEventDetector, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->rsCompositionTimeoutDetector_ = nullptr;
    mainThread->InitRSEventDetector();
}

/**
 * @tc.name: SetRSEventDetectorLoopStartTag001
 * @tc.desc: Test RSMainThreadUtilTest.SetRSEventDetectorLoopStartTag, with init
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadUtilTest, SetRSEventDetectorLoopStartTag001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->InitRSEventDetector();
    mainThread->SetRSEventDetectorLoopStartTag();
}

/**
 * @tc.name: SetRSEventDetectorLoopStartTag002
 * @tc.desc: Test RSMainThreadUtilTest.SetRSEventDetectorLoopStartTag, without init
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadUtilTest, SetRSEventDetectorLoopStartTag002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->rsCompositionTimeoutDetector_ = nullptr;
    mainThread->SetRSEventDetectorLoopStartTag();
}

/**
 * @tc.name: SetRSEventDetectorLoopFinishTag001
 * @tc.desc: Test RSMainThreadUtilTest.SetRSEventDetectorLoopFinishTag, with init
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadUtilTest, SetRSEventDetectorLoopFinishTag001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->InitRSEventDetector();
    auto isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = false;
    mainThread->SetRSEventDetectorLoopFinishTag();
    mainThread->isUniRender_ = true;
    mainThread->SetRSEventDetectorLoopFinishTag();
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: SetRSEventDetectorLoopFinishTag002
 * @tc.desc: Test RSMainThreadUtilTest.SetRSEventDetectorLoopFinishTag, without init
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadUtilTest, SetRSEventDetectorLoopFinishTag002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->rsCompositionTimeoutDetector_ = nullptr;
    mainThread->SetRSEventDetectorLoopFinishTag();
}

/**
 * @tc.name: RenderServiceTreeDump
 * @tc.desc: Test RSMainThreadUtilTest.RenderServiceTreeDump, str is an empty string
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadUtilTest, RenderServiceTreeDump, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    std::string str = "";
    mainThread->RenderServiceTreeDump(str);
}

/**
 * @tc.name: SetFocusAppInfo
 * @tc.desc: Test RSMainThreadUtilTest.SetFocusAppInfo, input pid, uid is -1, str is an empty string
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadUtilTest, SetFocusAppInfo, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    std::string str = "";
    int32_t pid = INVALID_VALUE;
    int32_t uid = INVALID_VALUE;
    mainThread->SetFocusAppInfo(pid, uid, str, str, 0);
}

/**
 * @tc.name: SetFocusAppInfo002
 * @tc.desc: Test SetFocusAppInfo while change focus node
 * @tc.type: FUNC
 * @tc.require: issueI9LOXQ
 */
HWTEST_F(RSMainThreadUtilTest, SetFocusAppInfo002, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    NodeId id = 0;
    auto oldFocusNode = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    auto newFocusNode = std::make_shared<RSSurfaceRenderNode>(id + 1, mainThread->context_);
    mainThread->focusNodeId_ = oldFocusNode->GetId();

    std::string str = "";
    int32_t pid = INVALID_VALUE;
    int32_t uid = INVALID_VALUE;
    mainThread->SetFocusAppInfo(pid, uid, str, str, newFocusNode->GetId());
    ASSERT_EQ(mainThread->GetFocusNodeId(), newFocusNode->GetId());
}

/**
 * @tc.name: GetContext
 * @tc.desc: Test if context has been initialized
 * @tc.type: FUNC
 * @tc.require: issueI6Q9A2
 */
HWTEST_F(RSMainThreadUtilTest, GetContext, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    auto& context = mainThread->GetContext();
    uint64_t time = 0;
    ASSERT_EQ(context.GetTransactionTimestamp(), time);
}

/**
 * @tc.name: AddActiveNode
 * @tc.desc: Test AddActiveNode, add invalid node id, check if fails
 * @tc.type: FUNC
 * @tc.require: issueI6Q9A2
 */
HWTEST_F(RSMainThreadUtilTest, AddActiveNode, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->context_->activeNodesInRoot_.clear();
    // invalid nodeid
    NodeId id = INVALID_NODEID;
    auto node = std::make_shared<RSRenderNode>(id, mainThread->context_);
    mainThread->context_->AddActiveNode(node);
    ASSERT_EQ(static_cast<int>(mainThread->context_->activeNodesInRoot_.size()), 0);
}

/**
 * @tc.name: CheckAndUpdateInstanceContentStaticStatus01
 * @tc.desc: Test static instance(no dirty) would be classify as only basic geo transform
 * @tc.type: FUNC
 * @tc.require: issueI8IXTX
 */
HWTEST_F(RSMainThreadUtilTest, CheckAndUpdateInstanceContentStaticStatus01, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->context_->activeNodesInRoot_.clear();
    // valid nodeid
    NodeId id = 1;
    auto node = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    ASSERT_NE(node, nullptr);
    mainThread->CheckAndUpdateInstanceContentStaticStatus(node);
    ASSERT_EQ(node->GetSurfaceCacheContentStatic(), true);
}

/**
 * @tc.name: CheckAndUpdateInstanceContentStaticStatus02
 * @tc.desc: Test new instance would not be classify as only basic geo transform
 * @tc.type: FUNC
 * @tc.require: issueI8IXTX
 */
HWTEST_F(RSMainThreadUtilTest, CheckAndUpdateInstanceContentStaticStatus02, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->context_->activeNodesInRoot_.clear();
    // valid nodeid
    NodeId id = 1;
    auto node = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    ASSERT_NE(node, nullptr);
    node->SetIsOnTheTree(true, id, id);
    node->SetContentDirty();
    mainThread->context_->AddActiveNode(node);
    ASSERT_EQ(static_cast<int>(mainThread->context_->activeNodesInRoot_.size()), 1);
    mainThread->CheckAndUpdateInstanceContentStaticStatus(node);
    ASSERT_EQ(node->GetSurfaceCacheContentStatic(), false);
}

/**
 * @tc.name: IsNeedProcessBySingleFrameComposerTest001
 * @tc.desc: Test IsNeedProcessBySingleFrameComposerTest when TransactionData is null
 * @tc.type: FUNC
 * @tc.require: issueI9HPBS
 */
HWTEST_F(RSMainThreadUtilTest, IsNeedProcessBySingleFrameComposerTest001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    std::unique_ptr<RSTransactionData> transactionData = nullptr;
    ASSERT_FALSE(mainThread->IsNeedProcessBySingleFrameComposer(transactionData));
    transactionData = std::make_unique<RSTransactionData>();
    mainThread->isUniRender_ = true;
    ASSERT_FALSE(mainThread->IsNeedProcessBySingleFrameComposer(transactionData));
}

/**
 * @tc.name: IsNeedProcessBySingleFrameComposerTest002
 * @tc.desc: Test IsNeedProcessBySingleFrameComposerTest when SingleFrameComposer enabled by app process
 * @tc.type: FUNC
 * @tc.require: issueI9HPBS
 */
HWTEST_F(RSMainThreadUtilTest, IsNeedProcessBySingleFrameComposerTest002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    auto transactionData = std::make_unique<RSTransactionData>();
    mainThread->isUniRender_ = true;
    pid_t pid = 1;
    transactionData->SetSendingPid(pid);
    RSSingleFrameComposer::AddOrRemoveAppPidToMap(true, pid);
    ASSERT_TRUE(mainThread->IsNeedProcessBySingleFrameComposer(transactionData));
}

/**
 * @tc.name: IsNeedProcessBySingleFrameComposerTest003
 * @tc.desc: Test IsNeedProcessBySingleFrameComposerTest when animation node exists
 * @tc.type: FUNC
 * @tc.require: issueI9HPBS
 */
HWTEST_F(RSMainThreadUtilTest, IsNeedProcessBySingleFrameComposerTest003, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    auto transactionData = std::make_unique<RSTransactionData>();
    mainThread->isUniRender_ = true;
    pid_t pid = 1;
    transactionData->SetSendingPid(pid);
    RSSingleFrameComposer::AddOrRemoveAppPidToMap(true, pid);

    NodeId id = 1;
    auto node = std::make_shared<RSRenderNode>(id, mainThread->context_);
    mainThread->context_->RegisterAnimatingRenderNode(node);
    ASSERT_FALSE(mainThread->IsNeedProcessBySingleFrameComposer(transactionData));
}

/**
 * @tc.name: IsNeedProcessBySingleFrameComposerTest004
 * @tc.desc: Test IsNeedProcessBySingleFrameComposerTest when multi-window shown on screen
 * @tc.type: FUNC
 * @tc.require: issueI9HPBS
 */
HWTEST_F(RSMainThreadUtilTest, IsNeedProcessBySingleFrameComposerTest004, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    auto transactionData = std::make_unique<RSTransactionData>();
    mainThread->isUniRender_ = true;
    pid_t pid = 1;
    transactionData->SetSendingPid(pid);
    RSSingleFrameComposer::AddOrRemoveAppPidToMap(true, pid);

    NodeId firstWindowNodeId = 2;
    auto firstWindowNode = std::make_shared<RSSurfaceRenderNode>(firstWindowNodeId, mainThread->context_);
    firstWindowNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    NodeId firstWindowChildNodeId = 3;
    auto firstWindowChildNode = std::make_shared<RSSurfaceRenderNode>(firstWindowChildNodeId, mainThread->context_);
    firstWindowChildNode->MarkUIHidden(false);
    firstWindowNode->AddChild(firstWindowChildNode);
    firstWindowNode->GenerateFullChildrenList();
    mainThread->context_->nodeMap.RegisterRenderNode(firstWindowNode);

    NodeId secondWindowNodeId = 2;
    auto secondWindowNode = std::make_shared<RSSurfaceRenderNode>(secondWindowNodeId, mainThread->context_);
    secondWindowNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    NodeId secondWindowChildNodeId = 3;
    auto secondWindowChildNode = std::make_shared<RSSurfaceRenderNode>(secondWindowChildNodeId, mainThread->context_);
    secondWindowChildNode->MarkUIHidden(false);
    secondWindowNode->AddChild(secondWindowChildNode);
    secondWindowNode->GenerateFullChildrenList();
    mainThread->context_->nodeMap.RegisterRenderNode(secondWindowNode);
    ASSERT_FALSE(mainThread->IsNeedProcessBySingleFrameComposer(transactionData));
}

/**
 * @tc.name: ShowWatermark
 * @tc.desc: ShowWatermark test
 * @tc.type: FUNC
 * @tc.require: issueI78T3Z
 */
HWTEST_F(RSMainThreadUtilTest, ShowWatermark, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    const uint32_t color[8] = { 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80 };
    uint32_t colorLength = sizeof(color) / sizeof(color[0]);
    const int32_t offset = 0;
    Media::InitializationOptions opts;
    int32_t stride = 3;
    std::unique_ptr<Media::PixelMap> pixelMap1 = Media::PixelMap::Create(color, colorLength, offset, stride, opts);
    mainThread->ShowWatermark(std::move(pixelMap1), true);
    ASSERT_EQ(mainThread->GetWatermarkFlag(), true);
    mainThread->ShowWatermark(nullptr, false);
    ASSERT_EQ(mainThread->GetWatermarkFlag(), false);
}

/**
 * @tc.name: MergeToEffectiveTransactionDataMap001
 * @tc.desc: Test RSMainThreadUtilTest.MergeToEffectiveTransactionDataMap
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSMainThreadUtilTest, MergeToEffectiveTransactionDataMap001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->Start();
    ASSERT_EQ(mainThread->effectiveTransactionDataIndexMap_.empty(), true);
    mainThread->effectiveTransactionDataIndexMap_[0].first = 0;
    TransactionDataMap dataMap;
    auto data = std::make_unique<RSTransactionData>();
    ASSERT_NE(data, nullptr);
    data->SetIndex(1);
    dataMap[0].emplace_back(std::move(data));
    data = std::make_unique<RSTransactionData>();
    ASSERT_NE(data, nullptr);
    data->SetIndex(3);
    dataMap[0].emplace_back(std::move(data));
    dataMap[0].emplace_back(nullptr);
    mainThread->MergeToEffectiveTransactionDataMap(dataMap);
    mainThread->effectiveTransactionDataIndexMap_.clear();
}

/**
 * @tc.name: GetWatermarkImg
 * @tc.desc: GetWatermarkImg test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadUtilTest, GetWatermarkImg, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->GetWatermarkImg();
}

/**
 * @tc.name: IsWatermarkFlagChanged
 * @tc.desc: IsWatermarkFlagChanged test
 * @tc.type: FUNC
 * @tc.require: issuesIA8LNR
 */
HWTEST_F(RSMainThreadUtilTest, IsWatermarkFlagChanged, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->IsWatermarkFlagChanged();
}

/**
 * @tc.name: SetIdleTimerExpiredFlag
 * @tc.desc: SetIdleTimerExpiredFlag test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadUtilTest, SetIdleTimerExpiredFlag, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->SetIdleTimerExpiredFlag(true);
}

/**
 * @tc.name: SetFocusLeashWindowId
 * @tc.desc: Test RSMainThreadUtilTest.SetFocusLeashWindowId
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSMainThreadUtilTest, SetFocusLeashWindowId, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->Start();
    NodeId id = 1;
    auto node1 = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    ASSERT_NE(node1, nullptr);
    RSSurfaceRenderNodeConfig config;
    auto node2 = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(node2, nullptr);
    node1->SetParent(node2);
    node1->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    node2->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;

    mainThread->context_ = std::make_shared<RSContext>();
    mainThread->context_->nodeMap.renderNodeMap_[0] = node1;
    mainThread->focusNodeId_ = 0;
    mainThread->SetFocusLeashWindowId();
}

/**
 * @tc.name: SetFocusLeashWindowId002
 * @tc.desc: Test SetFocusLeashWindowId while nodeMap is empty
 * @tc.type: FUNC
 * @tc.require: issueI98VTC
 */
HWTEST_F(RSMainThreadUtilTest, SetFocusLeashWindowId002, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    mainThread->SetFocusLeashWindowId();
    ASSERT_EQ(mainThread->GetFocusLeashWindowId(), INVALID_NODEID);
}

/**
 * @tc.name: SetFocusLeashWindowId003
 * @tc.desc: Test SetFocusLeashWindowId while focus node don't have parent
 * @tc.type: FUNC
 * @tc.require: issueI98VTC
 */
HWTEST_F(RSMainThreadUtilTest, SetFocusLeashWindowId003, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    auto node = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(mainThread, nullptr);
    ASSERT_NE(node, nullptr);

    ASSERT_NE(mainThread->context_, nullptr);
    mainThread->context_->nodeMap.renderNodeMap_[node->GetId()] = node;
    std::string str = "";
    mainThread->SetFocusAppInfo(-1, -1, str, str, node->GetId());
    mainThread->SetFocusLeashWindowId();
    ASSERT_EQ(mainThread->GetFocusLeashWindowId(), INVALID_NODEID);
}

/**
 * @tc.name: SetFocusLeashWindowId004
 * @tc.desc: Test SetFocusLeashWindowId while focus node's type don't match
 * @tc.type: FUNC
 * @tc.require: issueI98VTC
 */
HWTEST_F(RSMainThreadUtilTest, SetFocusLeashWindowId004, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    auto childNode = RSTestUtil::CreateSurfaceNode();
    auto parentNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(mainThread, nullptr);
    ASSERT_NE(childNode, nullptr);
    ASSERT_NE(parentNode, nullptr);

    parentNode->AddChild(parentNode);
    childNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);
    parentNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);

    mainThread->context_->nodeMap.renderNodeMap_[childNode->GetId()] = childNode;
    mainThread->context_->nodeMap.renderNodeMap_[parentNode->GetId()] = parentNode;
    std::string str = "";
    mainThread->SetFocusAppInfo(-1, -1, str, str, childNode->GetId());
    mainThread->SetFocusLeashWindowId();
    ASSERT_EQ(mainThread->GetFocusLeashWindowId(), INVALID_NODEID);
}

/**
 * @tc.name: SetFocusLeashWindowId005
 * @tc.desc: Test SetFocusLeashWindowId while focus node's parent's type don't match
 * @tc.type: FUNC
 * @tc.require: issueI98VTC
 */
HWTEST_F(RSMainThreadUtilTest, SetFocusLeashWindowId005, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    auto childNode = RSTestUtil::CreateSurfaceNode();
    auto parentNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(mainThread, nullptr);
    ASSERT_NE(childNode, nullptr);
    ASSERT_NE(parentNode, nullptr);

    parentNode->AddChild(parentNode);
    childNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    parentNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);

    mainThread->context_->nodeMap.renderNodeMap_[childNode->GetId()] = childNode;
    mainThread->context_->nodeMap.renderNodeMap_[parentNode->GetId()] = parentNode;
    std::string str = "";
    mainThread->SetFocusAppInfo(-1, -1, str, str, childNode->GetId());
    mainThread->SetFocusLeashWindowId();
    ASSERT_EQ(mainThread->GetFocusLeashWindowId(), INVALID_NODEID);
}

/**
 * @tc.name: SetIsCachedSurfaceUpdated
 * @tc.desc: Test RSMainThreadUtilTest.SetIsCachedSurfaceUpdated
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSMainThreadUtilTest, SetIsCachedSurfaceUpdated, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->SetIsCachedSurfaceUpdated(true);
}

/**
 * @tc.name: PrintCurrentStatus
 * @tc.desc: Test RSMainThreadUtilTest.PrintCurrentStatus
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSMainThreadUtilTest, PrintCurrentStatus, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->PrintCurrentStatus();
}

/**
 * @tc.name: SetDeviceType
 * @tc.desc: Test RSMainThreadUtilTest.SetDeviceType
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSMainThreadUtilTest, SetDeviceType, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    system::SetParameter("const.product.devicetype", "pc");
    mainThread->SetDeviceType();
    system::SetParameter("const.product.devicetype", "tablet");
    mainThread->SetDeviceType();
    system::SetParameter("const.product.devicetype", "others");
    mainThread->SetDeviceType();
    system::SetParameter("const.product.devicetype", "phone");
    mainThread->SetDeviceType();
}

/**
 * @tc.name: CacheCommands
 * @tc.desc: Test RSMainThreadUtilTest.CacheCommands
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSMainThreadUtilTest, CacheCommands, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto data = std::make_unique<RSTransactionData>();
    ASSERT_NE(data, nullptr);
    data->SetIndex(1);
    mainThread->cachedSkipTransactionDataMap_[0].emplace_back(std::move(data));
    data = std::make_unique<RSTransactionData>();
    ASSERT_NE(data, nullptr);
    data->SetIndex(2);
    mainThread->cachedSkipTransactionDataMap_[0].emplace_back(std::move(data));
    mainThread->CacheCommands();
}

/**
 * @tc.name: CheckIfNodeIsBundle
 * @tc.desc: Test RSMainThreadUtilTest.CheckIfNodeIsBundle
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSMainThreadUtilTest, CheckIfNodeIsBundle, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    RSSurfaceRenderNodeConfig config;
    auto node = std::make_shared<RSSurfaceRenderNode>(config);
    node->name_ = "WallpaperView";
    mainThread->CheckIfNodeIsBundle(node);
    ASSERT_TRUE(mainThread->noBundle_);
}

/**
 * @tc.name: InformHgmNodeInfo
 * @tc.desc: Test RSMainThreadUtilTest.InformHgmNodeInfo
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSMainThreadUtilTest, InformHgmNodeInfo, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->currentBundleName_ = "test";
    mainThread->InformHgmNodeInfo();
    mainThread->currentBundleName_ = "";
    mainThread->noBundle_ = true;
    mainThread->InformHgmNodeInfo();
}

/**
 * @tc.name: CheckParallelSubThreadNodesStatus
 * @tc.desc: Test RSMainThreadUtilTest.CheckParallelSubThreadNodesStatus
 * @tc.type: FUNC
 * @tc.require: issueI8V6MD
 */
HWTEST_F(RSMainThreadUtilTest, CheckParallelSubThreadNodesStatus, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->subThreadNodes_.clear();
    mainThread->subThreadNodes_.push_back(nullptr);
    mainThread->isUiFirstOn_ = false;
    mainThread->CheckParallelSubThreadNodesStatus();

    auto node1 = std::make_shared<RSSurfaceRenderNode>(0xFFFFFFFFFFFFFFFF);
    node1->cacheProcessStatus_ = CacheProcessStatus::DOING;
    node1->name_ = "node1";
    node1->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    node1->hasAbilityComponent_ = true;
    node1->abilityNodeIds_.emplace(10);
    node1->abilityNodeIds_.emplace(11);
    auto node2 = std::make_shared<RSSurfaceRenderNode>(2);
    node2->cacheProcessStatus_ = CacheProcessStatus::DOING;
    node2->name_ = "node2";
    node2->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    auto node3 = std::make_shared<RSSurfaceRenderNode>(0xFFFFFFFFFFFFFFF0);
    node3->cacheProcessStatus_ = CacheProcessStatus::DOING;
    node3->name_ = "node3";
    node3->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    node3->hasAbilityComponent_ = false;
    // create child nodes
    auto childNode1 = std::make_shared<RSSurfaceRenderNode>(3);
    childNode1->name_ = "childNode1";
    childNode1->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    auto childNode2 = std::make_shared<RSSurfaceRenderNode>(4);
    childNode2->name_ = "childNode2";
    childNode2->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    auto childNode3 = std::make_shared<RSSurfaceRenderNode>(5);
    childNode3->name_ = "childNode3";

    node2->AddChild(childNode1);
    node2->AddChild(childNode2);
    node2->AddChild(childNode3);
    mainThread->subThreadNodes_.push_back(node1);
    mainThread->subThreadNodes_.push_back(node2);
    mainThread->subThreadNodes_.push_back(node3);

    mainThread->cacheCmdSkippedInfo_.clear();
    mainThread->CheckParallelSubThreadNodesStatus();
}

/**
 * @tc.name: IsNeedSkip
 * @tc.desc: IsNeedSkip test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadUtilTest, IsNeedSkip, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->IsNeedSkip(0, 0);
}

/**
 * @tc.name: SkipCommandByNodeId001
 * @tc.desc: SkipCommandByNodeId test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadUtilTest, SkipCommandByNodeId001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    std::vector<std::unique_ptr<RSTransactionData>> transactionVec;
    mainThread->SkipCommandByNodeId(transactionVec, -1);
}

/**
 * @tc.name: SkipCommandByNodeId002
 * @tc.desc: SkipCommandByNodeId test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadUtilTest, SkipCommandByNodeId002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    // prepare transactionVec
    std::vector<std::unique_ptr<RSTransactionData>> transactionVec;
    auto data = std::make_unique<RSTransactionData>();
    data->SetIndex(1);
    transactionVec.emplace_back(std::move(data));
    data = std::make_unique<RSTransactionData>();
    data->SetIndex(2);
    data->payload_.resize(2);
    data->payload_[0] = std::tuple<NodeId, FollowType, std::unique_ptr<RSCommand>>(0, FollowType::NONE, nullptr);
    transactionVec.emplace_back(std::move(data));
    std::pair<std::vector<NodeId>, bool> info = {{0}, true};
    mainThread->cacheCmdSkippedInfo_.insert({0, info});
    mainThread->SkipCommandByNodeId(transactionVec, 0);
}

/**
 * @tc.name: ConsumeAndUpdateAllNodes001
 * @tc.desc: ConsumeAndUpdateAllNodes test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadUtilTest, ConsumeAndUpdateAllNodes001, TestSize.Level1)
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
HWTEST_F(RSMainThreadUtilTest, ConsumeAndUpdateAllNodes002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    bool isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = false;
    mainThread->ConsumeAndUpdateAllNodes();
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: CheckSubThreadNodeStatusIsDoing001
 * @tc.desc: CheckSubThreadNodeStatusIsDoing test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadUtilTest, CheckSubThreadNodeStatusIsDoing001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->subThreadNodes_.clear();
    auto rsSurfaceRenderNode1 = RSTestUtil::CreateSurfaceNode();
    rsSurfaceRenderNode1->SetCacheSurfaceProcessedStatus(CacheProcessStatus::WAITING);
    auto rsSurfaceRenderNode2 = RSTestUtil::CreateSurfaceNode();
    rsSurfaceRenderNode2->SetCacheSurfaceProcessedStatus(CacheProcessStatus::DOING);
    mainThread->subThreadNodes_.emplace_back(rsSurfaceRenderNode1);
    mainThread->subThreadNodes_.emplace_back(rsSurfaceRenderNode2);
    mainThread->CheckSubThreadNodeStatusIsDoing(rsSurfaceRenderNode2->GetId());
}

/**
 * @tc.name: CheckSubThreadNodeStatusIsDoing002
 * @tc.desc: CheckSubThreadNodeStatusIsDoing test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadUtilTest, CheckSubThreadNodeStatusIsDoing002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->subThreadNodes_.clear();
    auto rsSurfaceRenderNode1 = RSTestUtil::CreateSurfaceNode();
    rsSurfaceRenderNode1->SetCacheSurfaceProcessedStatus(CacheProcessStatus::DOING);
    auto rsSurfaceRenderNode2 = RSTestUtil::CreateSurfaceNode();
    rsSurfaceRenderNode1->AddChild(rsSurfaceRenderNode2);
    mainThread->subThreadNodes_.emplace_back(rsSurfaceRenderNode1);
    mainThread->CheckSubThreadNodeStatusIsDoing(rsSurfaceRenderNode2->GetId());
}

/**
 * @tc.name: CollectInfoForHardwareComposer001
 * @tc.desc: CheckSubThreadNodeStatusIsDoing test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadUtilTest, CollectInfoForHardwareComposer001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    bool isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = false;
    mainThread->CollectInfoForHardwareComposer();
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: CollectInfoForHardwareComposer002
 * @tc.desc: CheckSubThreadNodeStatusIsDoing test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadUtilTest, CollectInfoForHardwareComposer002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    bool isUniRender = mainThread->isUniRender_;
    mainThread->isUniRender_ = true;
    mainThread->CollectInfoForHardwareComposer();
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: IsLastFrameUIFirstEnbaled001
 * @tc.desc: IsLastFrameUIFirstEnbaled test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadUtilTest, IsLastFrameUIFirstEnbaled001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->subThreadNodes_.clear();
    // not app window
    NodeId id = 1;
    auto node = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    node->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    mainThread->subThreadNodes_.push_back(node);
    mainThread->IsLastFrameUIFirstEnabled(0);
    // app window
    id = 2;
    node = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    node->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    mainThread->subThreadNodes_.push_back(node);
    mainThread->IsLastFrameUIFirstEnabled(2);
}


/**
 * @tc.name: IsLastFrameUIFirstEnbaled002
 * @tc.desc: IsLastFrameUIFirstEnbaled test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadUtilTest, IsLastFrameUIFirstEnbaled002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->subThreadNodes_.clear();
    // parent
    NodeId id = 1;
    auto node1 = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    node1->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    // child
    id = 2;
    auto node2 = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    node2->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    node1->AddChild(node2);
    mainThread->subThreadNodes_.push_back(node1);
    mainThread->IsLastFrameUIFirstEnabled(2);
}

/**
 * @tc.name: CheckIfHardwareForcedDisabled
 * @tc.desc: CheckIfHardwareForcedDisabled test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadUtilTest, CheckIfHardwareForcedDisabled, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->CheckIfHardwareForcedDisabled();
}

/**
 * @tc.name: MergeToEffectiveTransactionDataMap
 * @tc.desc: MergeToEffectiveTransactionDataMap test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadUtilTest, MergeToEffectiveTransactionDataMap, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    TransactionDataMap tsDataMap;
    tsDataMap.emplace(0, std::vector<std::unique_ptr<RSTransactionData>>());
    auto data = std::make_unique<RSTransactionData>();
    data->SetIndex(1);
    tsDataMap[0].emplace_back(std::move(data));
    mainThread->MergeToEffectiveTransactionDataMap(tsDataMap);
}

/**
 * @tc.name: CheckSurfaceNeedProcess
 * @tc.desc: CheckSurfaceNeedProcess test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadUtilTest, CheckSurfaceNeedProcess, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    OcclusionRectISet occRectISet;
    // not focus node
    NodeId id = 1;
    auto node = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    mainThread->CheckSurfaceNeedProcess(occRectISet, node);
    // focus node
    id = mainThread->focusNodeId_;
    node = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    mainThread->CheckSurfaceNeedProcess(occRectISet, node);
}

/**
 * @tc.name: GetRegionVisibleLevel001
 * @tc.desc: GetRegionVisibleLevel test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadUtilTest, GetRegionVisibleLevel001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    // empty region
    Occlusion::Region occRegion;
    mainThread->GetRegionVisibleLevel(occRegion, occRegion);
}

/**
 * @tc.name: GetRegionVisibleLevel002
 * @tc.desc: GetRegionVisibleLevel test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadUtilTest, GetRegionVisibleLevel002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    // equal region
    Occlusion::Region occRegion;
    occRegion.rects_.emplace_back(Occlusion::Rect(0, 1, 1, 0));
    mainThread->GetRegionVisibleLevel(occRegion, occRegion);
}

/**
 * @tc.name: GetRegionVisibleLevel003
 * @tc.desc: GetRegionVisibleLevel test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadUtilTest, GetRegionVisibleLevel003, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    Occlusion::Region occRegion1;
    occRegion1.rects_.emplace_back(Occlusion::Rect(0, 1, 1, 0));
    Occlusion::Region occRegion2;
    occRegion2.rects_.emplace_back(Occlusion::Rect(0, 2, 2, 0));
    mainThread->GetRegionVisibleLevel(occRegion1, occRegion2);
}

/**
 * @tc.name: CollectInfoForHardwareComposer003
 * @tc.desc: CollectInfoForHardwareComposer003 Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadUtilTest, CollectInfoForHardwareComposer003, TestSize.Level1)
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
    node3->SetIsOnTheTree(true);
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node3);
    auto node4 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    node4->SetIsOnTheTree(true);
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node4);
    auto node5 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    node5->SetIsOnTheTree(true);
    node5->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    node5->isHardwareEnabledNode_ = true;
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node5);
    mainThread->CollectInfoForHardwareComposer();
    mainThread->isUniRender_ = isUniRender;
}

/**
 * @tc.name: CheckNodeHasToBePreparedByPid001
 * @tc.desc: CheckNodeHasToBePreparedByPid Test, Classify By Root
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadUtilTest, CheckNodeHasToBePreparedByPid001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    NodeId id = 1;
    bool isClassifyByRoot = true;
    mainThread->CheckNodeHasToBePreparedByPid(id, isClassifyByRoot);
}

/**
 * @tc.name: CheckNodeHasToBePreparedByPid002
 * @tc.desc: CheckNodeHasToBePreparedByPid Test, not Classify By Root
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadUtilTest, CheckNodeHasToBePreparedByPid002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    NodeId id = 1;
    bool isClassifyByRoot = false;
    mainThread->CheckNodeHasToBePreparedByPid(id, isClassifyByRoot);
}

/**
 * @tc.name: SetSystemAnimatedScenes001
 * @tc.desc: SetSystemAnimatedScenes Test, case set 1
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadUtilTest, SetSystemAnimatedScenes001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    SystemAnimatedScenes scenesInit = mainThread->systemAnimatedScenes_;
    SystemAnimatedScenes scenes = SystemAnimatedScenes::ENTER_MISSION_CENTER;
    ASSERT_TRUE(mainThread->SetSystemAnimatedScenes(scenes));
    scenes = SystemAnimatedScenes::EXIT_MISSION_CENTER;
    ASSERT_TRUE(mainThread->SetSystemAnimatedScenes(scenes));
    scenes = SystemAnimatedScenes::ENTER_TFS_WINDOW;
    ASSERT_TRUE(mainThread->SetSystemAnimatedScenes(scenes));
    scenes = SystemAnimatedScenes::EXIT_TFU_WINDOW;
    ASSERT_TRUE(mainThread->SetSystemAnimatedScenes(scenes));
    scenes = SystemAnimatedScenes::ENTER_WINDOW_FULL_SCREEN;
    ASSERT_TRUE(mainThread->SetSystemAnimatedScenes(scenes));
    scenes = SystemAnimatedScenes::EXIT_WINDOW_FULL_SCREEN;
    ASSERT_TRUE(mainThread->SetSystemAnimatedScenes(scenes));
    scenes = SystemAnimatedScenes::ENTER_MAX_WINDOW;
    ASSERT_TRUE(mainThread->SetSystemAnimatedScenes(scenes));
    scenes = SystemAnimatedScenes::EXIT_MAX_WINDOW;
    ASSERT_TRUE(mainThread->SetSystemAnimatedScenes(scenes));
    mainThread->systemAnimatedScenes_ = scenesInit;
}

/**
 * @tc.name: SetSystemAnimatedScenes002
 * @tc.desc: SetSystemAnimatedScenes Test, case set 2
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadUtilTest, SetSystemAnimatedScenes002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    SystemAnimatedScenes scenesInit = mainThread->systemAnimatedScenes_;
    SystemAnimatedScenes scenes = SystemAnimatedScenes::ENTER_SPLIT_SCREEN;
    ASSERT_TRUE(mainThread->SetSystemAnimatedScenes(scenes));
    scenes = SystemAnimatedScenes::EXIT_SPLIT_SCREEN;
    ASSERT_TRUE(mainThread->SetSystemAnimatedScenes(scenes));
    scenes = SystemAnimatedScenes::ENTER_APP_CENTER;
    ASSERT_TRUE(mainThread->SetSystemAnimatedScenes(scenes));
    scenes = SystemAnimatedScenes::EXIT_APP_CENTER;
    ASSERT_TRUE(mainThread->SetSystemAnimatedScenes(scenes));
    scenes = SystemAnimatedScenes::APPEAR_MISSION_CENTER;
    ASSERT_TRUE(mainThread->SetSystemAnimatedScenes(scenes));
    scenes = SystemAnimatedScenes::ENTER_WIND_CLEAR;
    ASSERT_TRUE(mainThread->SetSystemAnimatedScenes(scenes));
    scenes = SystemAnimatedScenes::ENTER_WIND_RECOVER;
    ASSERT_TRUE(mainThread->SetSystemAnimatedScenes(scenes));
    scenes = SystemAnimatedScenes::OTHERS;
    ASSERT_TRUE(mainThread->SetSystemAnimatedScenes(scenes));
    mainThread->systemAnimatedScenes_ = scenesInit;
}

/**
 * @tc.name: SetSystemAnimatedScenes003
 * @tc.desc: SetSystemAnimatedScenes Test, System Animated Scenes Disabled
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadUtilTest, SetSystemAnimatedScenes003, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto systemAnimatedScenesEnabled = mainThread->systemAnimatedScenesEnabled_;
    mainThread->systemAnimatedScenesEnabled_ = false;
    ASSERT_TRUE(mainThread->SetSystemAnimatedScenes(SystemAnimatedScenes::OTHERS));
    mainThread->systemAnimatedScenesEnabled_ = systemAnimatedScenesEnabled;
}

/**
 * @tc.name: SetVSyncRateByVisibleLevel001
 * @tc.desc: SetVSyncRateByVisibleLevel Test, Check Vsyncrate when RSVisibleLevel is RS_SEMI_DEFAULT_VISIBLE
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadUtilTest, SetVSyncRateByVisibleLevel001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    if (mainThread->appVSyncDistributor_ == nullptr) {
        auto vsyncGenerator = CreateVSyncGenerator();
        auto vsyncController = new VSyncController(vsyncGenerator, 0);
        mainThread->appVSyncDistributor_ = new VSyncDistributor(vsyncController, "WMVSyncConnection");
    }
    auto& vsyncDistributor = mainThread->appVSyncDistributor_;
    ASSERT_NE(vsyncDistributor, nullptr);
    vsyncDistributor->connectionsMap_.clear();
    sptr<VSyncConnection> connection = new VSyncConnection(vsyncDistributor, "WMVSyncConnection_0");
    uint32_t tmpPid = 0;
    if (vsyncDistributor->QosGetPidByName(connection->info_.name_, tmpPid) == VSYNC_ERROR_OK) {
        vsyncDistributor->connectionsMap_[tmpPid].push_back(connection);
        std::map<NodeId, RSVisibleLevel> pidVisMap;
        pidVisMap[static_cast<NodeId>(tmpPid)] = RSVisibleLevel::RS_SEMI_DEFAULT_VISIBLE;
        std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;
        mainThread->lastVisMapForVsyncRate_.clear();
        mainThread->SetVSyncRateByVisibleLevel(pidVisMap, curAllSurfaces);
        ASSERT_EQ(connection->highPriorityRate_, (int32_t)SIMI_VISIBLE_RATE);
    }
}

/**
 * @tc.name: SetVSyncRateByVisibleLevel002
 * @tc.desc: SetVSyncRateByVisibleLevel Test, Check Vsyncrate when RSVisibleLevel is RS_SYSTEM_ANIMATE_SCENE
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadUtilTest, SetVSyncRateByVisibleLevel002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    if (mainThread->appVSyncDistributor_ == nullptr) {
        auto vsyncGenerator = CreateVSyncGenerator();
        auto vsyncController = new VSyncController(vsyncGenerator, 0);
        mainThread->appVSyncDistributor_ = new VSyncDistributor(vsyncController, "WMVSyncConnection");
    }
    auto& vsyncDistributor = mainThread->appVSyncDistributor_;
    ASSERT_NE(vsyncDistributor, nullptr);
    vsyncDistributor->connectionsMap_.clear();
    sptr<VSyncConnection> connection = new VSyncConnection(vsyncDistributor, "WMVSyncConnection_0");
    uint32_t tmpPid = 0;
    if (vsyncDistributor->QosGetPidByName(connection->info_.name_, tmpPid) == VSYNC_ERROR_OK) {
        vsyncDistributor->connectionsMap_[tmpPid].push_back(connection);
        std::map<NodeId, RSVisibleLevel> pidVisMap;
        pidVisMap[static_cast<NodeId>(tmpPid)] = RSVisibleLevel::RS_SYSTEM_ANIMATE_SCENE;
        std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;
        mainThread->lastVisMapForVsyncRate_.clear();
        mainThread->SetVSyncRateByVisibleLevel(pidVisMap, curAllSurfaces);
        ASSERT_EQ(connection->highPriorityRate_, (int32_t)SYSTEM_ANIMATED_SCENES_RATE);
    }
}

/**
 * @tc.name: SetVSyncRateByVisibleLevel003
 * @tc.desc: SetVSyncRateByVisibleLevel Test, Check Vsyncrate when RSVisibleLevel is RS_INVISIBLE
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadUtilTest, SetVSyncRateByVisibleLevel003, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    if (mainThread->appVSyncDistributor_ == nullptr) {
        auto vsyncGenerator = CreateVSyncGenerator();
        auto vsyncController = new VSyncController(vsyncGenerator, 0);
        mainThread->appVSyncDistributor_ = new VSyncDistributor(vsyncController, "WMVSyncConnection");
    }
    auto& vsyncDistributor = mainThread->appVSyncDistributor_;
    ASSERT_NE(vsyncDistributor, nullptr);
    vsyncDistributor->connectionsMap_.clear();
    sptr<VSyncConnection> connection = new VSyncConnection(vsyncDistributor, "WMVSyncConnection_0");
    uint32_t tmpPid = 0;
    if (vsyncDistributor->QosGetPidByName(connection->info_.name_, tmpPid) == VSYNC_ERROR_OK) {
        vsyncDistributor->connectionsMap_[tmpPid].push_back(connection);
        std::map<NodeId, RSVisibleLevel> pidVisMap;
        pidVisMap[static_cast<NodeId>(tmpPid)] = RSVisibleLevel::RS_INVISIBLE;
        std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;
        mainThread->lastVisMapForVsyncRate_.clear();
        mainThread->SetVSyncRateByVisibleLevel(pidVisMap, curAllSurfaces);
        ASSERT_EQ(connection->highPriorityRate_, (int32_t)INVISBLE_WINDOW_RATE);
    }
}

/**
 * @tc.name: SetVSyncRateByVisibleLevel004
 * @tc.desc: SetVSyncRateByVisibleLevel Test, Check Vsyncrate when RSVisibleLevel is RS_UNKNOW_VISIBLE_LEVEL
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadUtilTest, SetVSyncRateByVisibleLevel004, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    if (mainThread->appVSyncDistributor_ == nullptr) {
        auto vsyncGenerator = CreateVSyncGenerator();
        auto vsyncController = new VSyncController(vsyncGenerator, 0);
        mainThread->appVSyncDistributor_ = new VSyncDistributor(vsyncController, "WMVSyncConnection");
    }
    auto& vsyncDistributor = mainThread->appVSyncDistributor_;
    ASSERT_NE(vsyncDistributor, nullptr);
    vsyncDistributor->connectionsMap_.clear();
    sptr<VSyncConnection> connection = new VSyncConnection(vsyncDistributor, "WMVSyncConnection_0");
    uint32_t tmpPid = 0;
    if (vsyncDistributor->QosGetPidByName(connection->info_.name_, tmpPid) == VSYNC_ERROR_OK) {
        vsyncDistributor->connectionsMap_[tmpPid].push_back(connection);
        std::map<NodeId, RSVisibleLevel> pidVisMap;
        pidVisMap[static_cast<NodeId>(tmpPid)] = RSVisibleLevel::RS_UNKNOW_VISIBLE_LEVEL;
        std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;
        mainThread->lastVisMapForVsyncRate_.clear();
        mainThread->SetVSyncRateByVisibleLevel(pidVisMap, curAllSurfaces);
        ASSERT_EQ(connection->highPriorityRate_, (int32_t)DEFAULT_RATE);
    }
}

/**
 * @tc.name: SetSystemAnimatedScenes004
 * @tc.desc: SetVSyncRateByVisibleLevel Test, Check Vsyncrate when SystemAnimatedScenes is ENTER_MISSION_CENTER
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadUtilTest, SetSystemAnimatedScenes004, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->systemAnimtedSceneEnabled_ = true;
    mainThread->SetSystemAnimatedScenes(SystemAnimatedScenes::ENTER_MISSION_CENTER);
    ASSERT_NE(mainThread->systemAnimatedScenesList_.empty(), true);
    if (mainThread->appVSyncDistributor_ == nullptr) {
        auto vsyncGenerator = CreateVSyncGenerator();
        auto vsyncController = new VSyncController(vsyncGenerator, 0);
        mainThread->appVSyncDistributor_ = new VSyncDistributor(vsyncController, "WMVSyncConnection");
    }
    auto& vsyncDistributor = mainThread->appVSyncDistributor_;
    ASSERT_NE(vsyncDistributor, nullptr);
    vsyncDistributor->connectionsMap_.clear();
    sptr<VSyncConnection> connection = new VSyncConnection(vsyncDistributor, "WMVSyncConnection_0");
    uint32_t tmpPid = 0;
    ASSERT_EQ(vsyncDistributor->QosGetPidByName(connection->info_.name_, tmpPid), VSYNC_ERROR_OK);
    vsyncDistributor->connectionsMap_[tmpPid].push_back(connection);
    std::map<NodeId, RSVisibleLevel> pidVisMap;
    pidVisMap[static_cast<NodeId>(tmpPid)] = RSVisibleLevel::RS_UNKNOW_VISIBLE_LEVEL;
    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;
    mainThread->lastVisMapForVsyncRate_.clear();
    mainThread->SetVSyncRateByVisibleLevel(pidVisMap, curAllSurfaces);
    ASSERT_NE(connection->highPriorityRate_, (int32_t)SYSTEM_ANIMATED_SCENES_RATE);
}

/**
 * @tc.name: SetSystemAnimatedScenes005
 * @tc.desc: SetVSyncRateByVisibleLevel Test, Check Vsyncrate when SystemAnimatedScenes is ENTER_TFS_WINDOW
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadUtilTest, SetSystemAnimatedScenes005, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->systemAnimtedSceneEnabled_ = true;
    mainThread->SetSystemAnimatedScenes(SystemAnimatedScenes::ENTER_TFS_WINDOW);
    ASSERT_NE(mainThread->systemAnimatedScenesList_.empty(), true);
    if (mainThread->appVSyncDistributor_ == nullptr) {
        auto vsyncGenerator = CreateVSyncGenerator();
        auto vsyncController = new VSyncController(vsyncGenerator, 0);
        mainThread->appVSyncDistributor_ = new VSyncDistributor(vsyncController, "WMVSyncConnection");
    }
    auto& vsyncDistributor = mainThread->appVSyncDistributor_;
    ASSERT_NE(vsyncDistributor, nullptr);
    vsyncDistributor->connectionsMap_.clear();
    sptr<VSyncConnection> connection = new VSyncConnection(vsyncDistributor, "WMVSyncConnection_0");
    uint32_t tmpPid = 0;
    ASSERT_EQ(vsyncDistributor->QosGetPidByName(connection->info_.name_, tmpPid), VSYNC_ERROR_OK);
    vsyncDistributor->connectionsMap_[tmpPid].push_back(connection);
    std::map<NodeId, RSVisibleLevel> pidVisMap;
    pidVisMap[static_cast<NodeId>(tmpPid)] = RSVisibleLevel::RS_UNKNOW_VISIBLE_LEVEL;
    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;
    mainThread->lastVisMapForVsyncRate_.clear();
    mainThread->SetVSyncRateByVisibleLevel(pidVisMap, curAllSurfaces);
    ASSERT_NE(connection->highPriorityRate_, (int32_t)SYSTEM_ANIMATED_SCENES_RATE);
}

/**
 * @tc.name: SetSystemAnimatedScenes006
 * @tc.desc: SetVSyncRateByVisibleLevel Test, Check Vsyncrate when SystemAnimatedScenes is ENTER_WINDOW_FULL_SCREEN
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadUtilTest, SetSystemAnimatedScenes006, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->systemAnimtedSceneEnabled_ = true;
    mainThread->SetSystemAnimatedScenes(SystemAnimatedScenes::ENTER_WINDOW_FULL_SCREEN);
    ASSERT_NE(mainThread->systemAnimatedScenesList_.empty(), true);
    if (mainThread->appVSyncDistributor_ == nullptr) {
        auto vsyncGenerator = CreateVSyncGenerator();
        auto vsyncController = new VSyncController(vsyncGenerator, 0);
        mainThread->appVSyncDistributor_ = new VSyncDistributor(vsyncController, "WMVSyncConnection");
    }
    auto& vsyncDistributor = mainThread->appVSyncDistributor_;
    ASSERT_NE(vsyncDistributor, nullptr);
    vsyncDistributor->connectionsMap_.clear();
    sptr<VSyncConnection> connection = new VSyncConnection(vsyncDistributor, "WMVSyncConnection_0");
    uint32_t tmpPid = 0;
    ASSERT_EQ(vsyncDistributor->QosGetPidByName(connection->info_.name_, tmpPid), VSYNC_ERROR_OK);
    vsyncDistributor->connectionsMap_[tmpPid].push_back(connection);
    std::map<NodeId, RSVisibleLevel> pidVisMap;
    pidVisMap[static_cast<NodeId>(tmpPid)] = RSVisibleLevel::RS_UNKNOW_VISIBLE_LEVEL;
    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;
    mainThread->lastVisMapForVsyncRate_.clear();
    mainThread->SetVSyncRateByVisibleLevel(pidVisMap, curAllSurfaces);
    ASSERT_NE(connection->highPriorityRate_, (int32_t)SYSTEM_ANIMATED_SCENES_RATE);
}

/**
 * @tc.name: SetSystemAnimatedScenes007
 * @tc.desc: SetVSyncRateByVisibleLevel Test, Check Vsyncrate when SystemAnimatedScenes is ENTER_MAX_WINDOW
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadUtilTest, SetSystemAnimatedScenes007, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->systemAnimtedSceneEnabled_ = true;
    mainThread->SetSystemAnimatedScenes(SystemAnimatedScenes::ENTER_MAX_WINDOW);
    ASSERT_NE(mainThread->systemAnimatedScenesList_.empty(), true);
    if (mainThread->appVSyncDistributor_ == nullptr) {
        auto vsyncGenerator = CreateVSyncGenerator();
        auto vsyncController = new VSyncController(vsyncGenerator, 0);
        mainThread->appVSyncDistributor_ = new VSyncDistributor(vsyncController, "WMVSyncConnection");
    }
    auto& vsyncDistributor = mainThread->appVSyncDistributor_;
    ASSERT_NE(vsyncDistributor, nullptr);
    vsyncDistributor->connectionsMap_.clear();
    sptr<VSyncConnection> connection = new VSyncConnection(vsyncDistributor, "WMVSyncConnection_0");
    uint32_t tmpPid = 0;
    ASSERT_EQ(vsyncDistributor->QosGetPidByName(connection->info_.name_, tmpPid), VSYNC_ERROR_OK);
    vsyncDistributor->connectionsMap_[tmpPid].push_back(connection);
    std::map<NodeId, RSVisibleLevel> pidVisMap;
    pidVisMap[static_cast<NodeId>(tmpPid)] = RSVisibleLevel::RS_UNKNOW_VISIBLE_LEVEL;
    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;
    mainThread->lastVisMapForVsyncRate_.clear();
    mainThread->SetVSyncRateByVisibleLevel(pidVisMap, curAllSurfaces);
    ASSERT_NE(connection->highPriorityRate_, (int32_t)SYSTEM_ANIMATED_SCENES_RATE);
}

/**
 * @tc.name: SetSystemAnimatedScenes008
 * @tc.desc: SetVSyncRateByVisibleLevel Test, Check Vsyncrate when SystemAnimatedScenes is ENTER_SPLIT_SCREEN
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadUtilTest, SetSystemAnimatedScenes008, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->systemAnimtedSceneEnabled_ = true;
    mainThread->SetSystemAnimatedScenes(SystemAnimatedScenes::ENTER_SPLIT_SCREEN);
    ASSERT_NE(mainThread->systemAnimatedScenesList_.empty(), true);
    if (mainThread->appVSyncDistributor_ == nullptr) {
        auto vsyncGenerator = CreateVSyncGenerator();
        auto vsyncController = new VSyncController(vsyncGenerator, 0);
        mainThread->appVSyncDistributor_ = new VSyncDistributor(vsyncController, "WMVSyncConnection");
    }
    auto& vsyncDistributor = mainThread->appVSyncDistributor_;
    ASSERT_NE(vsyncDistributor, nullptr);
    vsyncDistributor->connectionsMap_.clear();
    sptr<VSyncConnection> connection = new VSyncConnection(vsyncDistributor, "WMVSyncConnection_0");
    uint32_t tmpPid = 0;
    ASSERT_EQ(vsyncDistributor->QosGetPidByName(connection->info_.name_, tmpPid), VSYNC_ERROR_OK);
    vsyncDistributor->connectionsMap_[tmpPid].push_back(connection);
    std::map<NodeId, RSVisibleLevel> pidVisMap;
    pidVisMap[static_cast<NodeId>(tmpPid)] = RSVisibleLevel::RS_UNKNOW_VISIBLE_LEVEL;
    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;
    mainThread->lastVisMapForVsyncRate_.clear();
    mainThread->SetVSyncRateByVisibleLevel(pidVisMap, curAllSurfaces);
    ASSERT_NE(connection->highPriorityRate_, (int32_t)SYSTEM_ANIMATED_SCENES_RATE);
}

/**
 * @tc.name: SetSystemAnimatedScenes009
 * @tc.desc: SetVSyncRateByVisibleLevel Test, Check Vsyncrate when SystemAnimatedScenes is ENTER_APP_CENTER
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadUtilTest, SetSystemAnimatedScenes009, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->SetSystemAnimatedScenes(SystemAnimatedScenes::ENTER_APP_CENTER);
    ASSERT_NE(mainThread->systemAnimatedScenesList_.empty(), true);
    if (mainThread->appVSyncDistributor_ == nullptr) {
        auto vsyncGenerator = CreateVSyncGenerator();
        auto vsyncController = new VSyncController(vsyncGenerator, 0);
        mainThread->appVSyncDistributor_ = new VSyncDistributor(vsyncController, "WMVSyncConnection");
    }
    auto& vsyncDistributor = mainThread->appVSyncDistributor_;
    ASSERT_NE(vsyncDistributor, nullptr);
    vsyncDistributor->connectionsMap_.clear();
    sptr<VSyncConnection> connection = new VSyncConnection(vsyncDistributor, "WMVSyncConnection_0");
    uint32_t tmpPid = 0;
    ASSERT_EQ(vsyncDistributor->QosGetPidByName(connection->info_.name_, tmpPid), VSYNC_ERROR_OK);
    vsyncDistributor->connectionsMap_[tmpPid].push_back(connection);
    std::map<NodeId, RSVisibleLevel> pidVisMap;
    pidVisMap[static_cast<NodeId>(tmpPid)] = RSVisibleLevel::RS_UNKNOW_VISIBLE_LEVEL;
    std::vector<RSBaseRenderNode::SharedPtr> curAllSurfaces;
    mainThread->lastVisMapForVsyncRate_.clear();
    mainThread->SetVSyncRateByVisibleLevel(pidVisMap, curAllSurfaces);
    ASSERT_NE(connection->highPriorityRate_, (int32_t)SYSTEM_ANIMATED_SCENES_RATE);
}

/**
 * @tc.name: IsDrawingGroupChanged
 * @tc.desc: IsDrawingGroupChanged Test, not Classify By Root
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadUtilTest, IsDrawingGroupChanged, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    NodeId id = 1;
    auto node = std::make_shared<RSRenderNode>(id);
    mainThread->IsDrawingGroupChanged(*node);
}

/**
 * @tc.name: CheckAndUpdateInstanceContentStaticStatus003
 * @tc.desc: CheckAndUpdateInstanceContentStaticStatus Test, nullptr
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadUtilTest, CheckAndUpdateInstanceContentStaticStatus003, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->CheckAndUpdateInstanceContentStaticStatus(nullptr);
}

/**
 * @tc.name: UpdateRogSizeIfNeeded
 * @tc.desc: UpdateRogSizeIfNeeded Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadUtilTest, UpdateRogSizeIfNeeded, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    // prepare context
    auto contextInit = mainThread->context_;
    auto context = std::make_shared<RSContext>();
    auto rootNode = context->GetGlobalRootRenderNode();
    NodeId id = 1;
    RSDisplayNodeConfig config;
    auto childDisplayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    rootNode->AddChild(childDisplayNode);
    mainThread->context_ = context;
    mainThread->UpdateRogSizeIfNeeded();
    mainThread->context_ = contextInit;
}

/**
 * @tc.name: UpdateUIFirstSwitch001
 * @tc.desc: UpdateUIFirstSwitch Test, root node nullptr
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadUtilTest, UpdateUIFirstSwitch001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto rootNode = mainThread->context_->globalRootRenderNode_;
    mainThread->context_->globalRootRenderNode_ = nullptr;
    mainThread->UpdateUIFirstSwitch();
    mainThread->context_->globalRootRenderNode_ = rootNode;
}

/**
 * @tc.name: UpdateUIFirstSwitch002
 * @tc.desc: UpdateUIFirstSwitch Test, with surfacenode child
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadUtilTest, UpdateUIFirstSwitch002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto rootNode = mainThread->context_->globalRootRenderNode_;
    // one child
    NodeId id = 1;
    auto node1 = std::make_shared<RSRenderNode>(id);
    id = 2;
    auto node2 = std::make_shared<RSSurfaceRenderNode>(id);
    node1->AddChild(node2);
    mainThread->context_->globalRootRenderNode_ = node1;
    mainThread->UpdateUIFirstSwitch();
    mainThread->context_->globalRootRenderNode_ = rootNode;
}

/**
 * @tc.name: UpdateUIFirstSwitch003
 * @tc.desc: UpdateUIFirstSwitch Test, with displaynode child
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadUtilTest, UpdateUIFirstSwitch003, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto rootNode = mainThread->context_->globalRootRenderNode_;
    // one child
    NodeId id = 1;
    auto node1 = std::make_shared<RSRenderNode>(id);
    id = 2;
    RSDisplayNodeConfig config;
    auto node2 = std::make_shared<RSDisplayRenderNode>(id, config);
    node1->AddChild(node2);
    mainThread->context_->globalRootRenderNode_ = node1;
    mainThread->UpdateUIFirstSwitch();
    mainThread->context_->globalRootRenderNode_ = rootNode;
}

/**
 * @tc.name: ReleaseSurface
 * @tc.desc: ReleaseSurface Test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSMainThreadUtilTest, ReleaseSurface, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->tmpSurfaces_.push(nullptr);
    mainThread->ReleaseSurface();
}

/**
 * @tc.name: RefreshEntireDisplay
 * @tc.desc: RefreshEntireDisplay Test
 * @tc.type: FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSMainThreadUtilTest, RefreshEntireDisplay, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->RefreshEntireDisplay();
    ASSERT_EQ(mainThread->IsCurtainScreenUsingStatusChanged(), true);
}

/**
 * @tc.name: SetCurtainScreenUsingStatus
 * @tc.desc: SetCurtainScreenUsingStatus Test
 * @tc.type: FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSMainThreadUtilTest, SetCurtainScreenUsingStatus, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->SetCurtainScreenUsingStatus(true);
    ASSERT_EQ(mainThread->IsCurtainScreenOn(), true);

    // restore curtain screen status
    mainThread->SetCurtainScreenUsingStatus(false);
}

/**
 * @tc.name: UpdateDisplayNodeScreenId001
 * @tc.desc: UpdateDisplayNodeScreenId, when rootnode is nullptr.
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSMainThreadUtilTest, UpdateDisplayNodeScreenId001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    ASSERT_NE(mainThread->context_, nullptr);
    mainThread->context_->globalRootRenderNode_ = nullptr;
    mainThread->UpdateDisplayNodeScreenId();
    ASSERT_EQ(mainThread->displayNodeScreenId_, DEFAULT_DISPLAY_SCREEN_ID);
}

/**
 * @tc.name: UpdateDisplayNodeScreenId002
 * @tc.desc: UpdateDisplayNodeScreenId, root node has no child display node.
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSMainThreadUtilTest, UpdateDisplayNodeScreenId002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    ASSERT_NE(mainThread->context_, nullptr);
    NodeId id = 1;
    mainThread->context_->globalRootRenderNode_ = std::make_shared<RSRenderNode>(id);
    mainThread->UpdateDisplayNodeScreenId();
    ASSERT_EQ(mainThread->displayNodeScreenId_, DEFAULT_DISPLAY_SCREEN_ID);
}

/**
 * @tc.name: UpdateDisplayNodeScreenId003
 * @tc.desc: UpdateDisplayNodeScreenId, root node has one child display node.
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSMainThreadUtilTest, UpdateDisplayNodeScreenId003, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    NodeId rootId = 0;
    ASSERT_NE(mainThread->context_, nullptr);
    mainThread->context_->globalRootRenderNode_ = std::make_shared<RSRenderNode>(rootId);
    RSDisplayNodeConfig config;
    NodeId displayId = 1;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(displayId, config);
    uint64_t screenId = 1;
    displayNode->SetScreenId(screenId);
    mainThread->context_->globalRootRenderNode_->AddChild(displayNode);
    ASSERT_FALSE(mainThread->context_->globalRootRenderNode_->children_.empty());
    mainThread->UpdateDisplayNodeScreenId();
}

/**
 * @tc.name: ProcessScreenHotPlugEvents
 * @tc.desc: Test ProcessScreenHotPlugEvents
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSMainThreadUtilTest, ProcessScreenHotPlugEvents, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->ProcessScreenHotPlugEvents();
}

/**
 * @tc.name: CheckSystemSceneStatus001
 * @tc.desc: Test CheckSystemSceneStatus, APPEAR_MISSION_CENTER
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSMainThreadUtilTest, CheckSystemSceneStatus001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->SetSystemAnimatedScenes(SystemAnimatedScenes::APPEAR_MISSION_CENTER);
    mainThread->CheckSystemSceneStatus();
}

/**
 * @tc.name: CheckSystemSceneStatus002
 * @tc.desc: Test CheckSystemSceneStatus, ENTER_TFS_WINDOW
 * @tc.type: FUNC
 * @tc.require: issueI97LXT
 */
HWTEST_F(RSMainThreadUtilTest, CheckSystemSceneStatus002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->SetSystemAnimatedScenes(SystemAnimatedScenes::ENTER_TFS_WINDOW);
    mainThread->CheckSystemSceneStatus();
}

/**
 * @tc.name: UpdateNeedDrawFocusChange001
 * @tc.desc: test UpdateNeedDrawFocusChange while node don't has parent
 * @tc.type: FUNC
 * @tc.require: issueI9LOXQ
 */
HWTEST_F(RSMainThreadUtilTest, UpdateNeedDrawFocusChange001, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    NodeId id = 0;
    auto node = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    ASSERT_NE(node, nullptr);

    ASSERT_NE(mainThread->context_, nullptr);
    mainThread->context_->nodeMap.renderNodeMap_[node->GetId()] = node;
    mainThread->UpdateNeedDrawFocusChange(id);
    ASSERT_TRUE(node->GetNeedDrawFocusChange());
}

/**
 * @tc.name: UpdateNeedDrawFocusChange002
 * @tc.desc: test UpdateNeedDrawFocusChange while node's parent isn't leash window
 * @tc.type: FUNC
 * @tc.require: issueI9LOXQ
 */
HWTEST_F(RSMainThreadUtilTest, UpdateNeedDrawFocusChange002, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    NodeId id = 0;
    auto node = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    auto parentNode = std::make_shared<RSSurfaceRenderNode>(id + 1, mainThread->context_);
    ASSERT_NE(node, nullptr);
    ASSERT_NE(parentNode, nullptr);
    parentNode->AddChild(node);
    parentNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);

    ASSERT_NE(mainThread->context_, nullptr);
    mainThread->context_->nodeMap.renderNodeMap_[node->GetId()] = node;
    mainThread->context_->nodeMap.renderNodeMap_[parentNode->GetId()] = parentNode;
    mainThread->UpdateNeedDrawFocusChange(id);
    ASSERT_TRUE(node->GetNeedDrawFocusChange());
}

/**
 * @tc.name: UpdateNeedDrawFocusChange003
 * @tc.desc: test UpdateNeedDrawFocusChange while node's parent is leash window
 * @tc.type: FUNC
 * @tc.require: issueI9LOXQ
 */
HWTEST_F(RSMainThreadUtilTest, UpdateNeedDrawFocusChange003, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    NodeId id = 0;
    auto node = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    auto parentNode = std::make_shared<RSSurfaceRenderNode>(id + 1, mainThread->context_);
    ASSERT_NE(node, nullptr);
    ASSERT_NE(parentNode, nullptr);
    parentNode->AddChild(node);
    parentNode->SetSurfaceNodeType(RSSurfaceNodeType::LEASH_WINDOW_NODE);

    ASSERT_NE(mainThread->context_, nullptr);
    mainThread->context_->nodeMap.renderNodeMap_[node->GetId()] = node;
    mainThread->context_->nodeMap.renderNodeMap_[parentNode->GetId()] = parentNode;
    mainThread->UpdateNeedDrawFocusChange(id);
    ASSERT_FALSE(node->GetNeedDrawFocusChange());
}

/**
 * @tc.name: UpdateFocusNodeId001
 * @tc.desc: test UpdateFocusNodeId while focusNodeId don't change
 * @tc.type: FUNC
 * @tc.require: issueI9LOXQ
 */
HWTEST_F(RSMainThreadUtilTest, UpdateFocusNodeId001, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    NodeId id = 0;
    auto node = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    ASSERT_NE(node, nullptr);

    ASSERT_NE(mainThread->context_, nullptr);
    mainThread->context_->nodeMap.renderNodeMap_[node->GetId()] = node;
    mainThread->focusNodeId_ = id;
    mainThread->UpdateFocusNodeId(id);
    ASSERT_EQ(mainThread->GetFocusNodeId(), id);
}

/**
 * @tc.name: UpdateFocusNodeId002
 * @tc.desc: test UpdateFocusNodeId while newfocusNodeId is invalid
 * @tc.type: FUNC
 * @tc.require: issueI9LOXQ
 */
HWTEST_F(RSMainThreadUtilTest, UpdateFocusNodeId002, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    NodeId id = 0;
    auto node = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    ASSERT_NE(node, nullptr);

    ASSERT_NE(mainThread->context_, nullptr);
    mainThread->context_->nodeMap.renderNodeMap_[node->GetId()] = node;
    mainThread->focusNodeId_ = id;
    mainThread->UpdateFocusNodeId(INVALID_NODEID);
    ASSERT_EQ(mainThread->GetFocusNodeId(), id);
}

/**
 * @tc.name: UpdateFocusNodeId003
 * @tc.desc: test UpdateFocusNodeId while focus node change
 * @tc.type: FUNC
 * @tc.require: issueI9LOXQ
 */
HWTEST_F(RSMainThreadUtilTest, UpdateFocusNodeId003, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    NodeId id = 0;
    auto oldFocusNode = std::make_shared<RSSurfaceRenderNode>(id, mainThread->context_);
    auto newFocusNode = std::make_shared<RSSurfaceRenderNode>(id + 1, mainThread->context_);
    ASSERT_NE(oldFocusNode, nullptr);
    ASSERT_NE(newFocusNode, nullptr);

    mainThread->context_->nodeMap.renderNodeMap_[oldFocusNode->GetId()] = oldFocusNode;
    mainThread->context_->nodeMap.renderNodeMap_[newFocusNode->GetId()] = newFocusNode;
    mainThread->focusNodeId_ = oldFocusNode->GetId();
    mainThread->UpdateFocusNodeId(newFocusNode->GetId());
    ASSERT_EQ(mainThread->GetFocusNodeId(), newFocusNode->GetId());
}

/**
 * @tc.name: CheckUIExtensionCallbackDataChanged001
 * @tc.desc: test CheckUIExtensionCallbackDataChanged, no need to callback (2 frames of empty callback data)
 * @tc.type: FUNC
 * @tc.require: issueIABHAX
 */
HWTEST_F(RSMainThreadUtilTest, UIExtensionNodesTraverseAndCallback001, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->lastFrameUIExtensionDataEmpty_ = true;
    mainThread->uiExtensionCallbackData_.clear();
    ASSERT_FALSE(mainThread->CheckUIExtensionCallbackDataChanged());
}

/**
 * @tc.name: CheckUIExtensionCallbackDataChanged002
 * @tc.desc: test CheckUIExtensionCallbackDataChanged, first frame of empty callbackdata, need to callback
 * @tc.type: FUNC
 * @tc.require: issueIABHAX
 */
HWTEST_F(RSMainThreadUtilTest, CheckUIExtensionCallbackDataChanged002, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->lastFrameUIExtensionDataEmpty_ = false;
    mainThread->uiExtensionCallbackData_.clear();
    ASSERT_TRUE(mainThread->CheckUIExtensionCallbackDataChanged());
}
} // namespace OHOS::Rosen
