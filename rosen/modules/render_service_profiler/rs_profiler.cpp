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

#include <cstdio>
#include <cstdlib>
#include <dlfcn.h>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

#include "../../../../../utils/log/rs_trace.h"
#include "../../../render_service_base/include/pipeline/rs_context.h"
#include "rs_profiler.h"
#include "rs_profiler_base.h"
#include "rs_profiler_capturedata.h"
#include "rs_profiler_network.h"
#include "rs_profiler_telemetry.h"

#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_render_service_connection.h"
#include "pipeline/rs_uni_render_util.h"

namespace OHOS::Rosen {

// (user): Move to RSProfiler
static RSRenderService* g_renderService = nullptr;
static RSMainThread* g_renderServiceThread = nullptr;
static uint64_t g_frameBeginTimestamp = 0u;
static double g_dirtyRegionPercentage = 0.0;

/*
    To visualize the damage region (as it's set for KHR_partial_update), you can set the following variable:
    'hdc shell param set rosen.dirtyregiondebug.enabled 6'
*/
static double GetDirtyRegionRelative(RSContext& context)
{
    const std::shared_ptr<RSBaseRenderNode>& rootNode = context.GetGlobalRootRenderNode();
    // otherwise without these checks device might get stuck on startup
    if (rootNode && (rootNode->GetChildrenCount() == 1)) {
        auto displayNode =
            RSBaseRenderNode::ReinterpretCast<RSDisplayRenderNode>(rootNode->GetSortedChildren().front());
        if (displayNode) {
            const std::shared_ptr<RSBaseRenderNode> nodePtr = displayNode->shared_from_this();
            auto displayNodePtr = nodePtr->ReinterpretCastTo<RSDisplayRenderNode>();
            if (!displayNodePtr) {
                RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode ReinterpretCastTo fail");
                return -1.0;
            }

            const RectI displayResolution = displayNodePtr->GetDirtyManager()->GetSurfaceRect();
            const double displayWidth = displayResolution.GetWidth();
            const double displayHeight = displayResolution.GetHeight();
            {
                RS_TRACE_NAME(
                    "TWO_D: Display resolution: Width/Height= " + std::to_string(displayResolution.GetWidth()) + ", " +
                    std::to_string(displayResolution.GetHeight()));
            }
            const unsigned int bufferAge = 3;
            const bool isAlignedDirtyRegion =
                false; // not to update the RSRenderFrame/DirtyManager and just calculate dirty region
            RSUniRenderUtil::MergeDirtyHistory(displayNodePtr, bufferAge, isAlignedDirtyRegion);
            std::vector<NodeId> hasVisibleDirtyRegionSurfaceVec;
            Occlusion::Region const dirtyRegion = RSUniRenderUtil::MergeVisibleDirtyRegion(
                displayNodePtr, hasVisibleDirtyRegionSurfaceVec, isAlignedDirtyRegion);

            const auto& visibleDirtyRects = dirtyRegion.GetRegionRects();
            double accumulatedArea = 0.0;
            for (auto& rect : visibleDirtyRects) {
                auto width = rect.right_ - rect.left_;
                auto height = rect.bottom_ - rect.top_;
                accumulatedArea += width * height;
                RS_TRACE_NAME("TWO_D: DirtyRegion: Left/Bott= " + std::to_string(rect.left_) + ", " +
                              std::to_string(rect.bottom_) + "; Right/Top= " + std::to_string(rect.right_) + ", " +
                              std::to_string(rect.top_) + ", Width/Height= " + std::to_string(width) + ", " +
                              std::to_string(height));
            }

            const double dirtyRegionPercentage = accumulatedArea / (displayWidth * displayHeight);
            {
                RS_TRACE_NAME("TWO_D: Dirty Region Area(%)= " + std::to_string(dirtyRegionPercentage));
            }
            return dirtyRegionPercentage;
        }
    }
    return -1.0;
}

void RSProfiler::Init(RSRenderService* renderService)
{
    g_renderService = renderService;
    g_renderServiceThread = RSMainThread::Instance();

    static std::thread const THREAD(Network::Run);
}

RSRenderService* RSProfiler::GetRenderService()
{
    return g_renderService;
}

RSMainThread* RSProfiler::GetRenderServiceThread()
{
    return g_renderServiceThread;
}

void RSProfiler::RenderServiceOnCreateConnection(pid_t remotePid)
{
    if (Network::debugWriteRSFile.IsOpen()) {
        if (Network::debugWriteStartTime == 0.0) {
            Network::debugWriteStartTime = Utils::Now();
        }

        Network::debugWriteRSFile.AddHeaderPID(remotePid);

        const std::vector<pid_t>& pidList = Network::debugWriteRSFile.GetHeaderPIDList();
        std::stringstream ss;
        for (auto itemPid : pidList) {
            ss << itemPid << " ";
        }
    }
}

void RSProfiler::RenderServiceConnectionOnRemoteRequest(RSRenderServiceConnection* connection, uint32_t code,
    MessageParcel& data, MessageParcel& /*reply*/, MessageOption& option)
{
    if (Network::debugWriteRSFile.IsOpen() && (Network::debugWriteStartTime > 0.0)) {
        pid_t connectionPid = GetRenderService()->GetConnectionPID(connection);
        const auto& pidList = Network::debugWriteRSFile.GetHeaderPIDList();
        if (std::find(std::begin(pidList), std::end(pidList), connectionPid) != std::end(pidList)) {
            int writeInt = 0;

            const double curTime = Utils::Now();
            double dt = curTime - Network::debugWriteStartTime;

            std::stringstream ss(std::ios::in | std::ios::out | std::ios::binary);

            char headerType = 1; // parcel data
            ss.write(reinterpret_cast<const char*>(&headerType), sizeof(headerType));

            ss.write(reinterpret_cast<const char*>(&dt), sizeof(dt));

            // set sending pid
            ss.write(reinterpret_cast<const char*>(&connectionPid), sizeof(connectionPid));

            ss.write(reinterpret_cast<const char*>(&code), sizeof(code));

            writeInt = data.GetDataSize();
            ss.write(reinterpret_cast<const char*>(&writeInt), sizeof(writeInt));
            ss.write(reinterpret_cast<const char*>(data.GetData()), data.GetDataSize());

            writeInt = option.GetFlags();
            ss.write(reinterpret_cast<const char*>(&writeInt), sizeof(writeInt));
            writeInt = option.GetWaitTime();
            ss.write(reinterpret_cast<const char*>(&writeInt), sizeof(writeInt));

            Network::SendBinary(reinterpret_cast<void*>(ss.str().data()), ss.str().size());
            constexpr int headerSize = 8;
            Network::debugWriteRSFile.WriteRSData(
                dt, reinterpret_cast<void*>(ss.str().data() + (1 + headerSize)), ss.str().size() - (1 + headerSize));
        }
    }

    if (Network::debugReadRSFile.IsOpen()) {
        RSProfilerBase::SpecParseModeSet(SpecParseMode::READ);
        RSProfilerBase::SpecParseReplacePIDSet(
            Network::debugReadRSFile.GetHeaderPIDList(), Network::debugReadRSFilePID, Network::debugReadParentNode);
    } else if (Network::debugWriteRSFile.IsOpen()) {
        RSProfilerBase::SpecParseModeSet(SpecParseMode::WRITE);
    } else {
        RSProfilerBase::SpecParseModeSet(SpecParseMode::NONE);
    }
}

void RSProfiler::UnmarshalThreadOnRecvParcel(const MessageParcel* parcel, RSTransactionData* data)
{
    if (Network::debugReadRSFile.IsOpen()) {
        const volatile auto intPtr =
            reinterpret_cast<intptr_t>(parcel); // gcc C++ optimization error, not working without volatile
        if ((intPtr & 1) != 0) {
            constexpr int bits30 = 30;
            data->SetSendingPid(data->GetSendingPid() | (1 << bits30));
        }
    }
}

uint64_t RSProfiler::TimePauseApply(uint64_t time)
{
    return RSProfilerBase::TimePauseApply(time);
}

void RSProfiler::MainThreadOnProcessCommand()
{
    if (Network::debugReadRSFile.IsOpen()) {
        RSMainThread::Instance()->ResetAnimationStamp();
    }
}

void RSProfiler::MainThreadOnRenderBegin()
{
    constexpr double maxPercent = 100.0;
    g_dirtyRegionPercentage = GetDirtyRegionRelative(GetRenderServiceThread()->GetContext()) * maxPercent;
}

void RSProfiler::MainThreadOnRenderEnd() {}

void RSProfiler::MainThreadOnFrameBegin()
{
    g_frameBeginTimestamp = Utils::RawNowNano();
}

void RSProfiler::MainThreadOnFrameEnd()
{
    const uint64_t frameStartTime = g_frameBeginTimestamp;
    const uint64_t framelenNanosecs = Utils::RawNowNano() - frameStartTime;
    const double dirtyPercent = g_dirtyRegionPercentage;

    RSMainThread* rsmt = GetRenderServiceThread();
    RSRenderService* mainRs = GetRenderService();

    static bool rdcSent = true;

    // CHECK COMMAND EXIST
    constexpr int minCmdLen = 2;
    constexpr uint32_t bits32 = 32;
    constexpr uint32_t mask32Bits = 0xFFffFFff;
    {
        const std::lock_guard<std::mutex> guard(Network::commandMutex_);
        if (!Network::commandData_.empty()) {
            if (Network::commandData_[0] == "rstree_contains" && Network::commandData_.size() >= minCmdLen &&
                !Network::commandData_[1].empty()) {
                auto& nodeMap = rsmt->GetContext().GetMutableNodeMap();

                std::map<std::string, std::tuple<NodeId, std::string>> xlist;
                nodeMap.GetSurfacesTrees(xlist);

                std::stringstream ss;
                for (auto xitem : xlist) {
                    if (strstr(xitem.first.c_str(), Network::commandData_[1].c_str())) {
                        ss << "*** " << xitem.first << " pid=" << (std::get<0>(xitem.second) >> bits32)
                           << " low_id=" << (std::get<0>(xitem.second) & mask32Bits) << std::endl;
                        ss << std::get<1>(xitem.second) << std::endl;
                    }
                }

                Network::commandOutput_ = "RSMainThread::MainLoop TREE_PIDS: count=" +
                                         std::to_string(static_cast<int>(nodeMap.GetRenderNodeCount())) +
                                         " time=" + std::to_string(Utils::Now()) + "\n";
                Network::commandOutput_ += ss.str();
            } else if (Network::commandData_[0] == "rsrecord_pause_at" && Network::commandData_.size() >= minCmdLen) {
                const double recordPauseTime = atof(Network::commandData_[1].c_str());

                if (!Network::debugReadRSFile.IsOpen()) {
                    return;
                }
                if (Network::debugReadStartTime <= 0.0) {
                    return;
                }

                const double recordPlayTime = Utils::Now() - Network::debugReadStartTime;

                if (recordPlayTime > recordPauseTime) {
                    return;
                }

                RS_LOGD("RSMainThread::MainLoop Server REPLAY record_play_time=%lf record_pause_time=%lf",
                    recordPlayTime, recordPauseTime);

                const uint64_t curTimeRaw = Utils::RawNowNano();
                const uint64_t pauseTimeStart = curTimeRaw + (recordPauseTime - recordPlayTime) * 1'000'000'000;

                RSProfilerBase::TimePauseAt(curTimeRaw, pauseTimeStart);

                rsmt->ResetAnimationStamp();

                Network::commandOutput_ = "OK"; //: " + std::to_string(record_pause_time - record_play_time);
            } else if (Network::commandData_[0] == "rsrecord_pause_now") {
                if (!Network::debugReadRSFile.IsOpen()) {
                    return;
                }
                if (Network::debugReadStartTime <= 0.0) {
                    return;
                }

                const uint64_t curTimeRaw = Utils::RawNowNano();
                const double recordPlayTime = Utils::Now() - Network::debugReadStartTime;

                RSProfilerBase::TimePauseAt(curTimeRaw, curTimeRaw);
                Network::commandOutput_ = "OK: " + std::to_string(recordPlayTime);
            } else if (Network::commandData_[0] == "rsrecord_pause_resume") {
                if (!Network::debugReadRSFile.IsOpen()) {
                    return;
                }
                if (Network::debugReadStartTime <= 0.0) {
                    return;
                }

                const uint64_t curTimeRaw = std::chrono::duration_cast<std::chrono::nanoseconds>(
                    std::chrono::steady_clock::now().time_since_epoch())
                                                .count();

                RSProfilerBase::TimePauseResume(curTimeRaw);

                rsmt->ResetAnimationStamp();

                Network::commandOutput_ = "OK";
            } else if (Network::commandData_[0] == "rsrecord_pause_clear") {
                RSProfilerBase::TimePauseClear();
                Network::commandOutput_ = "OK";
            } else if (Network::commandData_[0] == "rstree_fix") {
                auto& nodeMap = rsmt->GetContext().GetMutableNodeMap();
                if (Network::commandData_.size() >= minCmdLen) {
                    const NodeId nodeId = atoll(Network::commandData_[1].c_str());

                    auto baseNode = nodeMap.GetRenderNode(nodeId);
                    if (baseNode != nullptr) {
                        auto* node = static_cast<RSRenderNode*>(baseNode.get());

                        RSProperties& prop = node->GetMutableRenderProperties();

                        constexpr int screenH = 2635;
                        constexpr int screenW = 1344;
                        prop.SetBounds(Vector4f(0, 0, screenW, screenH));
                        prop.SetFrame(Vector4f(0, 0, screenW, screenH));

                        auto* surfNode = static_cast<RSSurfaceRenderNode*>(baseNode.get());
                        auto& regConst = const_cast<Occlusion::Region&>(surfNode->GetVisibleRegion());
                        Occlusion::Rect const rc = { 0, 0, screenW, screenH };
                        regConst = *(new Occlusion::Region(rc));

                        const std::shared_ptr<RectF> p = std::make_shared<RectF>(0, 0, screenW, screenH);
                        prop.SetDrawRegion(p);

                        Network::commandOutput_ = "OK";
                    } else {
                        Network::commandOutput_ = "FAILED";
                    }

                    RSMainThread::Instance()->PostTask([]() {
                        RSMainThread::Instance()->SetAccessibilityConfigChanged();
                        RSMainThread::Instance()->RequestNextVSync();
                    });
                }
            } else if (Network::commandData_[0] == "rstree_kill_node") {
                auto& nodeMap = rsmt->GetContext().GetMutableNodeMap();
                if (Network::commandData_.size() >= minCmdLen) {
                    const NodeId nodeId = atoll(Network::commandData_[1].c_str());

                    auto node = nodeMap.GetRenderNode(nodeId);

                    if (node != nullptr) {
                        node->RemoveFromTree(false);
                        Network::commandOutput_ = "OK";
                    } else {
                        Network::commandOutput_ = "FAILED";
                    }

                    RSMainThread::Instance()->PostTask([]() {
                        RSMainThread::Instance()->SetAccessibilityConfigChanged();
                        RSMainThread::Instance()->RequestNextVSync();
                    });
                }
            } else if (Network::commandData_[0] == "rstree_setparent") {
                auto& nodeMap = rsmt->GetContext().GetMutableNodeMap();
                constexpr int rstreeSetparentCmdMinLen = 5;
                if (Network::commandData_.size() >= rstreeSetparentCmdMinLen) {
                    const NodeId pid1 = atoll(Network::commandData_[1].c_str());
                    const NodeId node1 = atoll(Network::commandData_[2].c_str());

                    const NodeId pid2 = atoll(Network::commandData_[3].c_str());
                    const NodeId node2 = atoll(Network::commandData_[4].c_str());

                    const NodeId n1 = (pid1 << bits32) | node1;
                    const NodeId n2 = (pid2 << bits32) | node2;

                    auto ptr1 = nodeMap.GetRenderNode(n1);
                    auto ptr2 = nodeMap.GetRenderNode(n2);
                    if (ptr1 != nullptr && ptr2 != nullptr) {
                        ptr1->AddChild(ptr2);
                        Network::commandOutput_ = "OK";
                    } else {
                        Network::commandOutput_ = "FAILED";
                    }

                    RSMainThread::Instance()->PostTask([]() {
                        RSMainThread::Instance()->SetAccessibilityConfigChanged();
                        RSMainThread::Instance()->RequestNextVSync();
                    });
                }
            } else if (Network::commandData_[0] == "rstree_getroot") {
                Network::commandOutput_ = "";
                auto& nodeMap = rsmt->GetContext().GetMutableNodeMap();
                const NodeId nid = nodeMap.GetRandomSurfaceNode();
                auto node = nodeMap.GetRenderNode<RSRenderNode>(nid);
                while (node) {
                    switch (node->GetType()) {
                        case RSRenderNodeType::UNKNOW:
                            Network::commandOutput_ += "pid=" + std::to_string(node->GetId() >> bits32) +
                                                      " node_id=" + std::to_string(node->GetId() & mask32Bits) +
                                                      "|UNKNOWN;\n";
                            break;
                        case RSRenderNodeType::RS_NODE:
                            Network::commandOutput_ += "pid=" + std::to_string(node->GetId() >> bits32) +
                                                      " node_id=" + std::to_string(node->GetId() & mask32Bits) +
                                                      "|RS_NODE;\n";
                            break;
                        case RSRenderNodeType::DISPLAY_NODE:
                            Network::commandOutput_ += "pid=" + std::to_string(node->GetId() >> bits32) +
                                                      " node_id=" + std::to_string(node->GetId() & mask32Bits) +
                                                      "|DISPLAY_NODE;\n";
                            break;
                        case RSRenderNodeType::SURFACE_NODE:
                            Network::commandOutput_ += "pid=" + std::to_string(node->GetId() >> bits32) +
                                                      " node_id=" + std::to_string(node->GetId() & mask32Bits) +
                                                      "|SURFACE_NODE;\n";
                            break;
                        case RSRenderNodeType::PROXY_NODE:
                            Network::commandOutput_ += "pid=" + std::to_string(node->GetId() >> bits32) +
                                                      " node_id=" + std::to_string(node->GetId() & mask32Bits) +
                                                      "|PROXY_NODE;\n";
                            break;
                        case RSRenderNodeType::CANVAS_NODE:
                            Network::commandOutput_ += "pid=" + std::to_string(node->GetId() >> bits32) +
                                                      " node_id=" + std::to_string(node->GetId() & mask32Bits) +
                                                      "|CANVAS_NODE;\n";
                            break;
                        case RSRenderNodeType::EFFECT_NODE:
                            Network::commandOutput_ += "pid=" + std::to_string(node->GetId() >> bits32) +
                                                      " node_id=" + std::to_string(node->GetId() & mask32Bits) +
                                                      "|EFFECT_NODE;\n";
                            break;
                        case RSRenderNodeType::ROOT_NODE:
                            Network::commandOutput_ += "pid=" + std::to_string(node->GetId() >> bits32) +
                                                      " node_id=" + std::to_string(node->GetId() & mask32Bits) +
                                                      "|ROOT_NODE;\n";
                            break;
                        case RSRenderNodeType::CANVAS_DRAWING_NODE:
                            Network::commandOutput_ += "pid=" + std::to_string(node->GetId() >> bits32) +
                                                      " node_id=" + std::to_string(node->GetId() & mask32Bits) +
                                                      "|CANVAS_DRAWING_NODE;\n";
                            break;
                    }
                    auto parent = node->GetParent().lock();
                    if (parent == nullptr || parent->GetId() == 0) {
                        break;
                    }
                    node = parent;
                }
                Network::commandOutput_ += "ROOT_ID=" + std::to_string(node->GetId()); // DISPLAY_NODE;ohos.sceneboard
            } else if (Network::commandData_[0] == "rstree_node_mod" && Network::commandData_.size() >= minCmdLen &&
                       !Network::commandData_[1].empty()) {
                auto& nodeMap = rsmt->GetContext().GetMutableNodeMap();
                if (Network::commandData_.size() >= minCmdLen) {
                    const NodeId nodeId = std::atoll(Network::commandData_[1].c_str());

                    auto node = nodeMap.GetRenderNode(nodeId);

                    if (node != nullptr) {
                        Network::commandOutput_ = "MOD_LIST=";
                        node->DumpModifiers(Network::commandOutput_);
                    } else {
                        Network::commandOutput_ = "FAILED";
                    }

                    RSMainThread::Instance()->PostTask([]() {
                        RSMainThread::Instance()->SetAccessibilityConfigChanged();
                        RSMainThread::Instance()->RequestNextVSync();
                    });
                }
            } else if (Network::commandData_[0] == "rstree_node_prop" && Network::commandData_.size() >= minCmdLen &&
                       !Network::commandData_[1].empty()) {
                auto& nodeMap = rsmt->GetContext().GetMutableNodeMap();
                if (Network::commandData_.size() >= minCmdLen) {
                    const NodeId nodeId = atoll(Network::commandData_[1].c_str());

                    auto node = nodeMap.GetRenderNode(nodeId);

                    if (node != nullptr) {
                        Network::commandOutput_ = "PROP_LIST=";
                        node->DumpRenderProperties(Network::commandOutput_);
                    } else {
                        Network::commandOutput_ = "FAILED";
                    }

                    RSMainThread::Instance()->PostTask([]() {
                        RSMainThread::Instance()->SetAccessibilityConfigChanged();
                        RSMainThread::Instance()->RequestNextVSync();
                    });
                }
            } else if (Network::commandData_[0] == "rstree_pid" && Network::commandData_.size() >= minCmdLen &&
                       !Network::commandData_[1].empty()) {
                Network::commandOutput_ = "";

                auto& nodeMap = rsmt->GetContext().GetMutableNodeMap();

                const int checkPid = atoi(Network::commandData_[1].c_str());

                std::map<NodeId, std::string> xlist;
                nodeMap.GetSurfacesTreesByPid(checkPid, xlist);

                std::stringstream ss;
                for (const auto& xitem : xlist) {
                    ss << "*** " << xitem.first << " pid=" << static_cast<int>(xitem.first >> bits32)
                       << " low_id=" << static_cast<int>(xitem.first & mask32Bits) << std::endl;
                    ss << xitem.second << std::endl;
                }

                Network::commandOutput_ += "RSMainThread::MainLoop TREE_PIDS: count=" +
                                          std::to_string(static_cast<int>(nodeMap.GetRenderNodeCount())) +
                                          " time=" + std::to_string(Utils::Now()) + "\n";
                Network::commandOutput_ += ss.str();
            } else if (Network::commandData_[0] == "rssurface_pid" && Network::commandData_.size() >= minCmdLen &&
                       !Network::commandData_[1].empty()) {
                Network::commandOutput_ = "";

                const int checkPid = atoi(Network::commandData_[1].c_str());

                const NodeId rootNid = ((static_cast<NodeId>(checkPid)) << bits32) | 1;
                std::string val;
                mainRs->DumpSurfaceNode(val, rootNid);

                Network::commandOutput_ += val;
            } else if (Network::commandData_[0] == "rstree_kill_pid" && Network::commandData_.size() >= minCmdLen &&
                       !Network::commandData_[1].empty()) {
                const int pid = atoi(Network::commandData_[1].c_str());
                if (pid > 0) {
                    auto& nodeMap = rsmt->GetContext().GetMutableNodeMap();
                    const NodeId rootNid = (static_cast<NodeId>(pid) << bits32) | 1;
                    auto node = nodeMap.GetRenderNode(rootNid);
                    Network::debugReadParentNode = 0;
                    if (node != nullptr) {
                        auto p = node->GetParent().lock();
                        if (p != nullptr) {
                            Network::debugReadParentNode = p->GetId();
                        }
                    }
                    Network::commandOutput_ =
                        "OK parent: parent_pid=" + std::to_string(Network::debugReadParentNode >> 32) +
                        " parent_node=" + std::to_string(Network::debugReadParentNode & mask32Bits);
                    nodeMap.FilterNodeByPid(pid);
                    RSMainThread::Instance()->PostTask([]() {
                        RSMainThread::Instance()->SetAccessibilityConfigChanged();
                        RSMainThread::Instance()->RequestNextVSync();
                    });
                } else {
                    Network::commandOutput_ = "ERROR: pid is zero";
                }
            } else if (Network::commandData_[0] == "rstree_prepare_replay" && Network::commandData_.size() >= minCmdLen &&
                       !Network::commandData_[1].empty()) {
                const int pid = atoi(Network::commandData_[1].c_str());
                if (pid > 0) {
                    auto& nodeMap = rsmt->GetContext().GetMutableNodeMap();
                    Network::commandOutput_ = "OK";
                    nodeMap.FilterForReplay(pid);
                    RSMainThread::Instance()->PostTask([]() {
                        RSMainThread::Instance()->SetAccessibilityConfigChanged();
                        RSMainThread::Instance()->RequestNextVSync();
                    });
                } else {
                    Network::commandOutput_ = "ERROR: pid is zero";
                }
            } else if (Network::commandData_[0] == "rscon_print") {
                Network::commandOutput_.clear();
                mainRs->DumpConnections(Network::commandOutput_);
            } else if (Network::commandData_[0] == "save_rdc") {
                rdcSent = false;
                RSSystemProperties::SetSaveRDC(true);

                RSMainThread::Instance()->PostTask([]() {
                    RSMainThread::Instance()->SetAccessibilityConfigChanged();
                    RSMainThread::Instance()->RequestNextVSync();
                });

                std::stringstream ss;
                ss << "Recording current frame cmds (for .rdc) into : /data/default.drawing";
                Network::commandOutput_ = ss.str();
            }
            else {
                Network::commandOutput_ = "COMMAND NOT FOUND";
            }
            Network::commandData_.clear();
        }
    }

    const bool saveRDCneeded = RSSystemProperties::GetSaveRDC() && !rdcSent;
    if (saveRDCneeded) {
        RSMainThread::Instance()->PostTask([]() {
            RSMainThread::Instance()->SetAccessibilityConfigChanged();
            RSMainThread::Instance()->RequestNextVSync();
        });
        std::string const path = "/data/storage/el2/base/temp/HuaweiReplay";
        int rdcnum = 0;
        std::vector<std::string> filenameVec;
        // Checks is the folder exists
        if (std::filesystem::exists(path)) {
            for (const auto& entry : std::filesystem::directory_iterator(path)) {
                std::string const filestr = entry.path();
                RS_LOGD("TWO_D_RDC : %s", filestr.c_str());

                constexpr int rdcExtLen = 3;
                if (filestr.substr(filestr.length() - rdcExtLen) == "rdc") {
                    filenameVec.push_back(filestr);
                    rdcnum++;
                }
            }
            constexpr int maxCaptureNumber = 3;
            if (rdcnum == maxCaptureNumber) {
                std::sort(filenameVec.begin(), filenameVec.end());

                const int size = 1 + filenameVec[1].length();
                char* buf = new char[size];
                buf[0] = static_cast<char>(PackageID::RS_PROFILER_RDC_BINARY);
                std::memcpy(&buf[1], filenameVec[1].c_str(), filenameVec[1].length());

                // sends .rdc path to the client
                Network::SendBinary(buf, size);

                RSSystemProperties::SetSaveRDC(false);
                rdcSent = true;
            }
        }
    }

    if (mainRs != nullptr && Network::debugWriteRSFile.IsOpen() && Network::debugWriteStartTime > 0.0) {
        const double curTime = frameStartTime * 1e-9; // Utils::Now();
        const double timeSinceRecordStart = curTime - Network::debugWriteStartTime;
        if (timeSinceRecordStart > 0.0) {
            RSCaptureData cd;
            cd.SetTime(timeSinceRecordStart);
            cd.SetProperty(RSCaptureData::KEY_RS_FRAME_LEN, framelenNanosecs);
            cd.SetProperty(RSCaptureData::KEY_RS_CMD_COUNT, RSProfilerBase::ParsedCmdCountGet());
            cd.SetProperty(RSCaptureData::KEY_RS_PIXEL_IMAGE_ADDED, RSProfilerBase::ImagesAddedCountGet());
            constexpr double d10 = 10.0;
            cd.SetProperty(RSCaptureData::KEY_RS_DIRTY_REGION, floor(dirtyPercent * d10) / d10);

            std::vector<char> cdData;
            cd.Serialize(cdData);

            const char headerType = 2; // TYPE: RS METRICS
            cdData.insert(cdData.begin(), headerType);

            Network::SendBinary((void*)cdData.data(), cdData.size());
            Network::debugWriteRSFile.WriteRSMetrics(0, timeSinceRecordStart, (void*)cdData.data(), cdData.size());
        }
    }

    // Test parsing of /proc/net/dev
    // Need to check which interfaces are used and which we are mostly interested inz
    // Also need to get the difference from frame to frame
    std::string const iface = std::string("wlan0");
    auto statsVec = Network::GetStats(iface);
    constexpr int bitsInOneByte = 8;
    if (!statsVec.empty()) {
        for (auto& bw : statsVec) {
            RS_LOGD("TWO_D: Iface: %s; Transmitted: %f; Received: %f", iface.c_str(), (float)bw.transmittedBytes * bitsInOneByte,
                (float)bw.receivedBytes * bitsInOneByte);
        }
    }
}

bool RSProfiler::IsRecording()
{
    return Network::debugWriteRSFile.IsOpen();
}

bool RSProfiler::IsPlaying()
{
    return Network::debugReadRSFile.IsOpen();
}

} // namespace OHOS::Rosen