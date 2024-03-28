/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "js_fontcollection.h"
#include "utils/log.h"
namespace OHOS::Rosen {
thread_local napi_ref JsFontCollection::constructor_ = nullptr;
const std::string CLASS_NAME = "FontCollection";
napi_value JsFontCollection::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    if (napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr) != napi_ok) {
        LOGE("napi_get_cb_info failed");
        return nullptr;
    }

    JsFontCollection* jsFontCollection = new(std::nothrow) JsFontCollection();
    if (napi_wrap(env, jsThis, jsFontCollection, JsFontCollection::Destructor, nullptr, nullptr) != napi_ok) {
        delete jsFontCollection;
        LOGE("napi_wrap failed");
        return nullptr;
    }
    return jsThis;
}

napi_value JsFontCollection::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("disableFallback", JsFontCollection::DisableFallback),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
        sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        LOGE("napi_define_class failed");
        return nullptr;
    }

    if (napi_create_reference(env, constructor, 1, &constructor_) != napi_ok) {
        LOGE("napi_create_reference failed");
        return nullptr;
    }

    if (napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor) != napi_ok) {
        LOGE("napi_set_named_property failed");
        return nullptr;
    }
    return exportObj;
}

void JsFontCollection::Destructor(napi_env env, void* nativeObject, void* finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsFontCollection* napi = reinterpret_cast<JsFontCollection*>(nativeObject);
        delete napi;
    }
}

JsFontCollection::JsFontCollection()
{
    m_fontCollection = OHOS::Rosen::FontCollection::Create();
}

std::shared_ptr<FontCollection> JsFontCollection::GetFontCollection()
{
    return m_fontCollection;
}

napi_value JsFontCollection::DisableFallback(napi_env env, napi_callback_info info)
{
    JsFontCollection* me = CheckParamsAndGetThis<JsFontCollection>(env, info);
    return (me != nullptr) ? me->OnDisableFallback(env, info) : nullptr;
}

napi_value JsFontCollection::OnDisableFallback(napi_env env, napi_callback_info info)
{
    if (m_fontCollection == nullptr) {
        LOGE("m_fontCollection is nullptr");
        return nullptr;
    }

    m_fontCollection->DisableFallback();
    return NapiGetUndefined(env);
}
} // namespace OHOS::Rosen
