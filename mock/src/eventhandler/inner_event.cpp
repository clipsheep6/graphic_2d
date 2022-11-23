/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "inner_event.h"

#include <condition_variable>
#include <mutex>
#include <vector>
#include "singleton.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
class WaiterImp final : public InnerEvent::Waiter {
public:
    WaiterImp(){};
    ~WaiterImp() override{};

    void Wait() final
    {
    }

    void Notify() final
    {
    }

private:
    std::mutex mutex_;
    std::condition_variable condition_;
    uint32_t waitingCount_ {0};
    bool finished_ {false};
};
}  // unnamed namespace

// Implementation for event pool.
class InnerEventPool : public DelayedRefSingleton<InnerEventPool> {

public:

    InnerEvent::Pointer Get()
    {
      return InnerEvent::Pointer(nullptr, nullptr);
    }

private:
    static void Drop(InnerEvent *event)
    {
    }

    void Put(InnerEvent::Pointer &&event)
    {
    }

    static const size_t MAX_BUFFER_POOL_SIZE = 64;

    std::mutex poolLock_;
    std::vector<InnerEvent::Pointer> events_;

    // Used to statistical peak value of count of using inner events.
    size_t usingCount_ {0};
    size_t nextPeakUsingCount_ {MAX_BUFFER_POOL_SIZE};
};

InnerEvent::Pointer InnerEvent::Get()
{
    return InnerEvent::Pointer(nullptr, nullptr);
}

InnerEvent::Pointer InnerEvent::Get(uint32_t innerEventId, int64_t param)
{
    return InnerEvent::Pointer(nullptr, nullptr);
}

InnerEvent::Pointer InnerEvent::Get(const Callback &callback, const std::string &name)
{
    return InnerEvent::Pointer(nullptr, nullptr);
}

void InnerEvent::ClearEvent()
{

}

void InnerEvent::WarnSmartPtrCastMismatch()
{
}

const std::shared_ptr<InnerEvent::Waiter> &InnerEvent::CreateWaiter()
{
    return nullptr;
}

bool InnerEvent::HasWaiter() const
{
    return false;
}

const std::shared_ptr<HiTraceId> InnerEvent::GetOrCreateTraceId()
{
    return nullptr;
}

const std::shared_ptr<HiTraceId> InnerEvent::GetTraceId()
{
    return nullptr;
}

std::string InnerEvent::Dump()
{
    return {};
}
}  // namespace AppExecFwk
}  // namespace OHOS
