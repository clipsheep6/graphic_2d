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

#include "graphic_dumper_helper_impl.h"

#include <chrono>
#include <cstdarg>
#include <vector>

#include <iservice_registry.h>
#include <securec.h>
#include <system_ability_definition.h>
#include <unistd.h>

#include "graphic_dumper_hilog.h"
#include "ipc/igraphic_dumper_command.h"

#define RET_IF_NOT_OK(ret)    \
    do {                      \
        if (ret != GD_OK) {   \
            return ret;       \
        }                     \
    } while(0)                \

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "GraphicDumperHelperImpl" };
constexpr int INFO_SIZE_MAX = 4096;
}

GraphicDumperClientListener::GraphicDumperClientListener(sptr<GraphicDumperHelperImpl>& helper)
    : helperImpl_(helper)
{
}

void GraphicDumperClientListener::OnConfigChange(const std::string &tag, const std::string &val)
{
    GDLOGI("%{public}s -> %{public}s", tag.c_str(), val.c_str());
    if (helperImpl_) {
        helperImpl_->DispenseOnConfigChange(tag, val);
    }
}

void GraphicDumperClientListener::OnDump(const std::string &tag)
{
    if (helperImpl_) {
        std::string flag = "*#dp#*.";
        if (tag.find(flag) == 0) {
            std::string sTag = tag.substr(flag.length());
            helperImpl_->DispenseOnDump(sTag);
        }
    }
}

sptr<GraphicDumperHelper> GraphicDumperHelperImpl::GetInstance()
{
    if (currentHelper == nullptr) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> guard(mutex);
        if (currentHelper == nullptr) {
            currentHelper = new GraphicDumperHelperImpl();
        }
    }
    return currentHelper;
}

GraphicDumperHelperImpl::GraphicDumperHelperImpl()
{
}

void GraphicDumperHelperImpl::SetNoopInstance()
{
        static std::mutex mutex;
        std::lock_guard<std::mutex> guard(mutex);
        currentHelper = new GraphicDumperHelperNoop();
}

GDError GraphicDumperHelperImpl::Init()
{
    std::lock_guard<std::mutex> lock(initMutex_);
    if (serverConnected) {
        return GD_OK;
    }
    if (access("/data/gdumper_enable", F_OK) != 0) {
        SetNoopInstance();
        return GD_ERROR_NOT_SUPPORT;
    }

    auto now = std::chrono::steady_clock::now().time_since_epoch();
    auto nowTime = (int64_t)std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
    constexpr int64_t reconnectTime = 1000;
    if ((nowTime - requestConnectTime) < reconnectTime) {
        return GD_ERROR_OUT_OF_RANGE;
    }
    requestConnectTime = nowTime;
    GDError ret = InitSA(GRAPHIC_DUMPER_SERVICE_SA_ID);
    if (ret == GD_OK) {
        {
            std::lock_guard<std::mutex> guard(onConfigChangeMutex_);
            for (const auto& iter : onConfigChangeMap_) {
                ret = AddClientListener(iter.first);
                RET_IF_NOT_OK(ret);
            }
        }

        {
            std::lock_guard<std::mutex> guard(onDumperFuncsMutex_);
            for (const auto& iter : onDumpFuncsMap_) {
                ret = AddClientListener(iter.first);
                RET_IF_NOT_OK(ret);
            }
        }

        {
            std::lock_guard<std::mutex> guard(cacheInfoMutex_);
            for (const auto& infoStruct : cacheInfo_) {
                ret = service_->SendInfo(infoStruct.tag, infoStruct.info);
                GDLOG_SUCCESS("SendInfo is %{public}d", ret);
            }
            cacheInfo_.clear();
            cacheInfo_.shrink_to_fit();
        }
    }
    return ret;
}

GDError GraphicDumperHelperImpl::InitSA(int32_t systemAbilityId)
{
    if (serverConnected) {
        return GD_OK;
    }

    auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sam == nullptr) {
        GDLOG_FAILURE_RET(GD_ERROR_SAMGR);
    }

    auto remoteObject = sam->GetSystemAbility(systemAbilityId);
    if (remoteObject == nullptr) {
        GDLOG_FAILURE_RET(GD_ERROR_SERVICE_NOT_FOUND);
    }

    sptr<IRemoteObject::DeathRecipient> deathRecipient = new GDumperServiceDeathRecipient();
    if ((remoteObject->IsProxyObject()) && (!remoteObject->AddDeathRecipient(deathRecipient))) {
        GDLOGFE("Failed to add death recipient");
    }

    service_ = iface_cast<IGraphicDumperService>(remoteObject);
    if (service_ == nullptr) {
        GDLOG_FAILURE_RET(GD_ERROR_PROXY_NOT_INCLUDE);
    }
    serverConnected = true;
    GDLOG_SUCCESS("service_ = iface_cast");
    return GD_OK;
}

GDError GraphicDumperHelperImpl::AddClientListener(const std::string &tag)
{
    if (Init() != GD_OK) {
        return GD_ERROR_SERVICE_NOT_FOUND;
    }

    if (listener_ == nullptr) {
        std::lock_guard<std::mutex> guard(clientListenerMutex_);
        if (listener_ == nullptr) {
            sptr<GraphicDumperHelperImpl> helper = this;
            listener_ = new GraphicDumperClientListener(helper);
        }
    }
    return service_->AddClientListener(tag, listener_);
}

GDError GraphicDumperHelperImpl::SendInfo(const std::string &tag, const char *fmt, ... )
{
    GDLOGFE("SendInfo is start");
    char info[INFO_SIZE_MAX]; // clear by vsnprintf
    {
        static std::mutex sprintfMutex_;
        std::lock_guard<std::mutex> lock(sprintfMutex_);

        va_list args;
        va_start(args, fmt);
        int ret = vsnprintf_s(info, sizeof(info), (sizeof(info) - 1), fmt, args);
        GDLOGFE("SendInfo is ret = %{public}d",ret);
        va_end(args);
        if (ret < 0) {
            return GD_ERROR_INVALID_PARAM;
        }
    }

    GDError retInit = Init();
    GDLOGFE("SendInfo is retInit = %{public}d",retInit);
    if (retInit == GD_OK) {
        return service_->SendInfo(tag, std::string(info));
        GDLOGFE("SendInfo is ok");
    } else if (retInit == GD_ERROR_NOT_SUPPORT) {
        return retInit;
    } else {
        struct InfoStruct infoStruct = {
            .tag = tag,
            .info = info,
        };
        std::lock_guard<std::mutex> guard(cacheInfoMutex_);
        cacheInfo_.push_back(infoStruct);
        return GD_OK;
    }
}

int32_t GraphicDumperHelperImpl::AddConfigChangeListener(const std::string &tag, OnConfigChangeFunc func)
{
    {
        std::lock_guard<std::mutex> guard(onConfigChangeMutex_);
        if (onConfigChangeMap_.find(tag) != onConfigChangeMap_.end()) {
            onConfigChangeMap_[tag]->insert({ ++onConfigChangeFuncId_, func });
            return onConfigChangeFuncId_;
        }
    }
    int ret = AddClientListener(tag);
    if (ret != GD_OK) {
        return 0;
    }
    auto onConfigChangeFuncs = std::make_unique<std::map<int32_t, OnConfigChangeFunc>>();
    onConfigChangeFuncs->insert({ ++onConfigChangeFuncId_, func });
    onConfigChangeMap_[tag] = std::move(onConfigChangeFuncs);
    return onConfigChangeFuncId_;
}

GDError GraphicDumperHelperImpl::RemoveConfigChangeListener(const int32_t listenerId)
{
    std::lock_guard<std::mutex> guard(onConfigChangeMutex_);
    for (auto& [k, v] : onConfigChangeMap_) {
        if (v->find(listenerId) != v->end()) {
            v->erase(listenerId);
            return GD_OK;
        }
    }
    return GD_ERROR_INVALID_PARAM;
}

int32_t GraphicDumperHelperImpl::AddDumpListener(const std::string &tag, OnDumpFunc func)
{
    std::lock_guard<std::mutex> guard(onDumperFuncsMutex_);
    if (onDumpFuncsMap_.find(tag) != onDumpFuncsMap_.end()) {
        onDumpFuncsMap_[tag]->insert({ ++onDumperFuncId_, func });
        return onDumperFuncId_;
    }
    auto dumpTag = "*#dp#*." + tag;
    int ret = AddClientListener(dumpTag);
    if (ret != GD_OK) {
        return 0;
    }
    auto onDumpFuncs = std::make_unique<std::map<int32_t, OnDumpFunc>>();
    onDumpFuncs->insert({ ++onDumperFuncId_, func });
    onDumpFuncsMap_[tag] = std::move(onDumpFuncs);
    return onDumperFuncId_;
}

GDError GraphicDumperHelperImpl::RemoveDumpListener(const int32_t listenerId)
{
    std::lock_guard<std::mutex> guard(onDumperFuncsMutex_);
    for (auto& [k, v] : onDumpFuncsMap_) {
        if (v->find(listenerId) != v->end()) {
            v->erase(listenerId);
            return GD_OK;
        }
    }
    return GD_ERROR_INVALID_PARAM;
}

void GraphicDumperHelperImpl::SetConnectState(bool state)
{
    GDLOGFI("");
    serverConnected = state;
}

void GraphicDumperHelperImpl::DispenseOnConfigChange(const std::string &tag, const std::string &val)
{
    GDLOGI("%{public}s -> %{public}s", tag.c_str(), val.c_str());
    std::lock_guard<std::mutex> guard(onConfigChangeMutex_);
    if (onConfigChangeMap_.empty()) {
        return;
    }
    if (!onConfigChangeMap_[tag]->empty()) {
        for (const auto &[id, func] : *onConfigChangeMap_[tag]) {
            func(tag, val);
        }
    }
}

void GraphicDumperHelperImpl::DispenseOnDump(const std::string &tag)
{
    std::lock_guard<std::mutex> guard(onDumperFuncsMutex_);
    if (onDumpFuncsMap_.empty()) {
        return;
    }

    if (tag == "--all") {
        for (const auto& [t, mapPtr] : onDumpFuncsMap_) {
            if (mapPtr != nullptr) {
                for (const auto &[id, func] : *mapPtr) {
                    func();
                }
            }
        }
        return;
    }

    if (onDumpFuncsMap_.find(tag) != onDumpFuncsMap_.end()) {
        if (onDumpFuncsMap_[tag] != nullptr) {
            for (const auto &[id, func] : *onDumpFuncsMap_[tag]) {
                func();
            }
        }
    }
}

void GDumperServiceDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &object)
{
    auto helper = GraphicDumperHelperImpl::GetInstance();
    auto helperImpl = reinterpret_cast<GraphicDumperHelperImpl *>(helper.GetRefPtr());
    helperImpl->SetConnectState(false);
}
} // namespace OHOS
