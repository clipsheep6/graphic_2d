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

#include "sync_fence.h"

#include <unistd.h>
#include <errno.h>
#include <securec.h>

#include "hilog/log.h"

namespace OHOS {
using namespace OHOS::HiviewDFX;

namespace {
constexpr HiLogLabel LABEL = { LOG_CORE, 0xD001400, "SyncFence" };
constexpr int32_t INVALID_FD = -1;
constexpr int64_t INVALID_TIMESTAMP = -1;

#define SYNC_IOC_FILE_INFO _IOWR(SYNC_IOC_MAGIC, 4, struct SyncFileInfo)
}  // namespace

const sptr<SyncFence> SyncFence::INVALID_FENCE = sptr<SyncFence>(new SyncFence(INVALID_FD));

SyncFence::SyncFence(int32_t fenceFd) : fenceFd_(fenceFd)
{
    HiLog::Debug(LABEL, "%{public}s fenceFd: %{public}d", __func__, fenceFd_.Get());
}

SyncFence::~SyncFence()
{
}

int32_t SyncFence::Wait(uint32_t timeout)
{
    (void)timeout;
    return 0;
}

sptr<SyncFence> SyncFence::MergeFence(const std::string &name,
                const sptr<SyncFence>& fence1, const sptr<SyncFence>& fence2)
{
    (void)name;
    (void)fence1;
    (void)fence2;
    return INVALID_FENCE;
}

int64_t SyncFence::SyncFileReadTimestamp()
{
    return INVALID_TIMESTAMP;
}

int32_t SyncFence::Dup() const
{
    return ::dup(fenceFd_);
}

int32_t SyncFence::Get() const
{
    return fenceFd_;
}

} // namespace OHOS
