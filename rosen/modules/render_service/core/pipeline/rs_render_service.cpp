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

#include "rs_render_service.h"
#include "rs_main_thread.h"
#include "rs_render_service_connection.h"

#include <unordered_set>
#include <unistd.h>

#include <iservice_registry.h>
#include <platform/common/rs_log.h>
#include <system_ability_definition.h>

namespace OHOS {
namespace Rosen {
RSRenderService::RSRenderService() {}

RSRenderService::~RSRenderService() noexcept {}

bool RSRenderService::Init()
{
    screenManager_ = CreateOrGetScreenManager();
    if (screenManager_ == nullptr || !screenManager_->Init()) {
        ROSEN_LOGE("RSRenderService CreateOrGetScreenManager fail.");
        return false;
    }

    mainThread_ = RSMainThread::Instance();
    if (mainThread_ == nullptr) {
        return false;
    }
    mainThread_->Init();
 
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        ROSEN_LOGE("RSRenderService GetSystemAbilityManager fail.");
        return false;
    }
    samgr->AddSystemAbility(RENDER_SERVICE, this);

    return true;
}

void RSRenderService::Run()
{
    ROSEN_LOGE("RSRenderService::Run");
    mainThread_->Start();
}

sptr<RSIRenderServiceConnection> RSRenderService::CreateConnection(const sptr<RSIConnectionToken>& token)
{
    auto tokenObj = token->AsObject();
    sptr<RSIRenderServiceConnection> newConn(
        new RSRenderServiceConnection(this, mainThread_, screenManager_, tokenObj));

    sptr<RSIRenderServiceConnection> tmp;
    std::unique_lock<std::mutex> lock(mutex_);
    // if connections_ has the same token one, replace it.
    if (connections_.count(tokenObj) > 0) {
        tmp = connections_.at(tokenObj);
    }
    connections_[tokenObj] = newConn;
    lock.unlock();

    return newConn;
}

void RSRenderService::RemoveConnection(sptr<IRemoteObject> token)
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (connections_.count(token) == 0) {
        return;
    }

    auto tmp = connections_.at(token);
    connections_.erase(token);
    lock.unlock();
}

int RSRenderService::Dump(int fd, const std::vector<std::u16string>& args)
{
    std::unordered_set<std::u16string> argSets;
    std::u16string arg1(u"display");
    std::u16string arg2(u"surface");
    for (decltype(args.size()) index = 0; index < args.size(); ++index) {
        argSets.insert(args[index]);
    }
    std::string dumpString;
    if (screenManager_ == nullptr) {
        return OHOS::INVALID_OPERATION;
    }
    if (args.size() == 0 || argSets.count(arg1) != 0) {
        screenManager_->DisplayDump(dumpString);
    }
    if (args.size() == 0 || argSets.count(arg2) != 0) {
        mainThread_->ScheduleTask([this, &dumpString]() {
            return screenManager_->SurfaceDump(dumpString);
        }).wait();
    }
    if (dumpString.size() == 0) {
        return OHOS::INVALID_OPERATION;
    }
    write(fd, dumpString.c_str(), dumpString.size());
    return OHOS::NO_ERROR;
}
} // namespace Rosen
} // namespace OHOS
