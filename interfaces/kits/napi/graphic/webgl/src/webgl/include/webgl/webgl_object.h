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

#ifndef ROSENRENDER_ROSEN_WEBGL_OBJECT
#define ROSENRENDER_ROSEN_WEBGL_OBJECT

#include <map>
#include "../../../common/napi/n_exporter.h"
#include "../../../common/napi/n_class.h"       // for NClass
#include "../../../common/napi/n_func_arg.h"    // for NFuncArg, NARG_CNT, ZERO
#include "../../../common/napi/n_val.h"               // for NVal

namespace OHOS {
namespace Rosen {
class WebGLObject {
public:
    WebGLObject() {};
    ~WebGLObject() {}

    template<class T>
    inline static NVal CreateObjectInstance(napi_env env, T **instance)
    {
        napi_value obj = NClass::InstantiateClass(env, T::className, {});
        if (obj == nullptr) {
            return {env, nullptr};
        }
        *instance = NClass::GetEntityOf<T>(env, obj);
        if (*instance == nullptr) {
            return {env, nullptr};
        }
        return NVal(env, obj);
    }
};

class WebGLObjectManager {
public:
    WebGLObjectManager(napi_env env) : env_(env) {}
    ~WebGLObjectManager() {
        for (auto it = objects.begin(); it != objects.end(); it++) {
            napi_delete_reference(env_, it->second);
        }
        objects.clear();
    }

    static WebGLObjectManager *GetWebGLObjectManager(napi_env env, napi_value thisVar, uint32_t type);

    napi_value GetObject(uint64_t key);
    napi_value GetNapiValue(uint64_t key);
    bool AddObject(uint64_t key, napi_value obj);
    void DeleteObject(uint64_t key);

    enum {
        WEBGL_OBJECT_PROGRAM = 0,
        WEBGL_OBJECT_SHADER,
        WEBGL_OBJECT_BUFFER,
        WEBGL_OBJECT_FRAME_BUFFER,
        WEBGL_OBJECT_RENDER_BUFFER,
        WEBGL_OBJECT_TEXTURE,
        WEBGL_OBJECT_ACTIVE_INFO,
        WEBGL_OBJECT_QUERY,
        WEBGL_OBJECT_UNIFORM_LOCATION,
        WEBGL_OBJECT_VERTEX_ARRAY,
        WEBGL_OBJECT_MAX
    };
private:
    napi_env env_;
    WebGLObjectManager(const WebGLObjectManager&) = delete;
    WebGLObjectManager& operator=(const WebGLObjectManager&) = delete;
    std::map<uint64_t, napi_ref> objects;
};

class WebGLObjectContext {
public:
    WebGLObjectContext() {}
    ~WebGLObjectContext();

    WebGLObjectManager *GetWebGLObjectManager(int type) {
        if (type >= WebGLObjectManager::WEBGL_OBJECT_MAX) {
            return nullptr;
        }
        return webGLObjectManager_[type];
    }
    bool CreateWebGLObjectManager(napi_env env);
private:
    WebGLObjectManager *webGLObjectManager_[WebGLObjectManager::WEBGL_OBJECT_MAX] = { nullptr };
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSENRENDER_ROSEN_WEBGL_OBJECT