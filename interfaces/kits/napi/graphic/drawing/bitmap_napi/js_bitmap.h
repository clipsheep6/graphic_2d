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

#ifndef OHOS_ROSEN_JS_BITMAP_H
#define OHOS_ROSEN_JS_BITMAP_H

#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>
#include "image/bitmap.h"


namespace OHOS::Rosen {
namespace Drawing {
class JsBitmap final {
public:
    JsBitmap();
    ~JsBitmap();

    static napi_value Init(napi_env env, napi_value exportObj);
    static napi_value Constructor(napi_env env, napi_callback_info info);
    static void Destructor(napi_env env, void *nativeObject, void *finalize);
    static napi_value CreateJsBitmap(napi_env env, const std::shared_ptr<Bitmap> bitmap);
    static napi_value Build(napi_env env, napi_callback_info info);
    static napi_value GetWidth(napi_env env, napi_callback_info info);
    static napi_value GetHeight(napi_env env, napi_callback_info info);
    
    Bitmap* GetBitmap();

private:
    napi_value OnBuild(napi_env env, napi_callback_info info);
    napi_value OnGetWidth(napi_env env, napi_callback_info info);
    napi_value OnGetHeight(napi_env env, napi_callback_info info);
    static thread_local napi_ref constructor_;

    Bitmap* bitmap_;
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_JS_BITMAP_H