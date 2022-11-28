/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_EVENTHANDLER_H
#define OHOS_EVENTHANDLER_H
#ifdef ROSEN_OHOS
#include <event_handler.h>
#else
#include <string>
#include <memory>

#include "rs_event_queue.h"
#include "rs_event_runner.h"

namespace OHOS {
namespace AppExecFwk {
struct Caller {
    std::string file_ {""};
    int         line_ {0};
    std::string func_ {""};
#if __has_builtin(__builtin_FILE)
    Caller(std::string file = __builtin_FILE(), int line = __builtin_LINE(), std::string func = __builtin_FUNCTION())
        : file_(file), line_(line), func_(func) {
    }
#else
    Caller() {
    }
#endif
    std::string ToString()
    {
        if (file_.empty()) {
            return "[ ]";
        }
        size_t split = file_.find_last_of("/\\");
        if (split == std::string::npos) {
            split = 0;
        }
        std::string caller("[" + file_.substr(split + 1) + "(" + func_ + ":" + std::to_string(line_) + ")]");
        return caller;
    }
};
class EventHandler {
public:
    using Callback = std::function<void()>;
    using Priority = EventQueue::Priority;

    EventHandler() {};
    explicit EventHandler(const std::shared_ptr<EventRunner> &runner = nullptr) {};
    virtual ~EventHandler() {};

    static std::shared_ptr<EventHandler> Current()
    {
        return nullptr;
    }

    void RemoveTask(const std::string &name)
    {
        return;
    }
    
    inline bool PostTask(const Callback &callback, const std::string &name = std::string(), int64_t delayTime = 0,
        Priority priority = Priority::LOW)
    {
        return true;
    }

    inline bool PostTask(const Callback &callback, Priority priority, Caller caller = {})
    {
        return PostTask(callback, caller.ToString(), 0, priority);
    }
    void RemoveAllEvents()
    {
        return;
    }
};
} // namespace AppExecFwk
} // namespace OHOS
#endif
#endif // OHOS_EVENTHANDLER_H
