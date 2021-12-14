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

#include "wmclient_native_test_36.h"

#include <cstdio>
#include <fstream>
#include <securec.h>
#include <string.h>
#include <sstream>
#include <option_parser.h>

#include <display_type.h>
#include <window_manager.h>
#include <zlib.h>
#include <gslogger.h>

#include "inative_test.h"
#include "native_test_class.h"
#include "util.h"

using namespace OHOS;

namespace {
class WMClientNativeTest36 : public INativeTest, public IScreenShotCallback {
public:
    std::string GetDescription() const override 
    {
        constexpr const char *desc = "screencap(--stride=200 (ms) --total-time=3 (s))";
        return desc;
    }

    std::string GetDomain() const override 
    {
        constexpr const char *domain = "wmclient";
        return domain;
    }

    int32_t GetID() const override 
    {
        constexpr int32_t id = 36;
        return id;
    }

    uint32_t GetLastTime() const override 
    {
        constexpr uint32_t lastTime = LAST_TIME_FOREVER;
        return lastTime;
    }

    void Run(int32_t argc, const char **argv) override 
    {
        OptionParser parser;
        parser.AddOption("s", "stride", s);
        parser.AddOption("t", "total-time", t);
        if (parser.Parse(argc, argv)) {
            GSLOG7SE(ERROR) << parser.GetErrorString();
            ExitTest();
            return;
        }

        auto initRet = WindowManager::GetInstance()->Init();

        if (initRet) {
            printf("init failed with %s\n", WMErrorStr(initRet).c_str());
            ExitTest();
            return;
        }

        auto wm = WindowManager::GetInstance();

        while (true) {
            time = time + s;
            wm->ListenNextScreenShot(0, this);

            if (time >= (t*1000-s)) {
                break;
            }

            usleep(s*1000);
        }

        printf("screencap completed\n");
        ExitTest();
        return;
    }

    int32_t Compress(uint8_t *c,
                     unsigned long &ul, const uint8_t *data, uint32_t size) const 
    {
        auto ret = compress(c, &ul, data, size);

        if (ret) {
            printf("compress failed, %d\n", ret);
        }

        return ret;
    }

    void OnScreenShot(const struct WMImageInfo &info) override 
    {
        count = count + 1;
        auto ptr = static_cast<const uint8_t *>(info.data);
        size = info.size;
        clen = compressBound(size);
        auto compressed = std::make_unique<uint8_t[]>(clen);
        unsigned long ulength = clen;

        if (Compress(compressed.get(), ulength, ptr, size) != Z_OK) {
            printf("compress failed\n");
            ExitTest();
            return;
        }

        if (!flag) {
            std::string command;
            std::string filePath = "/data/screencap/";
            command = "mkdir -p " + filePath;
            auto fp = fopen(filePath.c_str(), "w");

            if (!fp) {
                system(command.c_str());
            }

            finalPath = filePath.append("rec.raw");
            std::ofstream rawDataFile_1(finalPath, std::ofstream::binary | std::ofstream::out);
            rawDataFile_1.write("RAW.diff", 8);
            rawDataFile_1.close();

            std::ofstream rawDataFile_2(finalPath, std::ofstream::binary | std::ofstream::app);
            rawDataFile_2.write(reinterpret_cast<const char *>(&info.width), 4);
            rawDataFile_2.write(reinterpret_cast<const char *>(&info.height), 4);
            rawDataFile_2.close();
            flag = true;
        }

        int32_t length = info.width * info.height * 4;
        std::ofstream rawDataFile(finalPath, std::ofstream::binary | std::ofstream::app);
        rawDataFile.write(reinterpret_cast<const char *>(&header_type), 4);
        rawDataFile.write(reinterpret_cast<const char *>(&position), 4);
        rawDataFile.write(reinterpret_cast<const char *>(&length), 4);
        rawDataFile.write(reinterpret_cast<const char *>(&ulength), 4);
        rawDataFile.write(reinterpret_cast<const char *>(compressed.get()), ulength);
        auto align = ulength - ulength / 4 * 4 ;

        for (int i = 0; i < 4 - align; i++) {
            rawDataFile.write(reinterpret_cast<const char *>(&temp), 1);
        }

        rawDataFile.close();
    }
private:
    int32_t time = 0;
    int32_t count = 0;
    int32_t temp = 0;
    std::string finalPath;
    const int32_t header_type = 2;
    const int32_t position = 0;
    int32_t clen;
    int32_t size = -1;
    bool flag = false;
    int32_t t = 3;
    int32_t s = 200;
} g_autoload;
} // namespace