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

#ifndef FRAMEWORKS_WMCLIENT_INCLUDE_WINDOW_MANAGER_SERVER_H
#define FRAMEWORKS_WMCLIENT_INCLUDE_WINDOW_MANAGER_SERVER_H

#include <queue>

#include <promise.h>
#include <refbase.h>
#include <window_manager_type.h>
#include <wms-client-protocol.h>

#include "singleton_delegator.h"
#include "wayland_service.h"
#include "wl_surface.h"

namespace OHOS {
struct WMSWindowInfo {
    GSError wret;
    int32_t wid;
    int32_t x;
    int32_t y;
    uint32_t width;
    uint32_t height;
};

class IWindowServerChange {
public:
    virtual ~IWindowServerChange() = default;
    virtual void OnWindowSizeChange(int32_t width, int32_t height) = 0;
    virtual void OnWindowPositionChange(int32_t width, int32_t height) = 0;
    virtual void OnSplitStatusChange(SplitStatus status) = 0;
};

class WindowManagerServer : public RefBase {
public:
    static sptr<WindowManagerServer> GetInstance();

    MOCKABLE void Init();
    MOCKABLE void Deinit();

    MOCKABLE sptr<Promise<struct WMSWindowInfo>> CreateWindow(
        const sptr<WlSurface> &wlSurface, int32_t did, WindowType type);

    virtual void RegisterWindowServerChange(int32_t wid, IWindowServerChange *change);

private:
    WindowManagerServer() = default;
    MOCKABLE ~WindowManagerServer() = default;
    static inline sptr<WindowManagerServer> instance = nullptr;
    static inline SingletonDelegator<WindowManagerServer> delegator;

    static void OnAppear(const GetServiceFunc get, const std::string &iname, uint32_t ver);
    static inline struct wms *wms = nullptr;

    static void OnWindowChange(void *, struct wms *,
        uint32_t status, uint32_t wid, int32_t x, int32_t y, int32_t width, int32_t height);
    static void OnWindowSizeChange(void *, struct wms *, int32_t wid, int32_t width, int32_t height);
    static void OnWindowPositionChange(void *, struct wms *, int32_t wid, int32_t x, int32_t y);
    static void OnSplitStatusChange(void *, struct wms *, int32_t wid, uint32_t status);

    static inline std::queue<sptr<Promise<struct WMSWindowInfo>>> promiseQueue;
    static inline std::map<int32_t, IWindowServerChange*> onWindowServiceChanges;
};
} // namespace OHOS

#endif // FRAMEWORKS_WMCLIENT_INCLUDE_WINDOW_MANAGER_SERVER_H
