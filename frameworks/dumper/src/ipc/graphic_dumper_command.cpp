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

#include "ipc/graphic_dumper_command.h"

#include "graphic_dumper_hilog.h"
#include "graphic_dumper_server.h"
//#include "graphic_dumper_type.h"
#include "ipc/igraphic_dumper_command.h"
#include "graphic_common.h"

#define REMOTE_RETURN(reply, gd_error) \
    reply.WriteInt32(gd_error);        \
    if (gd_error != GD_OK) {     \
        GDLOG_FAILURE_NO(gd_error);    \
    }                                  \
    break

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "GraphicDumperCommand" };
}

int32_t GraphicDumperCommand::OnRemoteRequest(uint32_t code, MessageParcel& data,
                            MessageParcel& reply, MessageOption& option)
{
    GDLOGFE("code %{public}d ", code);
    auto remoteDescriptor = data.ReadInterfaceToken();
    if (GetDescriptor() != remoteDescriptor) {
        return 1;
    }

    switch (code) {
        case IGRAPHIC_DUMPER_COMMAND_GET_CONFIG: {
            std::string k = data.ReadString();
            std::string v = data.ReadString();
            GDError ret = GetConfig(k, v);
            reply.WriteInt32(ret);
            reply.WriteString(v);
        } break;

        case IGRAPHIC_DUMPER_COMMAND_SET_CONFIG: {
            std::string k = data.ReadString();
            std::string v = data.ReadString();
            GDError ret = SetConfig(k, v);
            reply.WriteInt32(ret);
        } break;

        case IGRAPHIC_DUMPER_COMMAND_DUMP: {
            std::string v = data.ReadString();
            GDError ret = Dump(v);
            reply.WriteInt32(ret);
        } break;

        case IGRAPHIC_DUMPER_COMMAND_GET_LOG: {
            std::string tag = data.ReadString();
            std::string log = "";
            GDError ret = GetLog(tag, log);
            reply.WriteInt32(ret);
        } break;
        case IGRAPHIC_DUMPER_COMMAND_ADD_INFO_LISTENER: {
            auto tag = data.ReadString();
            auto remoteObject = data.ReadRemoteObject();
            if (remoteObject == nullptr) {
                REMOTE_RETURN(reply, GD_ERROR_NULLPTR);
            }

            auto l = iface_cast<IGraphicDumperInfoListener>(remoteObject);
            GDError ret = AddInfoListener(tag, l);
            REMOTE_RETURN(reply, ret);
        } break;

        default: {
            GDLOGFE("code %{public}d cannot process", code);
            return 1;
        } break;
    }
    return 0;
}

GDError GraphicDumperCommand::GetConfig(const std::string &k, std::string &v)
{
    GDLOGFE("%{public}s -> %{public}s", k.c_str(), v.c_str());
    GraphicDumperServer::GetInstance()->GetConfig(k, v);
    GDLOGFE("%{public}s -> %{public}s", k.c_str(), v.c_str());
    return GD_OK;
}

GDError GraphicDumperCommand::SetConfig(const std::string &k, const std::string &v)
{
    GDLOGFE("%{public}s -> %{public}s", k.c_str(), v.c_str());
    GraphicDumperServer::GetInstance()->SetConfig(k, v);
    return GD_OK;
}

GDError GraphicDumperCommand::Dump(const std::string &tag)
{
    GDLOGFE("%{public}s", tag.c_str());
    GraphicDumperServer::GetInstance()->Dump(tag);
    return GD_OK;
}

GDError GraphicDumperCommand::GetLog(const std::string &tag, std::string &log)
{
    GDLOGFE("%{public}s -> %{public}s", tag.c_str(), log.c_str());
    GraphicDumperServer::GetInstance()->GetLog(tag, log);
    return GD_OK;
}

GDError GraphicDumperCommand::AddInfoListener(const std::string &tag, sptr<IGraphicDumperInfoListener> &listener)
{
    GDLOGFE("");
    if (listener == nullptr) {
        return GD_ERROR_NULLPTR;
    }
    return GraphicDumperServer::GetInstance()->AddInfoListener(listener);
}
} // namespace OHOS
