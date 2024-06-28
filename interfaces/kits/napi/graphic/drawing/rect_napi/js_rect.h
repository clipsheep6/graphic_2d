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

#ifndef OHOS_ROSEN_JS_RECT_H
#define OHOS_ROSEN_JS_RECT_H

#include <memory>
#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>

#include "utils/rect.h"

namespace OHOS::Rosen {
namespace Drawing {
class JsRect final {
public:
    explicit JsRect(Rect* rect) : m_rect(rect) {};
    ~JsRect();

    static napi_value Init(napi_env env, napi_value exportObj);
    static napi_value Constructor(napi_env env, napi_callback_info info);
    static void Destructor(napi_env env, void *nativeObject, void *finalize);
    static napi_value GetWidth(napi_env env, napi_callback_info info);
    static napi_value GetHeight(napi_env env, napi_callback_info info);
    static napi_value Intersect(napi_env env, napi_callback_info info);
    static napi_value SetLeft(napi_env env, napi_callback_info info);
    static napi_value SetTop(napi_env env, napi_callback_info info);
    static napi_value SetRight(napi_env env, napi_callback_info info);
    static napi_value SetBottom(napi_env env, napi_callback_info info);
    static napi_value GetLeft(napi_env env, napi_callback_info info);
    static napi_value GetTop(napi_env env, napi_callback_info info);
    static napi_value GetRight(napi_env env, napi_callback_info info);
    static napi_value GetBottom(napi_env env, napi_callback_info info);
    static napi_value Copy(napi_env env, napi_callback_info info);

    Rect* GetRect();

private:
    napi_value OnGetWidth(napi_env env, napi_callback_info info);
    napi_value OnGetHeight(napi_env env, napi_callback_info info);
    napi_value OnIntersect(napi_env env, napi_callback_info info);
    napi_value OnSetLeft(napi_env env, napi_callback_info info);
    napi_value OnSetTop(napi_env env, napi_callback_info info);
    napi_value OnSetRight(napi_env env, napi_callback_info info);
    napi_value OnSetBottom(napi_env env, napi_callback_info info);
    napi_value OnGetLeft(napi_env env, napi_callback_info info);
    napi_value OnGetTop(napi_env env, napi_callback_info info);
    napi_value OnGetRight(napi_env env, napi_callback_info info);
    napi_value OnGetBottom(napi_env env, napi_callback_info info);
    napi_value OnCopy(napi_env env, napi_callback_info info);
    
    static thread_local napi_ref constructor_;
    Rect* m_rect = nullptr;
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_JS_RECT_H