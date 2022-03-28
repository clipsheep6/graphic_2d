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

#include "cJSON.h"
#include "util.h"

#include <ctime>
#include <sys/time.h>
#include <string>

#include <vsync_helper.h>
#include <securec.h>
#include <sstream>
#include <include/codec/SkCodec.h>
#include <string.h>

namespace OHOS {
static const int MAX_FILE_NAME = 512;
static const int READ_SIZE = 8192;
static const std::string BOOT_PIC_CONFIGFILE = "config.json";
int64_t GetNowTime()
{
    struct timeval start = {};
    gettimeofday(&start, nullptr);
    constexpr uint32_t secToUsec = 1000 * 1000;
    return static_cast<int64_t>(start.tv_sec) * secToUsec + start.tv_usec;
}

void PostTask(std::function<void()> func, uint32_t delayTime)
{
    auto handler = AppExecFwk::EventHandler::Current();
    if (handler) {
        handler->PostTask(func, delayTime);
    }
}

int GetFreqFromConfig(const char* filestream, int length)
{
    int result = 0;
    std::string JParamsString;
    JParamsString.assign(filestream, length);
    cJSON* overallData = cJSON_Parse(JParamsString.c_str());
    if (overallData == nullptr) {
        LOG("The config json file fails to compile.");
        return -1;
    }
    cJSON* frameRate = cJSON_GetObjectItem(overallData, "FrameRate");
    if (frameRate != nullptr) {
        result = frameRate->valueint;
        LOG("freq: %{public}d", result);
    }
    cJSON_Delete(overallData);
    return result;
}

bool ReadzipFile(const std::string& srcFilePath, std::vector<ImageStruct>& outBgImgVec, int32_t& outReq)
{
    zlib_filefunc_def *zipFuncPtrs = nullptr;
    unzFile zipfile = unzOpen2(srcFilePath.c_str(), zipFuncPtrs);
    if (zipfile == nullptr) {
        LOG("zip file not found");
        return false;
    }

    unz_global_info globalInfo;
    if (unzGetGlobalInfo(zipfile, &globalInfo) != UNZ_OK) {
        LOG("could not read file global info");
        unzClose(zipfile);
        return false;
    }

    char readBuffer[READ_SIZE];
    int  bufsize = READ_SIZE * 3;
    char* buffer = static_cast<char *>(malloc(bufsize));
    if (buffer == nullptr) {
        LOG("malloc memory size:[%{public}d] error.", bufsize);
        unzClose(zipfile);
        return false;
    }
    char* destBuffer = buffer;
    for (unsigned long i = 0; i < globalInfo.number_entry; ++i) {
        unz_file_info fileInfo;
        char filename[MAX_FILE_NAME];
        if (unzGetCurrentFileInfo(
            zipfile,
            &fileInfo,
            filename,
            MAX_FILE_NAME,
            nullptr, 0, nullptr, 0) != UNZ_OK) {
            LOG("could not read file info");
            unzClose(zipfile);
            free(destBuffer);
            return false;
        }

        const size_t fileNameLength = strlen(filename);
        if (filename[fileNameLength - 1] != '/') {
            if (unzOpenCurrentFile(zipfile) != UNZ_OK) {
                LOG("could not open file");
                unzClose(zipfile);
                free(destBuffer);
                return false;
            }
            int error = UNZ_OK;
            int totalLen = 0;
            do {
                error = unzReadCurrentFile(zipfile, readBuffer, READ_SIZE);
                if (error < 0) {
                    LOG("unzReadCurrentFile error %{public}d", error);
                    unzCloseCurrentFile(zipfile);
                    unzClose(zipfile);
                    free(destBuffer);
                    return false;
                }
                if (error > 0) {
                    if (totalLen + error > bufsize) {
                        bufsize += READ_SIZE;
                        destBuffer = static_cast<char *>(realloc(buffer, bufsize));
                        if (destBuffer == nullptr) {
                            LOG("realloc memory size:[%{public}d] error.", bufsize);
                            unzCloseCurrentFile(zipfile);
                            unzClose(zipfile);
                            free(buffer);
                            return false;
                        }
                    }
                    auto tmpret = memcpy_s(destBuffer + totalLen, bufsize - error, readBuffer, error);
                    if (tmpret != EOK) {
                        LOG("memcpy_s error %{public}d", tmpret);
                    }
                    totalLen += error;
                }
            } while (error > 0);

            if (totalLen > 0) {
                if (strstr(filename, BOOT_PIC_CONFIGFILE.c_str()) != nullptr) {
                    outReq = GetFreqFromConfig(destBuffer, totalLen);
                } else {
                    auto skData = SkData::MakeWithCopy(destBuffer, totalLen);
                    if (!skData) {
                        LOG("skdata memory data is null. update data failed");
                        unzCloseCurrentFile(zipfile);
                        unzClose(zipfile);
                        free(destBuffer);
                        return false;
                    }
                    auto codec = SkCodec::MakeFromData(skData);
                    ImageStruct tmpstruct;
                    tmpstruct.fileName = filename;
                    tmpstruct.imageData = SkImage::MakeFromEncoded(skData);
                    outBgImgVec.push_back(tmpstruct);
                }
            }
        }
        unzCloseCurrentFile(zipfile);

        if (i < (globalInfo.number_entry - 1)) {
            if (unzGoToNextFile(zipfile) != UNZ_OK) {
                LOG("could not read next file");
                unzClose(zipfile);
                free(destBuffer);
                return false;
            }
        }
    }

    return true;
}

int RemoveDir(const char *dir)
{
    char curDir[] = ".";
    char upDir[] = "..";
    DIR *dirp;
    struct dirent *dp;
    struct stat dirStat;

    if (access(dir, F_OK) != 0) {
        LOG("can not access dir");
        return 0;
    }
    int statRet = stat(dir, &dirStat);
    if (statRet < 0) {
        LOG("dir statRet: %{public}d", statRet);
        return -1;
    }

    if (S_ISREG(dirStat.st_mode)) {
        remove(dir);
    } else if (S_ISDIR(dirStat.st_mode)) {
        dirp = opendir(dir);
        while ((dp = readdir(dirp)) != nullptr) {
            if ((strcmp(curDir, dp->d_name) == 0) || (strcmp(upDir, dp->d_name) == 0)) {
                continue;
            }

            std::string dirName = dir;
            dirName += "/";
            dirName += dp->d_name;
            RemoveDir(dirName.c_str());
        }
        closedir(dirp);
        LOG("remove empty dir finally");
        rmdir(dir);
    } else {
        LOG("unknown file type");
    }
    return 0;
}

int CountPicNum(const char *dir, int32_t& picNum)
{
    char curDir[] = ".";
    char upDir[] = "..";
    DIR *dirp;
    struct dirent *dp;
    struct stat dirStat;

    if (access(dir, F_OK) != 0) {
        LOG("can not access dir");
        return picNum;
    }
    int statRet = stat(dir, &dirStat);
    if (statRet < 0) {
        LOG("dir statRet: %{public}d", statRet);
        return picNum;
    }
    if (S_ISREG(dirStat.st_mode)) {
        picNum += 1;
    } else  if (S_ISDIR(dirStat.st_mode)) {
        dirp = opendir(dir);
        while ((dp = readdir(dirp)) != nullptr) {
            if ((strcmp(curDir, dp->d_name) == 0) || (strcmp(upDir, dp->d_name) == 0)) {
                continue;
            }

            std::string dirName = dir;
            dirName += "/";
            dirName += dp->d_name;
            CountPicNum(dirName.c_str(), picNum);
        }
        closedir(dirp);
        LOG("remove empty dir finally");
        return picNum;
    } else {
        LOG("unknown file type");
    }
    return picNum;
}

void WaitRenderServiceInit()
{
    while (true) {
        sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        sptr<IRemoteObject> remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
        if (remoteObject != nullptr) {
            LOG("renderService is inited");
            break;
        } else {
            LOG("renderService is not inited, wait");
            sleep(1);
        }
    }
}
} // namespace OHOS
