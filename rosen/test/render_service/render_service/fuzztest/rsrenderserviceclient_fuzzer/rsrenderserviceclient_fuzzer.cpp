/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "rsrenderserviceclient_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>

#include "message_parcel.h"
#include "securec.h"

#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_render_service_connection.h"
#include "platform/ohos/rs_irender_service.h"
#include "transaction/rs_render_service_client.h"

namespace OHOS {
namespace Rosen {
constexpr size_t MAX_SIZE = 4;
static inline std::shared_ptr<RSRenderServiceClient> rsClient = nullptr;

namespace {
const uint8_t* data_ = nullptr;
size_t size_ = 0;
size_t pos;
} // namespace

/*
 * describe: get data from outside untrusted data(data_) which size is according to sizeof(T)
 * tips: only support basic type
 */
template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (data_ == nullptr || objectSize > size_ - pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, data_ + pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    pos += objectSize;
    return object;
}

bool DoCommitTransaction(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    data_ = data;
    size_ = size;
    pos = 0;

    MessageParcel datas;
    datas.WriteBuffer(data, size);
    rsClient = std::make_shared<RSRenderServiceClient>();
    auto transactionData = RSBaseRenderUtil::ParseTransactionData(datas);
    rsClient->CommitTransaction(transactionData);
    return true;
}

bool DoExecuteSynchronousTask(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    data_ = data;
    size_ = size;
    pos = 0;

    MessageParcel datas;
    datas.WriteBuffer(data, size);
    auto type = datas.ReadInt16();
    auto subType = datas.ReadInt16();
    auto func = RSCommandFactory::Instance().GetUnmarshallingFunc(type, subType);
    auto command = static_cast<RSSyncTask*>((*func)(data));
    std::shared_ptr<RSSyncTask> task(command);
    rsClient->ExecuteSynchronousTask(task);
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

    // initialize
    data_ = data;
    size_ = size;
    pos = 0;

    MessageParcel datas;
    datas.WriteBuffer(data, size);
    auto nodeId = datas.ReadUint64();
    auto surfaceName = datas.ReadString();
    auto bundleName = datas.ReadString();
    RSSurfaceRenderNodeConfig config = { .id = nodeId, .name = surfaceName, .bundleName = bundleName };
    rsClient->CreateNode(config);
    rsClient->CreateNodeAndSurface(config);
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

    // initialize
    data_ = data;
    size_ = size;
    pos = 0;

    MessageParcel datas;
    datas.WriteBuffer(data, size);
    int32_t pid = datas.ReadInt32();
    int32_t uid = datas.ReadInt32();
    std::string bundleName = datas.ReadString();
    std::string abilityName = datas.ReadString();
    uint64_t focusNodeId = datas.ReadUint64();
    rsClient->SetFocusAppInfo(pid, uid, bundleName, abilityName, focusNodeId);
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

    // initialize
    data_ = data;
    size_ = size;
    pos = 0;

    MessageParcel datas;
    datas.WriteBuffer(data, size);
    std::string name = datas.ReadString();
    uint32_t width = datas.ReadUint32();
    uint32_t height = datas.ReadUint32();
    sptr<Surface> surface = nullptr;
    auto remoteObject = datas.ReadRemoteObject();
    if (remoteObject != nullptr) {
        auto bufferProducer = iface_cast<IBufferProducer>(remoteObject);
        surface = Surface::CreateSurfaceAsProducer(bufferProducer);
    }

    ScreenId mirrorId = datas.ReadUint64();
    int32_t flags = datas.ReadInt32();
    rsClient->CreateVirtualScreen(name, width, height, surface, mirrorId, flags);
    ScreenId id = datas.ReadUint64();
    rsClient->SetVirtualScreenSurface(id, surface);
    rsClient->RemoveVirtualScreen(id);
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

    // initialize
    data_ = data;
    size_ = size;
    pos = 0;

    MessageParcel datas;
    datas.WriteBuffer(data, size);
    auto remoteObject = datas.ReadRemoteObject();
    if (remoteObject == nullptr) {
        ret = ERR_NULL_OBJECT;
        break;
    }
    sptr<RSIScreenChangeCallback> cb = iface_cast<RSIScreenChangeCallback>(remoteObject);
    if (cb == nullptr) {
        ret = ERR_NULL_OBJECT;
        break;
    }
    rsClient->SetScreenChangeCallback(cb);
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

    // initialize
    data_ = data;
    size_ = size;
    pos = 0;

    MessageParcel datas;
    datas.WriteBuffer(data, size);
    ScreenId id = datas.ReadUint64();
    uint32_t modeId = datas.ReadUint32();
    rsClient->SetScreenActiveMode(id, modeId);
    rsClient->GetScreenActiveMode(id);
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

    // initialize
    data_ = data;
    size_ = size;
    pos = 0;

    MessageParcel datas;
    datas.WriteBuffer(data, size);
    ScreenId id = datas.ReadUint64();
    int32_t sceneId = datas.ReadInt32();
    int32_t rate = datas.ReadInt32();
    rsClient->SetScreenRefreshRate(id, sceneId, rate);
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

    // initialize
    data_ = data;
    size_ = size;
    pos = 0;

    MessageParcel datas;
    datas.WriteBuffer(data, size);
    ScreenId id = datas.ReadUint64();
    int32_t sceneId = datas.ReadInt32();
    int32_t rate = datas.ReadInt32();
    rsClient->SetScreenRefreshRate(id, sceneId, rate);
    rsClient->GetScreenCurrentRefreshRate(id);
    rsClient->GetScreenSupportedRefreshRates(id);
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

    // initialize
    data_ = data;
    size_ = size;
    pos = 0;

    MessageParcel datas;
    datas.WriteBuffer(data, size);
    int32_t mode = datas.ReadInt32();
    rsClient->SetRefreshRateMode(mode);
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

    // initialize
    data_ = data;
    size_ = size;
    pos = 0;

    MessageParcel datas;
    datas.WriteBuffer(data, size);
    ScreenId id = datas.ReadUint64();
    uint32_t width = datas.ReadUint32();
    uint32_t height = datas.ReadUint32();
    rsClient->SetVirtualScreenResolution(id, width, height);
    rsClient->GetVirtualScreenResolution(id);
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

    // initialize
    data_ = data;
    size_ = size;
    pos = 0;

    MessageParcel datas;
    datas.WriteBuffer(data, size);
    ScreenId id = datas.ReadUint64();
    uint32_t status = datas.ReadUint32();
    rsClient->SetScreenPowerStatus(id, static_cast<ScreenPowerStatus>(status));
    rsClient->GetScreenPowerStatus(id);
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

    // initialize
    data_ = data;
    size_ = size;
    pos = 0;

    MessageParcel datas;
    datas.WriteBuffer(data, size);
    auto remoteObject = data.ReadRemoteObject();
    if (remoteObject == nullptr) {
        ret = ERR_NULL_OBJECT;
        break;
    }
    sptr<RSISurfaceCaptureCallback> cb = iface_cast<RSISurfaceCaptureCallback>(remoteObject);
    if (cb == nullptr) {
        ret = ERR_NULL_OBJECT;
        break;
    }
    float scaleX = datas.ReadFloat();
    float scaleY = datas.ReadFloat();
    SurfaceCaptureType surfaceCaptureType = static_cast<SurfaceCaptureType>(data.ReadUint8());
    rsClient->TakeSurfaceCapture(id, cb, scaleX, scaleY, surfaceCaptureType);
    return true;
}

bool DoRegisterApplicationAgent(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    data_ = data;
    size_ = size;
    pos = 0;

    MessageParcel datas;
    datas.WriteBuffer(data, size);
    uint32_t pid = datas.ReadUint32();
    auto remoteObject = datas.ReadRemoteObject();
    if (remoteObject == nullptr) {
        ret = ERR_NULL_OBJECT;
        break;
    }
    sptr<IApplicationAgent> app = iface_cast<IApplicationAgent>(remoteObject);
    if (app == nullptr) {
        ret = ERR_NULL_OBJECT;
        break;
    }
    rsClient->RegisterApplicationAgent(pid, app);
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

    // initialize
    data_ = data;
    size_ = size;
    pos = 0;

    MessageParcel datas;
    datas.WriteBuffer(data, size);
    ScreenId id = datas.ReadUint64();
    rsClient->GetScreenSupportedModes(id);
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

    // initialize
    data_ = data;
    size_ = size;
    pos = 0;

    MessageParcel datas;
    datas.WriteBuffer(data, size);
    auto pid = datas.ReadInt32();
    rsClient->GetMemoryGraphic(pid);
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

    // initialize
    data_ = data;
    size_ = size;
    pos = 0;

    MessageParcel datas;
    datas.WriteBuffer(data, size);
    ScreenId id = datas.ReadUint64();
    rsClient->GetScreenCapability(id);
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

    // initialize
    data_ = data;
    size_ = size;
    pos = 0;

    MessageParcel datas;
    datas.WriteBuffer(data, size);
    ScreenId id = datas.ReadUint64();
    rsClient->GetScreenData(id);
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
    data_ = data;
    size_ = size;
    pos = 0;

    MessageParcel datas;
    datas.WriteBuffer(data, size);
    ScreenId id = datas.ReadUint64();
    uint32_t level = datas.ReadUint32();
    rsClient->SetScreenBacklight(id, level);
    rsClient->GetScreenBacklight(id);
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

    // initialize
    data_ = data;
    size_ = size;
    pos = 0;

    MessageParcel datas;
    datas.WriteBuffer(data, size);
    NodeId id = datas.ReadUint64();
    auto remoteObject = datas.ReadRemoteObject();
    bool isFromRenderThread = datas.ReadBool();
    if (remoteObject == nullptr) {
        ret = ERR_NULL_OBJECT;
        break;
    }
    sptr<RSIBufferAvailableCallback> cb = iface_cast<RSIBufferAvailableCallback>(remoteObject);
    if (cb == nullptr) {
        ret = ERR_NULL_OBJECT;
        break;
    }
    rsClient->RegisterBufferAvailableListener(id, cb, isFromRenderThread);
    rsClient->RegisterBufferClearListener(id, cb);
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

    // initialize
    data_ = data;
    size_ = size;
    pos = 0;

    MessageParcel datas;
    datas.WriteBuffer(data, size);
    ScreenId id = datas.ReadUint64();
    std::vector<ScreenColorGamut> mode;
    rsClient->GetScreenSupportedColorGamuts(id, mode);

    std::vector<ScreenHDRMetadataKey> keys;
    rsClient->GetScreenSupportedMetaDataKeys(id, keys);
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

    // initialize
    data_ = data;
    size_ = size;
    pos = 0;

    MessageParcel datas;
    datas.WriteBuffer(data, size);
    ScreenId id = datas.ReadUint64();
    int32_t modeIdx = datas.ReadInt32();
    rsClient->SetScreenColorGamut(id, modeIdx);
    ScreenColorGamut mode;
    rsClient->GetScreenColorGamut(id, mode);
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

    // initialize
    data_ = data;
    size_ = size;
    pos = 0;

    MessageParcel datas;
    datas.WriteBuffer(data, size);
    ScreenId id = datas.ReadUint64();
    ScreenGamutMap mode = static_cast<ScreenGamutMap>(data.ReadInt32());
    rsClient->SetScreenGamutMap(id, mode);
    rsClient->GetScreenGamutMap(id, mode);
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

    // initialize
    data_ = data;
    size_ = size;
    pos = 0;

    MessageParcel datas;
    datas.WriteBuffer(data, size);
    std::string name = datas.ReadString();
    auto remoteObj = datas.ReadRemoteObject();
    if (remoteObj == nullptr) {
        ret = ERR_NULL_OBJECT;
        break;
    }
    if (!remoteObj->IsProxyObject()) {
        ret = ERR_UNKNOWN_OBJECT;
        break;
    }
    auto token = iface_cast<VSyncIConnectionToken>(remoteObj);
    if (token == nullptr) {
        ret = ERR_UNKNOWN_OBJECT;
        break;
    }
    rsClient->CreateVSyncConnection(name, token);
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

    // initialize
    data_ = data;
    size_ = size;
    pos = 0;

    MessageParcel datas;
    datas.WriteBuffer(data, size);
    ScreenId id = datas.ReadUint64();
    RSScreenHDRCapability screenHDRCapability;
    rsClient->GetScreenHDRCapability(id, screenHDRCapability);
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

    // initialize
    data_ = data;
    size_ = size;
    pos = 0;

    MessageParcel datas;
    datas.WriteBuffer(data, size);
    ScreenId id = datas.ReadUint64();
    RSScreenType type;
    rsClient->GetScreenType(id, type);
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

    // initialize
    data_ = data;
    size_ = size;
    pos = 0;

    MessageParcel datas;
    datas.WriteBuffer(data, size);
    NodeId id = datas.ReadUint64();
#ifndef USE_ROSEN_DRAWING
    SkBitmap bm;
#else
    Drawing::Bitmap bm;
#endif
    rsClient->GetBitmap(id, bm);
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

    // initialize
    data_ = data;
    size_ = size;
    pos = 0;

    MessageParcel datas;
    datas.WriteBuffer(data, size);
    ScreenId id = datas.ReadUint64();
    uint32_t skipFrameInterval = datas.ReadUint32();
    rsClient->SetScreenSkipFrameInterval(id, skipFrameInterval);
    return true;
}

bool DoRegisterOcclusionChangeCallback(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    data_ = data;
    size_ = size;
    pos = 0;

    MessageParcel datas;
    datas.WriteBuffer(data, size);
    auto remoteObject = datas.ReadRemoteObject();
    if (remoteObject == nullptr) {
        ret = ERR_NULL_OBJECT;
        break;
    }
    sptr<RSIOcclusionChangeCallback> callback = iface_cast<RSIOcclusionChangeCallback>(remoteObject);
    if (callback == nullptr) {
        ret = ERR_NULL_OBJECT;
        break;
    }
    rsClient->RegisterOcclusionChangeCallback(callback);
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

    // initialize
    data_ = data;
    size_ = size;
    pos = 0;

    MessageParcel datas;
    datas.WriteBuffer(data, size);
    uint32_t num = datas.ReadUint32();
    rsClient->SetAppWindowNum(num);
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

    // initialize
    data_ = data;
    size_ = size;
    pos = 0;

    MessageParcel datas;
    datas.WriteBuffer(data, size);
    std::shared_ptr<Media::PixelMap> watermarkImg =
        std::shared_ptr<Media::PixelMap>(data.ReadParcelable<Media::PixelMap>());
    bool isShow = datas.ReadBool();
    rsClient->ShowWatermark(watermarkImg, isShow);
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

    // initialize
    data_ = data;
    size_ = size;
    pos = 0;

    MessageParcel datas;
    datas.WriteBuffer(data, size);
    std::shared_ptr<Media::PixelMap> watermarkImg =
        std::shared_ptr<Media::PixelMap>(data.ReadParcelable<Media::PixelMap>());
    bool isShow = datas.ReadBool();
    rsClient->ShowWatermark(watermarkImg, isShow);
    return true;
}

bool DoReportEventResponse(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    data_ = data;
    size_ = size;
    pos = 0;

    MessageParcel datas;
    datas.WriteBuffer(data, size);
    DataBaseRs info;
    rsClient->ReadDataBaseRs(info, datas);
    rsClient->ReportEventResponse(info);
    rsClient->ReportEventComplete(info);
    rsClient->ReportEventJankFrame(info);
    return true;
}

bool DoExecuteSynchronousTask(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    data_ = data;
    size_ = size;
    pos = 0;

    MessageParcel datas;
    datas.WriteBuffer(data, size);
    auto type = datas.ReadInt16();
    auto subType = datas.ReadInt16();
    if (type != RS_NODE_SYNCHRONOUS_READ_PROPERTY) {
        ret = ERR_INVALID_STATE;
        break;
    }
    auto func = RSCommandFactory::Instance().GetUnmarshallingFunc(type, subType);
    if (func == nullptr) {
        ret = ERR_INVALID_STATE;
        break;
    }
    auto command = static_cast<RSSyncTask*>((*func)(data));
    if (command == nullptr) {
        ret = ERR_INVALID_STATE;
        break;
    }
    std::shared_ptr<RSSyncTask> task(command);
    rsClient->ExecuteSynchronousTask(task);
    return true;
}

bool DoSetHardwareEnabled(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    data_ = data;
    size_ = size;
    pos = 0;

    MessageParcel datas;
    datas.WriteBuffer(data, size);
    auto id = datas.ReadUint64();
    auto isEnabled = datas.ReadBool();
    rsClient->SetHardwareEnabled(id, isEnabled);
    return true;
}

bool DoRegisterHgmConfigChangeCallback(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    data_ = data;
    size_ = size;
    pos = 0;

    MessageParcel datas;
    datas.WriteBuffer(data, size);
    auto remoteObject = datas.ReadRemoteObject();
    if (remoteObject == nullptr) {
        ret = ERR_NULL_OBJECT;
        break;
    }
    sptr<RSIHgmConfigChangeCallback> callback = iface_cast<RSIHgmConfigChangeCallback>(remoteObject);
    if (callback == nullptr) {
        ret = ERR_NULL_OBJECT;
        break;
    }
    rsClient->RegisterHgmConfigChangeCallback(callback);
    return true;
}

} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}