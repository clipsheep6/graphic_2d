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

#ifndef INTERFACES_INNERKITS_WMSERVICE_IADJACENT_MODE_CHANGE_LISTENER_CLAZZ_H
#define INTERFACES_INNERKITS_WMSERVICE_IADJACENT_MODE_CHANGE_LISTENER_CLAZZ_H

namespace OHOS {
class IAdjacentModeChangeListenerClazz {
public:
    virtual ~IAdjacentModeChangeListenerClazz() = default;

    virtual void OnAdjacentModeChange(int32_t wid, int32_t x, int32_t y, int32_t width, int32_t height, AdjacentModeStatus status) = 0;
};
} // namespace OHOS

#endif // INTERFACES_INNERKITS_WMSERVICE_IADJACENT_MODE_CHANGE_LISTENER_CLAZZ_H
