/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "pipeline/rs_vblank_idle_corrector.h"

#include "hgm_core.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"
#include "screen_manager/rs_screen_manager.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr uint32_t IDLE_FRAME_COUNT_THRESHOLD = 2;
};

void RSVBlankIdleCorrector::SetScreenVBlankIdle(ScreenId id)
{
    bool isCorrectorEnabled = HgmCore::Instance().IsVBlankIdleCorrectEnabled();
    if (isCorrectorEnabled) {
        currIdleScreenId_ = id;
        idleFrameCount_ = IDLE_FRAME_COUNT_THRESHOLD;
        isVBlankIdle_ = true;
    }
}

void RSVBlankIdleCorrector::ProcessScreenConstraint(uint64_t timestamp, uint64_t constraintRelativeTime)
{
    auto screenManager = CreateOrGetScreenManager();
    if (screenManager == nullptr) {
        RS_LOGE("RSHardwareThread CreateOrGetScreenManager fail.");
        return;
    }

    auto defaultScreenId = screenManager->GetDefaultScreenId();
    bool isCorrectorEnabled = HgmCore::Instance().IsVBlankIdleCorrectEnabled();
    if (!isCorrectorEnabled) {
        idleFrameCount_ = 0;
        isVBlankIdle_ = false;
        screenManager->SetScreenConstraint(defaultScreenId, 0, ScreenConstraintType::CONSTRAINT_NONE);
        return;
    }

    if (isVBlankIdle_) {
        uint64_t pipelineOffset = static_cast<uint64_t>(HgmCore::Instance().GetPipelineOffset());
        if (idleFrameCount_ > 0) {
            uint64_t absoluteTime = timestamp + pipelineOffset;
            screenManager->SetScreenConstraint(currIdleScreenId_,
                absoluteTime, ScreenConstraintType::CONSTRAINT_ABSOLUTE);
        } else {
            screenManager->SetScreenConstraint(currIdleScreenId_, 0, ScreenConstraintType::CONSTRAINT_NONE);
        }
        idleFrameCount_--;
        if (idleFrameCount_ < 0) {
            currIdleScreenId_ = defaultScreenId;
            idleFrameCount_ = 0;
            isVBlankIdle_ = false;
        }
    } else if (constraintRelativeTime > 0) {
        screenManager->SetScreenConstraint(defaultScreenId,
            constraintRelativeTime, ScreenConstraintType::CONSTRAINT_RELATIVE);
    } else {
        screenManager->SetScreenConstraint(defaultScreenId, 0, ScreenConstraintType::CONSTRAINT_NONE);
    }
}
}
}
