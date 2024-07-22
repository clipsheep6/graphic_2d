/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef ERROR_CODE_TEST_H
#define ERROR_CODE_TEST_H

#include <map>
#include <thread>

#include "test_base.h"

class ThreadErrorCodeTest : public TestBase {
public:
    ThreadErrorCodeTest() : TestBase() {}
    ~ThreadErrorCodeTest() = default;

    template<typename Function, typename... Args>
    void CreateThread(Function&& function, Args&&... args)
    {
        // 直接创建线程
        threads_.emplace_back(std::thread(std::forward<Function>(function), std::ref(args)...));
    }
    void DrawTextOnCanvas(OH_Drawing_Canvas* canvas, const char* text);
    void BlockThread()
    {
        for (auto& thread : threads_) {
            thread.join();
        }
    }

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;

private:
    std::vector<std::thread> threads_;
};

#endif // ERROR_CODE_TEST_H