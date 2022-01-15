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

#ifndef UTILS_SEMAPHORE_EXPORT_LOCAL_SEMAPHORE_H
#define UTILS_SEMAPHORE_EXPORT_LOCAL_SEMAPHORE_H

#include <semaphore.h>

namespace OHOS {
class LocalSemaphore {
public:
    LocalSemaphore(int count = 0);
    ~LocalSemaphore();

    void Inc();
    void Dec();

private:
    sem_t sem;
};
} // namespace OHOS

#endif // UTILS_SEMAPHORE_EXPORT_LOCAL_SEMAPHORE_H
