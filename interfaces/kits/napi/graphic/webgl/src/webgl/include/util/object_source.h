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

#ifndef WEBGL_SHADER_PRECISION_FORMAT_H_OBJECT_SOURCE_H
#define WEBGL_SHADER_PRECISION_FORMAT_H_OBJECT_SOURCE_H

#include <map>
#include "../../../common/napi/n_exporter.h"

#ifdef __cplusplus
extern "C" {
#endif

namespace OHOS {
namespace Rosen {
class ObjectSource {
public:
    ObjectSource(napi_env env) : env_(env) {}
    ~ObjectSource() {
        for (auto it = objects.begin(); it != objects.end(); it++) {
            napi_delete_reference(env_, it->second);
        }
        objects.clear();
    }

    bool AddObject(uint64_t key, napi_value obj)
    {
        if (objects.find(key) != objects.end()) {
            return false;
        }
        napi_ref ref;
        napi_status status = napi_create_reference(env_, obj, 1, &ref);
        if (status != napi_ok) {
            return false;
        }
        objects.insert({key, ref});
        return true;
    }

    napi_value GetObject(uint64_t key)
    {
        napi_value retNull = nullptr;
        napi_get_null(env_, &retNull);
        auto it = objects.find(key);
        if (it == objects.end()) {
            return retNull;
        }
        napi_value obj;
        napi_status status = napi_get_reference_value(env_, it->second, &obj);
        if (status != napi_ok) {
            return retNull;
        }
        return obj;
    }

    void DeleteObject(uint64_t key)
    {
        auto it = objects.find(key);
        if (it == objects.end()) {
            return;
        }
        napi_delete_reference(env_, it->second);
        objects.erase(it);
    }

    enum {
        WEBGL_OBJECT_PROGRAM = 0,
        WEBGL_OBJECT_SHADER,
        WEBGL_OBJECT_BUFFER,
        WEBGL_OBJECT_FRAME_BUFFER,
        WEBGL_OBJECT_RENDER_BUFFER,
        WEBGL_OBJECT_TEXTURE,
        WEBGL_OBJECT_MAX
    };
private:
    napi_env env_;
    ObjectSource(const ObjectSource&) = delete;
    ObjectSource& operator=(const ObjectSource&) = delete;
    std::map<uint64_t, napi_ref> objects;
};
} // namespace Rosen
} // namespace OHOS

#ifdef __cplusplus
}
#endif

#endif // WEBGL_SHADER_PRECISION_FORMAT_H_OBJECT_SOURCE_H
