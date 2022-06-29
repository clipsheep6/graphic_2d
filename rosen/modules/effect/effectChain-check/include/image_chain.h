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

#ifndef IMAGE_CHAIN_H
#define IMAGE_CHAIN_H

#include "algo_filter.h"
#include "input.h"

class ImageChain
{
public:
    ImageChain(std::vector< Input* > inputs);
    void Render();
private:
    void CreatTexture(unsigned int& TextureID);
    void SeriesRendering();
    void ParallelRendering();
    bool _flagSeries = false;
    unsigned int _frameBuffer;
    unsigned int _srcTexture;
    unsigned int _outputTexture;
    unsigned int _resultTexture;
    std::vector< Input* > myInputs_;
};

#endif