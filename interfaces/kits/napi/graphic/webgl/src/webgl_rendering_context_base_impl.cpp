/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "context/webgl_rendering_context_base_impl.h"

#include "context/webgl2_rendering_context_base.h"
#include "context/webgl_rendering_context_base.h"
#include "napi/n_class.h"
#include "util/log.h"
#include "util/object_source.h"
#include "util/util.h"

namespace OHOS {
namespace Rosen {
namespace Impl {
using namespace std;
#define SET_ERROR(error)                                \
    do {                                                \
        LOGE("WebGL set error code %{public}u", error); \
        SetError(error);                                \
    } while (0)

#define SET_ERROR_WITH_LOG(error, info, ...)                                \
    do {                                                                    \
        LOGE("WebGL set error code %{public}u" info, error, ##__VA_ARGS__); \
        SetError(error);                                                    \
    } while (0)
} // namespace Impl
} // namespace Rosen
} // namespace OHOS