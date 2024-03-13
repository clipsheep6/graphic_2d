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

#include "params/rs_display_render_params.h"

namespace OHOS::Rosen {
RSDisplayRenderParams::RSDisplayRenderParams() {}

std::vector<RSBaseRenderNode::SharedPtr>& RSDisplayRenderParams::GetAllMainAndLeashSurfaces()
{
    return allMainAndLeashSurfaces_;
}

void RSDisplayRenderParams::SetAllMainAndLeashSurfaces(
    std::vector<RSBaseRenderNode::SharedPtr>& allMainAndLeashSurfaces)
{
    std::swap(allMainAndLeashSurfaces_, allMainAndLeashSurfaces);
}

void RSDisplayRenderParams::OnSync(const std::unique_ptr<RSRenderParams>& target)
{
    auto targetDisplayParams = static_cast<RSDisplayRenderParams*>(target.get());
    if (targetDisplayParams == nullptr) {
        RS_LOGE("RSDisplayRenderParams::OnSync targetDisplayParams is nullptr");
        return;
    }
    targetDisplayParams->SetAllMainAndLeashSurfaces(allMainAndLeashSurfaces_);
    RSRenderParams::OnSync(target);
}

} // namespace OHOS::Rosen
