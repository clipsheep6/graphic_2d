/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "dcl_command.h"
#include <sstream>
#include <vector>

namespace OHOS {
namespace Rosen {
DCLCommand::DCLCommand(int32_t argc, char* argv[])
{
    std::vector<std::string> argvNew(argv, argv + argc);
    ParseCommand(argvNew);
}

void DCLCommand::ParseCommand(std::vector<std::string> argv)
{
    if (argv.size() == twoParam) {
        std::cout << "iterate frame by default, beginFrame = " << beginFrame << ", endFrame = " <<
            endFrame << std::endl;
    } else if (argv.size() == threeParam) {
        if (strcmp(argv.back().c_str(), "--help") != 0 || strcmp(argv.back().c_str(), "-h") != 0) {
            std::cout << dclMsg << std::endl;
        }
    } else {
        for (int i = 2; i < argv.size(); ++i) {
            std::string option = argv[i];
            std::string augment;
            if (i < argv.size() - 1) {
                augment = argv[i + 1];
            }
            if (commandMap.count(option) > 0) {
                HandleCommand(option, augment);
            }
        }
        CheckParameter();
    }
}

DCLCommand::DCLCommand(std::string commandLine)
{
    std::istringstream ss(commandLine);
    std::string param;
    std::vector<std::string> params;
    if (commandLine.find("drawing_ending_sample") == std::string::npos) {
        params.emplace_back("drawing_engine_sample");
        params.emplace_back("dcl");
    }
    while (ss >> param) {
        params.emplace_back(param);
    }
    ParseCommand(params);
}

void DCLCommand::HandleCommand(std::string option, std::string augment)
{
    switch (commandMap.at(option)) {
        case CommandType::CT_T:
            switch (std::stoi(augment.c_str())) {
                case int(IterateType::ITERATE_FRAME):
                    iterateType = IterateType::ITERATE_FRAME;
                    break;
                case int(IterateType::ITERATE_OPITEM):
                    iterateType = IterateType::ITERATE_OPITEM;
                    break;
                case int(IterateType::ITERATE_OPITEM_MANUALLY):
                    iterateType = IterateType::ITERATE_OPITEM_MANUALLY;
                    break;
                default:
                    std::cout <<"Wrong Parameter: iterateType" << std::endl;
                    return;
            }
            break;
        case CommandType::CT_B:
            beginFrame = std::stoi(augment.c_str());
            break;
        case CommandType::CT_E:
            endFrame = std::stoi(augment.c_str());
            break;
        case CommandType::CT_L:
            loop = std::stoi(augment.c_str());
            break;
        case CommandType::CT_S:
            opItemStep = std::stoi(augment.c_str());
            break;
        case CommandType::CT_I:
            inputFilePath = augment;
            if (inputFilePath.back() != '/') {
                inputFilePath += '/';
            }
            break;
        case CommandType::CT_O:
            outputFilePath = augment;
            if (outputFilePath.back() != '/') {
                outputFilePath += '/';
            }
            break;
        case CommandType::CT_H:
            std::cout << dclMsg <<std::endl;
            break;
        default:
            std::cout << "other unknown args:" <<std::endl;
            break;
    }
}

void DCLCommand::CheckParameter()
{
    if (beginFrame < 0 || beginFrame > endFrame) {
        std::cout << "Wrong Parameter: beginFrame or endFrame!" << std::endl;
        beginFrame = 0;
        int initEndFrame = 100;
        endFrame = initEndFrame;
    }
    if (loop < 0) {
        std::cout << "Wrong parameter: loop!" << std::endl;
        loop = 1;
    }
    if (opItemStep < 0) {
        std::cout << "Wrong Parameter: opItemStep!" << std::endl;
        opItemStep = 1;
    }
}
}
}