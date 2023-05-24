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

#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_RS_SHADER_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_RS_SHADER_H

#ifndef USE_ROSEN_DRAWING
#include "include/core/SkShader.h"
#else
#include "effect/shader_effect.h"
#endif

#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSShader {
public:
    static std::shared_ptr<RSShader> CreateRSShader();
#ifndef USE_ROSEN_DRAWING
    static std::shared_ptr<RSShader> CreateRSShader(const sk_sp<SkShader>& skShader);

    void SetSkShader(const sk_sp<SkShader>& skShader);
    const sk_sp<SkShader>& GetSkShader() const;
#else
    static std::shared_ptr<RSShader> CreateRSShader(const std::shared_ptr<Drawing::ShaderEffect>& drShader);

    void SetDrawingShader(const std::shared_ptr<Drawing::ShaderEffect>& drShader);
    const std::shared_ptr<Drawing::ShaderEffect>& GetDrawingShader() const;
#endif
    ~RSShader() = default;

private:
    RSShader() = default;
    RSShader(const RSShader&) = delete;
    RSShader(const RSShader&&) = delete;
    RSShader& operator=(const RSShader&) = delete;
    RSShader& operator=(const RSShader&&) = delete;

#ifndef USE_ROSEN_DRAWING
    sk_sp<SkShader> skShader_ = nullptr;
#else
    std::shared_ptr<Drawing::ShaderEffect> drShader_ = nullptr;
#endif
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_SHADER_H
