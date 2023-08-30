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

#include "../include/webgl/webgl_object.h"

#include "__config"                          // for std
#include "iosfwd"                            // for string
#include "js_native_api_types.h"             // for napi_property_descriptor
#include "memory"                            // for make_unique, unique_ptr
#include "new"                               // for operator delete
#include "string"                            // for basic_string
#include "tuple"                             // for tuple, tie
#include "type_traits"                       // for move
#include "vector"                            // for vector

#include "../include/context/webgl_rendering_context_base.h"
#include "../include/util/log.h"
#include "../include/util/util.h"

namespace OHOS {
namespace Rosen {
using namespace std;

bool WebGLObjectManager::AddObject(uint64_t key, napi_value obj)
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

napi_value WebGLObjectManager::GetNapiValue(uint64_t key)
{
    auto it = objects.find(key);
    if (it == objects.end()) {
        return nullptr;
    }
    napi_value obj;
    napi_status status = napi_get_reference_value(env_, it->second, &obj);
    if (status != napi_ok) {
        return nullptr;
    }
    return obj;
}

napi_value WebGLObjectManager::GetObject(uint64_t key)
{
    napi_value retNull = nullptr;
    napi_get_null(env_, &retNull);
    napi_value obj = GetNapiValue(key);
    if (obj == nullptr) {
        return retNull;
    }
    return obj;
}

void WebGLObjectManager::DeleteObject(uint64_t key)
{
    auto it = objects.find(key);
    if (it == objects.end()) {
        return;
    }
    objects.erase(it);
    napi_delete_reference(env_, it->second);
}

WebGLObjectManager *WebGLObjectManager::GetWebGLObjectManager(napi_env env, napi_value thisVar, uint32_t type)
{
    WebGLRenderingContextBasicBase *context = Util::GetContextObject(env, thisVar, "webgl");
    if (context == nullptr) {
        LOGE("Failed to GetWebGLObjectManager type %{public}u ", type);
        return nullptr;
    }
    return context->GetWebGLObjectManager(type);
}

WebGLObjectContext::~WebGLObjectContext()
{
    for (uint32_t i = 0; i < WebGLObjectManager::WEBGL_OBJECT_MAX; i++) {
        delete webGLObjectManager_[i];
    }
}
bool WebGLObjectContext::CreateWebGLObjectManager(napi_env env)
{
    for (uint32_t i = 0; i < WebGLObjectManager::WEBGL_OBJECT_MAX; i++) {
        if (webGLObjectManager_[i] != nullptr) {
            continue;
        }
        webGLObjectManager_[i] = new WebGLObjectManager(env);
        if (webGLObjectManager_[i] == nullptr) {
            return false;
        }
    }
    return true;
}
} // namespace Rosen
} // namespace OHOS
