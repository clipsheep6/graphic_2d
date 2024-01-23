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

#ifndef TASK_POOL_EXECUTOR_H
#define TASK_POOL_EXECUTOR_H
 
#include <array>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <future>
#include <list>
#include <map>
#include <mutex>
#include <pthread.h>
#include <securec.h>
#include <string>
#include <thread>
#include <unistd.h>
#include <utility>
#ifdef RES_SCHED_ENABLE
#include "frame_report.h"
#endif

namespace OHOS {
namespace Rosen {

template<class T, int SIZE>
class TaskQueue {
public:
    TaskQueue() = default;
    ~TaskQueue() = default;

    constexpr void Push(T&& t)
    {
        int newHead = (head_ + 1) % SIZE;
        if (newHead == tail_) {
            return;
        }
        buffer_[head_] = std::move(t);
        head_ = newHead;
    }

    constexpr T Pop()
    {
        if (tail_ == head_) {
            return nullptr;
        }
        int index = tail_;
        tail_ = (tail_ + 1) % SIZE;
        T t = std::move(buffer_[index]);
        buffer_[index] = nullptr;
        return t;
    }

    constexpr bool HasTask() const
    {
        return head_ != tail_;
    }

    constexpr bool HasSpace() const
    {
        return ((head_ + 1) % SIZE) != tail_;
    }

    constexpr int Size() const
    {
        if (head_ > tail_) {
            return head_ - tail_;
        } else {
            return tail_ - head_ + SIZE;
        }
    }

private:
    TaskQueue(const TaskQueue&) = delete;
    void operator=(const TaskQueue&) = delete;
    T buffer_[SIZE];
    int head_ = 0;
    int tail_ = 0;
};

class TaskPoolExecutor {
public:
    using Task = std::function<void()>;
    static constexpr int DEFAULT_THREAD_COUNT = 3;
    static constexpr int QUEUE_SIZE = 512;
    static constexpr int MAX_THREAD_NAME_LEN = 16;
    static constexpr int WAIT_SLEEP_TIME = 100;

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
    static constexpr int REQUEST_THREAD_PRIORITY_ID = 100005;
    static constexpr int REQUEST_THREAD_PRIORITY_LOAD = 0;
    static constexpr int REQUEST_THREAD_PRIORITY_NUM = 0;
#endif
    std::mutex mutex_;
    std::condition_variable condition_;
    int waitingThread_ = 0;
    bool running_ = true;
    TaskQueue<Task, QUEUE_SIZE> taskQueue_;
};
} // namespace Rosen
} // namespace OHOS
#endif