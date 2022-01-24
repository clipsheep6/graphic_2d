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

#include "base_blur_filter.h"

void BaseBlurFilter::SetValue(std::string key, void* value, int size)
{
    if (key.compare("offsets") == 0) {
        if (size != -1) {
            float* offsets = (float*)value;
            for (int i = 0; i < size; i++) {
                params_.offsets_[i] = *(offsets + i);
            }
        }
    }
    if (key.compare("weights") == 0) {
        if (size != -1) {
            float* weights = (float*)value;
            for (int i = 0; i < size; i++) {
                params_.weights_[i] = *(weights + i);
            }
        }
    }
}

void BaseBlurFilter::CreateProgram(std::string vertexString, std::string fragmentString)
{
    mProgram_ = new Program();
    mProgram_->Compile(vertexString, fragmentString);
    mProgram_->UseProgram();

    weightsID_ = glGetUniformLocation(mProgram_->programID_, "weight");
    offsetID_ = glGetUniformLocation(mProgram_->programID_, "offset");
}

void BaseBlurFilter::LoadFilterParams()
{
    Use();
    glUniform1fv(weightsID_, RADIUS, params_.weights_);
    glUniform1fv(offsetID_, RADIUS, params_.offsets_);
}


void BaseBlurFilter::DoProcess(ProcessDate& data)
{
    Prepare(data);
    LoadFilterParams();
    Draw(data);
}