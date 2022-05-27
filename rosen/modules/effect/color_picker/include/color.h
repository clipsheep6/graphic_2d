/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef _COLOR_H_
#define _COLOR_H_


namespace OHOS {
namespace Rosen {

class Color {
public:
    Color() {

    }
    Color(short red, short green, short blue, short alpha) :
        red_(red),
        green_(green),
        blue_(blue),
        alpha_(alpha)
    {

    }
    Color(uint32_t val) {
        alpha_ = (val & 0xff000000) >> 24;
        red_ = (val & 0xff0000) >> 16;
        green_ = (val & 0xff00) >> 8;
        blue_ = val & 0xff;
    }

    short red_ = 0;
    short green_ = 0;
    short blue_ = 0;
    short alpha_ = 0;
};

} // namespace Rosen
} // namespace OHOS

#endif // _COLOR_H_
