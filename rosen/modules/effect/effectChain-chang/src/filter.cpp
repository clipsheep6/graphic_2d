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

#include "filter.h"

void Filter::Process(ProcessDate& data)
{
    DoProcess(data);
    if (this->GetFilterType() == filter_type::ALGOFILTER) {
        std::swap(data.srcTextureID, data.outputTextureID);
    }
    if (GetNextFilter()) {
        GetNextFilter()->Process(data);
    }
}

void Filter::AddNextFilter(Filter* next)
{
    if (nextNum_ < nextPtrMax_) {
        next_ = next;
        if (next) {
            next->AddPreviousFilter(this);
        }
    }
    if (nextNum_ == nextPtrMax_) return;
    nextNum_++;
}

void Filter::AddPreviousFilter(Filter* previous)
{
    if (preNum_ < prePtrMax_) {
        previous_ = previous;
    }
    if (preNum_ == prePtrMax_) return;
    preNum_++;
}

Filter* Filter::GetNextFilter(void)
{
    return next_;
}

Filter* Filter::GetPreviousFilter(void)
{
    return previous_;
}

int Filter::GetInputNumber()
{
    return preNum_;
}

int Filter::GetOutputNumber()
{
    return nextNum_;
}

int Filter::GetMaxInputNumber()
{
    return prePtrMax_;
}

int Filter::GetMaxOutputNumber()
{
    return nextPtrMax_;
}