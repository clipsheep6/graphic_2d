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

#include "algo_filter_factory.h"

AlgoFilter* AlgoFilterFactory::GetFilter(std::string name) {
    if (name.compare("GaussianBlur") == 0) {
        return new GaussianBlurFilter();
    }

    if (name.compare("HorizontalBlur") == 0) {
        return new HorizontalBlurFilter();
    }

    if (name.compare("VerticalBlur") == 0) {
        return new VerticalBlurFilter();
    }

    if (name.compare("Scale") == 0) {
        return new ScaleFilter();
    }
    return nullptr;
}
