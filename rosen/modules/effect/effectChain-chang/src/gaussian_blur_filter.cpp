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

#include "gaussian_blur_filter.h"

GaussianBlurFilter::GaussianBlurFilter()
{
    narrowFilter_ = new ScaleFilter();
    narrowFilter_->SetScale(multiNarrow);
    enlargeFilter_ = new ScaleFilter();
    enlargeFilter_->SetScale(multiEnlarge);
    horizontalBlurFilter_ = new HorizontalBlurFilter();
    verticalBlurFilter_ = new VerticalBlurFilter();
}

GaussianBlurFilter::~GaussianBlurFilter()
{
    delete narrowFilter_;
    delete enlargeFilter_;
    delete horizontalBlurFilter_;
    delete verticalBlurFilter_;
}

void GaussianBlurFilter::DoProcess(ProcessDate& data)
{
    narrowFilter_->Process(data);
    horizontalBlurFilter_->Process(data);
    verticalBlurFilter_->Process(data);
    enlargeFilter_->Process(data);
    std::swap(data.srcTextureID, data.outputTextureID);
}

void GaussianBlurFilter::SetValue(std::string key, void* value, int size)
{
    horizontalBlurFilter_->SetValue(key, value, size);
    verticalBlurFilter_->SetValue(key, value, size);
}

std::string GaussianBlurFilter::GetVertexShader()
{
    return std::string();
}

std::string GaussianBlurFilter::GetFragmentShader()
{
    return std::string();
}