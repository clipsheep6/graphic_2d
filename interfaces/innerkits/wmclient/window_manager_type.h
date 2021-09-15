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

#ifndef INTERFACES_INNERKITS_WMCLIENT_WINDOW_MANAGER_TYPE_H
#define INTERFACES_INNERKITS_WMCLIENT_WINDOW_MANAGER_TYPE_H

#include "wm_common.h"

#ifdef __cplusplus
namespace OHOS {
#endif
struct WMDisplayInfo {
    int32_t id;
    uint32_t width;
    uint32_t height;
    uint32_t phyWidth;
    uint32_t phyHeight;
    uint32_t vsync;
};
#ifdef __cplusplus
} // namespace OHOS
#endif

#endif // INTERFACES_INNERKITS_WMCLIENT_WINDOW_MANAGER_TYPE_H
