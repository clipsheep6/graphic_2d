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

#ifndef ROSENRENDER_ROSEN_WEBGL_QUERY
#define ROSENRENDER_ROSEN_WEBGL_QUERY

#include "napi/n_exporter.h"
#include "webgl_object.h"

namespace OHOS {
namespace Rosen {
class WebGLQuery final : public NExporter, WebGLObject {
public:
    inline static const std::string className = "WebGLQuery";
    inline static const int objectType = WEBGL_OBJECT_QUERY;

    bool Export(napi_env env, napi_value exports) override;

    std::string GetClassName() override;

    static napi_value Constructor(napi_env env, napi_callback_info info);
    static NVal CreateObjectInstance(napi_env env, WebGLQuery **instance)
    {
        return WebGLObject::CreateObjectInstance<WebGLQuery>(env, instance);
    }
    void SetQuery(unsigned int query)
    {
        m_query = query;
    }

    unsigned int GetQuery() const
    {
        return m_query;
    }

    explicit WebGLQuery() : m_query(0) {};

    WebGLQuery(napi_env env, napi_value exports) : NExporter(env, exports), m_query(0) {};

    ~WebGLQuery() {};

    GLenum GetTarget()
    {
        return m_target;
    }
    void SetTarget(GLenum target)
    {
        m_target = target;
    }
private:
    unsigned int m_query;
    GLenum m_target { 0 };
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSENRENDER_ROSEN_WEBGL_QUERY
