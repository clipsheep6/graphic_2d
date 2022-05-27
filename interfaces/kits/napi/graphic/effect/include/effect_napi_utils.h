/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef EFFECT_NAPI_UTILS_H
#define EFFECT_NAPI_UTILS_H

#include <hilog/log.h>

#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"


#define VALUE_IS_OK(x) ((x) == napi_ok)
#define VALUE_NOT_NULL(p) ((p) != nullptr)
#define VALUE_IS_READY(x, p) (VALUE_IS_OK(x) && VALUE_NOT_NULL(p))

#define VALUE_ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

#define VALUE_NAPI_CHECK_RET_D(x, res, msg) \
do \
{ \
    if (!(x)) \
    { \
        msg; \
        return (res); \
    } \
} while (0)

#define VALUE_NAPI_CHECK_RET(x, res) \
do \
{ \
    if (!(x)) \
    { \
        return (res); \
    } \
} while (0)

#define VALUE_JS_ARGS(env, info, status, argc, argv, thisVar) \
do \
{ \
    status = napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr); \
} while (0)

#define VALUE_JS_NO_ARGS(env, info, status, thisVar) \
do \
{ \
    status = napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr); \
} while (0)

#define NAPI_CREATE_ASYNC_WORK(env, status, workName, exec, complete, aContext, work) \
do \
{ \
    napi_value _resource = nullptr; \
    napi_create_string_utf8((env), (workName), NAPI_AUTO_LENGTH, &_resource); \
    (status) = napi_create_async_work(env, nullptr, _resource, (exec), \
            (complete), static_cast<void*>((aContext).get()), &(work)); \
    if ((status) == napi_ok) { \
        (status) = napi_queue_async_work((env), (work)); \
        if ((status) == napi_ok) { \
            (aContext).release(); \
        } \
    } \
} while (0)

namespace OHOS {
namespace Rosen {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN, "EffectNapi"};
#define EFFECT_LOG_I(fmt, ...) OHOS::HiviewDFX::HiLog::Info(LABEL, \
        "%{public}s:%{public}d " fmt, __func__, __LINE__, ##__VA_ARGS__)

#define EFFECT_LOG_E(fmt, ...) OHOS::HiviewDFX::HiLog::Error(LABEL, \
        "%{public}s:%{public}d " fmt, __func__, __LINE__, ##__VA_ARGS__)
        
class EffectNapiUtils {
public:
    static bool GetBufferByName(napi_env env, napi_value root, const char* name, void **res, size_t* len);
    static bool GetUint32ByName(napi_env env, napi_value root, const char* name, uint32_t *res);
    static bool GetInt32ByName(napi_env env, napi_value root, const char* name, int32_t *res);
    static bool GetBoolByName(napi_env env, napi_value root, const char* name, bool *res);
    static bool GetNodeByName(napi_env env, napi_value root, const char* name, napi_value *res);
    static bool GetUtf8String(napi_env env, napi_value root, std::string &res, bool eof = true);
    static napi_valuetype getType(napi_env env, napi_value root);
    static bool CreateArrayBuffer(napi_env env, void* src, size_t srcLen, napi_value *res);
    static bool CreateUtf8String(napi_env env, std::string str, napi_value *res, int srcLen = -1);
};
} // namespace Rosen
} // namespace OHOS
#endif // EFFECT_NAPI_UTILS_H