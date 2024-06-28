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

#ifndef OHOS_ROSEN_JS_IMAGE_H
#define OHOS_ROSEN_JS_IMAGE_H

#include <memory>

#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>

namespace OHOS::Rosen {
namespace Drawing {

#ifndef DRAWING_API
#ifdef _WIN32
#define DRAWING_EXPORT __attribute__((dllexport))
#define DRAWING_IMPORT __attribute__((dllimport))
#else
#define DRAWING_EXPORT __attribute__((visibility("default")))
#define DRAWING_IMPORT __attribute__((visibility("default")))
#endif
#ifdef MODULE_DRAWING
#define DRAWING_API DRAWING_EXPORT
#else
#define DRAWING_API DRAWING_IMPORT
#endif
#endif

class Image;
class JsImage final {
public:
    explicit JsImage(std::shared_ptr<Image> image) : m_image(image) {};
    ~JsImage() = default;

    static napi_value Constructor(napi_env env, napi_callback_info info);
    static void Destructor(napi_env env, void *nativeObject, void *finalize);

    static napi_value Init(napi_env env, napi_value exportObj);
    static napi_value BuildFromBitmap(napi_env env, napi_callback_info info);
    static napi_value GetWidth(napi_env env, napi_callback_info info);
    static napi_value GetHeight(napi_env env, napi_callback_info info);
    static napi_value IsOpaque(napi_env env, napi_callback_info info);
    static napi_value GetImageInfo(napi_env env, napi_callback_info info);

    std::shared_ptr<Image> GetImage();

private:
    static thread_local napi_ref constructor_;
    std::shared_ptr<Image> m_image;
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_JS_IMAGE_H