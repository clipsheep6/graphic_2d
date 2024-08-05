/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef PLUGIN_RENDER_H
#define PLUGIN_RENDER_H

#include <string>
#include <unordered_map>
#include <ace/xcomponent/native_interface_xcomponent.h>
#include <napi/native_api.h>

#include "egl_core.h"

namespace OHOS {
class PluginRender {
public:
    explicit PluginRender(const std::string &id);
    ~PluginRender()
    {
        if (eglcore_ != nullptr) {
            eglcore_->Release();
            delete eglcore_;
            eglcore_ = nullptr;
        }
    }
    static PluginRender *GetInstance(std::string &id);
    static void Release(std::string &id);
    static napi_value NapiDrawRectangle(napi_env env, napi_callback_info info);
    static void Export(napi_env env, napi_value exports);

public:
    static std::unordered_map<std::string, PluginRender *> instance_;
    static OH_NativeXComponent_Callback callback_;

    EGLCore *eglcore_ = nullptr;
    std::string id_ = "";
};
} // namespace OHOS
#endif // PLUGIN_RENDER_H