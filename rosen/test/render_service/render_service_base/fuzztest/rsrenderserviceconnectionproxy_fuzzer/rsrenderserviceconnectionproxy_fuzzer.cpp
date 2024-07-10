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

#include "rsrenderserviceconnectionproxy_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <hilog/log.h>
#include <iremote_stub.h>
#include <iservice_registry.h>
#include <memory>
#include <securec.h>
#include <system_ability_definition.h>
#include <unistd.h>

#include "command/rs_animation_command.h"
#include "command/rs_node_showing_command.h"
#include "platform/ohos/rs_render_service_connection_proxy.h"
namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
int g_one = 1;
} // namespace

auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
RSRenderServiceConnectionProxy rsRenderServiceConnectionProxy(remoteObject);

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
bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    sptr<Surface> surface;
    sptr<IConsumerSurface> consumer = IConsumerSurface::Create("DisplayNode");
    sptr<IBufferProducer> producer = consumer->GetProducer();
    surface = Surface::CreateSurfaceAsProducer(producer);
    int pid = GetData<int>();
    uint64_t timeoutNS = GetData<uint64_t>();
    RSSurfaceRenderNodeConfig config;
    config.id = timeoutNS;
    std::string name("name");
    uint64_t id1 = GetData<uint64_t>();
    int32_t pid1 = GetData<int32_t>();
    int32_t uid = GetData<int32_t>();
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    RSScreenHDRCapability screenHdrCapability;
    GraphicCM_ColorSpaceType colorSpace = GraphicCM_ColorSpaceType::GRAPHIC_CM_SRGB_FULL;
    RSScreenType screenType = RSScreenType::VIRTUAL_TYPE_SCREEN;
    sptr<IApplicationAgent> app;
    FrameRateRange range;
    sptr<RSIScreenChangeCallback> callback;
    sptr<RSISurfaceCaptureCallback> callback1;
    sptr<RSIBufferAvailableCallback> callback2;
    sptr<RSIBufferClearCallback> callback3;
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    sptr<RSIOcclusionChangeCallback> rsIOcclusionChangeCallback = iface_cast<RSIOcclusionChangeCallback>(remoteObject);
    sptr<RSISurfaceOcclusionChangeCallback> callbackTwo = iface_cast<RSISurfaceOcclusionChangeCallback>(remoteObject);
    sptr<RSIHgmConfigChangeCallback> rsIHgmConfigChangeCallback = iface_cast<RSIHgmConfigChangeCallback>(remoteObject);
    std::vector<ScreenColorGamut> mode;
    std::vector<ScreenHDRMetadataKey> keys;
    std::vector<ScreenHDRFormat> hdrFormats;
    std::vector<GraphicCM_ColorSpaceType> colorSpaces;
    std::vector<float> partitionPoints;
    std::vector<std::string> packageList;
    callback = iface_cast<RSIScreenChangeCallback>(remoteObject);
    std::shared_ptr<Media::PixelMap> pixelmap = std::make_shared<Media::PixelMap>();
    std::shared_ptr<Drawing::DrawCmdList> drawCmdList = std::make_shared<Drawing::DrawCmdList>();
    Drawing::Rect rect;
    RSRenderServiceConnectionProxy rsRenderServiceConnectionProxy(remoteObject);
    rsRenderServiceConnectionProxy.GetMemoryGraphic(pid);
    rsRenderServiceConnectionProxy.CreateNode(config);
    rsRenderServiceConnectionProxy.CreateNodeAndSurface(config);
    rsRenderServiceConnectionProxy.SetFocusAppInfo(pid1, uid, name, name, id1);
    rsRenderServiceConnectionProxy.CreateVirtualScreen(name, width, height, surface);
    rsRenderServiceConnectionProxy.SetVirtualScreenSurface(id1, surface);
    rsRenderServiceConnectionProxy.SyncFrameRateRange(id1, range, true);
    rsRenderServiceConnectionProxy.GetScreenCurrentRefreshRate(id1);
    rsRenderServiceConnectionProxy.GetScreenSupportedRefreshRates(id1);
    rsRenderServiceConnectionProxy.RegisterApplicationAgent(width, app);
    rsRenderServiceConnectionProxy.GetScreenSupportedColorGamuts(id1, mode);
    rsRenderServiceConnectionProxy.GetScreenSupportedMetaDataKeys(id1, keys);
    rsRenderServiceConnectionProxy.GetScreenHDRCapability(id1, screenHdrCapability);
    rsRenderServiceConnectionProxy.GetScreenSupportedHDRFormats(id1, hdrFormats);
    rsRenderServiceConnectionProxy.GetScreenSupportedColorSpaces(id1, colorSpaces);
    rsRenderServiceConnectionProxy.GetScreenColorSpace(id1, colorSpace);
    rsRenderServiceConnectionProxy.SetScreenColorSpace(id1, colorSpace);
    rsRenderServiceConnectionProxy.GetScreenType(id1, screenType);
    rsRenderServiceConnectionProxy.GetPixelmap(id1, pixelmap, &rect, drawCmdList);
    rsRenderServiceConnectionProxy.RegisterOcclusionChangeCallback(rsIOcclusionChangeCallback);
    rsRenderServiceConnectionProxy.RegisterHgmConfigChangeCallback(rsIHgmConfigChangeCallback);
    rsRenderServiceConnectionProxy.RegisterHgmRefreshRateModeChangeCallback(rsIHgmConfigChangeCallback);
    rsRenderServiceConnectionProxy.RegisterHgmRefreshRateUpdateCallback(rsIHgmConfigChangeCallback);
    rsRenderServiceConnectionProxy.NotifyPackageEvent(width, packageList);
    return true;
}
bool DoCommitTransaction(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::unique_ptr<RSTransactionData> transactionData;
    std::shared_ptr<MessageParcel> parcel = std::make_shared<MessageParcel>();
    OnRemoteDiedCallback onRemoteDiedCallback = []() {};
    rsRenderServiceConnectionProxy.CommitTransaction(transactionData);
    transactionData = std::make_unique<RSTransactionData>();
    std::unique_ptr<RSCommand> command = std::make_unique<RSAnimationCallback>(g_one, g_one, FINISHED);
    NodeId nodeId = g_one;
    FollowType followType = FollowType::FOLLOW_TO_PARENT;
    transactionData->AddCommand(command, nodeId, followType);
    rsRenderServiceConnectionProxy.CommitTransaction(transactionData);
    rsRenderServiceConnectionProxy.FillParcelWithTransactionData(transactionData, parcel);
    rsRenderServiceConnectionProxy.NotifyRefreshRateEvent(eventInfo);
    rsRenderServiceConnectionProxy.NotifyTouchEvent(pid1, "", 0, uid);
    rsRenderServiceConnectionProxy.ReportEventResponse(info);
    rsRenderServiceConnectionProxy.ReportEventComplete(info);
    rsRenderServiceConnectionProxy.ReportEventJankFrame(info);
    rsRenderServiceConnectionProxy.ReportGameStateData(gameStateDataInfo);
    rsRenderServiceConnectionProxy.SetHardwareEnabled(id1, true, SelfDrawingNodeType::DEFAULT);
    rsRenderServiceConnectionProxy.SetCacheEnabledForRotation(true);
    rsRenderServiceConnectionProxy.SetOnRemoteDiedCallback(onRemoteDiedCallback);
    return true;
}
bool DoExecuteSynchronousTask(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSSyncTask> task;
    rsRenderServiceConnectionProxy.ExecuteSynchronousTask(task);
    NodeId targetId;
    std::shared_ptr<RSRenderPropertyBase> property = std::make_shared<RSRenderPropertyBase>();
    task = std::make_shared<RSNodeGetShowingPropertyAndCancelAnimation>(targetId, property);
    rsRenderServiceConnectionProxy.ExecuteSynchronousTask(task);

    return true;
}
bool DoCreateVSyncConnection(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    sptr<VSyncIConnectionToken> token;
    std::string name("name");
    uint64_t id = GetData<uint64_t>();
    uint64_t windowNodeId = GetData<uint64_t>();
    std::shared_ptr<Media::PixelMap> watermarkImg = std::make_shared<Media::PixelMap>();
    rsRenderServiceConnectionProxy.CreateVSyncConnection(name, token, id, windowNodeId);
    token = new IRemoteStub<VSyncIConnectionToken>();
    rsRenderServiceConnectionProxy.CreateVSyncConnection(name, token, id, windowNodeId);
    rsRenderServiceConnectionProxy.ShowWatermark(watermarkImg, true);

    return true;
}
bool DoCreatePixelMapFromSurface(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    sptr<Surface> surface;
    Rect srcRect;
    rsRenderServiceConnectionProxy.CreatePixelMapFromSurface(surface, srcRect);
    sptr<IConsumerSurface> consumer = IConsumerSurface::Create("DisplayNode");
    sptr<IBufferProducer> producer = consumer->GetProducer();
    surface = Surface::CreateSurfaceAsProducer(producer);
    rsRenderServiceConnectionProxy.CreatePixelMapFromSurface(surface, srcRect);

    return true;
}
bool DoSet2DRenderCtrl(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

#ifdef RS_ENABLE_VK
    rsRenderServiceConnectionProxy.Set2DRenderCtrl(false);
    ASSERT_TRUE(true);
    rsRenderServiceConnectionProxy.Set2DRenderCtrl(true);
    ASSERT_TRUE(true);
#endif
    return true;
}
bool DoSetScreenChangeCallback(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    sptr<RSIScreenChangeCallback> callback;
    rsRenderServiceConnectionProxy.SetScreenChangeCallback(callback);
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    callback = iface_cast<RSIScreenChangeCallback>(remoteObject);
    rsRenderServiceConnectionProxy.SetScreenChangeCallback(callback);

    return true;
}
bool DoRegisterApplicationAgent(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t pid = GetData<uint32_t>();
    sptr<IApplicationAgent> app;
    rsRenderServiceConnectionProxy.RegisterApplicationAgent(pid, app);
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    app = iface_cast<IApplicationAgent>(remoteObject);
    rsRenderServiceConnectionProxy.RegisterApplicationAgent(pid, app);
    return true;
}
bool DoTakeSurfaceCapture(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    NodeId id = GetData<uint64_t>();
    sptr<RSISurfaceCaptureCallback> callback;
    float scaleX = GetData<float>();
    float scaleY = GetData<float>();
    bool useDma = false;
    SurfaceCaptureType surfaceCaptureType = SurfaceCaptureType::UICAPTURE;
    bool isSync = true;
    rsRenderServiceConnectionProxy.TakeSurfaceCapture(id, callback, scaleX, 
    scaleY, useDma, surfaceCaptureType, isSync);

    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    callback = iface_cast<RSISurfaceCaptureCallback>(remoteObject);
    rsRenderServiceConnectionProxy.TakeSurfaceCapture(id, callback, scaleX, 
    scaleY, useDma, surfaceCaptureType, isSync);
    return true;
}
bool DoRegisterBufferClearListener(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    NodeId id = GetData<uint64_t>();
    sptr<RSIBufferClearCallback> callback;
    rsRenderServiceConnectionProxy.RegisterBufferClearListener(id, callback);
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    callback = iface_cast<RSIBufferClearCallback>(remoteObject);
    rsRenderServiceConnectionProxy.RegisterBufferClearListener(id, callback);
    return true;
}
bool DoRegisterBufferAvailableListener(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    NodeId id = GetData<uint64_t>();
    sptr<RSIBufferAvailableCallback> callback;
    bool isFromRenderThread = true;
    rsRenderServiceConnectionProxy.RegisterBufferAvailableListener(id, callback, isFromRenderThread);
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    callback = iface_cast<RSIBufferAvailableCallback>(remoteObject);
    rsRenderServiceConnectionProxy.RegisterBufferAvailableListener(id, callback, isFromRenderThread);
    return true;
}
bool DoGetScreenGamutMap(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    ScreenId id = GetData<uint64_t>();
    ScreenGamutMap mode = GAMUT_MAP_EXTENSION;
    rsRenderServiceConnectionProxy.SetScreenGamutMap(id, mode);
    rsRenderServiceConnectionProxy.SetScreenCorrection(id, ScreenRotation::ROTATION_90);
    rsRenderServiceConnectionProxy.GetScreenGamutMap(id, mode);
    return true;
}
bool DoRegisterSurfaceOcclusionChangeCallback(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    sptr<RSIOcclusionChangeCallback> callback = iface_cast<RSIOcclusionChangeCallback>(remoteObject);
    rsRenderServiceConnectionProxy.RegisterOcclusionChangeCallback(callback);
    NodeId id = GetData<uint64_t>();
    rsRenderServiceConnectionProxy.UnRegisterSurfaceOcclusionChangeCallback(id);
    sptr<RSISurfaceOcclusionChangeCallback> callbackTwo = iface_cast<RSISurfaceOcclusionChangeCallback>(remoteObject);
    std::vector<float> partitionPoints;
    rsRenderServiceConnectionProxy.RegisterSurfaceOcclusionChangeCallback(id, callbackTwo, partitionPoints);
    return true;
}
bool DoGetMemoryGraphics(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    rsRenderServiceConnectionProxy.GetMemoryGraphics();
    rsRenderServiceConnectionProxy.GetDefaultScreenId();
    rsRenderServiceConnectionProxy.GetActiveScreenId();
    rsRenderServiceConnectionProxy.GetAllScreenIds();
    rsRenderServiceConnectionProxy.GetUniRenderEnabled();
    rsRenderServiceConnectionProxy.GetCurrentRefreshRateMode();
    rsRenderServiceConnectionProxy.GetShowRefreshRateEnabled();
    rsRenderServiceConnectionProxy.ReportJankStats();
    rsRenderServiceConnectionProxy.RunOnRemoteDiedCallback();
    rsRenderServiceConnectionProxy.GetActiveDirtyRegionInfo();
    rsRenderServiceConnectionProxy.GetGlobalDirtyRegionInfo();
    rsRenderServiceConnectionProxy.GetLayerComposeInfo();
    rsRenderServiceConnectionProxy.SetShowRefreshRateEnabled(true);
    rsRenderServiceConnectionProxy.NotifyLightFactorStatus(true);
    rsRenderServiceConnectionProxy.SetVirtualScreenUsingStatus(true);
    rsRenderServiceConnectionProxy.SetCurtainScreenUsingStatus(true);
    rsRenderServiceConnectionProxy.SetCacheEnabledForRotation(true);
    return true;
}
bool DoRemoveVirtualScreen(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint64_t id1 = GetData<uint64_t>();
    uint32_t width = GetData<uint32_t>();
    int32_t pid1 = GetData<int32_t>();
    Drawing::Bitmap bitmap;
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeDefault();
    DataBaseRs info;
    EventInfo eventInfo;
    rsRenderServiceConnectionProxy.SetAppWindowNum(width);
    rsRenderServiceConnectionProxy.RemoveVirtualScreen(id1);
    rsRenderServiceConnectionProxy.SetScreenActiveMode(id1, width);
    rsRenderServiceConnectionProxy.GetVirtualScreenResolution(id1);
    rsRenderServiceConnectionProxy.GetScreenActiveMode(id1);
    rsRenderServiceConnectionProxy.GetScreenSupportedModes(id1);
    rsRenderServiceConnectionProxy.GetScreenCapability(id1);
    rsRenderServiceConnectionProxy.GetScreenPowerStatus(id1);
    rsRenderServiceConnectionProxy.GetScreenData(id1);
    rsRenderServiceConnectionProxy.GetScreenBacklight(id1);
    rsRenderServiceConnectionProxy.SetScreenBacklight(id1, width);
    rsRenderServiceConnectionProxy.UnRegisterTypeface(id1);
    rsRenderServiceConnectionProxy.SetScreenSkipFrameInterval(id1, width);
    rsRenderServiceConnectionProxy.SetVirtualScreenResolution(id1, width, width);
    rsRenderServiceConnectionProxy.ResizeVirtualScreen(id1, width, width);
    rsRenderServiceConnectionProxy.ReportEventResponse(info);
    rsRenderServiceConnectionProxy.ReportEventComplete(info);
    rsRenderServiceConnectionProxy.ReportEventJankFrame(info);
    rsRenderServiceConnectionProxy.NotifyRefreshRateEvent(eventInfo);
    rsRenderServiceConnectionProxy.SetRefreshRateMode(pid1);
    rsRenderServiceConnectionProxy.SetScreenHDRFormat(id1, pid1);
    rsRenderServiceConnectionProxy.NotifyTouchEvent(pid1, pid1);
    rsRenderServiceConnectionProxy.SetScreenColorGamut(id1, pid1);
    rsRenderServiceConnectionProxy.SetScreenRefreshRate(id1, pid1, pid1);
    rsRenderServiceConnectionProxy.RegisterTypeface(id1, typeface);
    rsRenderServiceConnectionProxy.GetBitmap(id1, bitmap);
    return true;
}
bool DoSetScreenPowerStatus(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint64_t id1 = GetData<uint64_t>();
    uint32_t width = GetData<uint32_t>();
    ScreenPowerStatus status = (ScreenPowerStatus)width;
    ScreenColorGamut screenColorGamut = (ScreenColorGamut)width;
    ScreenGamutMap screenGamutMap = (ScreenGamutMap)width;
    ScreenHDRFormat screenHDRFormat = (ScreenHDRFormat)width;
    SystemAnimatedScenes systemAnimatedScenes = (SystemAnimatedScenes)width;
    GraphicPixelFormat pixelFormat = GRAPHIC_PIXEL_FMT_BGRA_8888;
    GameStateData gameStateDataInfo;
    MessageOption option;
    MessageParcel reply;
    MessageParcel messageParcel;
    DataBaseRs info;
    float cpuMemSize = GetData<float>();
    float gpuMemSize = GetData<float>();

    rsRenderServiceConnectionProxy.SetScreenPowerStatus(id1, status);
    rsRenderServiceConnectionProxy.GetScreenColorGamut(id1, screenColorGamut);
    rsRenderServiceConnectionProxy.SetScreenGamutMap(id1, screenGamutMap);
    rsRenderServiceConnectionProxy.GetScreenHDRFormat(id1, screenHDRFormat);
    rsRenderServiceConnectionProxy.SetSystemAnimatedScenes(systemAnimatedScenes);
    rsRenderServiceConnectionProxy.GetPixelFormat(id1, pixelFormat);
    rsRenderServiceConnectionProxy.SetPixelFormat(id1, pixelFormat);
    rsRenderServiceConnectionProxy.SetHardwareEnabled(id1, true, SelfDrawingNodeType::DEFAULT);
    rsRenderServiceConnectionProxy.ReportGameStateData(gameStateDataInfo);
    rsRenderServiceConnectionProxy.ReportGameStateDataRs(messageParcel, reply, option, gameStateDataInfo);
    rsRenderServiceConnectionProxy.ReportDataBaseRs(messageParcel, reply, option, info);
    rsRenderServiceConnectionProxy.GetTotalAppMemSize(cpuMemSize, gpuMemSize);

    return true;
}

#ifdef TP_FEATURE_ENABLE
bool OHOS::Rosen::DoSetTpFeatureConfigFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // get data
    int32_t tpFeature = GetData<int32_t>();
    std::string tpConfig = GetData<std::string>();

    // test
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    RSRenderServiceConnectionProxy rsRenderServiceConnectionProxy(remoteObject);
    RSRenderServiceConnectionProxy.SetTpFeatureConfig(tpFeature, tpConfig);
    return true;
}
#endif
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoSomethingInterestingWithMyAPI(data, size);
    OHOS::Rosen::DoCommitTransaction(data, size);
    OHOS::Rosen::DoExecuteSynchronousTask(data, size);
    OHOS::Rosen::DoCreateVSyncConnection(data, size);
    OHOS::Rosen::DoCreatePixelMapFromSurface(data, size);
    OHOS::Rosen::DoSet2DRenderCtrl(data, size);
    OHOS::Rosen::DoSetScreenChangeCallback(data, size);
    OHOS::Rosen::DoRegisterApplicationAgent(data, size);
    OHOS::Rosen::DoTakeSurfaceCapture(data, size);
    OHOS::Rosen::DoRegisterBufferClearListener(data, size);
    OHOS::Rosen::DoRegisterBufferAvailableListener(data, size);
    OHOS::Rosen::DoGetScreenGamutMap(data, size);
    OHOS::Rosen::DoRegisterSurfaceOcclusionChangeCallback(data, size);
    OHOS::Rosen::DoGetMemoryGraphics(data, size);
    OHOS::Rosen::DoRemoveVirtualScreen(data, size);
    OHOS::Rosen::DoSetScreenPowerStatus(data, size);
#ifdef TP_FEATURE_ENABLE
    OHOS::Rosen::DoSetTpFeatureConfigFuzzTest(data, size);
#endif
    return 0;
}
