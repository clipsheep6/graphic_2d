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
 
#include "test_base.h"

void TestBase::SetFileName(std::string fileName)
{
    fileName_ = fileName;
}

void TestBase::Recording(OH_Drawing_Canvas* canvas)
{
    OnRecording(canvas);
}

void TestBase::GetSize(uint32_t& width, uint32_t& height)
{
    width = width_;
    height = height_;
}

void TestBase::GetBackgroundColor(uint32_t& color)
{
    color = background_;
}