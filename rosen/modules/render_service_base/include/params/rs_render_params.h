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

#ifndef RENDER_SERVICE_BASE_PARAMS_RS_RENDER_PARAMS_H
#define RENDER_SERVICE_BASE_PARAMS_RS_RENDER_PARAMS_H

#include "common/rs_rect.h"
#include "property/rs_properties.h"
#include "utils/matrix.h"

namespace OHOS::Rosen {
class RSB_EXPORT RSRenderParams {
public:
    RSRenderParams() = default;
    virtual ~RSRenderParams() = default;

    virtual void SetMatrix(Drawing::Matrix matrix);
    const Drawing::Matrix GetMatrix() const;

    virtual void SetBoundsRect(Drawing::RectF boundsRect);
    const Drawing::Rect GetBounds() const;

    virtual void SetLocalDrawRect(RectI localDrawRect);
    const RectI GetLocalDrawRect() const;

    bool GetShouldPaint() const;
    void SetShouldPaint(bool shouldPaint);

    bool NeedSync() const;
    void SetNeedSync(bool needSync);

    // disable copy and move
    RSRenderParams(const RSRenderParams&) = delete;
    RSRenderParams(RSRenderParams&&) = delete;
    RSRenderParams& operator=(const RSRenderParams&) = delete;
    RSRenderParams& operator=(RSRenderParams&&) = delete;

    virtual void OnSync(const std::unique_ptr<RSRenderParams>& target);
protected:
    bool needSync_ = false;

private:
    Drawing::Matrix matrix_;
    Drawing::RectF boundsRect_;
    // this rect should map display coordination
    RectI localDrawRect_;
    bool shouldPaint_;
};
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_PARAMS_RS_RENDER_PARAMS_H
