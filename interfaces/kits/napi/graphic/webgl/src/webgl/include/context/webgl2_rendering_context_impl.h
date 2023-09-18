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

#ifndef ROSENRENDER_ROSEN_WEBGL2_RENDERING_CONTEXT_IMPL
#define ROSENRENDER_ROSEN_WEBGL2_RENDERING_CONTEXT_IMPL

#include "webgl_rendering_context_base_impl.h"
#include "../../../common/napi/n_exporter.h"

namespace OHOS {
namespace Rosen {
namespace Impl {
class WebGL2RenderingContextImpl : public WebGLRenderingContextBaseImpl {
public:
    explicit WebGL2RenderingContextImpl(int32_t version) : WebGLRenderingContextBaseImpl(version) {}
    ~WebGL2RenderingContextImpl() override {}

private:
    WebGL2RenderingContextImpl(const WebGL2RenderingContextImpl&) = delete;
    WebGL2RenderingContextImpl& operator=(const WebGL2RenderingContextImpl&) = delete;
};
} // Impl
} // namespace Rosen
} // namespace OHOS

#endif // ROSENRENDER_ROSEN_WEBGL2_RENDERING_CONTEXT_IMPL
