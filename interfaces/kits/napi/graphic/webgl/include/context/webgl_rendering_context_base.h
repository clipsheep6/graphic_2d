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

#ifndef ROSENRENDER_ROSEN_WEBGL_RENDERING_CONTEXT_BASE
#define ROSENRENDER_ROSEN_WEBGL_RENDERING_CONTEXT_BASE

#include <GLES2/gl2.h>
#include <GLES3/gl31.h>
#include <GLES2/gl2ext.h>
#include "securec.h"
#include "napi/n_exporter.h"
#include "webgl_rendering_context_basic_base.h"

#ifdef __cplusplus
extern "C" {
#endif

namespace OHOS {
namespace Rosen {
class WebGLRenderingContextBase  {
public:
    static napi_value Uniform3i(napi_env env, napi_callback_info info);

    static napi_value Uniform4i(napi_env env, napi_callback_info info);

    static napi_value ValidateProgram(napi_env env, napi_callback_info info);

    static napi_value VertexAttrib1f(napi_env env, napi_callback_info info);

    static napi_value VertexAttrib2f(napi_env env, napi_callback_info info);

    static napi_value VertexAttrib3f(napi_env env, napi_callback_info info);

    static napi_value VertexAttrib4f(napi_env env, napi_callback_info info);

    static napi_value VertexAttribPointer(napi_env env, napi_callback_info info);

    static napi_value IsFramebuffer(napi_env env, napi_callback_info info);

    static napi_value IsProgram(napi_env env, napi_callback_info info);

    static napi_value IsRenderbuffer(napi_env env, napi_callback_info info);

    static napi_value IsShader(napi_env env, napi_callback_info info);

    static napi_value IsTexture(napi_env env, napi_callback_info info);

    static napi_value LineWidth(napi_env env, napi_callback_info info);

    static napi_value LinkProgram(napi_env env, napi_callback_info info);

    static napi_value PixelStorei(napi_env env, napi_callback_info info);

    static napi_value PolygonOffset(napi_env env, napi_callback_info info);

    static napi_value FrontFace(napi_env env, napi_callback_info info);

    static napi_value GenerateMipmap(napi_env env, napi_callback_info info);

    static napi_value GetActiveAttrib(napi_env env, napi_callback_info info);

    static napi_value GetActiveUniform(napi_env env, napi_callback_info info);

    static napi_value GetAttribLocation(napi_env env, napi_callback_info info);

    static napi_value GetBufferParameter(napi_env env, napi_callback_info info);

    static napi_value GetParameter(napi_env env, napi_callback_info info);

    static napi_value GetError(napi_env env, napi_callback_info info);

    static napi_value GetFramebufferAttachmentParameter(napi_env env, napi_callback_info info);

    static napi_value GetProgramParameter(napi_env env, napi_callback_info info);

    static napi_value GetProgramInfoLog(napi_env env, napi_callback_info info);

    static napi_value GetRenderbufferParameter(napi_env env, napi_callback_info info);

    static napi_value GetShaderParameter(napi_env env, napi_callback_info info);

    static napi_value GetShaderPrecisionFormat(napi_env env, napi_callback_info info);

    static napi_value GetShaderInfoLog(napi_env env, napi_callback_info info);

    static napi_value GetShaderSource(napi_env env, napi_callback_info info);

    static napi_value GetTexParameter(napi_env env, napi_callback_info info);

    static napi_value GetUniform(napi_env env, napi_callback_info info);

    static napi_value GetAttachedShaders(napi_env env, napi_callback_info info);

    static napi_value VertexAttrib1fv(napi_env env, napi_callback_info info);

    static napi_value VertexAttrib2fv(napi_env env, napi_callback_info info);

    static napi_value VertexAttrib3fv(napi_env env, napi_callback_info info);

    static napi_value VertexAttrib4fv(napi_env env, napi_callback_info info);

    static napi_value GetVertexAttrib(napi_env env, napi_callback_info info);

    static napi_value GetDrawingBufferWidth(napi_env env, napi_callback_info info);
    static napi_value GetDrawingBufferHeight(napi_env env, napi_callback_info info);

    static napi_value GetFramebufferAttachmentObjName(
        napi_env env, napi_value thisVar, GLenum target, GLenum attachment, GLenum pname);

    static void GetRenderingContextBasePropertyDesc(std::vector<napi_property_descriptor> &props);

    static void SetError(napi_env env, napi_value thisVar, GLenum error, std::string func, int line);
};
} // namespace Rosen
} // namespace OHOS

#ifdef __cplusplus
}
#endif

#endif // ROSENRENDER_ROSEN_WEBGL_RENDERING_CONTEXT_BASE
