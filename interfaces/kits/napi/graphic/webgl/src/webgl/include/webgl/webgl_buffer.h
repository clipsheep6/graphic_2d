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

#ifndef ROSENRENDER_ROSEN_WEBGL_BUFFER
#define ROSENRENDER_ROSEN_WEBGL_BUFFER

#include "../../../common/napi/n_exporter.h"
#include "webgl_object.h"
#include "webgl_arg.h"

namespace OHOS {
namespace Rosen {
class WebGLBuffer final : public NExporter, public WebGLObject {
public:
    inline static const std::string className = "WebGLBuffer";
    inline static const int objectType = WEBGL_OBJECT_BUFFER;
    inline static const int DEFAULT_BUFFER = 0;

    bool Export(napi_env env, napi_value exports) override;

    std::string GetClassName() override;

    static napi_value Constructor(napi_env env, napi_callback_info info);

    void SetBufferId(uint32_t bufferId)
    {
        m_bufferId = bufferId;
    }

    uint32_t GetBufferId() const
    {
        return m_bufferId;
    }

    explicit WebGLBuffer() : m_bufferId(0) {};

    WebGLBuffer(napi_env env, napi_value exports) : NExporter(env, exports), m_bufferId(0) {};

    ~WebGLBuffer();

    void SaveBuffer(WebGLReadBufferArg *buffer) { m_bufferData = buffer; };
    WebGLReadBufferArg *GetBufferData() { return m_bufferData; }

    GLenum GetTarget() const { return m_target; }
    void SetTarget(GLenum target) { m_target = target; }

    static napi_value GetWebGLBufferObj(napi_env env, napi_value thisVar, uint32_t bufferId);

    static NVal CreateObjectInstance(napi_env env, WebGLBuffer **instance)
    {
        return WebGLObject::CreateObjectInstance<WebGLBuffer>(env, instance);
    }
    static WebGLBuffer *GetObjectInstance(napi_env env, napi_value obj)
    {
        return WebGLObject::GetObjectInstance<WebGLBuffer>(env, obj);
    }
private:
    WebGLReadBufferArg *m_bufferData { nullptr };
    uint32_t m_bufferId { 0 };
    GLenum m_target { 0 };
    // float m_params;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSENRENDER_ROSEN_WEBGL_BUFFER
