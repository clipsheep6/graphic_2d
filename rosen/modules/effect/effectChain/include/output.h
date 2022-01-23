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

#ifndef OUTPUT_H
#define OUTPUT_H

#include "algo_filter.h"

class Output :public AlgoFilter
{
public:
    Output();
    std::string GetVertexShader() override;
    std::string GetFragmentShader() override;
    virtual filter_type GetFilterType()override;
    void SaveRenderResultToImage(std::string name, int width = -1, int height = -1);
protected:
    void RenderOnScreen(GLuint& RenderTextureID);
    bool saveResult_ = false;
    int imageWidth_;
    int imageHeight_;
    std::string imageName_;
};

#endif

