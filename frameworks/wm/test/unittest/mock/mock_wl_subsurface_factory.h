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

#ifndef FRAMEWORKS_WM_TEST_UNITTEST_MOCK_MOCK_WL_SUBSURFACE_FACTORY_H
#define FRAMEWORKS_WM_TEST_UNITTEST_MOCK_MOCK_WL_SUBSURFACE_FACTORY_H

#include <gmock/gmock.h>

#include "wl_subsurface_factory.h"

namespace OHOS {
class MockWlSubsurfaceFactory : public WlSubsurfaceFactory {
public:
    MOCK_METHOD0(Init, void());
    MOCK_METHOD0(Deinit, void());
    MOCK_METHOD2(Create, sptr<WlSubsurface>(const sptr<WlSurface> &surf,
                                            const sptr<WlSurface> &parent));
};
} // namespace OHOS

#endif // FRAMEWORKS_WM_TEST_UNITTEST_MOCK_MOCK_WL_SUBSURFACE_FACTORY_H
