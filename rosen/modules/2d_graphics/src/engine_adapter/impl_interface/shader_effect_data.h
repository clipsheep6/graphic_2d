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

#ifndef SHADER_EFFECT_DATA_H
#define SHADER_EFFECT_DATA_H

namespace OHOS {
namespace Rosen {
namespace Drawing {
struct ShaderEffectData {
public:
    enum ShaderEffectDataType {
        UNKNOW_PATH_EFFECT,
        SKIA_SHADER_EFFECT,
    };
    virtual ~ShaderEffectData() {}
    virtual void SetShader(void* f) {}
    virtual void* GetShader() const { return nullptr; }
    ShaderEffectDataType type;
};
}
}
}
#endif