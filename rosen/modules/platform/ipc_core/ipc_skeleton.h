/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_IPC_IPC_SKELETON_H
#define OHOS_IPC_IPC_SKELETON_H

#include "iremote_object.h"

namespace OHOS {
class OHOS_EXPORT IPCSkeleton {
public:
    IPCSkeleton() = default;
    ~IPCSkeleton() = default;

    // default max is 4, only if you need a customize value
    static OHOS_EXPORT bool SetMaxWorkThreadNum(int maxThreadNum);

    // join current thread into work loop.
    static OHOS_EXPORT void JoinWorkThread();

    // remove current thread from work loop.
    static OHOS_EXPORT void StopWorkThread();

    static OHOS_EXPORT pid_t GetCallingPid();

    static OHOS_EXPORT pid_t GetCallingUid();

    static OHOS_EXPORT uint32_t GetCallingTokenID();

    static OHOS_EXPORT uint64_t GetCallingFullTokenID();

    static OHOS_EXPORT uint32_t GetFirstTokenID();

    static OHOS_EXPORT uint64_t GetFirstFullTokenID();

    static OHOS_EXPORT uint64_t GetSelfTokenID();

    static OHOS_EXPORT std::string GetLocalDeviceID();

    static OHOS_EXPORT std::string GetCallingDeviceID();

    static OHOS_EXPORT bool IsLocalCalling();

    static OHOS_EXPORT IPCSkeleton &GetInstance();

    static OHOS_EXPORT sptr<IRemoteObject> GetContextObject();

    static OHOS_EXPORT bool SetContextObject(sptr<IRemoteObject> &object);

    static OHOS_EXPORT int FlushCommands(IRemoteObject *object);

    static OHOS_EXPORT std::string ResetCallingIdentity();

    static OHOS_EXPORT bool SetCallingIdentity(std::string &identity);
};
} // namespace OHOS
#endif // OHOS_IPC_IPC_SKELETON_H
