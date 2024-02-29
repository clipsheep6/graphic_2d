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

#include "drawable/rs_property_drawable_ng.h"

#include "drawable/rs_property_drawable_content.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen {

RSPropertyDrawableNG::RSPropertyDrawableNG(std::shared_ptr<const RSPropertyDrawableContent> cmdList)
    : cmdList_(std::move(cmdList))
{}

void RSPropertyDrawableNG::OnDraw(RSPaintFilterCanvas& canvas) const
{
    if (cmdList_ == nullptr) {
        // empty draw cmd should be filter out during OnGenerate and OnUpdate, we should not reach here
        ROSEN_LOGE("RSPropertyDrawableNG::OnDraw, cmdList_ is null");
        return;
    }
    const auto& drawCmdList = cmdList_->drawCmdList_;
    if (drawCmdList == nullptr) {
        return;
    }
    drawCmdList->Playback(canvas);
}

} // namespace OHOS::Rosen
