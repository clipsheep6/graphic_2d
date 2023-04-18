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

#include "rs_surface_frame_ohos_gl.h"
#include "platform/common/rs_log.h"

#include <hilog/log.h>
#include "pipeline/rs_render_thread.h"

namespace OHOS {
namespace Rosen {


void RSSurfaceFrameOhosGl::SetDamageRegion(int32_t left, int32_t top, int32_t width, int32_t height)
{
    renderProxy_->DamageFrame(left, top, width, height);
}

void RSSurfaceFrameOhosGl::SetDamageRegion(const std::vector<RectI> &rects)
{
    renderProxy_->DamageFrame(rects);
}

int32_t RSSurfaceFrameOhosGl::GetBufferAge() const
{
    return renderProxy_->GetBufferAge();
}

} // namespace Rosen
} // namespace OHOS
