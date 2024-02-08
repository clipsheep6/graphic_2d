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

#include "rs_profiler.h"
#include "rs_profiler_base.h"
#include "rs_profiler_capturedata.h"
#include "rs_profiler_network.h"
#include "rs_profiler_telemetry.h"

#include "../../../../../utils/log/rs_trace.h"
#include "../../../render_service_base/include/pipeline/rs_context.h"

#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_render_service_connection.h"
#include "pipeline/rs_uni_render_util.h"

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

namespace OHOS::Rosen {

// TODO(user): Move to RSProfiler
static RSRenderService* RENDER_SERVICE = nullptr;
static RSMainThread* RENDER_SERVICE_THREAD = nullptr;
static uint64_t FRAME_BEGIN_TIMESTAMP = 0u;
static double DIRTY_REGION_PERCENTAGE = 0.0;

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
            std::shared_ptr<RSBaseRenderNode> const nodePtr = displayNode->shared_from_this();
            auto displayNodePtr = nodePtr->ReinterpretCastTo<RSDisplayRenderNode>();
            if (!displayNodePtr) {
                RS_LOGE("RSUniRenderVisitor::ProcessDisplayRenderNode ReinterpretCastTo fail");
                return -1.0;
            }

            RectI const displayResolution = displayNodePtr->GetDirtyManager()->GetSurfaceRect();
            double const displayWidth = displayResolution.GetWidth();
            double const displayHeight = displayResolution.GetHeight();
            {
                RS_TRACE_NAME(
                    "TWO_D: Display resolution: Width/Height= " + std::to_string(displayResolution.GetWidth()) + ", " +
                    std::to_string(displayResolution.GetHeight()));
            }
            unsigned int const bufferAge = 3;
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

            double const dirtyRegionPercentage = accumulatedArea / (displayWidth * displayHeight);
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
    RENDER_SERVICE = renderService;
    RENDER_SERVICE_THREAD = RSMainThread::Instance();

    static std::thread const THREAD(Network::ConsoleThreadRun);
}

RSRenderService* RSProfiler::GetRenderService()
{
    return RENDER_SERVICE;
}

RSMainThread* RSProfiler::GetRenderServiceThread()
{
    return RENDER_SERVICE_THREAD;
}

void RSProfiler::RenderServiceOnCreateConnection(pid_t remotePid)
{
    if (Network::debugWriteRSFile.IsOpen()) {
        if (Network::debugWriteStartTime == 0.0) {
            Network::debugWriteStartTime = Utils::Now();
        }

        Network::debugWriteRSFile.AddHeaderPID(remotePid);

        std::vector<pid_t> const& pidList = Network::debugWriteRSFile.GetHeaderPIDList();
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

            double const curTime = Utils::Now();
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

            Network::SendBinary((void*)ss.str().c_str(), ss.str().size());
            Network::debugWriteRSFile.WriteRSData(dt, (void*)(ss.str().c_str() + (1 + 8)), ss.str().size() - (1 + 8));
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
        volatile auto const intPtr =
            reinterpret_cast<intptr_t>(parcel); // gcc C++ optimization error, not working without volatile
        if ((intPtr & 1) != 0) {
            data->SetSendingPid(data->GetSendingPid() | (1 << 30));
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
    DIRTY_REGION_PERCENTAGE = GetDirtyRegionRelative(GetRenderServiceThread()->GetContext()) * 100.0;
}

void RSProfiler::MainThreadOnRenderEnd() {}

void RSProfiler::MainThreadOnFrameBegin()
{
    FRAME_BEGIN_TIMESTAMP = Utils::RawNowNano();
}

void RSProfiler::MainThreadOnFrameEnd()
{
    const uint64_t frameStartTime = FRAME_BEGIN_TIMESTAMP;
    const uint64_t framelenNanosecs = Utils::RawNowNano() - frameStartTime;
    const double dirtyPercent = DIRTY_REGION_PERCENTAGE;

    RSMainThread* rsmt = GetRenderServiceThread();
    RSRenderService* mainRs = GetRenderService();

    static bool rdcSent = true;

    // CHECK COMMAND EXIST
    {
        std::lock_guard<std::mutex> const guard(Network::consoleCmdMutex);
        if (!Network::consoleCmdData.empty()) {
            if (Network::consoleCmdData[0] == "rstree_contains" && Network::consoleCmdData.size() >= 2 &&
                !Network::consoleCmdData[1].empty()) {
                auto& nodeMap = rsmt->GetContext().GetMutableNodeMap();

                std::map<std::string, std::tuple<NodeId, std::string>> xlist;
                nodeMap.GetSurfacesTrees(xlist);

                std::stringstream ss;
                for (auto xitem : xlist) {
                    if (strstr(xitem.first.c_str(), Network::consoleCmdData[1].c_str())) {
                        ss << "*** " << xitem.first << " pid=" << (std::get<0>(xitem.second) >> 32)
                           << " low_id=" << (std::get<0>(xitem.second) & 0xFFffFFff) << std::endl;
                        ss << std::get<1>(xitem.second) << std::endl;
                    }
                }

                Network::consoleOutStr = "RSMainThread::MainLoop TREE_PIDS: count=" +
                                  std::to_string(static_cast<int>(nodeMap.GetRenderNodeCount())) +
                                  " time=" + std::to_string(Utils::Now()) + "\n";
                Network::consoleOutStr += ss.str();
            } else if (Network::consoleCmdData[0] == "rsrecord_pause_at" && Network::consoleCmdData.size() >= 2) {
                double const recordPauseTime = atof(Network::consoleCmdData[1].c_str());

                if (!Network::debugReadRSFile.IsOpen()) {
                    return;
                }
                if (Network::debugReadStartTime <= 0.0) {
                    return;
                }

                double const recordPlayTime = Utils::Now() - Network::debugReadStartTime;

                if (recordPlayTime > recordPauseTime) {
                    return;
                }

                RS_LOGD("RSMainThread::MainLoop Server REPLAY record_play_time=%lf record_pause_time=%lf",
                    recordPlayTime, recordPauseTime);

                uint64_t const curTimeRaw = Utils::RawNowNano();
                uint64_t const pauseTimeStart = curTimeRaw + (recordPauseTime - recordPlayTime) * 1'000'000'000;

                RSProfilerBase::TimePauseAt(curTimeRaw, pauseTimeStart);

                rsmt->ResetAnimationStamp();

                Network::consoleOutStr = "OK"; //: " + std::to_string(record_pause_time - record_play_time);
            } else if (Network::consoleCmdData[0] == "rsrecord_pause_now") {
                if (!Network::debugReadRSFile.IsOpen()) {
                    return;
                }
                if (Network::debugReadStartTime <= 0.0) {
                    return;
                }

                uint64_t const curTimeRaw = Utils::RawNowNano();
                double const recordPlayTime = Utils::Now() - Network::debugReadStartTime;

                RSProfilerBase::TimePauseAt(curTimeRaw, curTimeRaw);
                Network::consoleOutStr = "OK: " + std::to_string(recordPlayTime);
            } else if (Network::consoleCmdData[0] == "rsrecord_pause_resume") {
                if (!Network::debugReadRSFile.IsOpen()) {
                    return;
                }
                if (Network::debugReadStartTime <= 0.0) {
                    return;
                }

                uint64_t const curTimeRaw = std::chrono::duration_cast<std::chrono::nanoseconds>(
                    std::chrono::steady_clock::now().time_since_epoch())
                                                .count();

                RSProfilerBase::TimePauseResume(curTimeRaw);

                rsmt->ResetAnimationStamp();

                Network::consoleOutStr = "OK";
            } else if (Network::consoleCmdData[0] == "rsrecord_pause_clear") {
                RSProfilerBase::TimePauseClear();
                Network::consoleOutStr = "OK";
            } else if (Network::consoleCmdData[0] == "rstree_fix") {
                auto& nodeMap = rsmt->GetContext().GetMutableNodeMap();
                if (Network::consoleCmdData.size() >= 2) {
                    NodeId const nodeId = atoll(Network::consoleCmdData[1].c_str());

                    auto baseNode = nodeMap.GetRenderNode(nodeId);
                    if (baseNode != nullptr) {
                        auto* node = static_cast<RSRenderNode*>(baseNode.get());

                        RSProperties& prop = node->GetMutableRenderProperties();

                        prop.SetBounds(Vector4f(0, 0, 1344, 2635));
                        prop.SetFrame(Vector4f(0, 0, 1344, 2635));

                        auto* surfNode = static_cast<RSSurfaceRenderNode*>(baseNode.get());
                        /*
                                                auto defaultDisplay = DisplayManager::GetInstance().GetDefaultDisplay();
                                                surf_node->SetBounds(0, 0, defaultDisplay->GetWidth(),
                           defaultDisplay->GetHeight());
                        */
                        auto& regConst = const_cast<Occlusion::Region&>(surfNode->GetVisibleRegion());
                        Occlusion::Rect const rc = { 0, 0, 1344, 2635 };
                        regConst = *(new Occlusion::Region(rc));

                        std::shared_ptr<RectF> const p = std::make_shared<RectF>(0, 0, 1344, 2635);
                        prop.SetDrawRegion(p);

                        Network::consoleOutStr = "OK";
                    } else {
                        Network::consoleOutStr = "FAILED";
                    }

                    RSMainThread::Instance()->PostTask([]() {
                        RSMainThread::Instance()->SetAccessibilityConfigChanged();
                        RSMainThread::Instance()->RequestNextVSync();
                    });
                }
            } else if (Network::consoleCmdData[0] == "rstree_kill_node") {
                auto& nodeMap = rsmt->GetContext().GetMutableNodeMap();
                if (Network::consoleCmdData.size() >= 2) {
                    NodeId const nodeId = atoll(Network::consoleCmdData[1].c_str());

                    auto node = nodeMap.GetRenderNode(nodeId);

                    if (node != nullptr) {
                        node->RemoveFromTree(false);
                        Network::consoleOutStr = "OK";
                    } else {
                        Network::consoleOutStr = "FAILED";
                    }

                    RSMainThread::Instance()->PostTask([]() {
                        RSMainThread::Instance()->SetAccessibilityConfigChanged();
                        RSMainThread::Instance()->RequestNextVSync();
                    });
                }
            } else if (Network::consoleCmdData[0] == "rstree_setparent") {
                auto& nodeMap = rsmt->GetContext().GetMutableNodeMap();
                if (Network::consoleCmdData.size() >= 5) {
                    NodeId const pid1 = atoll(Network::consoleCmdData[1].c_str());
                    NodeId const node1 = atoll(Network::consoleCmdData[2].c_str());

                    NodeId const pid2 = atoll(Network::consoleCmdData[3].c_str());
                    NodeId const node2 = atoll(Network::consoleCmdData[4].c_str());

                    NodeId const n1 = (pid1 << 32) | node1;
                    NodeId const n2 = (pid2 << 32) | node2;

                    auto ptr1 = nodeMap.GetRenderNode(n1);
                    auto ptr2 = nodeMap.GetRenderNode(n2);
                    if (ptr1 != nullptr && ptr2 != nullptr) {
                        ptr1->AddChild(ptr2);
                        Network::consoleOutStr = "OK";
                    } else {
                        Network::consoleOutStr = "FAILED";
                    }

                    RSMainThread::Instance()->PostTask([]() {
                        RSMainThread::Instance()->SetAccessibilityConfigChanged();
                        RSMainThread::Instance()->RequestNextVSync();
                    });
                }
            } else if (Network::consoleCmdData[0] == "rstree_getroot") {
                Network::consoleOutStr = "";
                auto& nodeMap = rsmt->GetContext().GetMutableNodeMap();
                NodeId const nid = nodeMap.GetRandomSurfaceNode();
                auto node = nodeMap.GetRenderNode<RSRenderNode>(nid);
                while (node) {
                    switch (node->GetType()) {
                        case RSRenderNodeType::UNKNOW:
                            Network::consoleOutStr += "pid=" + std::to_string(node->GetId() >> 32) +
                                               " node_id=" + std::to_string(node->GetId() & 0xFFffFFff) + "|UNKNOWN;\n";
                            break;
                        case RSRenderNodeType::RS_NODE:
                            Network::consoleOutStr += "pid=" + std::to_string(node->GetId() >> 32) +
                                               " node_id=" + std::to_string(node->GetId() & 0xFFffFFff) + "|RS_NODE;\n";
                            break;
                        case RSRenderNodeType::DISPLAY_NODE:
                            Network::consoleOutStr += "pid=" + std::to_string(node->GetId() >> 32) +
                                               " node_id=" + std::to_string(node->GetId() & 0xFFffFFff) +
                                               "|DISPLAY_NODE;\n";
                            break;
                        case RSRenderNodeType::SURFACE_NODE:
                            Network::consoleOutStr += "pid=" + std::to_string(node->GetId() >> 32) +
                                               " node_id=" + std::to_string(node->GetId() & 0xFFffFFff) +
                                               "|SURFACE_NODE;\n";
                            break;
                        case RSRenderNodeType::PROXY_NODE:
                            Network::consoleOutStr += "pid=" + std::to_string(node->GetId() >> 32) +
                                               " node_id=" + std::to_string(node->GetId() & 0xFFffFFff) +
                                               "|PROXY_NODE;\n";
                            break;
                        case RSRenderNodeType::CANVAS_NODE:
                            Network::consoleOutStr += "pid=" + std::to_string(node->GetId() >> 32) +
                                               " node_id=" + std::to_string(node->GetId() & 0xFFffFFff) +
                                               "|CANVAS_NODE;\n";
                            break;
                        case RSRenderNodeType::EFFECT_NODE:
                            Network::consoleOutStr += "pid=" + std::to_string(node->GetId() >> 32) +
                                               " node_id=" + std::to_string(node->GetId() & 0xFFffFFff) +
                                               "|EFFECT_NODE;\n";
                            break;
                        case RSRenderNodeType::ROOT_NODE:
                            Network::consoleOutStr += "pid=" + std::to_string(node->GetId() >> 32) +
                                               " node_id=" + std::to_string(node->GetId() & 0xFFffFFff) +
                                               "|ROOT_NODE;\n";
                            break;
                        case RSRenderNodeType::CANVAS_DRAWING_NODE:
                            Network::consoleOutStr += "pid=" + std::to_string(node->GetId() >> 32) +
                                               " node_id=" + std::to_string(node->GetId() & 0xFFffFFff) +
                                               "|CANVAS_DRAWING_NODE;\n";
                            break;
                    }
                    auto parent = node->GetParent().lock();
                    if (parent == nullptr || parent->GetId() == 0) {
                        break;
                    }
                    node = parent;
                }
                Network::consoleOutStr += "ROOT_ID=" + std::to_string(node->GetId()); // DISPLAY_NODE;ohos.sceneboard
            } else if (Network::consoleCmdData[0] == "rstree_node_mod" && Network::consoleCmdData.size() >= 2 &&
                       !Network::consoleCmdData[1].empty()) {
                auto& nodeMap = rsmt->GetContext().GetMutableNodeMap();
                if (Network::consoleCmdData.size() >= 2) {
                    NodeId const nodeId = std::atoll(Network::consoleCmdData[1].c_str());

                    auto node = nodeMap.GetRenderNode(nodeId);

                    if (node != nullptr) {
                        //***
                        Network::consoleOutStr = "MOD_LIST=";
                        node->DumpModifiers(Network::consoleOutStr);
                    } else {
                        Network::consoleOutStr = "FAILED";
                    }

                    RSMainThread::Instance()->PostTask([]() {
                        RSMainThread::Instance()->SetAccessibilityConfigChanged();
                        RSMainThread::Instance()->RequestNextVSync();
                    });
                }
            } else if (Network::consoleCmdData[0] == "rstree_node_prop" && Network::consoleCmdData.size() >= 2 &&
                       !Network::consoleCmdData[1].empty()) {
                auto& nodeMap = rsmt->GetContext().GetMutableNodeMap();
                if (Network::consoleCmdData.size() >= 2) {
                    NodeId const nodeId = atoll(Network::consoleCmdData[1].c_str());

                    auto node = nodeMap.GetRenderNode(nodeId);

                    if (node != nullptr) {
                        //***
                        Network::consoleOutStr = "PROP_LIST=";
                        node->DumpRenderProperties(Network::consoleOutStr);
                    } else {
                        Network::consoleOutStr = "FAILED";
                    }

                    RSMainThread::Instance()->PostTask([]() {
                        RSMainThread::Instance()->SetAccessibilityConfigChanged();
                        RSMainThread::Instance()->RequestNextVSync();
                    });
                }
            } else if (Network::consoleCmdData[0] == "rstree_pid" && Network::consoleCmdData.size() >= 2 &&
                       !Network::consoleCmdData[1].empty()) {
                Network::consoleOutStr = "";

                auto& nodeMap = rsmt->GetContext().GetMutableNodeMap();

                int const checkPid = atoi(Network::consoleCmdData[1].c_str());

                std::map<NodeId, std::string> xlist;
                nodeMap.GetSurfacesTreesByPid(checkPid, xlist);

                std::stringstream ss;
                for (const auto& xitem : xlist) {
                    ss << "*** " << xitem.first << " pid=" << static_cast<int>(xitem.first >> 32)
                       << " low_id=" << static_cast<int>(xitem.first & 0xFFffFFff) << std::endl;
                    ss << xitem.second << std::endl;
                }

                Network::consoleOutStr += "RSMainThread::MainLoop TREE_PIDS: count=" +
                                   std::to_string(static_cast<int>(nodeMap.GetRenderNodeCount())) +
                                   " time=" + std::to_string(Utils::Now()) + "\n";
                Network::consoleOutStr += ss.str();
            } else if (Network::consoleCmdData[0] == "rssurface_pid" && Network::consoleCmdData.size() >= 2 &&
                       !Network::consoleCmdData[1].empty()) {
                Network::consoleOutStr = "";

                // auto& nodeMap = RSMT->GetContext().GetMutableNodeMap();

                int const checkPid = atoi(Network::consoleCmdData[1].c_str());

                NodeId const rootNid = ((static_cast<NodeId>(checkPid)) << 32) | 1;
                std::string val;
                mainRs->DumpSurfaceNode(val, rootNid);

                Network::consoleOutStr += val;
            } else if (Network::consoleCmdData[0] == "rstree_kill_pid" && Network::consoleCmdData.size() >= 2 &&
                       !Network::consoleCmdData[1].empty()) {
                int const pid = atoi(Network::consoleCmdData[1].c_str());
                if (pid > 0) {
                    auto& nodeMap = rsmt->GetContext().GetMutableNodeMap();
                    NodeId const rootNid = (static_cast<NodeId>(pid) << 32) | 1;
                    auto node = nodeMap.GetRenderNode(rootNid);
                    Network::debugReadParentNode = 0;
                    if (node != nullptr) {
                        auto p = node->GetParent().lock();
                        if (p != nullptr) {
                            Network::debugReadParentNode = p->GetId();
                        }
                    }
                    Network::consoleOutStr = "OK parent: parent_pid=" + std::to_string(Network::debugReadParentNode >> 32) +
                                      " parent_node=" + std::to_string(Network::debugReadParentNode & 0xFFffFFff);
                    nodeMap.FilterNodeByPid(pid);
                    // RSMT->RequestNextVSync();
                    RSMainThread::Instance()->PostTask([]() {
                        RSMainThread::Instance()->SetAccessibilityConfigChanged();
                        RSMainThread::Instance()->RequestNextVSync();
                    });
                } else {
                    Network::consoleOutStr = "ERROR: pid is zero";
                }
            } else if (Network::consoleCmdData[0] == "rstree_prepare_replay" && Network::consoleCmdData.size() >= 2 &&
                       !Network::consoleCmdData[1].empty()) {
                int const pid = atoi(Network::consoleCmdData[1].c_str());
                if (pid > 0) {
                    auto& nodeMap = rsmt->GetContext().GetMutableNodeMap();
                    Network::consoleOutStr = "OK";
                    nodeMap.FilterForReplay(pid);
                    RSMainThread::Instance()->PostTask([]() {
                        RSMainThread::Instance()->SetAccessibilityConfigChanged();
                        RSMainThread::Instance()->RequestNextVSync();
                    });
                } else {
                    Network::consoleOutStr = "ERROR: pid is zero";
                }
            } else if (Network::consoleCmdData[0] == "rscon_print") {
                Network::consoleOutStr.clear();
                mainRs->DumpConnections(Network::consoleOutStr);
            } else if (Network::consoleCmdData[0] == "save_rdc") {
                rdcSent = false;
                RSSystemProperties::SetSaveRDC(true);

                RSMainThread::Instance()->PostTask([]() {
                    RSMainThread::Instance()->SetAccessibilityConfigChanged();
                    RSMainThread::Instance()->RequestNextVSync();
                });

                std::stringstream ss;
                ss << "Recording current frame cmds (for .rdc) into : /data/default.drawing";
                Network::consoleOutStr = ss.str();
            }
            /*
            else if (Network::CONSOLECmdData[0] == "root_node_by_pid" && Network::CONSOLECmdData.size() >= 2 && Network::CONSOLECmdData[1].size() >
            0)
            {
                int pid = atoi(Network::CONSOLECmdData[1].c_str());
                auto& nodeMap = RSMT->GetContext().GetMutableNodeMap();
                Network::CONSOLEOutStr = std::to_string(nodeMap.PidGetInstanceRootNodeId(pid));
            }*/
            else {
                Network::consoleOutStr = "COMMAND NOT FOUND";
            }
            Network::consoleCmdData.clear();
        }
    }

    // bool isSKPSaved = RSSystemProperties::GetIsSKPSaved();
    bool const saveRDCneeded = RSSystemProperties::GetSaveRDC() && !rdcSent;

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

                if (filestr.substr(filestr.length() - 3) == "rdc") {
                    filenameVec.push_back(filestr);
                    rdcnum++;
                }
            }
            if (rdcnum == 3) {
                std::sort(filenameVec.begin(), filenameVec.end());

                int const size = 1 + filenameVec[1].length();
                char* buf = new char[size];
                buf[0] = static_cast<char>(PackageID::RS_REPLAY_RDC_BINARY);
                std::memcpy(&buf[1], filenameVec[1].c_str(), filenameVec[1].length());

                // sends .rdc path to the client
                Network::SendBinary(buf, size);
                Network::Log("RDC filename sent: ", filenameVec[1]);

                RSSystemProperties::SetSaveRDC(false);
                rdcSent = true;
            }
        }
    }

    if (mainRs != nullptr && Network::debugWriteRSFile.IsOpen() && Network::debugWriteStartTime > 0.0) {
        double const curTime = frameStartTime * 1e-9; // Utils::Now();
        double const timeSinceRecordStart = curTime - Network::debugWriteStartTime;
        if (timeSinceRecordStart > 0.0) {
            // RS_LOGD("RSMainThread::MainLoop Server REPLAY render_frame_t1=%lf since_start=%lf", cur_time,
            // time_since_record_start); RS_LOGD("RSMainThread::MainLoop Server REPLAY render_frame_t2=%lf",
            // (frame_start_time + framelen_nanosecs) * 1e-9);

            RSCaptureData cd;
            cd.SetTime(timeSinceRecordStart);
            cd.SetProperty(RSCaptureData::KEY_RS_FRAME_LEN, framelenNanosecs);
            cd.SetProperty(RSCaptureData::KEY_RS_CMD_COUNT, RSProfilerBase::ParsedCmdCountGet());
            cd.SetProperty(RSCaptureData::KEY_RS_PIXEL_IMAGE_ADDED, RSProfilerBase::ImagesAddedCountGet());
            cd.SetProperty(RSCaptureData::KEY_RS_DIRTY_REGION, floor(dirtyPercent * 10.0) / 10.0);

            std::vector<char> cdData;
            cd.Serialize(cdData);

            char const headerType = 2; // TYPE: RS METRICS
            cdData.insert(cdData.begin(), headerType);

            Network::SendBinary((void*)cdData.data(), cdData.size());
            Network::debugWriteRSFile.WriteRSMetrics(0, timeSinceRecordStart, (void*)cdData.data(), cdData.size());
        }
    }

    // Test parsing of /proc/net/dev
    // Need to check which interfaces are used and which we are mostly interested inz
    // Also need to get the difference from frame to frame
    std::string const iface = std::string("wlan0");
    auto statsVec = Network::GetNetworkStats(iface);
    if (!statsVec.empty()) {
        for (auto& bw : statsVec) {
            RS_LOGD("TWO_D: Iface: %s; Transmitted: %f; Received: %f", iface.c_str(), (float)bw.transmittedBytes * 8,
                (float)bw.receivedBytes * 8);
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