/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "rs_frame_rate_linker.h"

#include "command/rs_frame_rate_linker_command.h"
#include "platform/common/rs_log.h"
#include "sandbox_utils.h"
#include "transaction/rs_transaction_proxy.h"
#include "transaction/rs_interfaces.h"

namespace OHOS {
namespace Rosen {
static bool g_isUniRenderEnabled = false;
FrameRateLinkerId RSFrameRateLinker::GenerateId()
{
    static pid_t pid_ = GetRealPid();
    static std::atomic<uint32_t> currentId_ = 0;

    auto currentId = currentId_.fetch_add(1, std::memory_order_relaxed);
    if (currentId == UINT32_MAX) {
        ROSEN_LOGE("RSFrameRateLinker Id overflow");
    }

    // concat two 32-bit numbers to one 64-bit number
    return ((FrameRateLinkerId)pid_ << 32) | (currentId);
}

std::shared_ptr<RSFrameRateLinker> RSFrameRateLinker::Create()
{
    std::shared_ptr<RSFrameRateLinker> linker(new RSFrameRateLinker());
    ROSEN_LOGI("RSFrameRateLinker Create %{public}" PRIu64, linker->GetId());
    return linker;
}

RSFrameRateLinker::RSFrameRateLinker() : id_(GenerateId())
{
    InitUniRenderEnabled();
}

RSFrameRateLinker::~RSFrameRateLinker()
{
}

FrameRateLinkerId RSFrameRateLinker::GetId() const
{
    return id_;
}

bool RSFrameRateLinker::IsUniRenderEnabled() const
{
    return g_isUniRenderEnabled;
}

void RSFrameRateLinker::UpdateFrameRateRange(const FrameRateRange& range)
{
    if (currentRange_ != range) {
        currentRange_ = range;
        std::unique_ptr<RSCommand> command = std::make_unique<RSFrameRateLinkerUpdateRange>(GetId(), range);
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->AddCommand(command, IsUniRenderEnabled());
        }
    }
}

void RSFrameRateLinker::UpdateFrameRateRangeImme(const FrameRateRange& range)
{
    if (currentRange_ != range) {
        currentRange_ = range;
        RSInterfaces::GetInstance().SyncFrameRateRange(range);
    }
}

void RSFrameRateLinker::SetEnable(bool enabled)
{
    isEnabled_ = enabled;
}

bool RSFrameRateLinker::IsEnable()
{
    return isEnabled_;
}

void RSFrameRateLinker::InitUniRenderEnabled()
{
    static bool inited = false;
    if (!inited) {
        inited = true;
        g_isUniRenderEnabled = RSSystemProperties::GetUniRenderEnabled();
        ROSEN_LOGE("RSFrameRateLinker::InitUniRenderEnabled:%{public}d", g_isUniRenderEnabled);
    }
}
} // namespace Rosen
} // namespace OHOS
