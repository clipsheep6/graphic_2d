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

#ifndef FRAMEWORKS_WMTEST_TEST_WMCLIENT_WMCLIENT_NATIVE_TEST_19_H
#define FRAMEWORKS_WMTEST_TEST_WMCLIENT_WMCLIENT_NATIVE_TEST_19_H

#include <cstdint>
#include <string>

#include <refbase.h>
#include <window.h>
#include <window_manager_service_client.h>
#include "inative_test.h"
#include "native_test_class.h"

namespace OHOS {
class WMClientNativeTest19 : public INativeTest, public IWindowChangeListenerClazz{
public:
    virtual ~WMClientNativeTest19() = default;
    virtual std::string GetDescription() const override;
    virtual std::string GetDomain() const override;
    virtual int32_t GetID() const override;
    virtual uint32_t GetLastTime() const override;

    virtual void Run(int32_t argc, const char **argv) override;

    virtual void OnWindowCreate(int32_t pid, int32_t wid) override;
    virtual void OnWindowDestroy(int32_t pid, int32_t wid) override;

protected:
    int32_t windowId = 0;
};
}

#endif // FRAMEWORKS_WMTEST_TEST_WMCLIENT_WMCLIENT_NATIVE_TEST_19_H
