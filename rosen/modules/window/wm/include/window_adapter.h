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

#ifndef OHOS_WINDOW_ADAPTER_H
#define OHOS_WINDOW_ADAPTER_H

#include <refbase.h>

#include "window.h"
#include "window_proxy.h"
#include "single_instance.h"
#include "window_property.h"
#include "window_manager_interface.h"

namespace OHOS {
namespace Rosen {
class WMSDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    virtual void OnRemoteDied(const wptr<IRemoteObject>& wptrDeath) override;
};

class WindowAdapter {
    DECLARE_SINGLE_INSTANCE_BASE(WindowAdapter);
public:
    ~WindowAdapter() = default;
    WMError AddWindow(sptr<IWindow>& window, const sptr<WindowProperty>& windowProperty, int renderNodeId);
    WMError RemoveWindow(const std::string& windowId);
    WMError SetWindowVisibility(const std::string& windowId, bool visibility);

    void ClearWindowAdapter();
private:
    WindowAdapter() = default;
    bool InitWMSProxyLocked();

    std::mutex mutex_;
    sptr<IWindowManager> windowManagerServiceProxy_ = nullptr;
    sptr<WMSDeathRecipient> wmsDeath_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_WINDOW_ADAPTER_H
