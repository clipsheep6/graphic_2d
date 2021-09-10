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

#include <string>
#include <hilog/log.h>
#include "graphic_napi_common.h"

using namespace OHOS;
void ConvertInfoForInt32(napi_env env, napi_value result, const char* key, int32_t value)
{
    GNAPI_LOG("%{public}s called", __PRETTY_FUNCTION__);
    napi_value num;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, value, &num));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, key, num));
}
void ConvertInfoForUint32(napi_env env, napi_value result, const char* key, uint32_t value)
{
    GNAPI_LOG("%{public}s called", __PRETTY_FUNCTION__);
    napi_value num;
    NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, value, &num));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, key, num));
}
void ConvertInfoForUndefined(napi_env env, napi_value result, const char* key)
{
    GNAPI_LOG("%{public}s called", __PRETTY_FUNCTION__);
    napi_value undefined;
    napi_get_undefined(env, &undefined);
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, key, undefined));
}
