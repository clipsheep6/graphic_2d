/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "hw_symbol_txt.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
    bool HWSymbolTxt::operatpr ==(HWSymbolTxt const &sym) const
    {
        if (colorList_.size() != sym.colorList_.size()) {
            return false;
        }
        for (size_t i = 0; i < colorList_.size(); i++) {
            if (colorList_[i].a_ != sym.colorList_[i].a_ ||
                colorList_[i].r_ != sym.colorList_[i].r_ ||
                colorList_[i].g_ != sym.colorList_[i].g_ ||
                colorList_[i].b_ != sym.colorList_[i].b_) {
                    return false;
                }
        }
        if (renderMode_ != sym.renderMode_) {
            return false;
        }
        return true;
    }

}
}
}