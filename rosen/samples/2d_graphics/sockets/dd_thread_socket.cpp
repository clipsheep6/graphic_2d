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
void DdThreadSocket::Process() const
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
        if (connected && ddSocket.Recv() <=0 && errno != EINTR){
            ddSocket.CloseClient();
            connected = false;
            continue;
        }
        HandleMsg(ddSocket);
    }
    std::cout << "Socket Process finished!" << std::endl;
    ddSocket.Close();
}

void HandleMsg(DdServerSocket &ddSocket) const
    {
        Json::Value clientJsonRoot;
        Json::Value serverJsonRoot;
        Json::Reader reader;
        serverJsonRoot["type"] = "RESPONSE";
        if (!reader.parse(ddSocket.GetRecvBuf(), clientJsonRoot)){
            std::cout << "json parse error" << std::endl;
            serverJsonRoot["retCode"] = static_cast<int>(ReturnCode::PARSE_FAIL);
            serverJsonRoot["result"] = "fail";
        } else if(!clientJsonRoot.isMember("cmd") || commandMap_.find(clientJsonRoot["cmd"]) == commandMap_.end()){
            std::cout << "The \'cmd\' attribute not found or invalid cmd type." << std::endl;
        } else {
            switch (commandMap_.at(clientJsonRoot["cmd"])) {
            case CommandType::PLAY:
                /* code */
                break;
            default:
                break;
            }
        }
        auto iterator = messageMap.begin();
        while (iterator != messageMap.end()) {
            if (SPUtils::IsSubString(spSocket.RecvBuf(), iterator->second)) {
                SpProfiler *profiler = SpProfilerFactory::GetProfilerItem(iterator->first);
                if (profiler == nullptr && (iterator->first == MessageType::SET_PKG_NAME)) {
                    std::string curPkgName = ResPkgOrPid(spSocket);
                    SpProfilerFactory::SetProfilerPkg(curPkgName);
                    std::string pidCmd = "pidof " + curPkgName;
                    std::string pidResult;
                    if (SPUtils::LoadCmd(pidCmd, pidResult)) {
                        SpProfilerFactory::SetProfilerPid(pidResult);
                    }
                    spSocket.Sendto(curPkgName);
                } else if (profiler == nullptr && (iterator->first == MessageType::SET_PROCESS_ID)) {
                    SpProfilerFactory::SetProfilerPid(ResPkgOrPid(spSocket));
                } else if (profiler == nullptr) {
                    std::string returnStr = iterator->second;
                    spSocket.Sendto(returnStr);
                } else {
                    std::map<std::string, std::string> data = profiler->ItemData();
                    std::string sendData = MapToString(data);
                    spSocket.Sendto(sendData);
                }
                break;
            }
            ++iterator;
        }
    }
}
}