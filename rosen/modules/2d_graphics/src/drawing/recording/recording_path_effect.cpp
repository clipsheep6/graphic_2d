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

#include "recording/recording_path_effect.h"

#include "recording/cmd_list_helper.h"
#include "recording/recording_path.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
RecordingPathEffect::RecordingPathEffect() noexcept : cmdList_(std::make_shared<PathEffectCmdList>()) {}

std::shared_ptr<RecordingPathEffect> RecordingPathEffect::CreateDashPathEffect(
    const std::vector<scalar>& intervals, scalar phase)
{
    auto pathEffect = std::make_shared<RecordingPathEffect>();
    auto intervalsData = CmdListHelper::AddVectorToCmdList<scalar>(*pathEffect->GetCmdList(), intervals);

    pathEffect->GetCmdList()->AddOp<CreateDashPathEffectOpItem>(intervalsData, phase);
    return pathEffect;
}

std::shared_ptr<RecordingPathEffect> RecordingPathEffect::CreatePathDashEffect(
    const Path& path, scalar advance, scalar phase, PathDashStyle style)
{
    auto pathEffect = std::make_shared<RecordingPathEffect>();
    auto pathHandle = CmdListHelper::AddRecordedToCmdList<RecordingPath>(*pathEffect->GetCmdList(), path);

    pathEffect->GetCmdList()->AddOp<CreatePathDashEffectOpItem>(pathHandle, advance, phase, style);
    return pathEffect;
}

std::shared_ptr<RecordingPathEffect> RecordingPathEffect::CreateCornerPathEffect(scalar radius)
{
    auto pathEffect = std::make_shared<RecordingPathEffect>();
    pathEffect->GetCmdList()->AddOp<CreateCornerPathEffectOpItem>(radius);
    return pathEffect;
}

std::shared_ptr<RecordingPathEffect> RecordingPathEffect::CreateDiscretePathEffect(
    scalar segLength, scalar dev, uint32_t seedAssist)
{
    auto pathEffect = std::make_shared<RecordingPathEffect>();
    pathEffect->GetCmdList()->AddOp<CreateDiscretePathEffectOpItem>(segLength, dev, seedAssist);
    return pathEffect;
}

std::shared_ptr<RecordingPathEffect> RecordingPathEffect::CreateSumPathEffect(
    const PathEffect& e1, const PathEffect& e2)
{
    auto pathEffect = std::make_shared<RecordingPathEffect>();
    auto pathEffectHandle1 = CmdListHelper::AddRecordedToCmdList<RecordingPathEffect>(*pathEffect->GetCmdList(), e1);
    auto pathEffectHandle2 = CmdListHelper::AddRecordedToCmdList<RecordingPathEffect>(*pathEffect->GetCmdList(), e2);

    pathEffect->GetCmdList()->AddOp<CreateSumPathEffectOpItem>(pathEffectHandle1, pathEffectHandle2);
    return pathEffect;
}

std::shared_ptr<RecordingPathEffect> RecordingPathEffect::CreateComposePathEffect(
    const PathEffect& e1, const PathEffect& e2)
{
    auto pathEffect = std::make_shared<RecordingPathEffect>();
    auto pathEffectHandle1 = CmdListHelper::AddRecordedToCmdList<RecordingPathEffect>(*pathEffect->GetCmdList(), e1);
    auto pathEffectHandle2 = CmdListHelper::AddRecordedToCmdList<RecordingPathEffect>(*pathEffect->GetCmdList(), e2);

    pathEffect->GetCmdList()->AddOp<CreateComposePathEffectOpItem>(pathEffectHandle1, pathEffectHandle2);
    return pathEffect;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
