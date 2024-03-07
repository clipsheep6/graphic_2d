/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.. All rights reserved.
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
#ifndef ROSEN_TEXT_PROPERTIES_PLACEHOLDERALIGNMENT_H
#define ROSEN_TEXT_PROPERTIES_PLACEHOLDERALIGNMENT_H

#include "rosen_text/properties/text_style.h"

namespace rosen {
enum class PlaceholderAlignment {
    BASELINE,
    ABOVEBASELINE,
    BELOWBASELINE,
    TOP,
    BOTTOM,
    MIDDLE,
};

class PlaceholderRun {
public:
    PlaceholderRun() {}
    PlaceholderRun(double width, double height,
        PlaceholderAlignment alignment,
        TextBaseline baseline,
        double baselineOffset);
    double width_ = 0;
    double height_ = 0;
    PlaceholderAlignment placeholderalignment_;
    TextBaseline textbaseline_;
    double baselineOffset_ = 0;
};
} // namespace rosen
#endif // ROSEN_TEXT_PROPERTIES_PLACEHOLDERALIGNMENT_H
