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

#ifndef FRAMEWORKS_WMCLIENT_TEST_UNITTEST_MOCK_MOCK_WINDOW_MANAGER_SERVER_H
#define FRAMEWORKS_WMCLIENT_TEST_UNITTEST_MOCK_MOCK_WINDOW_MANAGER_SERVER_H

#include <gmock/gmock.h>

#include "window_manager_server.h"

namespace OHOS {
class MockWindowManagerServer : public WindowManagerServer {
public:
    MOCK_METHOD0(Init, void());
    MOCK_METHOD0(Deinit, void());
    MOCK_METHOD3(CreateWindow, sptr<Promise<struct WMSWindowInfo>>(
        const sptr<WlSurface> &wlSurface, int32_t did, WindowType type));
};
} // namespace OHOS

#endif // FRAMEWORKS_WMCLIENT_TEST_UNITTEST_MOCK_MOCK_WINDOW_MANAGER_SERVER_H
