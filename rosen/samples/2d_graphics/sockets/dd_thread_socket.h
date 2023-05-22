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
#ifndef DD_THREAD_SOCKET_H
#define DD_THREAD_SOCKET_H

#include "dd_server_socket.h"
#include <string.h>
#include <error.h>
#include "json/json.h"

namespace OHOS {
namespace Rosen {
class DdThreadSocket {
public:
    void Process() const;

    std::string MapToString(std::map<std::string, std::string> dataMap) const
    {
        std::string result;
        int i = 0;
        std::string splitStr = "";
        for (auto iter = dataMap.cbegin(); iter != dataMap.cend(); ++iter) {
            printf("%s = %s\n", iter->first.c_str(), iter->second.c_str());
            if (i > 0) {
                splitStr = "$$";
            }
            result += splitStr + iter->first.c_str() + "||" + iter->second.c_str();
            i++;
        }
        return result;
    }

    std::string ResPkgOrPid(const SpServerSocket &spSocket) const
    {
        std::vector<std::string> sps;
        SPUtils::StrSplit(spSocket.RecvBuf(), "::", sps);
        return sps[1];
    }

    void HandleMsg(DdServerSocket &ddSocket) const;
private:
    enum class ReturnCode {
        SUCCESS,
        PARSE_FAIL,
        EXECUTE_FAIL,
    };
    enum class CommandType {
        PLAY,
        PAUSE,
        RESUME,
        STOP,
        TRICKPLAY,
        MULTIPLE_FRAMES,
        SINGLE_FRAME,
    };

    const std::unordered_map<std::string, CommandType> commandMap_ = {
        { std::string("play"), CommandType::PLAY },
        { std::string("pause"), CommandType::PAUSE },
        { std::string("resume"), CommandType::RESUME },
        { std::string("trickPlay"), CommandType::TRICKPLAY },
        { std::string("MULTIPLE_FRAMES"), CommandType::MULTIPLE_FRAMES },
        { std::string("SINGLE_FRAME"), CommandType::SINGLE_FRAME },
    };
};
}
}
#endif