/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "graphic_dumper_service.h"

#include "graphic_dumper_hilog.h"
#include "graphic_dumper_server.h"

#define REMOTE_RETURN(reply, gd_error) \
    reply.WriteInt32(gd_error);        \
    if (gd_error != GD_OK) {     \
        GDLOG_FAILURE_NO(gd_error);    \
    }                                  \
    break

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "GraphicDumperService" };
}

int32_t GraphicDumperService::OnRemoteRequest(uint32_t code, MessageParcel &data,
                                      MessageParcel &reply, MessageOption &option)
{
    auto remoteDescriptor = data.ReadInterfaceToken();
    if (GetDescriptor() != remoteDescriptor) {
        return 1;
    }

    switch (code) {
        case IGRAPHIC_DUMPER_SERVICE_ADD_CLIENT_LISTENER: {
            auto tag = data.ReadString();
            GDLOGFE("%{public}s", tag.c_str());
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                REMOTE_RETURN(reply, GD_ERROR_NULLPTR);
            }
            auto l = iface_cast<IGraphicDumperClientListener>(remoteObject);
            GDError ret = AddClientListener(tag, l);
            REMOTE_RETURN(reply, ret);
        } break;
        case IGRAPHIC_DUMPER_SERVICE_SEND_INFO: {
            std::string tag = data.ReadString();
            std::string info = data.ReadString();
            GDError ret = SendInfo(tag, info);
            GDLOGFE(" IGRAPHIC_DUMPER_SERVICE_SEND_INFO is %{public}s , %{public}s", tag.c_str() , info.c_str());
            REMOTE_RETURN(reply, ret);
        } break;

        default: {
            GDLOGFE("code %{public}d cannot process", code);
            return 1;
        } break;
    }
    return 0;
}

GDError GraphicDumperService::AddClientListener(const std::string &tag, sptr<IGraphicDumperClientListener> &listener)
{
    if (listener == nullptr) {
        return GD_ERROR_NULLPTR;
    }
    return GraphicDumperServer::GetInstance()->AddConfigListener(tag, listener);
}

GDError GraphicDumperService::SendInfo(const std::string &tag, const std::string &info)
{
    return GraphicDumperServer::GetInstance()->InfoHandle(tag, info);
}

void GDumperClientListenerDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &object)
{
    if (object == nullptr) {
        return;
    }
    GDLOGFE("");
    GraphicDumperServer::GetInstance()->RemoveConfigListener(object.promote());
}
} // namespace OHOS
