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

#include "vsync_distributor.h"
#include <algorithm>

namespace OHOS {
namespace Rosen {

VSyncConnection::VSyncConnection(sptr<VSyncDistributor> distributor, std::string name)
    : distributor_(distributor), name_(name)
{
    socketPair_ = new LocalSocketPair();
    socketPair_->CreateChannel(sizeof(int64_t), sizeof(int64_t));
}

VSyncConnection::~VSyncConnection()
{

}

VsyncError VSyncConnection::RequestNextVSync() {
    return VSYNC_ERROR_OK;
}

int32_t VSyncConnection::GetReceiveFd()
{
    return socketPair_->GetReceiveDataFd();
}

void VSyncConnection::PostEvent(int64_t now) {
    socketPair_->SendData(&now, sizeof(int64_t));
}

VSyncDistributor::VSyncDistributor(sptr<VSyncController> controller)
    : controller_(controller), mutex_(), con_(), connections_()
{
    threadLoop_ = std::make_unique<std::thread>(std::bind(&VSyncDistributor::ThreadMain, this));
}

VSyncDistributor::~VSyncDistributor()
{

}

void VSyncDistributor::AddConnection(const sptr<VSyncConnection>& connection)
{
    if (connection != nullptr) {
        std::lock_guard<std::mutex> locker(mutex_);
        connections_.push_back(connection);
    }
}

void VSyncDistributor::RemoveConnection(const sptr<VSyncConnection>& connection)
{
    auto it = find(connections_.begin(), connections_.end(), connection);
    if (it != connections_.end()) {
        connections_.erase(it);
    }
}

void VSyncDistributor::ThreadMain()
{
    int64_t timeStamp;
    while (true) {
        {
            std::unique_lock<std::mutex> locker(mutex_);
            timeStamp = event_.timeStamp;
            while (!timeStamp) {
                con_.wait(locker);
                timeStamp = event_.timeStamp;
            }
            event_.timeStamp = 0;
        }

        std::unique_lock<std::mutex> locker(mutex_);
        if (connections_.empty()) {
            con_.wait(locker);
            continue;
        }

        for (uint32_t i = 0; i < connections_.size(); i++) {
            connections_[i]->PostEvent(timeStamp);
        }
    }
}

void VSyncDistributor::EnableVSync()
{
    if (controller_ != nullptr) {
        controller_->SetCallback(this);
    }
}

void VSyncDistributor::DisableVSync()
{

}

void VSyncDistributor::OnVSyncEvent(int64_t now)
{
    std::lock_guard<std::mutex> locker(mutex_);
    event_.timeStamp = now;
    con_.notify_all();
}
}
}
