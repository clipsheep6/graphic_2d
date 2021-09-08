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

#ifdef __cplusplus
#include <functional>
#include <string>
#endif

#include "../common/graphic_common.h"

#ifdef __cplusplus
namespace OHOS {
#endif

enum WindowType {
    WINDOW_TYPE_NORMAL = 0,
    WINDOW_TYPE_STATUS_BAR = 10,
    WINDOW_TYPE_NAVI_BAR = 20,
    WINDOW_TYPE_ALARM_SCREEN = 30,
    WINDOW_TYPE_SYSTEM_UI = 31,
    WINDOW_TYPE_LAUNCHER = 40,
    WINDOW_TYPE_VIDEO = 41,
    WINDOW_TYPE_INPUT_METHOD = 50,
    WINDOW_TYPE_INPUT_METHOD_SELECTOR = 60,
    WINDOW_TYPE_VOLUME_OVERLAY = 70,
    WINDOW_TYPE_NOTIFICATION_SHADE = 80,
    WINDOW_TYPE_FLOAT = 90,
    WINDOW_TYPE_MAX,
};

enum SubwindowType {
    SUBWINDOW_TYPE_NORMAL = 0,
    SUBWINDOW_TYPE_VIDEO = 1,
    SUBWINDOW_TYPE_MAX,
};

enum WindowRotateType {
    WINDOW_ROTATE_TYPE_NORMAL = 0,
    WINDOW_ROTATE_TYPE_90 = 1,
    WINDOW_ROTATE_TYPE_180 = 2,
    WINDOW_ROTATE_TYPE_270 = 3,
    WINDOW_ROTATE_TYPE_FLIPPED = 4,
    WINDOW_ROTATE_TYPE_FLIPPED_90 = 5,
    WINDOW_ROTATE_TYPE_FLIPPED_180 = 6,
    WINDOW_ROTATE_TYPE_FLIPPED_270 = 7,
    WINDOW_ROTATE_TYPE_MAX,
};

enum WindowMode {
    WINDOW_MODE_UNSET = 0,
    WINDOW_MODE_FULL = 1,
    WINDOW_MODE_FREE = 2,
    WINDOW_MODE_MAX,
};

struct WMImageInfo {
    enum WMError wret;
    uint32_t width;
    uint32_t height;
    uint32_t format;
    uint32_t size;
    const void* data;
};

struct WMDisplayInfo {
    int32_t id;
    uint32_t width;
    uint32_t height;
    uint32_t phyWidth;
    uint32_t phyHeight;
    uint32_t vsync;
};

#ifdef __cplusplus
class TouchEvent;
class KeyEvent;

using OnKeyFunc                  = std::function<bool(KeyEvent)>;
using OnTouchFunc                = std::function<bool(TouchEvent)>;
using WindowPositionChangeFunc   = std::function<void(int32_t x, int32_t y)>;
using WindowSizeChangeFunc       = std::function<void(uint32_t width, uint32_t height)>;
using WindowVisibilityChangeFunc = std::function<void(bool visibility)>;
using WindowTypeChangeFunc       = std::function<void(WindowType type)>;
using WindowModeChangeFunc       = std::function<void(WindowMode mode)>;

} // namespace OHOS
#endif

#endif // INTERFACES_INNERKITS_WMCLIENT_WINDOW_MANAGER_TYPE_H
