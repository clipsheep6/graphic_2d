/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef ROSENRENDER_ROSEN_EXPOTER
#define ROSENRENDER_ROSEN_EXPOTER

#include <functional>
#include <string>

#include "napi/n_val.h"

namespace OHOS {
namespace Rosen {
class NExporter {
public:
    NExporter() : exports_(nullptr, nullptr) {};

    virtual ~NExporter() = default;

    NExporter(napi_env env, napi_value exports) : exports_(env, exports) {};

    virtual bool Export(napi_env env, napi_value exports) = 0;

    virtual std::string GetClassName() = 0;

protected:
    NVal exports_;
};
} // namespace Rosen
} // namespace OHOS
#endif // ROSENRENDER_ROSEN_EXPOTER
