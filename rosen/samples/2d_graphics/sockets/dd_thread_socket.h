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
#include "drawing_dcl.h"

namespace OHOS {
namespace Rosen {
class DdThreadSocket {
public:
    void Process(std::shared_ptr<DrawingDCL> drawingDcl) const;
    void HandleMsg(DdServerSocket &ddSocket, std::shared_ptr<DrawingDCL> drawingDcl) const;
    void HandleMsgParams(DdServerSocket &ddSocket, std::shared_ptr<DrawingDCL> drawingDcl, Json::Value& clientJsonRoot) const;
private:
    enum class ReturnCode {
        SUCCESS,
        PARSE_FAIL,
        EXECUTE_FAIL,
    };

    const std::unordered_map<std::string, ModeType> modeCmdMap_ = {
        { std::string("play"), ModeType::PLAY },
        { std::string("pause"), ModeType::PAUSE },
        { std::string("resume"), ModeType::RESUME },
    };

    const std::unordered_map<std::string, IterateType> iterateCmdType_ = {
        { std::string("MULTIPLE_FRAMES"), IterateType::MULTIPLE_FRAMES },
        { std::string("SINGLE_FRAME"), IterateType::SINGLE_FRAME },
        { std::string("SINGLE_OPERATER"), IterateType::ITERATE_OPITEM_MANUALLY },
    };

    const std::unordered_map<std::string, SpeedType> speedCmdMap_ = {
    { std::string("SPEEDDOWN"), SpeedType::SPEEDDOWN },
    { std::string("NORMAL"), SpeedType::NORMAL },
    { std::string("SPEEDUP"), SpeedType::SPEEDUP },
};
};
}
}
#endif