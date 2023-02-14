/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "ipc_skeleton.h"

namespace OHOS {
void IPCSkeleton::JoinWorkThread()
{
}

void IPCSkeleton::StopWorkThread()
{
}

bool IPCSkeleton::SetContextObject(sptr<IRemoteObject> &object)
{
    return false;
}

sptr<IRemoteObject> IPCSkeleton::GetContextObject()
{
    return nullptr;
}

bool IPCSkeleton::SetMaxWorkThreadNum(int maxThreadNum)
{
    return false;
}

pid_t IPCSkeleton::GetCallingPid()
{
    return -1;
}

pid_t IPCSkeleton::GetCallingUid()
{
    return -1;
}

uint32_t IPCSkeleton::GetCallingTokenID()
{
    return 0;
}

uint32_t IPCSkeleton::GetFirstTokenID()
{
    return 0;
}

std::string IPCSkeleton::GetLocalDeviceID()
{
    return "";
}

std::string IPCSkeleton::GetCallingDeviceID()
{
    return "";
}

IPCSkeleton &IPCSkeleton::GetInstance()
{
    static IPCSkeleton skeleton;
    return skeleton;
}

bool IPCSkeleton::IsLocalCalling()
{
    return true;
}

int IPCSkeleton::FlushCommands(IRemoteObject *object)
{
    return -1;
}

std::string IPCSkeleton::ResetCallingIdentity()
{
    return "";
}

bool IPCSkeleton::SetCallingIdentity(std::string &identity)
{
    return false;
}
}  // namespace OHOS
