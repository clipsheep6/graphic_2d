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

#ifndef FORM_SURFACE_DELEGATE_H
#define FORM_SURFACE_DELEGATE_H

#include "pointer_event.h"
#include "surface.h"
#include "iremote_object.h"
#include "ipc_object_proxy.h"

struct NativeFormCallback;

typedef struct ArkUIFormCallback {
    std::function<void(OHOS::sptr<OHOS::Surface>, uint32_t, uint32_t)> OnSurfaceCreated;
    std::function<void(uint32_t, uint32_t)> OnSurfaceChanged;
    std::function<void(std::shared_ptr<OHOS::MMI::PointerEvent>&)> OnDispatchPointerEvent;
} ArkUIFormCallback;

namespace OHOS {
class FormSurfaceDelegate : public RefBase {
public:
    FormSurfaceDelegate(sptr<IRemoteObject> delegate);

    void OnSurfaceCreated(OHOS::sptr<OHOS::Surface> surface, uint32_t width, uint32_t height);
    void OnSurfaceChanged(uint32_t width, uint32_t height);
    void OnSurfaceDestroyed();
    void OnDispatchPointerEvent(std::shared_ptr<OHOS::MMI::PointerEvent>& pointerEvent);
    void RegistCallback(uint64_t formId, std::string componentId, ArkUIFormCallback& arkUIFormCallback);

private:
    sptr<IRemoteObject> delegate_;
    ArkUIFormCallback arkUIFormCallback_;
    std::recursive_mutex mutex_;
    NativeFormCallback CreateNativeFormCallback();
};
}

#endif