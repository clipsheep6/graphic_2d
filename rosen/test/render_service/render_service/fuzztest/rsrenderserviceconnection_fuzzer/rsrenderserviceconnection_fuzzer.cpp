/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "rsrenderserviceconnection_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include <unordered_map>

#include "ipc_object_proxy.h"
#include "ipc_object_stub.h"
#include "iremote_object.h"
#include "message_parcel.h"
#include "securec.h"

#include "ipc_callbacks/pointer_luminance_callback_stub.h"
#include "ipc_callbacks/rs_occlusion_change_callback_stub.h"
#include "pipeline/rs_render_service.h"
#include "pipeline/rs_render_service_connection.h"
#include "pipeline/rs_hardware_thread.h"
#include "pipeline/rs_main_thread.h"
#include "platform/ohos/rs_render_service_connect_hub.cpp"
#include "screen_manager/rs_screen_manager.h"
#include "transaction/rs_render_service_client.h"
#include "transaction/rs_render_service_connection_stub.h"
#include "transaction/rs_transaction_proxy.h"

namespace OHOS {
namespace Rosen {
constexpr size_t MAX_SIZE = 4;
constexpr size_t SCREEN_WIDTH = 100;
constexpr size_t SCREEN_HEIGHT = 100;

auto mainThread = RSMainThread::Instance();
auto generator = CreateVSyncGenerator();
auto renderService = RSRenderServiceConnectHub::GetRenderService();
auto pid = getpid();
RSScreenManager* screenManager = impl::RSScreenManager::GetInstance().GetRefPtr();
auto tokenObj = new IRemoteStub<RSIConnectionToken>();
auto appVsyncController = new VSyncController(generator, 0);
auto appVsyncDistributor = new VSyncDistributor(appVsyncController, "app");
std::shared_ptr<RSRenderServiceConnection> rsConnection = std::make_shared<RSRenderServiceConnection>(
    pid, renderService, mainThread, screenManager, tokenObj, appVsyncDistributor);

namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (g_data == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}

bool DoRegisterApplicationAgent(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t pid = GetData<uint32_t>();
    MessageParcel message;
    auto remoteObject = message.ReadRemoteObject();
    sptr<IApplicationAgent> app = iface_cast<IApplicationAgent>(remoteObject);
    if (app) {
        rsConnection->RegisterApplicationAgent(pid, app);
    }
    return true;
}

bool DoCommitTransaction(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    auto transactionData = std::make_unique<RSTransactionData>();
    rsConnection->CommitTransaction(transactionData);
    return true;
}

class DerivedSyncTask : public RSSyncTask {
public:
    using RSSyncTask::RSSyncTask;
    bool CheckHeader(Parcel& parcel) const override
    {
        return true;
    }
    bool ReadFromParcel(Parcel& parcel) override
    {
        return true;
    }
    bool Marshalling(Parcel& parcel) const override
    {
        return true;
    }
    void Process(RSContext& context) override {}
};
bool DoExecuteSynchronousTask(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    g_data = data;
    g_size = size;
    g_pos = 0;

    uint64_t timeoutNS = GetData<uint64_t>();
    auto task = std::make_shared<DerivedSyncTask>(timeoutNS);
    rsConnection->ExecuteSynchronousTask(task);
    return true;
}

bool DoGetMemoryGraphic(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    g_data = data;
    g_size = size;
    g_pos = 0;

    int pid = GetData<int>();
    rsConnection->GetMemoryGraphic(pid);
    return true;
}

bool DoCreateNodeAndSurface(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }
    RSSurfaceRenderNodeConfig config = { .id = 0, .name = "test", .bundleName = "test" };
    rsConnection->CreateNode(config);
    rsConnection->CreateNodeAndSurface(config);
    return true;
}

bool DoGetScreenBacklight(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    ScreenId id = GetData<uint64_t>();
    uint32_t level = GetData<uint32_t>();
    RSHardwareThread::Instance().Start();
    rsConnection->SetScreenBacklight(id, level);
    rsConnection->GetScreenBacklight(id);
    return true;
}

bool DoGetScreenType(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    g_data = data;
    g_size = size;
    g_pos = 0;
    ScreenId id = GetData<uint64_t>();
    uint32_t level = GetData<uint32_t>();
    RSScreenType type = (RSScreenType)level;
    rsConnection->GetScreenType(id, type);
    return true;
}

bool DoRegisterBufferAvailableListener(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    g_data = data;
    g_size = size;
    g_pos = 0;
    uint64_t id = GetData<uint64_t>();
    bool isFromRenderThread = GetData<bool>();
    sptr<RSIBufferAvailableCallback> cb = nullptr;
    rsConnection->RegisterBufferAvailableListener(id, cb, isFromRenderThread);
    return true;
}

bool DoSetScreenSkipFrameInterval(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    g_data = data;
    g_size = size;
    g_pos = 0;
    ScreenId id = GetData<uint64_t>();
    uint32_t skipFrameInterval = GetData<uint32_t>();
    rsConnection->SetScreenSkipFrameInterval(id, skipFrameInterval);
    return true;
}

bool DoSetVirtualScreenResolution(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    g_data = data;
    g_size = size;
    g_pos = 0;
    ScreenId id = GetData<uint64_t>();
    rsConnection->SetVirtualScreenResolution(id, SCREEN_WIDTH, SCREEN_HEIGHT);
    rsConnection->GetVirtualScreenResolution(id);
    return true;
}

bool DoGetScreenSupportedColorGamuts(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    g_data = data;
    g_size = size;
    g_pos = 0;
    std::vector<ScreenColorGamut> mode;
    ScreenId id = GetData<uint64_t>();
    uint32_t screenCol = GetData<uint32_t>();
    mode.push_back((ScreenColorGamut)screenCol);
    rsConnection->GetScreenSupportedColorGamuts(id, mode);
    std::vector<ScreenHDRMetadataKey> keys;
    keys.push_back((ScreenHDRMetadataKey)screenCol);
    rsConnection->GetScreenSupportedMetaDataKeys(id, keys);
    return true;
}

bool DoGetScreenSupportedModes(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    g_data = data;
    g_size = size;
    g_pos = 0;
    ScreenId id = GetData<uint64_t>();
    rsConnection->GetScreenSupportedModes(id);
    return true;
}

bool DoGetScreenColorGamut(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    g_data = data;
    g_size = size;
    g_pos = 0;
    ScreenId id = GetData<uint64_t>();
    int32_t modeIdx = GetData<int32_t>();
    rsConnection->SetScreenColorGamut(id, modeIdx);
    uint32_t mod = GetData<uint32_t>();
    ScreenColorGamut mode = (ScreenColorGamut)mod;
    rsConnection->GetScreenColorGamut(id, mode);
    return true;
}

bool DoSetScreenPowerStatus(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    g_data = data;
    g_size = size;
    g_pos = 0;
    ScreenId id = GetData<uint64_t>();
    uint32_t status = GetData<uint32_t>();
    rsConnection->SetScreenPowerStatus(id, static_cast<ScreenPowerStatus>(status));
    rsConnection->GetScreenPowerStatus(id);
    return true;
}

bool DoSetScreenGamutMap(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    g_data = data;
    g_size = size;
    g_pos = 0;
    ScreenId id = GetData<uint64_t>();
    uint32_t mapMode = GetData<uint32_t>();
    ScreenGamutMap mode = (ScreenGamutMap)mapMode;
    rsConnection->SetScreenGamutMap(id, mode);
    rsConnection->GetScreenGamutMap(id, mode);
    return true;
}

bool DoSetAppWindowNum(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    g_data = data;
    g_size = size;
    g_pos = 0;
    uint32_t num = GetData<uint32_t>();
    rsConnection->SetAppWindowNum(num);
    return true;
}

bool DoCreateVirtualScreen(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    g_data = data;
    g_size = size;
    g_pos = 0;
    ScreenId id = GetData<uint64_t>();
    int32_t flags = GetData<int32_t>();
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("DisplayNode");
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(bp);
    rsConnection->CreateVirtualScreen("name", SCREEN_WIDTH, SCREEN_HEIGHT, pSurface, id, flags);
    rsConnection->SetVirtualScreenSurface(id, pSurface);
    rsConnection->RemoveVirtualScreen(id);
    return true;
}

bool DoSetPointerColorInversionConfig(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
 
    if (size < MAX_SIZE) {
        return false;
    }
 
    g_data = data;
    g_size = size;
    g_pos = 0;    float darkBuffer = GetData<float>();
    float brightBuffer = GetData<float>();
    int64_t interval = GetData<int64_t>();
    rsConnection->SetPointerColorInversionConfig(darkBuffer, brightBuffer, interval);
    return true;
}

class CustomPointerLuminanceChangeCallback : public RSPointerLuminanceChangeCallbackStub {
public:
    explicit CustomPointerLuminanceChangeCallback(const PointerLuminanceChangeCallback &callback) : cb_(callback) {}
    ~CustomPointerLuminanceChangeCallback() override {};
 
    void OnPointerLuminanceChanged(int32_t brightness) override
    {
        if (cb_ != nullptr) {
            cb_(brightness);
        }
    }

private:
    PointerLuminanceChangeCallback cb_;
};

bool DoRegisterPointerLuminanceChangeCallback(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
 
    if (size < MAX_SIZE) {
        return false;
    }
     PointerLuminanceChangeCallback callback = [](int32_t brightness) {};
    sptr<CustomPointerLuminanceChangeCallback> cb = new CustomPointerLuminanceChangeCallback(callback);
    rsConnection->RegisterPointerLuminanceChangeCallback(cb);
    return true;
}

bool DoSetScreenActiveMode(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    g_data = data;
    g_size = size;
    g_pos = 0;
    ScreenId id = GetData<uint64_t>();
    uint32_t modeId = GetData<uint32_t>();
    rsConnection->SetScreenActiveMode(id, modeId);
    rsConnection->GetScreenActiveMode(id);
    return true;
}

bool DoSetRefreshRateMode(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    g_data = data;
    g_size = size;
    g_pos = 0;
    int32_t refreshRateMode = GetData<int32_t>();
    rsConnection->SetRefreshRateMode(refreshRateMode);
    return true;
}

bool DoCreateVSyncConnection(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    g_data = data;
    g_size = size;
    g_pos = 0;
    uint64_t id = GetData<uint64_t>();
    sptr<VSyncIConnectionToken> token = new IRemoteStub<VSyncIConnectionToken>();
    rsConnection->CreateVSyncConnection("test", token, id);
    return true;
}

bool DoSetScreenRefreshRate(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    g_data = data;
    g_size = size;
    g_pos = 0;
    ScreenId id = GetData<uint64_t>();
    int32_t sceneId = GetData<int32_t>();
    int32_t rate = GetData<int32_t>();
    rsConnection->SetScreenRefreshRate(id, sceneId, rate);
    rsConnection->GetScreenCurrentRefreshRate(id);
    rsConnection->GetScreenSupportedRefreshRates(id);
    return true;
}

bool DoGetBitmap(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    g_data = data;
    g_size = size;
    g_pos = 0;
    Drawing::Bitmap bm;
    NodeId id = GetData<uint64_t>();
    rsConnection->GetBitmap(id, bm);
    return true;
}

bool DoGetScreenCapability(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    g_data = data;
    g_size = size;
    g_pos = 0;
    ScreenId id = GetData<uint64_t>();
    rsConnection->GetScreenCapability(id);
    return true;
}

bool DoGetScreenData(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    g_data = data;
    g_size = size;
    g_pos = 0;
    ScreenId id = GetData<uint64_t>();
    rsConnection->GetScreenData(id);
    return true;
}

bool DoGetScreenHDRCapability(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }
    
    g_data = data;
    g_size = size;
    g_pos = 0;
    ScreenId id = GetData<uint64_t>();
    RSScreenHDRCapability screenHDRCapability;
    rsConnection->GetScreenHDRCapability(id, screenHDRCapability);
    return true;
}

class CustomOcclusionChangeCallback : public RSOcclusionChangeCallbackStub {
public:
    explicit CustomOcclusionChangeCallback(const OcclusionChangeCallback& callback) : cb_(callback) {}
    ~CustomOcclusionChangeCallback() override {};

    void OnOcclusionVisibleChanged(std::shared_ptr<RSOcclusionData> occlusionData) override
    {
        if (cb_ != nullptr) {
            cb_(occlusionData);
        }
    }

private:
    OcclusionChangeCallback cb_;
};

bool DoRegisterOcclusionChangeCallback(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }
    OcclusionChangeCallback callback = [](std::shared_ptr<RSOcclusionData> data) {};
    sptr<CustomOcclusionChangeCallback> cb = new CustomOcclusionChangeCallback(callback);
    rsConnection->RegisterOcclusionChangeCallback(cb);
    return true;
}

bool DoShowWatermark(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    g_data = data;
    g_size = size;
    g_pos = 0;
        bool isShow = GetData<bool>();
    std::shared_ptr<Media::PixelMap> pixelMap1;
    rsConnection->ShowWatermark(pixelMap1, isShow);
    return true;
}

bool DoTakeSurfaceCapture(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    g_data = data;
    g_size = size;
    g_pos = 0;
        uint64_t nodeId = GetData<uint64_t>();
    sptr<RSISurfaceCaptureCallback> callback = nullptr;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = GetData<float>();
    captureConfig.scaleY = GetData<float>();
    captureConfig.useDma = GetData<bool>();
    captureConfig.useCurWindow = GetData<bool>();
    uint8_t type = GetData<uint8_t>();
    captureConfig.captureType = (SurfaceCaptureType)type;
    captureConfig.isSync = GetData<bool>();
    rsConnection->TakeSurfaceCapture(nodeId, callback, captureConfig);
    return true;
}

bool DoSetScreenChangeCallback(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }
        sptr<RSIScreenChangeCallback> callback = nullptr;
    rsConnection->SetScreenChangeCallback(callback);
    return true;
}

bool DoSetFocusAppInfo(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    MessageParcel datas;
    datas.WriteBuffer(data, size);
    int32_t pid = datas.ReadInt32();
    int32_t uid = datas.ReadInt32();
    std::string bundleName = datas.ReadString();
    std::string abilityName = datas.ReadString();
    uint64_t focusNodeId = datas.ReadUint64();
    rsConnection->SetFocusAppInfo(pid, uid, bundleName, abilityName, focusNodeId);
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoRegisterApplicationAgent(data, size);
    OHOS::Rosen::DoCommitTransaction(data, size);
    OHOS::Rosen::DoExecuteSynchronousTask(data, size);
    OHOS::Rosen::DoGetMemoryGraphic(data, size);
    OHOS::Rosen::DoCreateNodeAndSurface(data, size);
    OHOS::Rosen::DoGetScreenBacklight(data, size);
    OHOS::Rosen::DoGetScreenType(data, size);
    OHOS::Rosen::DoRegisterBufferAvailableListener(data, size);
    OHOS::Rosen::DoSetScreenSkipFrameInterval(data, size);
    OHOS::Rosen::DoSetVirtualScreenResolution(data, size);
    OHOS::Rosen::DoGetScreenSupportedColorGamuts(data, size);
    OHOS::Rosen::DoGetScreenSupportedModes(data, size);
    OHOS::Rosen::DoGetScreenColorGamut(data, size);
    OHOS::Rosen::DoSetScreenPowerStatus(data, size);
    OHOS::Rosen::DoSetScreenGamutMap(data, size);
    OHOS::Rosen::DoSetAppWindowNum(data, size);
    OHOS::Rosen::DoCreateVirtualScreen(data, size);
    OHOS::Rosen::DoSetPointerColorInversionConfig(data, size);
    OHOS::Rosen::DoRegisterPointerLuminanceChangeCallback(data, size);
    OHOS::Rosen::DoSetScreenActiveMode(data, size);
    OHOS::Rosen::DoSetRefreshRateMode(data, size);
    OHOS::Rosen::DoCreateVSyncConnection(data, size);
    OHOS::Rosen::DoSetScreenRefreshRate(data, size);
    OHOS::Rosen::DoGetBitmap(data, size);
    OHOS::Rosen::DoGetScreenCapability(data, size);
    OHOS::Rosen::DoGetScreenData(data, size);
    OHOS::Rosen::DoGetScreenHDRCapability(data, size);
    OHOS::Rosen::DoRegisterOcclusionChangeCallback(data, size);
    OHOS::Rosen::DoShowWatermark(data, size);
    OHOS::Rosen::DoTakeSurfaceCapture(data, size);
    OHOS::Rosen::DoSetScreenChangeCallback(data, size);
    OHOS::Rosen::DoSetFocusAppInfo(data, size);
    return 0;
}