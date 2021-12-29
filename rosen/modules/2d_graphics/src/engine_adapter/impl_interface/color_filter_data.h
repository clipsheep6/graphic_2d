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

#ifndef COLOR_FILTER_DATA_H
#define COLOR_FILTER_DATA_H

namespace OHOS {
namespace Rosen {
struct ColorFilterData {
public:
    enum ColorFilterDataType {
        UNKNOW_COLOR_FILTER,
        SKIA_COLOR_FILTER,
    };
    virtual ~ColorFilterData() {}
    virtual void SetColorFilter(void* f) {}
    virtual void* GetColorFilter() const { return nullptr; }
    ColorFilterDataType type;
};
}
}
#endif