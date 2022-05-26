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

#include "output.h"

std::string Output::GetVertexShader() {
    return R"SHADER(
        layout (location = 0) attribute vec3 vertexCoord;
        layout (location = 1) attribute vec2 inputTexCoord;
        varying vec2 texCoord;

        void main() {
            gl_Position = vec4(vertexCoord, 1.0);
            texCoord = vec2(inputTexCoord.x, inputTexCoord.y);
        }
    )SHADER";
}

std::string Output::GetFragmentShader() {
    return R"SHADER(
        varying vec2 texCoord;
        uniform sampler2D uTexture;
        void main() {
            gl_FragColor = texture(uTexture, texCoord)* 1.0;
        }
    )SHADER";
}

filter_type Output::GetFilterType() {
    return filter_type::OUTPUT;
}

void Output::SaveRenderResultToImage(std::string name, int width, int height) {
    saveResult_ = true;
    imageName_ = name;
    imageWidth_ = width;
    imageHeight_ = height;
}

void Output::RenderOnScreen(GLuint& RenderTextureID) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    Use();
    glBindTexture(GL_TEXTURE_2D, RenderTextureID);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

Output::Output() {
    CreateProgram(GetVertexShader(), GetFragmentShader());
}
