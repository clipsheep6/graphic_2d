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

#include "horizontal_blur_filter.h"

HorizontalBlurFilter::HorizontalBlurFilter()
{
    CreateProgram(GetVertexShader(), GetFragmentShader());
}

std::string HorizontalBlurFilter::GetFragmentShader()
{
    return R"SHADER(
        const int RADIUS = 3;
        varying vec2 texCoord;
        
        uniform sampler2D uTexture;
        uniform float weight[3];
        uniform float offset[3];

        void main() {
            vec2 tex_offset = 1.0 / textureSize(uTexture, 0); // gets size of single texel
            vec3 result = texture(uTexture, texCoord).rgb * weight[0]; // current fragment's contribution

            for (int i = 1; i < RADIUS; ++i) {
                result += texture(uTexture, texCoord + vec2(tex_offset.x, 0.0) * offset[i]).rgb * weight[i];
                result += texture(uTexture, texCoord - vec2(tex_offset.x, 0.0) * offset[i]).rgb * weight[i];
            }

            gl_FragColor = vec4(result, 1.0);
        }
    )SHADER";
}

std::string HorizontalBlurFilter::GetVertexShader()
{
    return R"SHADER(
        layout (location = 0) attribute vec3 vertexCoord;
        layout (location = 1) attribute vec2 inputTexCoord;
        varying vec2 texCoord;

        void main() {
            gl_Position = vec4(vertexCoord, 1.0);
            texCoord = inputTexCoord;
        }
    )SHADER";
}