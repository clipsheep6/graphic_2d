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

#include "scale_filter.h"

ScaleFilter::ScaleFilter()
{
    CreateProgram(GetVertexShader(), GetFragmentShader());
}

std::string ScaleFilter::GetVertexShader()
{
    return R"SHADER(
        layout (location = 0) attribute vec3 vertexCoord;
        layout (location = 1) attribute vec2 inputTexCoord;
        varying vec2 texCoord;

        void main()
        {
            gl_Position = vec4(vertexCoord, 1.0); 
            texCoord = vec2(inputTexCoord.x, inputTexCoord.y);
        }
    )SHADER";
}

std::string ScaleFilter::GetFragmentShader()
{
	return R"SHADER(
        varying vec2 texCoord;
        uniform sampler2D uTexture;
        void main()
        {
            gl_FragColor = texture(uTexture, texCoord)* 1.0;
        }
	)SHADER";
}
//
void ScaleFilter::SetScale(float scale)
{
    scale_ = scale;
}
//
void ScaleFilter::Prepare(ProcessDate& data)
{
    data.textureHeight = floorf(scale_ * data.textureHeight);
    data.textureWidth = floorf(scale_ * data.textureWidth);
    glBindTexture(GL_TEXTURE_2D, data.outputTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data.textureWidth, data.textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindFramebuffer(GL_FRAMEBUFFER, data.frameBufferID);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, data.outputTextureID, 0);
    glViewport(0, 0, data.textureWidth, data.textureHeight);
    glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void ScaleFilter::DoProcess(ProcessDate& data)
{
    Prepare(data);
    Draw(data);
}

