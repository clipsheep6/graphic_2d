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
#include <functional>
#include <mutex>
#include <pthread.h>
#include <securec.h>
#include <thread>
#include <unistd.h>
#include <utility>
#include <queue>
#include <cstdint>

#ifdef RES_SCHED_ENABLE
#include "rs_frame_report.h"
#endif

#ifdef _WIN32
#include <windows.h>
#define gettid GetCurrentThreadId
#endif

#ifdef _APPLE_
#define gettid getpid
#endif

#ifdef _gpu_linux_
#include <sys/types.h>
#include <sys/syscall.h>
#define gettid []()->int32_t { return static_cast<int32_t>(syscall(SYS_gettid)); }
#endif

namespace OHOS {
namespace Rosen {

template<class T, uint32_t SIZE>
class TaskQueue {
public:
    TaskQueue() = default;
    ~TaskQueue() = default;

    constexpr void Push(T&& t)
    {
        if (queue_.size() < SIZE) {
            queue_.push(t);
        }
    }

    constexpr T Pop()
    {
        T task;
        if (!queue_.empty()) {
            task = queue_.front();
            queue_.pop();
        }
        return task;
    }

    constexpr bool HasTask() const
    {
        return !queue_.empty();
    }

    constexpr bool HasSpace() const
    {
        return queue_.size() < SIZE;
    }

    constexpr uint32_t Size() const
    {
        return queue_.size();
    }

private:
    TaskQueue(const TaskQueue&) = delete;
    void operator=(const TaskQueue&) = delete;
    std::queue<T> queue_;
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
    static constexpr uint32_t DEFAULT_THREAD_COUNT = 3;
    static constexpr uint32_t QUEUE_SIZE = 512;
    static constexpr uint32_t WAIT_SLEEP_TIME = 100;
    std::mutex mutex_;
    std::condition_variable condition_;
    uint32_t waitingThread_ = 0;
    bool running_ = true;
    TaskQueue<Task, QUEUE_SIZE> taskQueue_;
};
} // namespace Rosen
} // namespace OHOS
#endif