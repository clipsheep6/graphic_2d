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
 
 
#ifndef VSYNC_TYPE_H
#define VSYNC_TYPE_H
 
namespace OHOS {
namespace Rosen {
    // Supports all refresh rates that can be evenly divided by 360
    constexpr uint32_t VSYNC_MAX_REFRESHRATE = 360; // 360hz at most
} // Rosen
 
typedef enum {
    VSYNC_MODE_LTPS,
    VSYNC_MODE_LTPO,
    VSYNC_MODE_INVALID
} VSyncMode;
} // OHOS
 
#endif // VSYNC_TYPE_H