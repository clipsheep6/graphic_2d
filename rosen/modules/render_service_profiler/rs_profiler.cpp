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

#include <filesystem>
#include <numeric>

#include "foundation/graphic/graphic_2d/utils/log/rs_trace.h"
#include "rs_profiler.h"
#include "rs_profiler_archive.h"
#include "rs_profiler_cache.h"
#include "rs_profiler_capture_recorder.h"
#include "rs_profiler_capturedata.h"
#include "rs_profiler_file.h"
#include "rs_profiler_json.h"
#include "rs_profiler_network.h"
#include "rs_profiler_settings.h"
#include "rs_profiler_telemetry.h"
#include "rs_profiler_utils.h"
#include "rs_profiler_packet.h"

#include "params/rs_display_render_params.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_render_service_connection.h"
#include "pipeline/rs_uni_render_util.h"

namespace OHOS::Rosen {

// (user): Move to RSProfiler
static RSRenderService* g_renderService = nullptr;
static std::atomic<int32_t> g_renderServiceCpuId = 0;
static std::atomic<int32_t> g_renderServiceRenderCpuId = 0;
static RSMainThread* g_mainThread = nullptr;
static RSContext* g_context = nullptr;
static uint64_t g_frameBeginTimestamp = 0u;
static uint64_t g_frameRenderBeginTimestamp = 0u;
static double g_dirtyRegionPercentage = 0.0;
static bool g_rdcSent = true;

static std::atomic<uint32_t> g_lastCacheImageCount = 0;

static RSFile g_recordFile {};
static double g_recordStartTime = 0.0;
static uint32_t g_frameNumber = 0;

static RSFile g_playbackFile {};
static double g_playbackStartTime = 0.0;
static NodeId g_playbackParentNodeId = 0;
static int g_playbackPid = 0;
static bool g_playbackShouldBeTerminated = true;
static double g_playbackPauseTime = 0;
static int g_playbackWaitFrames = 0;

static std::vector<std::pair<NodeId, uint32_t>> g_nodeListPerf;
static std::unordered_map<NodeId, uint32_t> g_mapNode2Count;
static std::unordered_map<NodeId, uint32_t> g_mapNode2UpTime;
static std::unordered_map<NodeId, uint32_t> g_mapNode2UpDownTime;
static NodeId g_calcPerfNode = 0;
static uint32_t g_calcPerfNodeTry = 0;
static bool g_calcPerfNodeExcludeDown = false;

static std::vector<std::pair<std::shared_ptr<RSRenderNode>,
            std::vector<std::shared_ptr<RSRenderNode>>>> g_calcPerfSavedChildren;
static NodeId g_blinkNodeId = 0;
static uint32_t g_blinkNodeCount = 0;
static std::vector<std::shared_ptr<RSRenderNode>> g_blinkSavedParentChildren;

constexpr int CALC_PERF_NODE_TIME_COUNT_MIN = 128; // min render tries
constexpr int CALC_PERF_NODE_TIME_COUNT_MAX = 4096; // max render tries
static uint32_t g_effectiveNodeTimeCount = CALC_PERF_NODE_TIME_COUNT_MAX;
static uint64_t g_calcPerfNodeTime[CALC_PERF_NODE_TIME_COUNT_MAX];
static int g_nodeListPerfCalcIndex = -1;

static std::string g_testDataFrame;
static std::vector<RSRenderNode::SharedPtr> g_childOfDisplayNodes;

#pragma pack(push, 1)
struct AlignedMessageParcel {
    uint8_t pad = 0u;
    MessageParcel parcel;
};
#pragma pack(pop)

void DeviceInfoToCaptureData(double time, const DeviceInfo& in, RSCaptureData& out)
{
    std::string frequency;
    std::string load;
    for (uint32_t i = 0; i < in.cpu.cores; i++) {
        frequency += std::to_string(in.cpu.coreFrequencyLoad[i].current);
        load += std::to_string(in.cpu.coreFrequencyLoad[i].load);
        if (i + 1 < in.cpu.cores) {
            frequency += ";";
            load += ";";
        }
    }

    out.SetTime(static_cast<float>(time));
    out.SetProperty(RSCaptureData::KEY_CPU_TEMP, in.cpu.temperature);
    out.SetProperty(RSCaptureData::KEY_CPU_CURRENT, in.cpu.current);
    out.SetProperty(RSCaptureData::KEY_CPU_LOAD, load);
    out.SetProperty(RSCaptureData::KEY_CPU_FREQ, frequency);
    out.SetProperty(RSCaptureData::KEY_GPU_LOAD, in.gpu.frequencyLoad.load);
    out.SetProperty(RSCaptureData::KEY_GPU_FREQ, in.gpu.frequencyLoad.current);
}

static pid_t GetPid(const std::shared_ptr<RSRenderNode>& node)
{
    return node ? Utils::ExtractPid(node->GetId()) : 0;
}

static NodeId GetNodeId(const std::shared_ptr<RSRenderNode>& node)
{
    return node ? Utils::ExtractNodeId(node->GetId()) : 0;
}

static void SendTelemetry(double time)
{
    if (time >= 0.0) {
        RSCaptureData captureData;
        DeviceInfoToCaptureData(time, RSTelemetry::GetDeviceInfo(), captureData);
        Network::SendCaptureData(captureData);
    }
}

/*
    To visualize the damage region (as it's set for KHR_partial_update), you can set the following variable:
    'hdc shell param set rosen.dirtyregiondebug.enabled 6'
*/
void RSProfiler::SetDirtyRegion(const Occlusion::Region& dirtyRegion)
{
    if (!IsRecording()) {
        return;
    }

    const double maxPercentValue = 100.0;
    g_dirtyRegionPercentage = maxPercentValue;

    if (!g_context) {
        return;
    }
    std::shared_ptr<RSDisplayRenderNode> displayNode = GetDisplayNode(*g_context);
    if (!displayNode) {
        return;
    }
    auto params = static_cast<RSDisplayRenderParams*>(displayNode->GetRenderParams().get());
    if (!params) {
        return;
    }

    auto screenInfo = params->GetScreenInfo();
    const uint64_t displayWidth = screenInfo.width;
    const uint64_t displayHeight = screenInfo.height;
    const uint64_t displayArea = displayWidth * displayHeight;

    auto rects = RSUniRenderUtil::ScreenIntersectDirtyRects(dirtyRegion, screenInfo);
    uint64_t dirtyRegionArea = 0;
    for (const auto& rect : rects) {
        dirtyRegionArea += static_cast<uint64_t>(rect.GetWidth() * rect.GetHeight());
    }

    if (displayArea > 0) {
        g_dirtyRegionPercentage =
            maxPercentValue * static_cast<double>(dirtyRegionArea) / static_cast<double>(displayArea);
    }
    if (g_dirtyRegionPercentage > maxPercentValue) {
        g_dirtyRegionPercentage = maxPercentValue;
    }
}

void RSProfiler::Init(RSRenderService* renderService)
{
    if (!IsEnabled()) {
        return;
    }

    g_renderService = renderService;
    g_mainThread = g_renderService ? g_renderService->mainThread_ : nullptr;
    g_context = g_mainThread ? g_mainThread->context_.get() : nullptr;

    if (!IsBetaRecordEnabled()) {
        static auto const networkRunLambda = []() {
            Network::Run();
        };
        static std::thread const thread(networkRunLambda);
    }
}

void RSProfiler::OnCreateConnection(pid_t pid)
{
    if (!IsEnabled()) {
        return;
    }

    if (IsRecording()) {
        g_recordFile.AddHeaderPid(pid);
    }
}

void RSProfiler::OnRemoteRequest(RSIRenderServiceConnection* connection, uint32_t code, MessageParcel& parcel,
    MessageParcel& /*reply*/, MessageOption& option)
{
    if (!IsEnabled()) {
        return;
    }

    if (IsRecording() && (g_recordStartTime > 0.0)) {
        const pid_t pid = GetConnectionPid(connection);
        const auto& pids = g_recordFile.GetHeaderPids();
        if (std::find(std::begin(pids), std::end(pids), pid) != std::end(pids)) {
            const double deltaTime = Now() - g_recordStartTime;

            std::stringstream stream(std::ios::in | std::ios::out | std::ios::binary);

            char headerType = 1; // parcel data
            stream.write(reinterpret_cast<const char*>(&headerType), sizeof(headerType));
            stream.write(reinterpret_cast<const char*>(&deltaTime), sizeof(deltaTime));

            // set sending pid
            stream.write(reinterpret_cast<const char*>(&pid), sizeof(pid));
            stream.write(reinterpret_cast<const char*>(&code), sizeof(code));

            const size_t dataSize = parcel.GetDataSize();
            stream.write(reinterpret_cast<const char*>(&dataSize), sizeof(dataSize));
            stream.write(reinterpret_cast<const char*>(parcel.GetData()), dataSize);

            const int32_t flags = option.GetFlags();
            stream.write(reinterpret_cast<const char*>(&flags), sizeof(flags));
            const int32_t waitTime = option.GetWaitTime();
            stream.write(reinterpret_cast<const char*>(&waitTime), sizeof(waitTime));

            const std::string out = stream.str();
            constexpr size_t headerOffset = 8 + 1;
            if (out.size() >= headerOffset) {
                g_recordFile.WriteRSData(deltaTime, out.data() + headerOffset, out.size() - headerOffset);
            }
            Network::SendBinary(out.data(), out.size());
        }
    }

    if (IsLoadSaveFirstScreenInProgress()) {
        // saving screen right now
    }
    if (IsPlaying()) {
        SetTransactionTimeCorrection(g_playbackStartTime, g_playbackFile.GetWriteTime());
        SetSubstitutingPid(g_playbackFile.GetHeaderPids(), g_playbackPid, g_playbackParentNodeId);
        SetMode(Mode::READ);
    } else if (IsRecording()) {
        SetMode(Mode::WRITE);
    } else {
        SetMode(Mode::NONE);
    }
}

void RSProfiler::OnRecvParcel(const MessageParcel* parcel, RSTransactionData* data)
{
    if (!IsEnabled()) {
        return;
    }

    if (parcel && IsParcelMock(*parcel)) {
        data->SetSendingPid(Utils::GetMockPid(data->GetSendingPid()));
    }
}

void RSProfiler::CreateMockConnection(pid_t pid)
{
    if (!IsEnabled() || !g_renderService) {
        return;
    }

    auto tokenObj = new IRemoteStub<RSIConnectionToken>();

    sptr<RSIRenderServiceConnection> newConn(new RSRenderServiceConnection(pid, g_renderService, g_mainThread,
        g_renderService->screenManager_, tokenObj, g_renderService->appVSyncDistributor_));

    sptr<RSIRenderServiceConnection> tmp;

    std::unique_lock<std::mutex> lock(g_renderService->mutex_);
    // if connections_ has the same token one, replace it.
    if (g_renderService->connections_.count(tokenObj) > 0) {
        tmp = g_renderService->connections_.at(tokenObj);
    }
    g_renderService->connections_[tokenObj] = newConn;
    lock.unlock();
    g_mainThread->AddTransactionDataPidInfo(pid);
}

RSRenderServiceConnection* RSProfiler::GetConnection(pid_t pid)
{
    if (!g_renderService) {
        return nullptr;
    }

    for (const auto& pair : g_renderService->connections_) {
        auto connection = static_cast<RSRenderServiceConnection*>(pair.second.GetRefPtr());
        if (connection->remotePid_ == pid) {
            return connection;
        }
    }

    return nullptr;
}

pid_t RSProfiler::GetConnectionPid(RSIRenderServiceConnection* connection)
{
    if (!g_renderService || !connection) {
        return 0;
    }

    for (const auto& pair : g_renderService->connections_) {
        auto renderServiceConnection = static_cast<RSRenderServiceConnection*>(pair.second.GetRefPtr());
        if (renderServiceConnection == connection) {
            return renderServiceConnection->remotePid_;
        }
    }

    return 0;
}

std::vector<pid_t> RSProfiler::GetConnectionsPids()
{
    if (!g_renderService) {
        return {};
    }

    std::vector<pid_t> pids;
    pids.reserve(g_renderService->connections_.size());
    for (const auto& pair : g_renderService->connections_) {
        pids.push_back(static_cast<RSRenderServiceConnection*>(pair.second.GetRefPtr())->remotePid_);
    }
    return pids;
}

void RSProfiler::OnProcessCommand()
{
    if (!IsEnabled()) {
        return;
    }

    if (IsPlaying()) {
        ResetAnimationStamp();
    }
}

void RSProfiler::OnRenderBegin()
{
    if (!IsEnabled()) {
        return;
    }
    RS_LOGD("RSProfiler::OnRenderBegin(): enabled"); // NOLINT
    g_renderServiceCpuId = Utils::GetCpuId();
}

void RSProfiler::OnRenderEnd()
{
    if (!IsEnabled()) {
        return;
    }
    
    g_renderServiceCpuId = Utils::GetCpuId();
}

void RSProfiler::OnParallelRenderBegin()
{
    if (!IsEnabled()) {
        return;
    }

    if (g_calcPerfNode > 0) {
        // force render thread to be on fastest CPU
        constexpr uint32_t spamCount = 10'000;
        std::vector<int> data;
        data.resize(spamCount);
        for (uint32_t i = 0; i < spamCount; i++) {
            for (uint32_t j = i + 1; j < spamCount; j++) {
                std::swap(data[i], data[j]);
            }
        }

        constexpr uint32_t trashCashStep = 64;
        constexpr uint32_t newCount = spamCount * trashCashStep;
        constexpr int trashNum = 0x7F;
        data.resize(newCount);
        for (uint32_t i = 0; i < spamCount; i++) {
            data[i * trashCashStep] = trashNum;
        }
    }

    g_frameRenderBeginTimestamp = RawNowNano();
}

void RSProfiler::OnParallelRenderEnd(uint32_t frameNumber)
{
    const uint64_t frameLengthNanosecs = RawNowNano() - g_frameRenderBeginTimestamp;
    CalcNodeWeigthOnFrameEnd(frameLengthNanosecs);

    if (!IsRecording() || (g_recordStartTime <= 0.0)) {
        return;
    }

    const double currentTime = static_cast<double>(g_frameRenderBeginTimestamp) * 1e-9;
    const double timeSinceRecordStart = currentTime - g_recordStartTime;

    if (timeSinceRecordStart > 0.0) {
        RSCaptureData captureData;
        captureData.SetTime(timeSinceRecordStart);
        captureData.SetProperty(RSCaptureData::KEY_RENDER_FRAME_NUMBER, frameNumber);
        captureData.SetProperty(RSCaptureData::KEY_RENDER_FRAME_LEN, frameLengthNanosecs);

        std::vector<char> out;
        DataWriter archive(out);
        char headerType = static_cast<char>(PackageID::RS_PROFILER_RS_METRICS);
        archive.Serialize(headerType);
        captureData.Serialize(archive);

        Network::SendBinary(out.data(), out.size());
        g_recordFile.WriteRSMetrics(0, timeSinceRecordStart, out.data(), out.size());
    }
}

bool RSProfiler::ShouldBlockHWCNode()
{
    if (!IsEnabled()) {
        return false;
    }

    return GetMode() == Mode::READ;
}

void RSProfiler::OnFrameBegin()
{
    if (!IsEnabled()) {
        return;
    }

    g_frameBeginTimestamp = RawNowNano();
    g_renderServiceCpuId = Utils::GetCpuId();
    g_frameNumber++;

    StartBetaRecord();
}

void RSProfiler::OnFrameEnd()
{
    if (!IsEnabled()) {
        return;
    }

    g_renderServiceCpuId = Utils::GetCpuId();
    ProcessCommands();
    ProcessSendingRdc();
    RecordUpdate();

    UpdateDirtyRegionBetaRecord(g_dirtyRegionPercentage);
    UpdateBetaRecord();
    BlinkNodeUpdate();
    CalcPerfNodeUpdate();

    g_renderServiceCpuId = Utils::GetCpuId();
}

void RSProfiler::CalcNodeWeigthOnFrameEnd(uint64_t frameLength)
{
    g_renderServiceRenderCpuId = Utils::GetCpuId();

    if (g_calcPerfNode == 0) {
        return;
    }

    g_calcPerfNodeTime[g_calcPerfNodeTry] = frameLength;
    g_calcPerfNodeTry++;
    if (g_calcPerfNodeTry < g_effectiveNodeTimeCount) {
        AwakeRenderServiceThreadResetCaches();
        return;
    }

    std::sort(g_calcPerfNodeTime, g_calcPerfNodeTime + g_effectiveNodeTimeCount);

    const uint16_t startCnt = g_effectiveNodeTimeCount / 4;
    const uint16_t endCnt = g_effectiveNodeTimeCount - startCnt;
    uint64_t avgValue = 0;
    for (auto i = startCnt; i < endCnt; i++) {
        avgValue += g_calcPerfNodeTime[i];
    }
    avgValue /= endCnt - startCnt;

    if (g_calcPerfNodeExcludeDown) {
        g_mapNode2UpTime[g_calcPerfNode] = avgValue;
    } else {
        g_mapNode2UpDownTime[g_calcPerfNode] = avgValue;
    }

    constexpr float nanoToMs = 1000'000.0f;

    if (g_calcPerfNodeExcludeDown) {
        Respond("CALC_PERF_NODE_RESULT: U->" + std::to_string(g_calcPerfNode) + " " + std::to_string(avgValue) +
            " inMS=" + std::to_string(avgValue / nanoToMs) + " tries=" + std::to_string(g_effectiveNodeTimeCount));
    } else {
        Respond("CALC_PERF_NODE_RESULT: UD->" + std::to_string(g_calcPerfNode) + " " + std::to_string(avgValue) +
            " inMS=" + std::to_string(avgValue / nanoToMs) + " tries=" + std::to_string(g_effectiveNodeTimeCount));
    }

    if (g_nodeListPerfCalcIndex < 0) {
        Network::SendRSTreeSingleNodePerf(g_calcPerfNode, avgValue);
    }

    AwakeRenderServiceThread();
    g_renderServiceCpuId = Utils::GetCpuId();
}

void RSProfiler::RenderServiceTreeDump(JsonWriter& out)
{
    RS_TRACE_NAME("GetDumpTreeJSON");

    if (!g_context) {
        return;
    }

    auto& animation = out["Animation Node"];
    animation.PushArray();
    for (auto& [nodeId, _] : g_context->animatingNodeList_) {
        animation.Append(nodeId);
    }
    animation.PopArray();

    auto& root = out["Root node"];
    const auto rootNode = g_context->GetGlobalRootRenderNode();
    if (rootNode) {
        DumpNode(*rootNode, root);
    } else {
        root.PushObject();
        root.PopObject();
    }
}

uint64_t RSProfiler::RawNowNano()
{
    return Utils::Now();
}

uint64_t RSProfiler::NowNano()
{
    return PatchTime(RawNowNano());
}

double RSProfiler::Now()
{
    return Utils::ToSeconds(NowNano());
}

bool RSProfiler::IsRecording()
{
    return IsEnabled() && g_recordFile.IsOpen();
}

bool RSProfiler::IsPlaying()
{
    return IsEnabled() && g_playbackFile.IsOpen();
}

void RSProfiler::ScheduleTask(std::function<void()> && task)
{
    if (g_mainThread) {
        g_mainThread->PostTask(std::move(task));
    }
}

void RSProfiler::RequestNextVSync()
{
    if (g_mainThread) {
        g_mainThread->RequestNextVSync();
    }
    ScheduleTask([]() { g_mainThread->RequestNextVSync(); });
}

void RSProfiler::AwakeRenderServiceThread()
{
    if (g_mainThread) {
        g_mainThread->RequestNextVSync();
        g_mainThread->SetAccessibilityConfigChanged();
        g_mainThread->SetDirtyFlag();
    }
    ScheduleTask([]() {
        g_mainThread->SetAccessibilityConfigChanged();
        g_mainThread->SetDirtyFlag();
        g_mainThread->RequestNextVSync();
    });
}

void RSProfiler::AwakeRenderServiceThreadResetCaches()
{
    RSMainThread::Instance()->PostTask([]() {
        RSMainThread::Instance()->SetAccessibilityConfigChanged();

        auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
        nodeMap.TraversalNodes([](const std::shared_ptr<RSBaseRenderNode>& node) {
            if (node == nullptr) {
                return;
            }
            node->SetSubTreeDirty(true);
            node->SetContentDirty();
            node->SetDirty();
        });

        nodeMap.TraverseSurfaceNodes([](const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) mutable {
            if (surfaceNode == nullptr) {
                return;
            }
            surfaceNode->NeedClearBufferCache();
            surfaceNode->GetRSSurfaceHandler()->ResetBufferAvailableCount();
            surfaceNode->GetRSSurfaceHandler()->CleanCache();
            surfaceNode->UpdateBufferInfo(nullptr, {}, nullptr, nullptr);
            surfaceNode->SetNotifyRTBufferAvailable(false);
            surfaceNode->SetContentDirty();
            surfaceNode->ResetHardwareEnabledStates();
        });

        RSMainThread::Instance()->RequestNextVSync();
    });
}


void RSProfiler::ResetAnimationStamp()
{
    if (g_mainThread && g_context) {
        g_mainThread->lastAnimateTimestamp_ = g_context->GetCurrentTimestamp();
    }
}

std::shared_ptr<RSRenderNode> RSProfiler::GetRenderNode(uint64_t id)
{
    return g_context ? g_context->GetMutableNodeMap().GetRenderNode(id) : nullptr;
}

bool RSProfiler::IsLoadSaveFirstScreenInProgress()
{
    return (GetMode() == Mode::WRITE_EMUL || GetMode() == Mode::READ_EMUL);
}

void RSProfiler::HiddenSpaceTurnOn()
{
    if (!g_childOfDisplayNodes.empty()) {
        HiddenSpaceTurnOff();
    }

    const auto& rootRenderNode = g_context->GetGlobalRootRenderNode();
    auto& children = *rootRenderNode->GetChildren();
    if (children.empty()) {
        return;
    }
    if (auto& displayNode = children.front()) {
        if (auto rootNode = GetRenderNode(Utils::PatchNodeId(0))) {
            g_childOfDisplayNodes = *displayNode->GetChildren();

            displayNode->ClearChildren();
            displayNode->AddChild(rootNode);
        }
    }

    g_mainThread->SetDirtyFlag();
    AwakeRenderServiceThread();
}

void RSProfiler::HiddenSpaceTurnOff()
{
    const auto& rootRenderNode = g_context->GetGlobalRootRenderNode();
    const auto& children = *rootRenderNode->GetChildren();
    if (children.empty()) {
        return;
    }
    if (auto& displayNode = children.front()) {
        displayNode->ClearChildren();
        for (const auto& child : g_childOfDisplayNodes) {
            displayNode->AddChild(child);
        }
        FilterMockNode(*g_context);
        g_childOfDisplayNodes.clear();
    }

    g_mainThread->SetDirtyFlag();

    AwakeRenderServiceThread();
}

std::string RSProfiler::FirstFrameMarshalling()
{
    std::stringstream stream;
    SetMode(Mode::WRITE_EMUL);
    DisableSharedMemory();
    MarshalNodes(*g_context, stream);
    EnableSharedMemory();
    SetMode(Mode::NONE);

    const int32_t focusPid = g_mainThread->focusAppPid_;
    stream.write(reinterpret_cast<const char*>(&focusPid), sizeof(focusPid));

    const int32_t focusUid = g_mainThread->focusAppUid_;
    stream.write(reinterpret_cast<const char*>(&focusUid), sizeof(focusUid));

    const uint64_t focusNodeId = g_mainThread->focusNodeId_;
    stream.write(reinterpret_cast<const char*>(&focusNodeId), sizeof(focusNodeId));

    const std::string bundleName = g_mainThread->focusAppBundleName_;
    size_t size = bundleName.size();
    stream.write(reinterpret_cast<const char*>(&size), sizeof(size));
    stream.write(reinterpret_cast<const char*>(bundleName.data()), size);

    const std::string abilityName = g_mainThread->focusAppAbilityName_;
    size = abilityName.size();
    stream.write(reinterpret_cast<const char*>(&size), sizeof(size));
    stream.write(reinterpret_cast<const char*>(abilityName.data()), size);

    return stream.str();
}

void RSProfiler::FirstFrameUnmarshalling(const std::string& data, uint32_t fileVersion)
{
    std::stringstream stream;
    stream.str(data);

    SetMode(Mode::READ_EMUL);

    DisableSharedMemory();
    UnmarshalNodes(*g_context, stream, fileVersion);
    EnableSharedMemory();

    SetMode(Mode::NONE);

    int32_t focusPid = 0;
    stream.read(reinterpret_cast<char*>(&focusPid), sizeof(focusPid));

    int32_t focusUid = 0;
    stream.read(reinterpret_cast<char*>(&focusUid), sizeof(focusUid));

    uint64_t focusNodeId = 0;
    stream.read(reinterpret_cast<char*>(&focusNodeId), sizeof(focusNodeId));

    size_t size = 0;
    stream.read(reinterpret_cast<char*>(&size), sizeof(size));
    std::string bundleName;
    bundleName.resize(size, ' ');
    stream.read(reinterpret_cast<char*>(bundleName.data()), size);

    stream.read(reinterpret_cast<char*>(&size), sizeof(size));
    std::string abilityName;
    abilityName.resize(size, ' ');
    stream.read(reinterpret_cast<char*>(abilityName.data()), size);

    focusPid = Utils::GetMockPid(focusPid);
    focusNodeId = Utils::PatchNodeId(focusNodeId);

    CreateMockConnection(focusPid);
    g_mainThread->SetFocusAppInfo(focusPid, focusUid, bundleName, abilityName, focusNodeId);
}

void RSProfiler::SaveRdc(const ArgList& args)
{
    g_rdcSent = false;
    RSSystemProperties::SetSaveRDC(true);
    RSSystemProperties::SetInstantRecording(true);

    AwakeRenderServiceThread();
    Respond("Recording current frame cmds (for .rdc) into : /data/default.drawing");
}

void RSProfiler::SaveSkp(const ArgList& args)
{
    RSSystemProperties::SetInstantRecording(true);
    AwakeRenderServiceThread();
    Respond("Recording current frame cmds into : /data/default.skp");
}

void RSProfiler::ProcessSendingRdc()
{
    if (!RSSystemProperties::GetSaveRDC() || g_rdcSent) {
        return;
    }
    AwakeRenderServiceThread();

    if (!RSCaptureRecorder::PullAndSendRdc()) {
        return;
    }
    RSSystemProperties::SetSaveRDC(false);
    g_rdcSent = true;
}

static uint32_t GetImagesAdded()
{
    const size_t count = ImageCache::Size();
    const uint32_t added = count - g_lastCacheImageCount;
    g_lastCacheImageCount = count;
    return added;
}

void RSProfiler::RecordUpdate()
{
    if (!IsRecording() || (g_recordStartTime <= 0.0)) {
        return;
    }

    const uint64_t frameLengthNanosecs = RawNowNano() - g_frameBeginTimestamp;

    const double currentTime = g_frameBeginTimestamp * 1e-9; // Now();
    const double timeSinceRecordStart = currentTime - g_recordStartTime;

    if (timeSinceRecordStart > 0.0) {
        RSCaptureData captureData;
        captureData.SetTime(timeSinceRecordStart);
        captureData.SetProperty(RSCaptureData::KEY_RS_FRAME_NUMBER, g_frameNumber);
        captureData.SetProperty(RSCaptureData::KEY_RS_FRAME_LEN, frameLengthNanosecs);
        captureData.SetProperty(RSCaptureData::KEY_RS_CMD_COUNT, GetCommandCount());
        captureData.SetProperty(RSCaptureData::KEY_RS_CMD_EXECUTE_COUNT, GetCommandExecuteCount());
        captureData.SetProperty(RSCaptureData::KEY_RS_CMD_PARCEL_LIST, GetCommandParcelList(g_recordStartTime));
        captureData.SetProperty(RSCaptureData::KEY_RS_PIXEL_IMAGE_ADDED, GetImagesAdded());
        captureData.SetProperty(RSCaptureData::KEY_RS_DIRTY_REGION, floor(g_dirtyRegionPercentage));
        captureData.SetProperty(RSCaptureData::KEY_RS_CPU_ID, g_renderServiceCpuId.load());

        std::vector<char> out;
        DataWriter archive(out);
        char headerType = static_cast<char>(PackageID::RS_PROFILER_RS_METRICS);
        archive.Serialize(headerType);
        captureData.Serialize(archive);

        Network::SendBinary(out.data(), out.size());
        g_recordFile.WriteRSMetrics(0, timeSinceRecordStart, out.data(), out.size());
    }

    WriteBetaRecordMetrics(g_recordFile, timeSinceRecordStart);
}

void RSProfiler::Respond(const std::string& message)
{
    Network::SendMessage(message);
}

void RSProfiler::SetSystemParameter(const ArgList& args)
{
    if (!SystemParameter::Set(args.String(0), args.String(1))) {
        Respond("There is no such a system parameter");
    }
}

void RSProfiler::GetSystemParameter(const ArgList& args)
{
    const auto parameter = SystemParameter::Find(args.String());
    Respond(parameter ? parameter->ToString() : "There is no such a system parameter");
}

void RSProfiler::DumpSystemParameters(const ArgList& args)
{
    Respond(SystemParameter::Dump());
}

void RSProfiler::DumpConnections(const ArgList& args)
{
    if (!g_renderService) {
        return;
    }

    std::string out;
    for (const auto& pid : GetConnectionsPids()) {
        out += "pid=" + std::to_string(pid);

        const std::string path = "/proc/" + std::to_string(pid) + "/cmdline";
        FILE* file = Utils::FileOpen(path, "r");
        if (const size_t size = Utils::FileSize(file)) {
            std::string content;
            content.resize(size);
            Utils::FileRead(file, content.data(), content.size());
            Utils::FileClose(file);
            out += " ";
            out += content;
        }
        if (Utils::IsFileValid(file)) {
            Utils::FileClose(file);
        }
        out += "\n";
    }
    Respond(out);
}

void RSProfiler::DumpNodeModifiers(const ArgList& args)
{
    if (const auto node = GetRenderNode(args.Node())) {
        Respond("Modifiers=" + DumpModifiers(*node));
    }
}

void RSProfiler::DumpNodeProperties(const ArgList& args)
{
    if (const auto node = GetRenderNode(args.Node())) {
        Respond("RenderProperties=" + DumpRenderProperties(*node));
    }
}

void RSProfiler::DumpTree(const ArgList& args)
{
    if (!g_context) {
        return;
    }

    std::map<std::string, std::tuple<NodeId, std::string>> list;
    GetSurfacesTrees(*g_context, list);

    std::string out = "Tree: count=" + std::to_string(static_cast<int>(GetRenderNodeCount(*g_context))) +
                      " time=" + std::to_string(Now()) + "\n";

    const std::string& node = args.String();
    for (const auto& item : list) {
        if (std::strstr(item.first.data(), node.data())) {
            out += "*** " + item.first + " pid=" + std::to_string(ExtractPid(std::get<0>(item.second))) +
                   " lowId=" + std::to_string(Utils::ExtractNodeId(std::get<0>(item.second))) + "\n" +
                   std::get<1>(item.second) + "\n";
        }
    }

    Respond(out);
}

void RSProfiler::DumpTreeToJson(const ArgList& args)
{
    if (!g_context) {
        return;
    }

    JsonWriter json;
    json.PushObject();
    RenderServiceTreeDump(json);

    auto& display = json["Display"];
    auto displayNode = GetDisplayNode(*g_context);
    auto dirtyManager = displayNode ? displayNode->GetDirtyManager() : nullptr;
    if (dirtyManager) {
        const auto displayRect = dirtyManager->GetSurfaceRect();
        display = { displayRect.GetLeft(), displayRect.GetTop(), displayRect.GetRight(), displayRect.GetBottom() };
    } else {
        display = { 0.0f, 0.0f, 0.0f, 0.0f };
    }

    json.PopObject();
    Network::SendRSTreeDumpJSON(json.GetDumpString());
}

void RSProfiler::ClearFilter(const ArgList& args)
{
    const auto node = GetRenderNode(args.Node());
    if (!node) {
        Respond("error: node not found");
        return;
    }

    node->GetMutableRenderProperties().backgroundFilter_ = nullptr;
    Respond("OK");
    AwakeRenderServiceThread();
}

void RSProfiler::DumpSurfaces(const ArgList& args)
{
    if (!g_context) {
        return;
    }

    std::map<NodeId, std::string> surfaces;
    GetSurfacesTrees(*g_context, args.Pid(), surfaces);

    std::string out;
    for (const auto& item : surfaces) {
        out += "*** " + std::to_string(item.first) + " pid=" + std::to_string(ExtractPid(item.first)) +
               " lowId=" + std::to_string(Utils::ExtractNodeId(item.first)) + "\n" + item.second + "\n";
    }

    out += "TREE: count=" + std::to_string(static_cast<int32_t>(GetRenderNodeCount(*g_context))) +
           " time=" + std::to_string(Now()) + "\n";

    Respond(out);
}

void RSProfiler::DumpNodeSurface(const ArgList& args)
{
    if (g_renderService) {
        std::string out;
        g_renderService->DumpSurfaceNode(out, Utils::GetRootNodeId(args.Node()));
        Respond(out);
    }
}

void RSProfiler::PatchNode(const ArgList& args)
{
    const auto node = GetRenderNode(args.Node());
    if (!node) {
        return;
    }

    const Vector4f screenRect = GetScreenRect(*g_context);

    auto surfaceNode = static_cast<RSSurfaceRenderNode*>(node.get());
    {
        auto& region = const_cast<Occlusion::Region&>(surfaceNode->GetVisibleRegion());
        region = Occlusion::Region({ screenRect[0], screenRect[1], screenRect[2], screenRect[3] }); // NOLINT
    }

    RSProperties& properties = node->GetMutableRenderProperties();
    properties.SetBounds(screenRect);
    properties.SetFrame(screenRect);
    properties.SetDrawRegion(std::make_shared<RectF>(screenRect));

    AwakeRenderServiceThread();
}

void RSProfiler::KillNode(const ArgList& args)
{
    if (const auto node = GetRenderNode(args.Node())) {
        node->RemoveFromTree(false);
        AwakeRenderServiceThread();
        Respond("OK");
    }
}

void RSProfiler::BlinkNode(const ArgList& args)
{
    if (const auto node = GetRenderNode(args.Node())) {
        const auto parent = node->GetParent().lock();
        auto parentChildren = parent ? parent->GetChildren() : nullptr;
        if (!parentChildren) {
            return;
        }
    
        g_blinkNodeId = args.Node();
        g_blinkNodeCount = 0;

        g_blinkSavedParentChildren.clear();
        g_blinkSavedParentChildren.push_back(parent);

        for (const auto& child : *parentChildren) {
            if (child != nullptr) {
                g_blinkSavedParentChildren.push_back(child);
            }
        }

        AwakeRenderServiceThread();
        Respond("OK");
    }
}

void RSProfiler::AttachChild(const ArgList& args)
{
    auto child = GetRenderNode(args.Uint64(0));
    auto parent = GetRenderNode(args.Uint64(1));
    if (parent && child) {
        parent->AddChild(child);
        AwakeRenderServiceThread();
        Respond("OK");
    }
}

void RSProfiler::KillPid(const ArgList& args)
{
    const pid_t pid = args.Pid();
    if (const auto node = GetRenderNode(Utils::GetRootNodeId(static_cast<uint64_t>(pid)))) {
        const auto parent = node->GetParent().lock();
        const std::string out =
            "parentPid=" + std::to_string(GetPid(parent)) + " parentNode=" + std::to_string(GetNodeId(parent));

        g_context->GetMutableNodeMap().FilterNodeByPid(pid);
        AwakeRenderServiceThread();
        Respond(out);
    }
}

void RSProfiler::GetRoot(const ArgList& args)
{
    if (!g_context) {
        return;
    }

    std::string out;

    const RSRenderNodeMap& map = g_context->GetMutableNodeMap();
    std::shared_ptr<RSRenderNode> node = map.GetRenderNode<RSRenderNode>(GetRandomSurfaceNode(*g_context));
    while (node && (node->GetId() != 0)) {
        std::string type;
        const RSRenderNodeType nodeType = node->GetType();
        if (nodeType == RSRenderNodeType::UNKNOW) {
            type = "UNKNOWN";
        } else if (nodeType == RSRenderNodeType::RS_NODE) {
            type = "NONE";
        } else if (nodeType == RSRenderNodeType::DISPLAY_NODE) {
            type = "DISPLAY_NODE";
        } else if (nodeType == RSRenderNodeType::EFFECT_NODE) {
            type = "EFFECT_NODE";
        } else if (nodeType == RSRenderNodeType::ROOT_NODE) {
            type = "ROOT_NODE";
        } else if (nodeType == RSRenderNodeType::CANVAS_DRAWING_NODE) {
            type = "CANVAS_DRAWING_NODE";
        }

        if (!type.empty()) {
            out += "pid=" + std::to_string(GetPid(node)) + " node_id=" + std::to_string(GetNodeId(node)) + "|" + type +
                   ";\n";
        }

        node = node->GetParent().lock();
    };

    if (node) {
        out += "ROOT_ID=" + std::to_string(node->GetId()); // DISPLAY_NODE;ohos.sceneboard
    }

    Respond(out);
}

void RSProfiler::GetDeviceInfo(const ArgList& args)
{
    Respond(RSTelemetry::GetDeviceInfoString());
}

void RSProfiler::GetDeviceFrequency(const ArgList& args)
{
    Respond(RSTelemetry::GetDeviceFrequencyString());
    Respond(RSTelemetry::GetCpuAffinityString());
}

void RSProfiler::FixDeviceEnv(const ArgList& args)
{
    constexpr int32_t cpu = 8;
    Utils::SetCpuAffinity(cpu);
    Respond("OK");
}

void RSProfiler::GetPerfTree(const ArgList& args)
{
    if (!g_context) {
        return;
    }

    g_nodeListPerf.clear();
    g_mapNode2Count.clear();

    auto rootNode = GetRenderNode(Utils::PatchNodeId(0));
    if (!rootNode) {
        Respond("ERROR");
        return;
    }

    PerfTreeFlatten(rootNode, g_nodeListPerf, g_mapNode2Count, 0);

    std::string outString;
    auto& nodeMap = g_context->GetMutableNodeMap();
    for (auto it = g_nodeListPerf.begin(); it != g_nodeListPerf.end(); it++) {
        auto node = nodeMap.GetRenderNode(it->first);
        if (!node) {
            continue;
        }
        std::string sNodeType;
        RSRenderNode::DumpNodeType(node->GetType(), sNodeType);
        outString += (it != g_nodeListPerf.begin() ? ", " : "") + std::to_string(it->first) + ":" +
                     std::to_string(g_mapNode2Count[it->first]) + " [" + sNodeType + "]";
    }

    std::unordered_set<uint64_t> perfNodesSet;
    for (const auto& item : g_nodeListPerf) {
        perfNodesSet.insert(item.first);
    }

    Network::SendRSTreePerfNodeList(perfNodesSet);
    Respond("OK: Count=" + std::to_string(g_nodeListPerf.size()) + " LIST=[" + outString + "]");
}

void RSProfiler::CalcPerfNodePrepareLo(const std::shared_ptr<RSRenderNode>& node, bool forceExcludeNode)
{
        if (!node || node->id_ == Utils::PatchNodeId(0)) {
        return;
    }

    const auto parent = node->GetParent().lock();
    auto parentChildren = parent ? parent->GetChildren() : nullptr;
    if (!parentChildren) {
        return;
    }
    
    std::vector<std::shared_ptr<RSRenderNode>> childList;
    for (const auto& child : *parentChildren) {
        if (child != nullptr) {
            childList.push_back(child);
        }
    }

    // NEW APPROACH!!!
    if (forceExcludeNode) {
        g_calcPerfSavedChildren.emplace_back(parent, childList);
        parent->RemoveChild(node);
        node->ResetParent();
    } else {
        g_calcPerfSavedChildren.clear();
    }
}

void RSProfiler::CalcPerfNodePrepare(NodeId nodeId, uint32_t timeCount, bool excludeDown)
{
    Respond("Debug: CalcPerfNodePrepare nodeId=" + std::to_string(nodeId) +
            " timeCount=" + std::to_string(timeCount) +
            " excludeDown=" + std::to_string(excludeDown ? 1 : 0));

    g_calcPerfNode = nodeId;
    g_effectiveNodeTimeCount = timeCount;
    g_calcPerfNodeExcludeDown = excludeDown;
    g_calcPerfSavedChildren.clear();

    auto node = GetRenderNode(g_calcPerfNode);
    CalcPerfNodePrepareLo(node, excludeDown);
}

static void DumpParameter(std::string& out, int& usual, const std::string& name, bool value)
{
    out += " " + name + "=" + (value ? "true" : "false");
    usual += value ? 0 : 1;
}

void RSProfiler::PrintNodeCacheLo(const std::shared_ptr<RSRenderNode>& node)
{
    std::string nodeStr = std::to_string(node->id_) + ": ";
    int usual = 0;

    const std::string cacheTypes[] = { "CacheType::NONE", "CacheType::DISABLED_CACHE", "CacheType::CONTENT" };
    const auto cacheType = static_cast<int>(node->GetCacheType());
    nodeStr += "CacheType=" +
               (cacheType > static_cast<int>(CacheType::CONTENT) ? std::to_string(cacheType) : cacheTypes[cacheType]);
    usual += cacheType == static_cast<int>(CacheType::NONE) ? 1 : 0;

    const std::string drawingCacheTypes[] = { "RSDrawingCacheType::DISABLED_CACHE", "RSDrawingCacheType::FORCED_CACHE",
        "RSDrawingCacheType::TARGETED_CACHE", "RSDrawingCacheType::FOREGROUND_FILTER_CACHE" };
    const int drawingCacheType = node->GetDrawingCacheType();
    nodeStr += "DrawingCacheType=" + (drawingCacheType > RSDrawingCacheType::FOREGROUND_FILTER_CACHE
                                             ? std::to_string(drawingCacheType)
                                             : drawingCacheTypes[drawingCacheType]);
    usual += (cacheType == RSDrawingCacheType::DISABLED_CACHE) ? 1 : 0;

    DumpParameter(nodeStr, usual, "HasCachedTexture", node->HasCachedTexture());
    DumpParameter(nodeStr, usual, "DrawInGroup", node->IsSuggestedDrawInGroup());
    DumpParameter(nodeStr, usual, "CacheSurfaceValid", node->IsCacheSurfaceValid());
    DumpParameter(nodeStr, usual, "CacheCompletedSurfaceValid", node->IsCacheCompletedSurfaceValid());
    if (constexpr int usualParamCnt = 6; usual >= usualParamCnt) {
        return;
    }
    Respond(nodeStr);
}

void RSProfiler::PrintNodeCache(const ArgList& args)
{
    NodeId nodeId = args.Uint64();
    auto node = GetRenderNode(nodeId);
    if (node == nullptr) {
        Respond("node not found");
        return;
    }
    PrintNodeCacheLo(node);
}

void RSProfiler::PrintNodeCacheAll(const ArgList& args)
{
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    nodeMap.TraversalNodes([](const std::shared_ptr<RSBaseRenderNode>& node) {
        if (node == nullptr) {
            return;
        }
        PrintNodeCacheLo(node);
    });
}

void RSProfiler::CalcPerfNode(const ArgList& args)
{
    NodeId nodeId = args.Uint64();
    uint32_t timeCount = args.Uint32(1);
    const bool excludeDown = args.Uint32(2) > 0;

    if (timeCount < CALC_PERF_NODE_TIME_COUNT_MIN) {
        timeCount = CALC_PERF_NODE_TIME_COUNT_MAX;
    }
    if (timeCount > CALC_PERF_NODE_TIME_COUNT_MAX) {
        timeCount = CALC_PERF_NODE_TIME_COUNT_MAX;
    }

    g_nodeListPerf.clear();
    g_nodeListPerf.emplace_back(nodeId, 0);
    g_mapNode2UpTime.clear();
    g_mapNode2UpDownTime.clear();
    g_nodeListPerfCalcIndex = 0;
    g_effectiveNodeTimeCount = timeCount;
    CalcPerfNodeAllStep();
    AwakeRenderServiceThread();
}

void RSProfiler::SocketShutdown(const ArgList& args)
{
    Network::ForceShutdown();
}

void RSProfiler::CalcPerfNodeAll(const ArgList& args)
{
    if (g_nodeListPerf.empty()) {
        Respond("ERROR");
        return;
    }

    g_mapNode2UpTime.clear();
    g_mapNode2UpDownTime.clear();
    g_nodeListPerfCalcIndex = 0;
    g_effectiveNodeTimeCount = CALC_PERF_NODE_TIME_COUNT_MIN;
    CalcPerfNodeAllStep();
    AwakeRenderServiceThread();
}

void RSProfiler::CalcPerfNodeAllStep()
{
    if (g_nodeListPerfCalcIndex < 0) {
        return;
    }

    const auto doublePerfListSize = static_cast<int32_t>(g_nodeListPerf.size()) * 2;
    if (g_nodeListPerfCalcIndex >= doublePerfListSize) {
        g_nodeListPerfCalcIndex = -1;

        for (auto it : g_nodeListPerf) {
            const auto node = GetRenderNode(it.first);
            const auto parent = node ? node->GetParent().lock() : nullptr;
            if (!parent || !g_mapNode2UpTime.count(node->id_) || !g_mapNode2UpDownTime.count(node->id_)) {
                continue;
            }

            int64_t ns = (int64_t)g_mapNode2UpDownTime[node->id_] - (int64_t)g_mapNode2UpTime[node->id_];
            constexpr float nsToMs = 1000.0f;
            double ms = static_cast<double>(ns) / nsToMs;
            ms /= nsToMs;

            Respond("CALC_RESULT [" + std::to_string(node->id_) + "] time=" + std::to_string(ms));
        }
        return;
    }

    const NodeId nodeId = g_nodeListPerf[g_nodeListPerfCalcIndex / 2].first;
    const bool excludeDown = g_nodeListPerfCalcIndex % 2;
    CalcPerfNodePrepare(nodeId, g_effectiveNodeTimeCount, excludeDown);
    AwakeRenderServiceThread();
}

void RSProfiler::TestSaveFrame(const ArgList& args)
{
    g_testDataFrame = FirstFrameMarshalling();
    Respond("Save Frame Size: " + std::to_string(g_testDataFrame.size()));
}

void RSProfiler::TestLoadFrame(const ArgList& args)
{
    FirstFrameUnmarshalling(g_testDataFrame, RSFILE_VERSION_LATEST);
    Respond("Load Frame Size: " + std::to_string(g_testDataFrame.size()));
}

void RSProfiler::TestSwitch(const ArgList& args)
{
    if (g_childOfDisplayNodes.empty()) {
        HiddenSpaceTurnOn();
        Respond("OK: HiddenSpaceTurnOn");
    } else {
        HiddenSpaceTurnOff();
        Respond("OK: HiddenSpaceTurnOff");
    }
}

void RSProfiler::RecordStart(const ArgList& args)
{
    g_recordStartTime = 0.0;

    ImageCache::Reset();
    g_lastCacheImageCount = 0;

    if (!OpenBetaRecordFile(g_recordFile)) {
        g_recordFile.Create(RSFile::GetDefaultPath());
    }

    g_recordFile.AddLayer(); // add 0 layer

    FilterMockNode(*g_context);

    g_recordFile.AddHeaderFirstFrame(FirstFrameMarshalling());

    const std::vector<pid_t> pids = GetConnectionsPids();
    for (pid_t pid : pids) {
        g_recordFile.AddHeaderPid(pid);
    }

    SetMode(Mode::WRITE);

    g_recordStartTime = Now();
    g_frameNumber = 0;

    if (IsBetaRecordStarted()) {
        return;
    }

    std::thread thread([]() {
        while (IsRecording()) {
            if (g_recordStartTime >= 0) {
                SendTelemetry(Now() - g_recordStartTime);
            }
            static constexpr int32_t GFX_METRICS_SEND_INTERVAL = 8;
            std::this_thread::sleep_for(std::chrono::milliseconds(GFX_METRICS_SEND_INTERVAL));
        }
    });
    thread.detach();

    Respond("Network: Record start");
}

void RSProfiler::RecordStop(const ArgList& args)
{
    if (!IsRecording()) {
        return;
    }

    g_recordFile.SetWriteTime(g_recordStartTime);

    std::stringstream stream(std::ios::in | std::ios::out | std::ios::binary);

    if (!IsBetaRecordStarted()) {
        // DOUBLE WORK - send header of file
        const char headerType = 0;
        stream.write(reinterpret_cast<const char*>(&headerType), sizeof(headerType));
        stream.write(reinterpret_cast<const char*>(&g_recordStartTime), sizeof(g_recordStartTime));

        const uint32_t pidCount = g_recordFile.GetHeaderPids().size();
        stream.write(reinterpret_cast<const char*>(&pidCount), sizeof(pidCount));
        for (auto item : g_recordFile.GetHeaderPids()) {
            stream.write(reinterpret_cast<const char*>(&item), sizeof(item));
        }

        uint32_t sizeFirstFrame = static_cast<uint32_t>(g_recordFile.GetHeaderFirstFrame().size());
        stream.write(reinterpret_cast<const char*>(&sizeFirstFrame), sizeof(sizeFirstFrame));
        stream.write(reinterpret_cast<const char*>(&g_recordFile.GetHeaderFirstFrame()[0]), sizeFirstFrame);

        ImageCache::Serialize(stream);
        Network::SendBinary(stream.str().data(), stream.str().size());
    }
    SaveBetaRecordFile(g_recordFile);
    g_recordFile.Close();
    g_recordStartTime = 0.0;

    ImageCache::Reset();
    g_lastCacheImageCount = 0;

    Respond("Network: Record stop (" + std::to_string(stream.str().size()) + ")");
}

void RSProfiler::PlaybackPrepareFirstFrame(const ArgList& args)
{
    g_playbackPid = args.Pid();
    g_playbackStartTime = 0.0;
    g_playbackPauseTime = args.Fp64(1);
    constexpr int pathArgPos = 2;
    std::string path = args.String(pathArgPos);
    if (!Utils::FileExists(path)) {
        Respond("Can't playback non existing file '" + path + "'");
        path = RSFile::GetDefaultPath();
    }

    ImageCache::Reset();

    Respond("Opening file " + path);
    g_playbackFile.Open(path);
    if (!g_playbackFile.IsOpen()) {
        Respond("Can't open file.");
        return;
    }
    std::string dataFirstFrame = g_playbackFile.GetHeaderFirstFrame();

    // get first frame data
    FirstFrameUnmarshalling(dataFirstFrame, g_playbackFile.GetVersion());
    // The number of frames loaded before command processing
    constexpr int defaultWaitFrames = 5;
    g_playbackWaitFrames = defaultWaitFrames;
    Respond("awake_frame " + std::to_string(g_playbackWaitFrames));
    AwakeRenderServiceThread();
}

void RSProfiler::PlaybackStart(const ArgList& args)
{
    HiddenSpaceTurnOn();

    for (size_t pid : g_playbackFile.GetHeaderPids()) {
        CreateMockConnection(Utils::GetMockPid(pid));
    }

    g_playbackStartTime = Now();

    const double pauseTime = g_playbackPauseTime;
    if (pauseTime > 0.0) {
        const uint64_t currentTime = RawNowNano();
        const uint64_t pauseTimeStart = currentTime + Utils::ToNanoseconds(pauseTime);
        TimePauseAt(currentTime, pauseTimeStart);
    }

    AwakeRenderServiceThread();

    g_playbackShouldBeTerminated = false;

    const auto timeoutLimit = args.Int64();
    std::thread thread([timeoutLimit]() {
        while (IsPlaying()) {
            const int64_t timestamp = static_cast<int64_t>(RawNowNano());

            PlaybackUpdate();
            if (g_playbackStartTime >= 0) {
                SendTelemetry(Now() - g_playbackStartTime);
            }

            const int64_t timeout = timeoutLimit - static_cast<int64_t>(RawNowNano()) + timestamp;
            if (timeout > 0) {
                std::this_thread::sleep_for(std::chrono::nanoseconds(timeout));
            }
        }
    });
    thread.detach();

    Respond("Playback start");
}

void RSProfiler::PlaybackStop(const ArgList& args)
{
    if (g_childOfDisplayNodes.empty()) {
        return;
    }
    HiddenSpaceTurnOff();
    FilterMockNode(*g_context);
    g_playbackShouldBeTerminated = true;

    Respond("Playback stop");
}

void RSProfiler::PlaybackUpdate()
{
    if (!IsPlaying()) {
        return;
    }

    const double deltaTime = Now() - g_playbackStartTime;

    std::vector<uint8_t> data;
    double readTime = 0.0;
    if (!g_playbackShouldBeTerminated && g_playbackFile.ReadRSData(deltaTime, data, readTime)) {
        std::stringstream stream(std::ios::in | std::ios::out | std::ios::binary);
        stream.write(reinterpret_cast<const char*>(data.data()), data.size());
        stream.seekg(0);

        pid_t pid = 0;
        stream.read(reinterpret_cast<char*>(&pid), sizeof(pid));

        RSRenderServiceConnection* connection = GetConnection(Utils::GetMockPid(pid));
        if (!connection) {
            const std::vector<pid_t>& pids = g_playbackFile.GetHeaderPids();
            if (!pids.empty()) {
                connection = GetConnection(Utils::GetMockPid(pids[0]));
            }
        }

        if (connection) {
            uint32_t code = 0;
            stream.read(reinterpret_cast<char*>(&code), sizeof(code));

            size_t dataSize = 0;
            stream.read(reinterpret_cast<char*>(&dataSize), sizeof(dataSize));

            auto* data = new uint8_t[dataSize];
            stream.read(reinterpret_cast<char*>(data), dataSize);

            int32_t flags = 0;
            stream.read(reinterpret_cast<char*>(&flags), sizeof(flags));

            int32_t waitTime = 0;
            stream.read(reinterpret_cast<char*>(&waitTime), sizeof(waitTime));

            AlignedMessageParcel parcel;
            parcel.parcel.SetMaxCapacity(dataSize + 1);
            parcel.parcel.WriteBuffer(data, dataSize);

            delete[] data;

            MessageOption option;
            option.SetFlags(flags);
            option.SetWaitTime(waitTime);

            MessageParcel reply;
            connection->OnRemoteRequest(code, parcel.parcel, reply, option);
        }
    }

    if (g_playbackShouldBeTerminated || g_playbackFile.RSDataEOF()) {
        g_playbackStartTime = 0.0;
        g_playbackFile.Close();
        g_playbackPid = 0;
        TimePauseClear();
        g_playbackShouldBeTerminated = false;
    }
}

void RSProfiler::PlaybackPrepare(const ArgList& args)
{
    const pid_t pid = args.Pid();
    if (!g_context || (pid == 0)) {
        return;
    }

    FilterForPlayback(*g_context, pid);
    AwakeRenderServiceThread();
    Respond("OK");
}

void RSProfiler::PlaybackPause(const ArgList& args)
{
    if (!IsPlaying()) {
        return;
    }

    const uint64_t currentTime = RawNowNano();
    const double recordPlayTime = Now() - g_playbackStartTime;
    TimePauseAt(currentTime, currentTime);
    Respond("OK: " + std::to_string(recordPlayTime));
}

void RSProfiler::PlaybackPauseAt(const ArgList& args)
{
    if (!IsPlaying()) {
        return;
    }

    const double pauseTime = args.Fp64();
    const double recordPlayTime = Now() - g_playbackStartTime;
    if (recordPlayTime > pauseTime) {
        return;
    }

    const uint64_t currentTime = RawNowNano();
    const uint64_t pauseTimeStart = currentTime + Utils::ToNanoseconds(pauseTime - recordPlayTime);

    TimePauseAt(currentTime, pauseTimeStart);
    ResetAnimationStamp();
    Respond("OK");
}

void RSProfiler::PlaybackPauseClear(const ArgList& args)
{
    TimePauseClear();
    Respond("OK");
}

void RSProfiler::PlaybackResume(const ArgList& args)
{
    if (!IsPlaying()) {
        return;
    }

    TimePauseResume(Utils::Now());
    ResetAnimationStamp();
    Respond("OK");
}

RSProfiler::Command RSProfiler::GetCommand(const std::string& command)
{
    static const std::map<std::string, Command> COMMANDS = {
        { "rstree_contains", DumpTree },
        { "rstree_fix", PatchNode },
        { "rstree_kill_node", KillNode },
        { "rstree_setparent", AttachChild },
        { "rstree_getroot", GetRoot },
        { "rstree_node_mod", DumpNodeModifiers },
        { "rstree_node_prop", DumpNodeProperties },
        { "rstree_pid", DumpSurfaces },
        { "rstree_kill_pid", KillPid },
        { "rstree_prepare_replay", PlaybackPrepare },
        { "rstree_save_frame", TestSaveFrame },
        { "rstree_load_frame", TestLoadFrame },
        { "rstree_switch", TestSwitch },
        { "rstree_dump_json", DumpTreeToJson },
        { "rstree_clear_filter", ClearFilter },
        { "rstree_blink_node", BlinkNode },
        { "rsrecord_start", RecordStart },
        { "rsrecord_stop", RecordStop },
        { "rsrecord_replay_prepare", PlaybackPrepareFirstFrame },
        { "rsrecord_replay", PlaybackStart },
        { "rsrecord_replay_stop", PlaybackStop },
        { "rsrecord_pause_now", PlaybackPause },
        { "rsrecord_pause_at", PlaybackPauseAt },
        { "rsrecord_pause_resume", PlaybackResume },
        { "rsrecord_pause_clear", PlaybackPauseClear },
        { "rssurface_pid", DumpNodeSurface },
        { "rscon_print", DumpConnections },
        { "save_rdc", SaveRdc },
        { "save_skp", SaveSkp },
        { "info", GetDeviceInfo },
        { "freq", GetDeviceFrequency },
        { "fixenv", FixDeviceEnv },
        { "set", SetSystemParameter },
        { "get", GetSystemParameter },
        { "params", DumpSystemParameters },
        { "get_perf_tree", GetPerfTree },
        { "calc_perf_node", CalcPerfNode },
        { "calc_perf_node_all", CalcPerfNodeAll },
        { "socket_shutdown", SocketShutdown },
        { "rstree_node_cache", PrintNodeCache },
        { "rstree_node_cache_all", PrintNodeCacheAll },
    };

    if (command.empty()) {
        return nullptr;
    }

    const auto delegate = COMMANDS.find(command);
    return (delegate != COMMANDS.end()) ? delegate->second : nullptr;
}

void RSProfiler::ProcessCommands()
{
    if (g_playbackWaitFrames > 0) {
        g_playbackWaitFrames--;
        Respond("awake_frame " + std::to_string(g_playbackWaitFrames));
        AwakeRenderServiceThread();
        return;
    }

    std::vector<std::string> commandData;
    if (!Network::PopCommand(commandData)) {
        return;
    }

    const std::string& command = commandData[0];
    if (const Command delegate = GetCommand(command)) {
        const ArgList args =
            (commandData.size() > 1) ? ArgList({ commandData.begin() + 1, commandData.end() }) : ArgList();
        delegate(args);
    } else if (!command.empty()) {
        Respond("Command has not been found: " + command);
    }
}

uint32_t RSProfiler::GetFrameNumber()
{
    return g_frameNumber;
}

void RSProfiler::BlinkNodeUpdate()
{
    if (g_blinkSavedParentChildren.empty()) {
        return;
    }

    constexpr uint32_t blinkTotalCount = 20;
    constexpr uint32_t blinkFrameDelay = 5;
    const uint32_t blinkStage = g_blinkNodeCount / blinkFrameDelay;
    if (const bool isOdd = blinkStage % 2) {
        // restore node
        const auto parentNode = g_blinkSavedParentChildren[0];
        if (!parentNode) {
            return;
        }
        for (size_t i = 1; i < g_blinkSavedParentChildren.size(); i++) {
            const auto child = g_blinkSavedParentChildren[i];
            parentNode->RemoveChild(child);
            child->ResetParent();
        }
        for (size_t i = 1; i < g_blinkSavedParentChildren.size(); i++) {
            const auto child = g_blinkSavedParentChildren[i];
            parentNode->AddChild(child);
        }
        if (blinkStage > blinkTotalCount) {
            g_blinkNodeCount = 0;
            g_blinkSavedParentChildren.clear();
        }
    } else {
        // remove node
        const auto parentNode = g_blinkSavedParentChildren[0];
        auto blinkNode = GetRenderNode(g_blinkNodeId);
        parentNode->RemoveChild(blinkNode);
        blinkNode->ResetParent();
    }

    g_blinkNodeCount++;
    AwakeRenderServiceThread();
}

void RSProfiler::CalcPerfNodeUpdate()
{
    if (g_calcPerfNode != 0 || g_nodeListPerfCalcIndex < 0) {
        return;
    }
    if (g_calcPerfNodeTry != 0) {
        g_calcPerfNodeTry = 0;
        g_nodeListPerfCalcIndex++;
        CalcPerfNodeAllStep();
        return;
    }
    for (const auto& item : g_calcPerfSavedChildren) {
        const auto parentNode = item.first;
        if (!parentNode) {
            continue;
        }
        for (const auto& child : item.second) {
            parentNode->RemoveChild(child);
            child->ResetParent();
        }
        for (const auto& child : item.second) {
            parentNode->AddChild(child);
        }
    }
    g_calcPerfSavedChildren.clear();
    g_calcPerfNodeTry++;
    AwakeRenderServiceThread();
}

} // namespace OHOS::Rosen