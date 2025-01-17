/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_UI_SHARE_CONTEXT_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_UI_SHARE_CONTEXT_H

#ifdef ROSEN_IOS
#include "render_context/render_context_egl_defines.h"
#else
#include "EGL/egl.h"
#endif
#include "common/rs_common_def.h"

namespace OHOS {
namespace Rosen {
class RSC_EXPORT RSUIShareContext final {
public:
    static RSUIShareContext& GetInstance();
    ~RSUIShareContext() = default;
    EGLContext GetRsRenderContext() const;
private:
    RSUIShareContext() = default;
    RSUIShareContext(const RSUIShareContext&) = delete;
    RSUIShareContext& operator=(const RSUIShareContext&) = delete;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_UI_SHARE_CONTEXT_H