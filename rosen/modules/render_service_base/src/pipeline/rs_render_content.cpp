/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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

#include "pipeline/rs_render_content.h"
#include "pipeline/rs_recording_canvas.h"

namespace OHOS {
namespace Rosen {
RSRenderContent::RSRenderContent() = default;

RSProperties& RSRenderContent::GetMutableRenderProperties()
{
    return renderProperties_;
}

const RSProperties& RSRenderContent::GetRenderProperties() const
{
    return renderProperties_;
}

void RSRenderContent::DrawPropertyDrawable(RSPropertyDrawableSlot index, RSPaintFilterCanvas& canvas) const
{
    auto& drawablePtr = propertyDrawablesVec_[static_cast<size_t>(index)];
    if (!drawablePtr) {
        return;
    }
    auto recordingCanvas = static_cast<RSRecordingCanvas*>(canvas.GetRecordingCanvas());
    if (recordingCanvas) {
        recordingCanvas->DrawPropertyDrawable(shared_from_this(), index);
    } else {
        drawablePtr->Draw(*this, canvas);
    }
}

void RSRenderContent::DrawPropertyDrawableRange(
    RSPropertyDrawableSlot begin, RSPropertyDrawableSlot end, RSPaintFilterCanvas& canvas) const
{
    if (auto recordingCanvas = static_cast<RSRecordingCanvas*>(canvas.GetRecordingCanvas())) {
        recordingCanvas->DrawPropertyDrawableRange(shared_from_this(), begin, end);
        return;
    }
    for (auto index = static_cast<size_t>(begin); index <= static_cast<size_t>(end); index++) {
        if (auto& drawablePtr = propertyDrawablesVec_[index]) {
            drawablePtr->Draw(*this, canvas);
        }
    }
}

RSRenderNodeType RSRenderContent::GetType() const
{
    return type_;
}
} // namespace Rosen
} // namespace OHOS
