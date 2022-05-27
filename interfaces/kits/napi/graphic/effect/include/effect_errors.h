/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef EFFECT_ERRORS_H
#define EFFECT_ERRORS_H

#include <cstdint>
#include "errors.h"
//#include "modules.h"

namespace OHOS {
namespace Rosen {
//constexpr int32_t BASE_EFFECT_ERR_OFFSET = ErrCodeOffset(SUBSYS_GRAPHIC, MODULE_EFFECT);

/* Effect defined errors */
const uint32_t ERR_EFFECT_INVALID_VALUE = 2;                                  // invalid size
const uint32_t ERROR = 1;                                                     // Operation error
const uint32_t SUCCESS = 0;                                                    // Operation success
//const int32_t ERROR = BASE_EFFECT_ERR_OFFSET;                                  // Operation failed
//const int32_t ERR_IPC = BASE_EFFECT_ERR_OFFSET + 1;                            // ipc error
//const int32_t ERR_EFFECT_UNKNOWN = BASE_EFFECT_ERR_OFFSET + 200;  // effect unknown error
}  // namespace Rosen
}  // namespace OHOS
#endif  // EFFECT_ERRORS_H
