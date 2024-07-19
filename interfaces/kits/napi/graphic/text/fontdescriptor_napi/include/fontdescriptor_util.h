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

#ifndef OHOS_ROSEN_FONTDESCRIPTOR_UTIL_H
#define OHOS_ROSEN_FONTDESCRIPTOR_UTIL_H

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "font_parser.h"
#include "refbase.h"
#include "napi_common.h"

namespace OHOS::Rosen {
struct UserData {
    std::shared_ptr<TextEngine::FontParser::FontDescriptor> matchDesc {nullptr};
};

struct CallbackData {
    std::set<std::shared_ptr<TextEngine::FontParser::FontDescriptor>> matchResult;
};

struct CallbackInfo : RefBase {
    napi_env env { nullptr };
    napi_ref ref { nullptr };
    napi_deferred deferred { nullptr };
    int32_t errCode { -1 };
    CallbackData data;
    UserData uData;
};

template <typename T>
static void DeletePtr(T &ptr)
{
    if (ptr != nullptr) {
        delete ptr;
        ptr = nullptr;
    }
}
}

#endif // OHOS_ROSEN_FONTDESCRIPTOR_UTIL_H