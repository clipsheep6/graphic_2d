/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef TEST_CASE_FACTORY_H
#define TEST_CASE_FACTORY_H
#include "testcase/test_base.h"

class TestCaseFactory {
public:
    TestCaseFactory() = default;
    ~TestCaseFactory(){};
    // explicit MyXNode(std::string id) : id_(id) {}
   
    static std::shared_ptr<TestBase> GetFunctionCpuCase(std::string caseName);
    static std::shared_ptr<TestBase> GetPerformanceCpuCase(std::string caseName);
    static std::unordered_map<std::string, std::function<std::shared_ptr<TestBase>()>> GetFunctionCpuCaseAll();
};


#endif
