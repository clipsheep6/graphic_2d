/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_COMMON_RS_MACROS_H
#define RENDER_SERVICE_CLIENT_CORE_COMMON_RS_MACROS_H

namespace OHOS {
namespace Rosen {
#ifdef MODULE_RSB
#define RSB_EXPORT OHOS_EXPORT
#else
#define RSB_EXPORT OHOS_IMPORT
#endif

#ifdef MODULE_RSC
#define RSC_EXPORT OHOS_EXPORT
#else
#define RSC_EXPORT OHOS_IMPORT
#endif

#ifdef __gnu_linux__
#define __libcpp_erase_if_container __detail::__erase_nodes_if
#endif
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_COMMON_RS_MACROS_H