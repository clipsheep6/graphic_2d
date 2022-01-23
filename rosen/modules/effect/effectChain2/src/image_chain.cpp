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

#include "image_chain.h"

ImageChain::ImageChain(std::vector<Input*> inputs) : myInputs_(inputs)
{
    CreatTexture(_srcTexture);
    CreatTexture(_outputTexture);

    glGenFramebuffers(1, &_frameBuffer);
    if (inputs.size() == 1)
    {
        _flagSeries = true;
    }
    else
    {
        CreatTexture(_resultTexture);
    }
}

void ImageChain::Render()
{
    if (_flagSeries)
    {
        myInputs_.at(0)->BindImageOnTexture(_srcTexture);
        SeriesRendering();
    }
}


void ImageChain::SeriesRendering()
{
    int width = myInputs_.at(0)->GetImageWidth();
    int height = myInputs_.at(0)->GetImageHeight();
    ProcessDate data
    {_srcTexture, _outputTexture, _frameBuffer, width, height};
    myInputs_.at(0)->Process(data);
}

void ImageChain::ParallelRendering()
{
}

void ImageChain::CreatTexture(unsigned int& TextureID)
{
    glGenTextures(1, &TextureID);
    glBindTexture(GL_TEXTURE_2D, TextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}
