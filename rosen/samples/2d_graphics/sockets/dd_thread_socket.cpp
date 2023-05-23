/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "dd_thread_socket.h"
namespace OHOS{
namespace Rosen{
void DdThreadSocket::Process(std::shared_ptr<DrawingDCL> drawingDcl) const
{
    DdServerSocket ddSocket;
    ddSocket.Init();
    bool connected = false;
    while (1) {
        if (!connected) {
            if(ddSocket.Accept() < 0) {
                printf("Create socket error: %s(errno: %d)\n", strerror(errno), errno);
                continue;
            }
            connected = true;
        }
        if (connected && ddSocket.Recv() <=0 && errno != EINTR) {
            ddSocket.CloseClient();
            connected = false;
            continue;
        }
        HandleMsg(ddSocket, drawingDcl);
        Json::Value serverJsonRoot;
        serverJsonRoot["type"] = "NOTIFY";
        if (drawingDcl->GetLoadCmdError()) {
            serverJsonRoot["retCode"] = std::to_string(static_cast<int>(ReturnCode::EXECUTE_FAIL));
            serverJsonRoot["result"] = "FAIL";
            ddSocket.Send(serverJsonRoot.toStyledString());
        } else if(drawingDcl->GetModeType() == ModeType::PLAY) {
            serverJsonRoot["params"]["indicator"] = drawingDcl->GetCurOpItemId();
            ddSocket.Send(serverJsonRoot.toStyledString());
        }
    }
    std::cout << "Socket Process finished!" << std::endl;
    ddSocket.Close();
}

bool DdThreadSocket::HandleMsgParams(DdServerSocket &ddSocket, std::shared_ptr<DrawingDCL> drawingDcl,
    Json::Value& clientJsonRoot)
{
    if (!clientJsonRoot.isMember("params")) {
        return false;
    }
    if (clientJsonRoot["params"].isMember("mode")) {
        std::string iterateStr = clientJsonRoot["params"]["mode"].asString();
        if (iterateCmdType_.at(iterateStr) == iterateCmdType_.end()) {
            std::cout << "Invalid iterate type." << std::endl;
            return false;
        } else {
            drawingDcl->SetIterateType(iterateCmdMap_.at(iterateStr));
        }
    }
    if (clientJsonRoot["params"].isMember("speed")) {
        std::string speedStr = clientJsonRoot["params"]["speed"].asString();
        if (speedCommandMap_.find(speedStr) == speedCommandMap_.end()) {
            std::cout << "Invalid speed type." << std::endl;
            return false;
        } else {
            drawingDcl->SetSpeedType(speedCommandMap_.at(speedStr));
        }
    }
    if (clientJsonRoot["params"].isMember("indicator")) {
        int opId = clientJsonRoot["params"]["indicator"].asInt();
        drawingDcl->SetDestOpItemId(opId);
    }
    return true;
}

void DdThreadSocket::HandleMsg(DdServerSocket &ddSocket, std::shared_ptr<DrawingDCL> drawingDcl) const
{
    Json::Value clientJsonRoot;
    Json::Value serverJsonRoot;
    Json::Reader reader;
    serverJsonRoot["type"] = "RESPONSE";
    bool isSucess = true;
    if (!reader.parse(ddSocket.GetRecvBuf(), clientJsonRoot)) {
        std::cout << "json parse error" << std::endl;
        isSucess = false;
        serverJsonRoot["retCode"] = std::to_string(static_cast<int>(ReturnCode::PARSE_FAIL));
        serverJsonRoot["result"] = "fail";
    } else if(!clientJsonRoot.isMember("cmd") || modeCmdMap_.find(clientJsonRoot["cmd"]) == modeCmdMap_.end()) {
        std::cout << "The \'cmd\' property not found or invalid cmd type." << std::endl;
    } else {
        drawingDcl->SetModeType(modeCmdMap_.at(clientJsonRoot["cmd"].asString()));
        isSuccess = HandleMsgParams();
    }

    if (isSucess) {
        serverJsonRoot["retCode"] = std::to_string(static_cast<int>(ReturnCode::SUCCESS));
        serverJsonRoot["result"] = "SUCCESS";
    } else {
        serverJsonRoot["retCode"] = std::to_string(static_cast<int>(ReturnCode::PARSE_FAIL));
        serverJsonRoot["result"] = "FAIL";
    }
    ddSocket.Send(serverJsonRoot.toStyledString());
}
}
}