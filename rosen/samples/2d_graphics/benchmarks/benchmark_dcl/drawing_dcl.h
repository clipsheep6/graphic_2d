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

#ifndef DRAWING_DCL
#define DRAWING_DCL

#include "benchmark.h"
#include "pipeline/rs_draw_cmd_list.h"
#include <sys/mman.h>
#include <unistd.h>
#include <chrono>
#include <sys/stat.h>
#include <message_parcel.h>
#include "dcl_command.h"

namespace OHOS {
namespace Rosen {
class DrawingDCL : public BenchMark {
public:
    DrawingDCL() { std::cout << "DrawingDCL" << std::endl; }

    DrawingDCL(int32_t argc, char* argvp[]);

    ~DrawingDCL() { std::cout << "~DrawingDCL" << std::endl; }

    bool GetDirectionAndStep(std::string command, bool &isMoreOps);

    bool IterateFrame(int &curLoop, int &frame);

    bool PlayBackByFrame(SkCanvas *skiaCanvas, bool isDumpPicture = false);

    bool PlayBackByOpItem(SkCanvas *skiaCanvas, bool isMoreOps = true);

    void UpdateParameters(bool isNeeded);

    void UpdateParametersFromDCLCommand(std::unique_ptr<DCLCommand> dclCommand);

    void PrintDurationTime(const std::string &description, std::chrono::time_point<std::chrono::system_clock> start);

    void Start() override;

    void Stop() override;

    void Test(SkCanvas *canvas, int width, int height) override;

    void Output() override;

    int LoadDrawCmdList(std::string dclFile);

private:
    friend class DCLCommand;
    std::unique_ptr<DrawCmdList> dcl = nullptr;
    IterateType iterateType = IterateType::ITERATE_FRAME;
    int beginFrame = 0;
    int endFrame = 100;
    int loop = 1;
    double opItemStep = 1;
    std::string inputFilePath = "/data/lkx/";
    std::string outputFilePath = "/data/lkx/";
    const static size_t RECORDING_PARCEL_MAX_CAPCITY = 234 * 1000 * 1024;
};

}
}

#endif