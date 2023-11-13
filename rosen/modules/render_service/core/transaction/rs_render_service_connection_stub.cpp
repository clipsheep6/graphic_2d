/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "rs_render_service_connection_stub.h"
#include "ivsync_connection.h"
#include "securec.h"
#include "sys_binder.h"

#include "command/rs_command_factory.h"
#include "pipeline/rs_base_render_util.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "pipeline/rs_unmarshal_thread.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_ashmem_helper.h"
#include "rs_trace.h"

#if defined (ENABLE_DDGR_OPTIMIZE)
#include "ddgr_renderer.h"
#endif

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t MAX_DATA_SIZE_FOR_UNMARSHALLING_IN_PLACE = 1024 * 30; // 30kB
constexpr size_t FILE_DESCRIPTOR_LIMIT = 15;
constexpr size_t MAX_OBJECTNUM = INT_MAX;
constexpr size_t MAX_DATA_SIZE = INT_MAX;

void CopyFileDescriptor(MessageParcel& old, MessageParcel& copied)
{
    binder_size_t* object = reinterpret_cast<binder_size_t*>(old.GetObjectOffsets());
    binder_size_t* copiedObject = reinterpret_cast<binder_size_t*>(copied.GetObjectOffsets());

    size_t objectNum = old.GetOffsetsSize();
    if (objectNum > MAX_OBJECTNUM) {
        return;
    }

    uintptr_t data = old.GetData();
    uintptr_t copiedData = copied.GetData();

    for (size_t i = 0; i < objectNum; i++) {
        const flat_binder_object* flat = reinterpret_cast<flat_binder_object*>(data + object[i]);
        flat_binder_object* copiedFlat = reinterpret_cast<flat_binder_object*>(copiedData + copiedObject[i]);

        if (flat->hdr.type == BINDER_TYPE_FD && flat->handle > 0) {
            int32_t val = dup(flat->handle);
            if (val < 0) {
                ROSEN_LOGW("CopyFileDescriptor dup failed, fd:%{public}d", val);
            }
            copiedFlat->handle = static_cast<uint32_t>(val);
        }
    }
}

std::shared_ptr<MessageParcel> CopyParcelIfNeed(MessageParcel& old)
{
    auto dataSize = old.GetDataSize();
    if (dataSize <= MAX_DATA_SIZE_FOR_UNMARSHALLING_IN_PLACE && old.GetOffsetsSize() < FILE_DESCRIPTOR_LIMIT) {
        return nullptr;
    }
    if (dataSize > MAX_DATA_SIZE) {
        return nullptr;
    }
    RS_TRACE_NAME("CopyParcelForUnmarsh: size:" + std::to_string(dataSize));
    void* base = malloc(dataSize);
    if (base == nullptr) {
        RS_LOGE("RSRenderServiceConnectionStub::CopyParcelIfNeed malloc failed");
        return nullptr;
    }
    if (memcpy_s(base, dataSize, reinterpret_cast<void*>(old.GetData()), dataSize) != 0) {
        RS_LOGE("RSRenderServiceConnectionStub::CopyParcelIfNeed copy parcel data failed");
        free(base);
        return nullptr;
    }

    auto parcelCopied = std::make_shared<MessageParcel>();
    if (!parcelCopied->ParseFrom(reinterpret_cast<uintptr_t>(base), dataSize)) {
        RS_LOGE("RSRenderServiceConnectionStub::CopyParcelIfNeed ParseFrom failed");
        free(base);
        return nullptr;
    }

    auto objectNum = old.GetOffsetsSize();
    if (objectNum != 0) {
        parcelCopied->InjectOffsets(old.GetObjectOffsets(), objectNum);
        CopyFileDescriptor(old, *parcelCopied);
    }
    if (parcelCopied->ReadInt32() != 0) {
        RS_LOGE("RSRenderServiceConnectionStub::CopyParcelIfNeed parcel data not match");
        return nullptr;
    }
    return parcelCopied;
}
}

int RSRenderServiceConnectionStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
#ifdef ENABLE_IPC_SECURITY_ACCESS_COUNTER
    if (!securityManager_.IsAccessTimesRestricted(code, securityUtils_.GetCodeAccessCounter(code))) {
        RS_LOGE("RSRenderServiceConnectionStub::OnRemoteRequest no permission to access codeID=%{public}u by "
                "pid=%{public}d with accessTimes = %{public}d.",
            code, GetCallingPid(), securityUtils_.GetCodeAccessCounter(code));
        return ERR_INVALID_STATE;
    }
#endif
    if (!securityManager_.IsInterfaceCodeAccessible(code)) {
        RS_LOGE("RSRenderServiceConnectionStub::OnRemoteRequest no permission to access codeID=%{public}u.", code);
        return ERR_INVALID_STATE;
    }
#ifdef ENABLE_IPC_SECURITY_ACCESS_COUNTER
    securityUtils_.IncreaseAccessCounter(code);
#endif
    int ret = ERR_NONE;
    switch (code) {
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::COMMIT_TRANSACTION): {
            RS_TRACE_NAME_FMT("Recv Parcel Size:%zu, fdCnt:%zu", data.GetDataSize(), data.GetOffsetsSize());
#if defined (ENABLE_DDGR_OPTIMIZE)
            DDGRRenderer::GetInstance().IntegrateSetIndex(++transDataIndex_);
#endif
            static bool isUniRender = RSUniRenderJudgement::IsUniRender();
            std::shared_ptr<MessageParcel> parsedParcel;
            if (data.ReadInt32() == 0) { // indicate normal parcel
                if (isUniRender) {
                    // in uni render mode, if parcel size over threshold,
                    // Unmarshalling task will be post to RSUnmarshalThread,
                    // copy the origin parcel to maintain the parcel lifetime
                    parsedParcel = CopyParcelIfNeed(data);
                }
                if (parsedParcel == nullptr) {
                    // no need to copy or copy failed, use original parcel
                    // execute Unmarshalling immediately
                    auto transactionData = RSBaseRenderUtil::ParseTransactionData(data);
                    CommitTransaction(transactionData);
                    break;
                }
            } else {
                // indicate ashmem parcel
                // should be parsed to normal parcel before Unmarshalling
                parsedParcel = RSAshmemHelper::ParseFromAshmemParcel(&data);
            }
            if (parsedParcel == nullptr) {
                RS_LOGE("RSRenderServiceConnectionStub::COMMIT_TRANSACTION failed: parsed parcel is nullptr");
                return ERR_INVALID_DATA;
            }
            if (isUniRender) {
                // post Unmarshalling task to RSUnmarshalThread
                RSUnmarshalThread::Instance().RecvParcel(parsedParcel);
            } else {
                // execute Unmarshalling immediately
                auto transactionData = RSBaseRenderUtil::ParseTransactionData(*parsedParcel);
                CommitTransaction(transactionData);
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_UNI_RENDER_ENABLED): {
            reply.WriteBool(GetUniRenderEnabled());
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_NODE): {
            auto nodeId = data.ReadUint64();
            auto surfaceName = data.ReadString();
            auto bundleName = data.ReadString();
            RSSurfaceRenderNodeConfig config = {.id = nodeId, .name = surfaceName, .bundleName = bundleName};
            reply.WriteBool(CreateNode(config));
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_NODE_AND_SURFACE): {
            auto nodeId = data.ReadUint64();
            auto surfaceName = data.ReadString();
            auto type = static_cast<RSSurfaceNodeType>(data.ReadUint8());
            auto bundleName = data.ReadString();
            RSSurfaceRenderNodeConfig config = {
                .id = nodeId, .name = surfaceName, .bundleName = bundleName, .nodeType = type};
            sptr<Surface> surface = CreateNodeAndSurface(config);
            if (surface == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            auto producer = surface->GetProducer();
            reply.WriteRemoteObject(producer->AsObject());
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_FOCUS_APP_INFO): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }

            int32_t pid = data.ReadInt32();
            int32_t uid = data.ReadInt32();
            std::string bundleName = data.ReadString();
            std::string abilityName = data.ReadString();
            uint64_t focusNodeId = data.ReadUint64();
            int32_t status = SetFocusAppInfo(pid, uid, bundleName, abilityName, focusNodeId);
            reply.WriteInt32(status);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_DEFAULT_SCREEN_ID): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = GetDefaultScreenId();
            reply.WriteUint64(id);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_ACTIVE_SCREEN_ID): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = GetActiveScreenId();
            reply.WriteUint64(id);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_ALL_SCREEN_IDS): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            std::vector<ScreenId> ids = GetAllScreenIds();
            reply.WriteUint32(ids.size());
            for (uint32_t i = 0; i < ids.size(); i++) {
                reply.WriteUint64(ids[i]);
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_VIRTUAL_SCREEN): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }

            // read the parcel data.
            std::string name = data.ReadString();
            uint32_t width = data.ReadUint32();
            uint32_t height = data.ReadUint32();
            sptr<Surface> surface = nullptr;
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject != nullptr) {
                auto bufferProducer = iface_cast<IBufferProducer>(remoteObject);
                surface = Surface::CreateSurfaceAsProducer(bufferProducer);
            }

            ScreenId mirrorId = data.ReadUint64();
            int32_t flags = data.ReadInt32();
            ScreenId id = CreateVirtualScreen(name, width, height, surface, mirrorId, flags);
            reply.WriteUint64(id);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_SURFACE): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }

            // read the parcel data.
            ScreenId id = data.ReadUint64();
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            auto bufferProducer = iface_cast<IBufferProducer>(remoteObject);
            sptr<Surface> surface = Surface::CreateSurfaceAsProducer(bufferProducer);
            if (surface == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            int32_t status = SetVirtualScreenSurface(id, surface);
            reply.WriteInt32(status);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REMOVE_VIRTUAL_SCREEN): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            RemoveVirtualScreen(id);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_CHANGE_CALLBACK): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }

            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            sptr<RSIScreenChangeCallback> cb = iface_cast<RSIScreenChangeCallback>(remoteObject);
            if (cb == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            int32_t status = SetScreenChangeCallback(cb);
            reply.WriteInt32(status);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_ACTIVE_MODE): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            uint32_t modeId = data.ReadUint32();
            SetScreenActiveMode(id, modeId);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_REFRESH_RATE): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            int32_t sceneId = data.ReadInt32();
            int32_t rate = data.ReadInt32();
            SetScreenRefreshRate(id, sceneId, rate);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_REFRESH_RATE_MODE): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            int32_t mode = data.ReadInt32();
            SetRefreshRateMode(mode);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SYNC_FRAME_RATE_RANGE): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            uint32_t min = data.ReadUint32();
            uint32_t max = data.ReadUint32();
            uint32_t preferred = data.ReadUint32();
            SyncFrameRateRange({min, max, preferred});
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_CURRENT_REFRESH_RATE): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            uint32_t refreshRate = GetScreenCurrentRefreshRate(id);
            reply.WriteUint32(refreshRate);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_CURRENT_REFRESH_RATE_MODE): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            int32_t refreshRateMode = GetCurrentRefreshRateMode();
            reply.WriteInt32(refreshRateMode);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_REFRESH_RATES): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            std::vector<int32_t> rates = GetScreenSupportedRefreshRates(id);
            reply.WriteUint64(static_cast<uint64_t>(rates.size()));
            for (auto ratesIter : rates) {
                reply.WriteInt32(ratesIter);
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_RESOLUTION): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            uint32_t width = data.ReadUint32();
            uint32_t height = data.ReadUint32();
            int32_t status = SetVirtualScreenResolution(id, width, height);
            reply.WriteInt32(status);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_POWER_STATUS): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            uint32_t status = data.ReadUint32();
            SetScreenPowerStatus(id, static_cast<ScreenPowerStatus>(status));
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::TAKE_SURFACE_CAPTURE): {
            NodeId id = data.ReadUint64();
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
            float scaleX = data.ReadFloat();
            float scaleY = data.ReadFloat();
            SurfaceCaptureType surfaceCaptureType = static_cast<SurfaceCaptureType>(data.ReadUint8());
            TakeSurfaceCapture(id, cb, scaleX, scaleY, surfaceCaptureType);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_APPLICATION_AGENT): {
            uint32_t pid = data.ReadUint32();
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            sptr<IApplicationAgent> app = iface_cast<IApplicationAgent>(remoteObject);
            if (app == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            RegisterApplicationAgent(pid, app);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_VIRTUAL_SCREEN_RESOLUTION): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            RSVirtualScreenResolution virtualScreenResolution = GetVirtualScreenResolution(id);
            reply.WriteParcelable(&virtualScreenResolution);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_ACTIVE_MODE): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            RSScreenModeInfo screenModeInfo = GetScreenActiveMode(id);
            reply.WriteParcelable(&screenModeInfo);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_MODES): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            std::vector<RSScreenModeInfo> screenSupportedModes = GetScreenSupportedModes(id);
            reply.WriteUint64(static_cast<uint64_t>(screenSupportedModes.size()));
            for (uint32_t modeIndex = 0; modeIndex < screenSupportedModes.size(); modeIndex++) {
                reply.WriteParcelable(&screenSupportedModes[modeIndex]);
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_MEMORY_GRAPHIC): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            auto pid = data.ReadInt32();
            MemoryGraphic memoryGraphic = GetMemoryGraphic(pid);
            reply.WriteParcelable(&memoryGraphic);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_MEMORY_GRAPHICS): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            std::vector<MemoryGraphic> memoryGraphics = GetMemoryGraphics();
            reply.WriteUint64(static_cast<uint64_t>(memoryGraphics.size()));
            for (uint32_t index = 0; index < memoryGraphics.size(); index++) {
                reply.WriteParcelable(&memoryGraphics[index]);
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_TOTAL_APP_MEM_SIZE): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            float cpuMemSize = 0.f;
            float gpuMemSize = 0.f;
            GetTotalAppMemSize(cpuMemSize, gpuMemSize);
            reply.WriteFloat(cpuMemSize);
            reply.WriteFloat(gpuMemSize);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_CAPABILITY): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            RSScreenCapability screenCapability = GetScreenCapability(id);
            reply.WriteParcelable(&screenCapability);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_POWER_STATUS): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            ScreenPowerStatus status = GetScreenPowerStatus(id);
            reply.WriteUint32(static_cast<uint32_t>(status));
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_DATA): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            RSScreenData screenData = GetScreenData(id);
            reply.WriteParcelable(&screenData);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_BACK_LIGHT): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            int32_t level = GetScreenBacklight(id);
            reply.WriteInt32(level);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_BACK_LIGHT): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            uint32_t level = data.ReadUint32();
            SetScreenBacklight(id, level);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_BUFFER_AVAILABLE_LISTENER): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            NodeId id = data.ReadUint64();
            auto remoteObject = data.ReadRemoteObject();
            bool isFromRenderThread = data.ReadBool();
            if (remoteObject == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            sptr<RSIBufferAvailableCallback> cb = iface_cast<RSIBufferAvailableCallback>(remoteObject);
            if (cb == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            RegisterBufferAvailableListener(id, cb, isFromRenderThread);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_BUFFER_CLEAR_LISTENER): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            NodeId id = data.ReadUint64();
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            sptr<RSIBufferClearCallback> cb = iface_cast<RSIBufferClearCallback>(remoteObject);
            if (cb == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            RegisterBufferClearListener(id, cb);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_GAMUTS): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            std::vector<uint32_t> modeSend;
            std::vector<ScreenColorGamut> mode;
            int32_t result = GetScreenSupportedColorGamuts(id, mode);
            reply.WriteInt32(result);
            if (result != StatusCode::SUCCESS) {
                break;
            }
            std::copy(mode.begin(), mode.end(), std::back_inserter(modeSend));
            reply.WriteUInt32Vector(modeSend);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_METADATAKEYS): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            std::vector<uint32_t> keySend;
            std::vector<ScreenHDRMetadataKey> keys;
            int32_t result = GetScreenSupportedMetaDataKeys(id, keys);
            reply.WriteInt32(result);
            if (result != StatusCode::SUCCESS) {
                break;
            }
            for (auto i : keys) {
                keySend.push_back(i);
            }
            reply.WriteUInt32Vector(keySend);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_GAMUT): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            ScreenColorGamut mode;
            int32_t result = GetScreenColorGamut(id, mode);
            reply.WriteInt32(result);
            if (result != StatusCode::SUCCESS) {
                break;
            }
            reply.WriteUint32(mode);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_GAMUT): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            int32_t modeIdx = data.ReadInt32();
            int32_t result = SetScreenColorGamut(id, modeIdx);
            reply.WriteInt32(result);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_GAMUT_MAP): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            ScreenGamutMap mode = static_cast<ScreenGamutMap>(data.ReadInt32());
            int32_t result = SetScreenGamutMap(id, mode);
            reply.WriteInt32(result);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_CORRECTION): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            ScreenRotation screenRotation = static_cast<ScreenRotation>(data.ReadInt32());
            int32_t result = SetScreenCorrection(id, screenRotation);
            reply.WriteInt32(result);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_GAMUT_MAP): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            ScreenGamutMap mode;
            int32_t result = GetScreenGamutMap(id, mode);
            reply.WriteInt32(result);
            if (result != StatusCode::SUCCESS) {
                break;
            }
            reply.WriteUint32(mode);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_VSYNC_CONNECTION): {
            std::string name = data.ReadString();
            auto remoteObj = data.ReadRemoteObject();
            uint64_t id = data.ReadUint64();
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
            sptr<IVSyncConnection> conn = CreateVSyncConnection(name, token, id);
            if (conn == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            reply.WriteRemoteObject(conn->AsObject());
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_HDR_CAPABILITY): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            RSScreenHDRCapability screenHDRCapability;
            int32_t result = GetScreenHDRCapability(id, screenHDRCapability);
            reply.WriteInt32(result);
            if (result != StatusCode::SUCCESS) {
                break;
            }
            reply.WriteParcelable(&screenHDRCapability);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_TYPE): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            RSScreenType type;
            int32_t result = GetScreenType(id, type);
            reply.WriteInt32(result);
            if (result != StatusCode::SUCCESS) {
                break;
            }
            reply.WriteUint32(type);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_BITMAP): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            NodeId id = data.ReadUint64();
#ifndef USE_ROSEN_DRAWING
            SkBitmap bm;
#else
            Drawing::Bitmap bm;
#endif
            bool result = GetBitmap(id, bm);
            reply.WriteBool(result);
            if (result) {
                RSMarshallingHelper::Marshalling(reply, bm);
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_PIXELMAP): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            NodeId id = data.ReadUint64();
            std::shared_ptr<Media::PixelMap> pixelmap =
                std::shared_ptr<Media::PixelMap>(data.ReadParcelable<Media::PixelMap>());
#ifndef USE_ROSEN_DRAWING
            SkRect rect;
#else
            Drawing::Rect rect;
#endif
            RSMarshallingHelper::Unmarshalling(data, rect);
            bool result = GetPixelmap(id, pixelmap, &rect);
            reply.WriteBool(result);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_SKIP_FRAME_INTERVAL): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ScreenId id = data.ReadUint64();
            uint32_t skipFrameInterval = data.ReadUint32();
            int32_t result = SetScreenSkipFrameInterval(id, skipFrameInterval);
            reply.WriteInt32(result);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_OCCLUSION_CHANGE_CALLBACK): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }

            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            sptr<RSIOcclusionChangeCallback> callback = iface_cast<RSIOcclusionChangeCallback>(remoteObject);
            if (callback == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            int32_t status = RegisterOcclusionChangeCallback(callback);
            reply.WriteInt32(status);
            break;
        }
        case static_cast<uint32_t>(
            RSIRenderServiceConnectionInterfaceCode::REGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }

            NodeId id = data.ReadUint64();
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            sptr<RSISurfaceOcclusionChangeCallback> callback =
                iface_cast<RSISurfaceOcclusionChangeCallback>(remoteObject);
            if (callback == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            std::vector<float> partitionPoints;
            if (!data.ReadFloatVector(&partitionPoints)) {
                ret = ERR_TRANSACTION_FAILED;
                break;
            }
            int32_t status = RegisterSurfaceOcclusionChangeCallback(id, callback, partitionPoints);
            reply.WriteInt32(status);
            break;
        }
        case static_cast<uint32_t>(
            RSIRenderServiceConnectionInterfaceCode::UNREGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }

            NodeId id = data.ReadUint64();
            int32_t status = UnRegisterSurfaceOcclusionChangeCallback(id);
            reply.WriteInt32(status);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_APP_WINDOW_NUM): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            uint32_t num = data.ReadUint32();
            SetAppWindowNum(num);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SHOW_WATERMARK): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            std::shared_ptr<Media::PixelMap> watermarkImg =
                std::shared_ptr<Media::PixelMap>(data.ReadParcelable<Media::PixelMap>());
            bool isShow = data.ReadBool();
            ShowWatermark(watermarkImg, isShow);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_JANK_STATS): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            ReportJankStats();
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_RESPONSE): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            DataBaseRs info;
            ReadDataBaseRs(info, data);
            ReportEventResponse(info);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_COMPLETE): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            DataBaseRs info;
            ReadDataBaseRs(info, data);
            ReportEventComplete(info);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_JANK_FRAME): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            DataBaseRs info;
            ReadDataBaseRs(info, data);
            ReportEventJankFrame(info);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::EXECUTE_SYNCHRONOUS_TASK): {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            auto type = data.ReadInt16();
            auto subType = data.ReadInt16();
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
            ExecuteSynchronousTask(task);
            if (!task->Marshalling(reply)) {
                ret = ERR_INVALID_STATE;
            }
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_HARDWARE_ENABLED) : {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            auto id = data.ReadUint64();
            auto isEnabled = data.ReadBool();
            SetHardwareEnabled(id, isEnabled);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_HGM_CFG_CALLBACK) : {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            sptr<RSIHgmConfigChangeCallback> callback = iface_cast<RSIHgmConfigChangeCallback>(remoteObject);
            if (callback == nullptr) {
                ret = ERR_NULL_OBJECT;
                break;
            }
            int32_t status = RegisterHgmConfigChangeCallback(callback);
            reply.WriteInt32(status);
            break;
        }
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_ROTATION_CACHE_ENABLED) : {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            auto isEnabled = data.ReadBool();
            SetCacheEnabledForRotation(isEnabled);
            break;
        }
#ifdef TP_FEATURE_ENABLE
        case static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_TP_FEATURE_CONFIG) : {
            auto token = data.ReadInterfaceToken();
            if (token != RSIRenderServiceConnection::GetDescriptor()) {
                ret = ERR_INVALID_STATE;
                break;
            }
            int32_t feature = data.ReadInt32();
            auto config = data.ReadCString();
            SetTpFeatureConfig(feature, config);
            break;
        }
#endif
        default: {
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }

    return ret;
}

void RSRenderServiceConnectionStub::ReadDataBaseRs(DataBaseRs& info, MessageParcel& data)
{
    info.appPid = data.ReadInt32();
    info.eventType =  data.ReadInt32();
    info.versionCode = data.ReadInt32();
    info.uniqueId = data.ReadInt64();
    info.inputTime = data.ReadInt64();
    info.beginVsyncTime = data.ReadInt64();
    info.endVsyncTime = data.ReadInt64();
    info.sceneId = data.ReadString();
    info.versionName = data.ReadString();
    info.bundleName = data.ReadString();
    info.processName = data.ReadString();
    info.abilityName = data.ReadString();
    info.pageUrl = data.ReadString();
    info.sourceType = data.ReadString();
    info.note = data.ReadString();
}

const RSInterfaceCodeSecurityManager RSRenderServiceConnectionStub::securityManager_ = \
    RSInterfaceCodeSecurityManager::CreateInstance<RSIRenderServiceConnectionInterfaceCodeAccessVerifier>();
} // namespace Rosen
} // namespace OHOS
