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

#include "algo_filter.h"

AlgoFilter::AlgoFilter()
{
        myMesh_ = new Mesh();
        myMesh_->Use();
}
void AlgoFilter::Prepare(ProcessDate& data)
{
    glBindTexture(GL_TEXTURE_2D, data.outputTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data.textureWidth, data.textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindFramebuffer(GL_FRAMEBUFFER, data.frameBufferID);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, data.outputTextureID, 0);
    glViewport(0, 0, data.textureWidth, data.textureHeight);
    glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void AlgoFilter::Draw(ProcessDate& data)
{
    Use();
    glBindTexture(GL_TEXTURE_2D, data.srcTextureID);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void AlgoFilter::CreateProgram(std::string vertexString, std::string fragmentString)
{
    mProgram_ = new Program();
    mProgram_->Compile(vertexString, fragmentString);
}


AlgoFilter::~AlgoFilter()
{
    delete mProgram_;
    delete myMesh_;
}

void AlgoFilter::Use()
{
    if (mProgram_ != nullptr) {
        mProgram_->UseProgram();
    }
}

filter_type AlgoFilter::GetFilterType()
{
    return filter_type::ALGOFILTER;
}

void AlgoFilter::SetValue(std::string key, void* value, int size)
{
}