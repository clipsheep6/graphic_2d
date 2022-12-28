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

#include "frame_collector.h"

namespace OHOS {
namespace Rosen {
FrameCollector &FrameCollector::GetInstance()
{
    static FrameCollector instance;
    return instance;
}

void FrameCollector::MarkFrameEvent(const FrameEventType &type, int64_t timeNs)
{
}

bool FrameCollector::ProcessUIMarkLocked(int32_t index, int64_t timeNs)
{
    return false;
}

void FrameCollector::ClearEvents()
{
}

FrameCollector::FrameCollector()
{
}

void FrameCollector::SwitchFunction(const char *key, const char *value, void *context)
{
}

void FrameCollector::SetRepaintCallback(std::function<void()> repaint)
{
}
} // namespace Rosen
} // namespace OHOS
