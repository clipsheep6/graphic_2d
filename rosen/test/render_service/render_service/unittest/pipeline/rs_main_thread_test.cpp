/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"
#include "limit_number.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_qos_thread.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSMainThreadTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSMainThreadTest::SetUpTestCase() {}
void RSMainThreadTest::TearDownTestCase() {}
void RSMainThreadTest::SetUp() {}
void RSMainThreadTest::TearDown() {}

/**
 * @tc.name: Start001
 * @tc.desc: Test RSMainThreadTest.Start
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, Start001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->Start();
}

/**
 * @tc.name: Start002
 * @tc.desc: Test RSMainThreadTest.Start
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, Start002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->runner_ = nullptr;
    mainThread->Start();
}

/**
 * @tc.name: ProcessCommand
 * @tc.desc: Test RSMainThreadTest.ProcessCommand
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, ProcessCommand, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->isUniRender_ = false;
    mainThread->ProcessCommand();
}

/**
 * @tc.name: RsEventParamDump
 * @tc.desc: Test RSMainThreadTest.RsEventParamDump
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, RsEventParamDump, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    std::string str = "";
    mainThread->RsEventParamDump(str);
    ASSERT_NE(str.size(), 0);
}

/**
 * @tc.name: RemoveRSEventDetector001
 * @tc.desc: Test RSMainThreadTest.RemoveRSEventDetector, with init
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, RemoveRSEventDetector001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->InitRSEventDetector();
    mainThread->RemoveRSEventDetector();
}

/**
 * @tc.name: RemoveRSEventDetector002
 * @tc.desc: Test RSMainThreadTest.RemoveRSEventDetector, without init
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, RemoveRSEventDetector002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->RemoveRSEventDetector();
}

/**
 * @tc.name: InitRSEventDetector
 * @tc.desc: Test RSMainThreadTest.InitRSEventDetector, without init
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, InitRSEventDetector, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->rsCompositionTimeoutDetector_ = nullptr;
    mainThread->InitRSEventDetector();
}

/**
 * @tc.name: SetRSEventDetectorLoopStartTag001
 * @tc.desc: Test RSMainThreadTest.SetRSEventDetectorLoopStartTag, with init
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, SetRSEventDetectorLoopStartTag001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->InitRSEventDetector();
    mainThread->SetRSEventDetectorLoopStartTag();
}

/**
 * @tc.name: SetRSEventDetectorLoopStartTag002
 * @tc.desc: Test RSMainThreadTest.SetRSEventDetectorLoopStartTag, without init
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, SetRSEventDetectorLoopStartTag002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->SetRSEventDetectorLoopStartTag();
}

/**
 * @tc.name: SetRSEventDetectorLoopFinishTag001
 * @tc.desc: Test RSMainThreadTest.SetRSEventDetectorLoopFinishTag, with init
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, SetRSEventDetectorLoopFinishTag001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->InitRSEventDetector();
    mainThread->SetRSEventDetectorLoopFinishTag();
}

/**
 * @tc.name: SetRSEventDetectorLoopFinishTag002
 * @tc.desc: Test RSMainThreadTest.SetRSEventDetectorLoopFinishTag, without init
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, SetRSEventDetectorLoopFinishTag002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->SetRSEventDetectorLoopFinishTag();
}

/**
 * @tc.name: WaitUtilUniRenderFinished
 * @tc.desc: Test RSMainThreadTest.WaitUtilUniRenderFinished
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, WaitUtilUniRenderFinished, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->NotifyUniRenderFinish();
    mainThread->WaitUtilUniRenderFinished();
}

/**
 * @tc.name: ProcessCommandForDividedRender001
 * @tc.desc: Test RSMainThreadTest.ProcessCommandForDividedRender
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, ProcessCommandForDividedRender001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->ProcessCommandForDividedRender();
}

/**
 * @tc.name: CalcOcclusion
 * @tc.desc: Test RSMainThreadTest.CalcOcclusion, doWindowAnimate_ is false, isUniRender_ is true
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, CalcOcclusion, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->doWindowAnimate_ = false;
    mainThread->isUniRender_ = true;
    mainThread->CalcOcclusion();
}

/**
 * @tc.name: CheckQosVisChanged001
 * @tc.desc: Test RSMainThreadTest.CheckQosVisChanged, pidVisMap is empty
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, CheckQosVisChanged001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    std::map<uint32_t, bool> pidVisMap;
    auto isVisibleChanged = mainThread->CheckQosVisChanged(pidVisMap);
    ASSERT_EQ(false, isVisibleChanged);
}

/**
 * @tc.name: CheckQosVisChanged002
 * @tc.desc: Test RSMainThreadTest.CheckQosVisChanged, pidVisMap is not empty
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, CheckQosVisChanged002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    std::map<uint32_t, bool> pidVisMap;
    pidVisMap[0] = true;
    mainThread->lastPidVisMap_[0] = false;
    auto isVisibleChanged = mainThread->CheckQosVisChanged(pidVisMap);
    ASSERT_EQ(true, isVisibleChanged);
}

/**
 * @tc.name: CheckQosVisChanged003
 * @tc.desc: Test RSMainThreadTest.CheckQosVisChanged, pidVisMap is not empty, lastPidVisMap_ equals to pidVisMap
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, CheckQosVisChanged003, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    std::map<uint32_t, bool> pidVisMap;
    pidVisMap[0] = true;
    mainThread->lastPidVisMap_[0] = true;
    auto isVisibleChanged = mainThread->CheckQosVisChanged(pidVisMap);
    ASSERT_EQ(false, isVisibleChanged);
}

/**
 * @tc.name: Animate001
 * @tc.desc: Test RSMainThreadTest.Animate, doWindowAnimate_ is false
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, Animate001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->doWindowAnimate_ = false;
    mainThread->Animate(0);
}

/**
 * @tc.name: Animate002
 * @tc.desc: Test RSMainThreadTest.Animate, doWindowAnimate_ is true
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, Animate002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->doWindowAnimate_ = true;
    mainThread->Animate(0);
}

/**
 * @tc.name: UnRegisterOcclusionChangeCallback
 * @tc.desc: Test RSMainThreadTest.Animate
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, UnRegisterOcclusionChangeCallback, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->UnRegisterOcclusionChangeCallback(nullptr);
}

/**
 * @tc.name: CleanOcclusionListener
 * @tc.desc: Test RSMainThreadTest.CleanOcclusionListener
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, CleanOcclusionListener, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    mainThread->CleanOcclusionListener();
}

/**
 * @tc.name: QosStateDump001
 * @tc.desc: When qosCal_ is false, QosStateDump's dump string is "QOS is disabled\n"
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, QosStateDump001, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    RSQosThread::GetInstance()->SetQosCal(false);
    std::string str = "";
    mainThread->QosStateDump(str);
    ASSERT_EQ(str, "QOS is disabled\n");
}

/**
 * @tc.name: QosStateDump002
 * @tc.desc: When qosCal_ is true, QosStateDump's dump string is "QOS is enabled\n"
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, QosStateDump002, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    RSQosThread::GetInstance()->SetQosCal(true);
    std::string str = "";
    mainThread->QosStateDump(str);
    ASSERT_EQ(str, "QOS is enabled\n");
}

/**
 * @tc.name: RenderServiceTreeDump
 * @tc.desc: Test RSMainThreadTest.RenderServiceTreeDump, str is an empty string
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, RenderServiceTreeDump, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    std::string str = "";
    mainThread->RenderServiceTreeDump(str);
}

/**
 * @tc.name: SetFocusAppInfo
 * @tc.desc: Test RSMainThreadTest.SetFocusAppInfo, input pid, uid is -1, str is an empty string
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSMainThreadTest, SetFocusAppInfo, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    std::string str = "";
    mainThread->SetFocusAppInfo(-1, -1, str, str);
}

/**
 * @tc.name: ProcessSyncRSTransactionData
 * @tc.desc: Test ProcessSyncRSTransactionData when TransactionData do not need sync
 * @tc.type: FUNC
 * @tc.require: issueI6Q9A2
 */
HWTEST_F(RSMainThreadTest, ProcessSyncRSTransactionData, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    auto rsTransactionData = std::make_unique<RSTransactionData>();
    pid_t pid = 0;
    mainThread->ProcessSyncRSTransactionData(rsTransactionData, pid);
    ASSERT_EQ(mainThread->syncTransactionDatas_.empty(), false);
}

/**
 * @tc.name: GetContext
 * @tc.desc: Test if context has been initialized
 * @tc.type: FUNC
 * @tc.require: issueI6Q9A2
 */
HWTEST_F(RSMainThreadTest, GetContext, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    auto& context = mainThread->GetContext();
    uint64_t time = 0;
    ASSERT_EQ(context.GetTransactionTimestamp(), time);
}

/**
 * @tc.name: ClassifyRSTransactionData
 * @tc.desc: Test ClassifyRSTransactionData when nodeId is 0
 * @tc.type: FUNC
 * @tc.require: issueI6Q9A2
 */
HWTEST_F(RSMainThreadTest, ClassifyRSTransactionData, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    auto rsTransactionData = std::make_unique<RSTransactionData>();
    std::unique_ptr<RSCommand> command = nullptr;
    NodeId nodeId = 0;
    FollowType followType = FollowType::NONE;
    rsTransactionData->AddCommand(command, nodeId, followType);
    ASSERT_EQ(mainThread->pendingEffectiveCommands_.empty(), true);
}

/**
 * @tc.name: ResetSortedChildren
 * @tc.desc: Test ResetSortedChildren, clear sortedChildren_ when it is not empty
 * @tc.type: FUNC
 * @tc.require: issueI6Q9A2
 */
HWTEST_F(RSMainThreadTest, ResetSortedChildren, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    NodeId nodeId = 0;
    std::weak_ptr<RSContext> context = {};
    auto node = std::make_shared<RSBaseRenderNode>(nodeId, context);
    auto childNode = std::make_shared<RSBaseRenderNode>(nodeId + 1, context);
    int index = -1;
    node->SetIsOnTheTree(true);
    node->AddChild(node, index);
    ASSERT_EQ(node->GetChildrenCount(), 0);

    node->AddChild(childNode, index);
    ASSERT_EQ(node->GetChildrenCount(), 1);
    ASSERT_TRUE(childNode->IsOnTheTree());

    node->GetSortedChildren();
    ASSERT_EQ(node->sortedChildren_.size(), 1);

    mainThread->ResetSortedChildren(node);
    ASSERT_EQ(node->sortedChildren_.size(), 0);
}

/**
 * @tc.name: AddActivePid
 * @tc.desc: Test AddActivePid, add pid, check if success
 * @tc.type: FUNC
 * @tc.require: issueI6Q9A2
 */
HWTEST_F(RSMainThreadTest, AddActivePid, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    pid_t pid = 1;
    mainThread->AddActivePid(pid);
    ASSERT_EQ(mainThread->activeProcessPids_.size(), 1);
    ASSERT_EQ(*(mainThread->activeProcessPids_.begin()), pid);
}
} // namespace OHOS::Rosen
