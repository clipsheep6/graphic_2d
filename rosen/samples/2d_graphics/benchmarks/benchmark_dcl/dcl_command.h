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
#ifndef DRAWING_DCL_COMMAND
#define DRAWING_DCL_COMMAND

#include <iostream>
#include <vector>
#include <unordered_map>

namespace OHOS{
namespace Rosen{
class DCLCommand{
public:
    DCLCommand(int32_t argc, char* argv[]);
    DCLCommand(std::string commandLine);
    ~DCLCommand()= default;
    void ParseCommand(std::vector<std::string> argv);
    void HandleCommand(std::string argStr, std::string value);
    void CheckParameter();
private:
    int iterateType = 0;
    int beginFrame = 0;
    int endFrame = 100;
    int loop = 1;
    double opItemStep = 1;
    std::string inputFilePath = "/data/lkx/";
    std::string outputFilePath = "/data/lkx/";

    friend class DrawingDCL;
    const int twoParam = 2;
    const int threeParam = 3;
    enum class CommandType {
        CT_T,
        CT_B,
        CT_E,
        CT_L,
        CT_S,
        CT_I,
        CT_O,
        CT_H,
    };

    const std::unordered_map<std::string, CommandType> commandMap = {
        { std::string("-t"), CommandType::CT_T },           { std::string("--type"), CommandType::CT_T }, 
        { std::string("-b"), CommandType::CT_T },           { std::string("--beginFrame"), CommandType::CT_T }, 
        { std::string("-e"), CommandType::CT_T },           { std::string("--endFrame"), CommandType::CT_T }, 
        { std::string("-l"), CommandType::CT_T },           { std::string("--loop"), CommandType::CT_T }, 
        { std::string("-s"), CommandType::CT_T },           { std::string("--step"), CommandType::CT_T }, 
        { std::string("-i"), CommandType::CT_T },           { std::string("--inputFilePath"), CommandType::CT_T }, 
        { std::string("-o"), CommandType::CT_T },           { std::string("--outputFilePath"), CommandType::CT_T }, 
        { std::string("-h"), CommandType::CT_T },           { std::string("--help"), CommandType::CT_T }, 
    };
    
    const std::string dclMsgErr = "error input!\n use command '--help' get more information\n";
    const std::string dclMsg = "usage: /data/drawing_engine_sample dcl <option> <argument> \n"
                                "-----------------------------------------------------------------------------\n"
                                "There are common commands list:\n"
                                " -t,--type                     set the type of playback, \n"
                                "                               \t0: iterate by frame,\n"
                                "                               \t1: iterate by opItem,\n"
                                "                               \t2: iterate by opItem using manual control,\n"
                                " -b,--beginFrame               set the start number of frames for playback, \n"
                                " -e,--beginFrame               set the end number of frames for playback, \n"
                                " -l,--loop                     set the loops of iterating by frame, \n"
                                " -s,--step                     set the step when iterating by opItem (the step can be a decimal), \n"
                                " -i,--inputFilePath            set the input path for drawCmdList files, \n"
                                " -o,--outputFilePath           set the output path for drawCmdList files, \n"
                                " -h,--help                     get help, \n"
                                "-----------------------------------------------------------------------------\n"
                                "Example: /data/drawing_ening_sample dcl -t 0 -b 1 -e 100 \n"
                                "-----------------------------------------------------------------------------\n"



};
}
}


#endif