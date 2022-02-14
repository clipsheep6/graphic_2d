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

#ifndef RENDER_UTILS_H
#define RENDER_UTILS_H
namespace OHOS {
namespace Rosen {
#define SAFE_DELETE(x) if (x) {delete x; x = nullptr;}
#define CHECKSTATE(state)  if (state == nullptr) { LOGE("state is null"); return;}
enum class RenderBackendType {
    GLES,
    VULKAN,
    SOFTWARE,
    RENDER_BACKEND_TYPE_COUNT,
};
}
}
#endif