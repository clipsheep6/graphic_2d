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
#include "drawing_dcl.h"
#include <sstream>
#include "platform/common/rs_log.h"


namespace OHOS{
namespace Rosen{
DrawingDCL::DrawingDCL(int32_t argc, char* argv[])
{
    std::unique_ptr<DCLCommand> dclCommand(new DCLCommand(argc, argv));
    UpdateParametersFromDCLCommand(std::move(dclCommand));
}

void DrawingDCL::PrintDurationTime(const std::string & description, std::chrono::time_point<std::chrono::system_clock> start)
{
    auto end = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds> (end - start);
    std::cout << description << duration.count() / 1000000.0 << "ms" << std::endl;
}

bool DrawingDCL::IterateFrame(int &cur_loop, int &frame)
{
    ++frame;
    if (frame > endFrame) {
        frame = beginFrame;
        if (++cur_loop == loop) {
            return false;
        }
    }
    return true;
}

bool DrawingDCL::PlayBackByFrame(SkCanvas * skiaCanvas, bool isDumpPictures)
{
    //read DrawCmdList from file
    auto start = std::chrono::system_clock::now();
    static int frame = beginFrame;
    static int cur_loop = 0;
    std::string dcl_file = inputFilePath + "frame" + std::to_string(frame) + ".txt";
    std::cout << "PlayBackFrame dcl_file:" << dcl_file << std::endl;
    
    if (LoadDrawCmdList(dcl_file) < 0){
        std::cout << "failed to loadDrawCmdList" << std::endl;
        IterateFrame(cur_loop, frame);
        return false;
    }
    PrintDurationTime("Load DrawCmdList file time is ", start);
    start = std::chrono::system_clock::now();
    dcl->Playback(*skiaCanvas);
    PrintDurationTime("The frame PlayBack time is ", start); 
    //if (isDumpPictures) {
        // TODO: dump bitmap from skia Canvas.
    //}
    return IterateFrame(cur_loop, frame);
}

bool DrawingDCL::PlayBackByOpItem(SkCanvas * skiaCanvas, bool isMoreOps) 
{
    auto start = std::chrono::system_clock::now();
    // read drawCmdList from file
    std::string dcl_file = inputFilePath + "frameByOpItem.txt";
    std::cout << "PlayBackFrame dcl_file:" << dcl_file << std::endl;

    if (LoadDrawCmdList(dcl_file) < 0){
        std::cout << "failed to loadDrawCmdList" << std::endl;
        return false;
    }
    PrintDurationTime("Load DrawCmdList file time is ", start);
    // Playback
    static double op_id = 0;
    int oldOpId = int(op_id);
    if (!isMoreOps) {
        op_id -= opItemStep;
        if (op_id < 0) op_id = 0;
        std::cout<< "This is already the first OpItem." << std::endl;
    } else {
        op_id += opItemStep;
    }
    std::cout << "play back to op_id = " << int(op_id) << std::endl;
    if (op_id < dcl->GetSize()) {
        std::cout << dcl->PlaybackWithParam(*skiaCanvas, 0, int(op_id), oldOpId) << std::endl;
    } else {
        std::cout << dcl->PlaybackWithParam(*skiaCanvas, 0, dcl->GetSize(), oldOpId) << std::endl;
        op_id = 0;
        return false;
    }
    return true;
}

bool DrawingDCL::GetDirectionAndStep(std::string command, bool &isMoreOps)
{
    if (command.empty()) return true;
    std::vector<std::string> words;
    std::stringstream ss(command);
    std::string word;
    while (ss >> word) {
        words.emplace_back(word);
    }
    if (words.size() > 2) {
        std::cout << "Too Many Parameter!" << std::endl;
        return false;
    }
    if (std::strcmp(words[0].c_str(), "l") == 0 || std::strcmp(words[0].c_str(), "L") == 0) {
        isMoreOps = false;
    } else if (std::strcmp(words[0].c_str(), "m") == 0 || std::strcmp(words[0].c_str(), "M") == 0) {
        isMoreOps = true;
    } else {
        std::cout << "Wrong Direction!" << std::endl;
        return false;
    }
    // check if the input for step is valid
    int dot_pos = -1;
    for (int i = 0; i < words[1].size(); ++i) {
        if (words[1][i] == '.' && dot_pos == -1){
            dot_pos = i;
        } else if (words[1][i] >= '0' && words[1][i] <= '9') {
            continue;
        } else {
            std::cout << "Please enter right step!" << std::endl;
            return false;
        }
    }
    opItemStep = std::stod(words[1]);
    return true;
}

void DrawingDCL::UpdateParametersFromDCLCommand(std::unique_ptr<DCLCommand> dclCommand)
{
    iterateType = dclCommand -> iterateType;
    beginFrame = dclCommand -> beginFrame;
    endFrame = dclCommand -> endFrame;
    loop = dclCommand -> loop;
    opItemStep = dclCommand -> opItemStep;
    inputFilePath = dclCommand -> inputFilePath;
    outputFilePath = dclCommand -> outputFilePath;
}

void DrawingDCL::UpdateParameters(bool notNeeded) 
{
    if (notNeeded) return;
    std::cout << "Please re-enter the parameters" << std::endl;
    std::string line;
    getline(std::cin, line);
    if (line.empty()) return;
    std::unique_ptr<DCLCommand> dclCommand(new DCLCommand(line));
    UpdateParametersFromDCLCommand(std::move(dclCommand));
}

void DrawingDCL::Start()
{
    std::cout << "DrawingDCL::Start+" << std::endl;
    std::cout << "DrawingDCL::Start-" << std::endl;
}

void DrawingDCL::Stop()
{
    std::cout << "DrawingDCL::Stop+" << std::endl;
    std::cout << "DrawingDCL::Stop-" << std::endl;
}

void DrawingDCL::Test(SkCanvas* canvas, int width, int height)
{
    std::cout << "DrawingDCL::Test+" << std::endl;
    auto start = std::chrono::system_clock::now();

    if (iterateType == 0) {
        UpdateParameters(PlayBackByFrame(canvas));
    } else if (iterateType == 1) {
        UpdateParameters(PlayBackByOpItem(canvas));
    } else if (iterateType == 2) {
        static bool isMoreOps = true;
        std::string opActionsStr = isMoreOps ? "more" : "less";
        std::cout << "Do you want to execute " << opItemStep << " OpItems " << opActionsStr << " ?\n"
            "To Modify, enter the action (More or less) and step size, for example, \"M 2\". \n"
            " Press Enter to continue." << std::endl;
        std::string line;
        do {
            getline(std::cin, line);
        } while (!GetDirectionAndStep(line, isMoreOps));
        UpdateParameters(PlayBackByOpItem(canvas, isMoreOps));
    }

    PrintDurationTime("This frame draw time is: ", start);
    std::cout << "DrawingDCL::Test-" << std::endl;
}

void DrawingDCL::Output()
{
}

class MyAllocator : public DefaultAllocator {
public:
    MyAllocator(int fd, size_t size, uint8_t * mapFile) : fd_(fd), size_(size), mapFile_(mapFile) {}

    void Dealloc(void *mapFile) {
        if (mapFile != mapFile_) {
            std::cout << "MyAllocator::Deallo data addr not match!" << std::endl;
        }
        if (mapFile_ != nullptr) {
            ::munmap(mapFile_, size_);
            mapFile_ = nullptr;
        }
        if (fd_ > 0) {
            ::close(fd_);
            fd_ = -1;
        }
    }
private:
    int fd_;
    size_t size;
    uint8_t * mapFile_;
};

int DrawingDCL::LoadDrawCmdList(std::string dcl_file)
{
    int32_t fd = open(dcl_file.c_str(), O_RDONLY);
    if (fd <= 0) {
        std::cout << "Open file failed" << dcl_file.c_str() << std::endl;
        return -1;
    }
    struct stat statbuf;
    if (fstat(fd, &statbuf) < 0) {
        close(fd);
        return -1;
    }
    std::cout << "statbuf.st_size = " << statbuf.st_size << std::endl;

    auto mapFile = static_cast<uint8_t *> (mmap(NULL, statbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0));
    if (mapFile == MAP_FAILED) {
        close(fd);
        return -1;
    }
    std::cout << "mapFile OK" << std::endl;

    MessageParcel messageParcel(new MyAllocator(fd, statbuf.st_size, mapFile));
    size_t RECORDING_PARCEL_MAX_CAPCITY = 234 * 1000 * 1024;
    messageParcel.SetMaxCapacity(RECORDING_PARCEL_MAX_CAPCITY);
    if (!messageParcel.ParseFrom(reinterpret_cast<uintptr_t>(mapFile), statbuf.st_size)) {
        munmap(mapFile, statbuf.st_size);
        close(fd);
        return -1;
    }
    std::cout << "messageParcel GetDataSize() = " << messageParcel.GetDataSize() << std::endl;

    dcl = std::unique_ptr<DrawCmdList>(DrawCmdList::Unmarshalling(messageParcel));
    if (dcl == nullptr) {
        std::cout << "dcl is nullptr" << std::endl;
        munmap(mapFile, statbuf.st_size);
        close(fd);
        return -1;
    }
    std::cout << "The size of Ops is " << dcl->GetSize() << std::endl;
    munmap(mapFile, statbuf.st_size);
    close(fd);
    return 0;    
}

}
}
