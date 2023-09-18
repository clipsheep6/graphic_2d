/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef ROSENRENDER_ROSEN_WEBGL_RENDERING_CONTEXT_BASIC_BASE
#define ROSENRENDER_ROSEN_WEBGL_RENDERING_CONTEXT_BASIC_BASE

#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <map>

#include "../../../common/napi/n_exporter.h"
#include "../canvas_render_context_base.h"
#include "webgl_context_attributes.h"

#ifdef __cplusplus
extern "C" {
#endif

namespace OHOS {
namespace Rosen {
namespace Impl {
class WebGLRenderingContextBaseImpl;
}

class WebGLRenderingContextBasicBase : public OHOS::Ace::CanvasRenderContextBase {
public:
    WebGLRenderingContextBasicBase() {};

    virtual ~WebGLRenderingContextBasicBase()
    {
        if (webGlContextAttributes != nullptr) {
            delete webGlContextAttributes;
        }
    };

    void SetEglWindow(void *window);

    void Create(void *context) override;

    void Init() override;

    void Attach(uint64_t textureId) override;

    void Update() override;

    void Detach() override;

    void SetBitMapPtr(char *bitMapPtr, int bitMapWidth, int bitMapHeight) override;

    uint64_t CreateTexture() override;

    void SetTexture(uint64_t id);

    void SetUpdateCallback(std::function<void()>) override;

    napi_value GetContextInstance(napi_env env, std::string className,
        napi_callback constructor, napi_finalize finalize_cb);

    virtual Impl::WebGLRenderingContextBaseImpl &GetWebGLRenderingContextImpl() = 0;
public:
    void drawImg();
    GLuint compileShader(GLenum type, const char *sources);
    int useProgram();
    EGLSurface mEGLSurface = nullptr;
    NativeWindow *mEglWindow = nullptr;
    WebGLContextAttributes *webGlContextAttributes = nullptr;
    char *mBitMapPtr = nullptr;
    int mBitMapWidth = 0;
    int mBitMapHeight = 0;
    std::function<void()> mUpdateCallback;
    napi_ref mContextRef = nullptr;

    std::map<GLenum, GLuint> queryMaps;
};
} // namespace Rosen
} // namespace OHOS

// OHOS::Rosen::WebGLRenderingContextBasicBase *GetWebGLInstance(std::string id);

#ifdef __cplusplus
}
#endif

#endif // ROSENRENDER_ROSEN_WEBGL_RENDERING_CONTEXT_BASIC_BASE
