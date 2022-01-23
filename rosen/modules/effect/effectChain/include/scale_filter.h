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

#ifndef SCALE_FILTER_H
#define SCALE_FILTER_H

#include "algo_filter.h"

class ScaleFilter :public AlgoFilter
{
public:
    ScaleFilter();
    std::string GetVertexShader() override;
    std::string GetFragmentShader() override;
    void SetScale(float scale);
private:
    virtual void Prepare(ProcessDate& data) override;
    float scale_ = 1.0f;
    friend class GaussianBlurFilter;
    void DoProcess(ProcessDate& data) override;
};

#endif

