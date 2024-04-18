/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#ifndef SKIA_TASK_EXECUTOR_H
#define SKIA_TASK_EXECUTOR_H

#include <condition_variable>
#include <cstdint>
#include <deque>
#include <functional>
#include <mutex>
#include <pthread.h>
#include <string>
#include <thread>
#include <unistd.h>
#include <utility>

#ifdef RES_SCHED_ENABLE
#include "rs_frame_report.h"
#endif

#ifdef _WIN32
#include <windows.h>
#define gettid GetCurrentThreadId
#endif

#ifdef __APPLE__
#define gettid getpid
#endif

#ifdef __gnu_linux__
#include <sys/types.h>
#include <sys/syscall.h>
#define gettid []()->int32_t { return static_cast<int32_t>(syscall(SYS_gettid)); }
#endif

namespace OHOS {
namespace Rosen {

template<class T>
class TaskQueue {
public:
    TaskQueue() = default;
    ~TaskQueue() = default;

    constexpr void Push(T&& t)
    {
        queue_.push_back(std::forward<T>(t));
    }

    constexpr T Pop()
    {
        T task;
        if (!queue_.empty()) {
            task = queue_.front();
            queue_.pop_front();
        }
        return task;
    }

    constexpr bool HasTask() const
    {
        return !queue_.empty();
    }

private:
    std::deque<T> queue_;
};

class TaskPoolExecutor {
public:
    using Task = std::function<void()>;
    static void PostTask(Task && task);

private:
    TaskPoolExecutor(const TaskPoolExecutor&) = delete;
    void operator=(const TaskPoolExecutor&) = delete;

    static TaskPoolExecutor& GetInstance();

    TaskPoolExecutor();
    ~TaskPoolExecutor() {}

    void InitThreadPool();

    void EnqueueTask(Task && task);
#ifdef RES_SCHED_ENABLE
    void PromoteThreadPriority();
#endif
    void ThreadLoop();

#ifdef RES_SCHED_ENABLE
    static constexpr uint32_t REQUEST_THREAD_PRIORITY_ID = 100005;
    static constexpr uint32_t REQUEST_THREAD_PRIORITY_LOAD = 0;
    static constexpr uint32_t REQUEST_THREAD_PRIORITY_NUM = 0;
#endif
    static constexpr uint32_t DEFAULT_THREAD_COUNT = 2;
    std::mutex mutex_;
    std::condition_variable condition_;
    uint32_t waitingThread_ = 0;
    bool running_ = true;
    TaskQueue<Task> taskQueue_;
};
} // namespace Rosen
} // namespace OHOS
#endif