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

#include <csignal>
#include <iostream>
#include <string>
#include <unistd.h>

#include "graphic_dumper_helper.h"
#include "graphic_dumper_hilog.h"
#include "graphic_dumper_type.h"

using namespace OHOS;

bool g_exitFlag = false;


namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "GraphicDumperHelperTest" };
constexpr uint32_t SLEEP_TIME = 1000;
constexpr uint32_t SLEEP_LOOP_TIME = 10000;

int32_t g_funcID = 0;
int32_t g_DumpID = 0;
bool printInfo = true;
std::string tagInfo = {};

void ConfigChangeFunc(const std::string &key, const std::string &val)
{
    GDLOGFI("%{public}s -> %{public}s", key.c_str(), val.c_str());
    if (key.find("info") != std::string::npos) {
        if (val.compare("true") == 0) {
            printInfo = true;
        } else {
            printInfo = false;
        }
    }
}

void DumpFunc()
{
    GDLOGFI("OnDump");
    GraphicDumperHelper::GetInstance()->SendInfo(tagInfo, "pid[%d] send dump info \n", getpid());
}
} // namespace

void Handler(int signal)
{
    switch (signal) {
        case SIGINT:
        case SIGKILL:
        case SIGTERM:
        case SIGTSTP:
        case SIGQUIT:
        case SIGHUP: {
            g_exitFlag = true;
        } break;
        default:
            break;
    }
}

int main(int argc, const char** argv)
{
    GDLOGFI("graphic dumper test start!");

    std::signal(SIGINT, Handler);
    std::signal(SIGKILL, Handler);
    std::signal(SIGTERM, Handler);
    std::signal(SIGTSTP, Handler);
    std::signal(SIGQUIT, Handler);
    std::signal(SIGHUP, Handler);

    int ret = 0;
    constexpr int ARGC_NUM = 2;
    if (argc == ARGC_NUM) {
        tagInfo = std::string{ argv[1] };
    } else {
        tagInfo = "A.B.info";
    }

    g_funcID =  GraphicDumperHelper::GetInstance()->AddConfigChangeListener(tagInfo + ".info", ConfigChangeFunc);
    g_DumpID =  GraphicDumperHelper::GetInstance()->AddDumpListener(tagInfo + ".dump", DumpFunc);

    int32_t count = 0;
    while (!g_exitFlag) {
        if (printInfo) {
            ret = GraphicDumperHelper::GetInstance()->SendInfo(tagInfo,
                "********** !!! just for test send info !!! **********\n");
                GDLOGFE("graphic dumper ret = %{public}d",ret);
            if (ret != 0) {
                GDLOGFE("graphic dumper service died!");
                break;
            }
        }

        ret = GraphicDumperHelper::GetInstance()->SendInfo("log." + tagInfo,
            "[%d]ABCEDFGHIG0123456789ABCEDFGHIG0123456789ABCEDFGHIG0123456789 just for test log %d!!!\n",
            getpid(),
            count++);
        if (ret != 0) {
            GDLOGFE("graphic dumper service died!");
            break;
        }

        usleep(SLEEP_TIME);
        ret = GraphicDumperHelper::GetInstance()->SendInfo("log." + tagInfo + ".1",
            "[%d]ABCEDFGHIG0123456789ABCEDFGHIG0123456789ABCEDFGHIG0123456789\njust for test2 log %d!!!\n",
            getpid(),
            count++);
        if (ret != 0) {
            GDLOGFE("graphic dumper service died!");
            break;
        }

        usleep(SLEEP_LOOP_TIME);
    }
    if (g_funcID != 0) {
        GraphicDumperHelper::GetInstance()->RemoveConfigChangeListener(g_funcID);
    }
    if (g_DumpID != 0) {
        GraphicDumperHelper::GetInstance()->RemoveDumpListener(g_DumpID);
    }
    usleep(SLEEP_TIME);
    return ret;
}