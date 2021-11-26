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

#include "graphic_bytrace.h"

#include <bytrace.h>
#include <hilog/log.h>

void GraphicBytrace::BytraceBegin(const std::string &proc)
{
    OHOS::HiviewDFX::HiLog::Info({ LOG_CORE, 0, "GraphicBytrace" }, "BytraceBegin %{public}s", proc.c_str());
    StartTrace(BYTRACE_TAG_GRAPHIC_AGP, proc.c_str());
}

void GraphicBytrace::BytraceEnd(const std::string &proc)
{
    OHOS::HiviewDFX::HiLog::Info({ LOG_CORE, 0, "GraphicBytrace" }, "BytraceEnd %{public}s", proc.c_str());
    FinishTrace(BYTRACE_TAG_GRAPHIC_AGP, proc.c_str());
}

ScopedBytrace::ScopedBytrace(const std::string &proc) : proc_(proc)
{
    OHOS::HiviewDFX::HiLog::Info({ LOG_CORE, 0, "GraphicBytrace" }, "ScopedBytrace %{public}s", proc_.c_str());
    GraphicBytrace::BytraceBegin(proc_);
}

ScopedBytrace::~ScopedBytrace()
{
    OHOS::HiviewDFX::HiLog::Info({ LOG_CORE, 0, "GraphicBytrace" }, "~ScopedBytrace %{public}s", proc_.c_str());
    GraphicBytrace::BytraceEnd(proc_);
}
