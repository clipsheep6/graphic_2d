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

#ifndef FRAMEWORKS_WMCLIENT_TEST_UNITTEST_MOCK_SINGLETON_MOCKER_H
#define FRAMEWORKS_WMCLIENT_TEST_UNITTEST_MOCK_SINGLETON_MOCKER_H

#include "singleton_container.h"

namespace OHOS {
template<class T, class MockT>
class SingletonMocker {
public:
    SingletonMocker()
    {
        origin = SingletonContainer::Get<T>();
        mock = new MockT();
        SingletonContainer::Set<T>(mock);
    }

    ~SingletonMocker()
    {
        SingletonContainer::Set<T>(origin);
    }

    sptr<T> Origin()
    {
        return origin;
    }

    sptr<MockT> Mock()
    {
        return mock;
    }

private:
    sptr<T> origin = nullptr;
    sptr<MockT> mock = nullptr;
};
} // namespace OHOS

#endif // FRAMEWORKS_WMCLIENT_TEST_UNITTEST_MOCK_SINGLETON_MOCKER_H
