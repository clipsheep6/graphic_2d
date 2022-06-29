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

#ifndef FILTER_H
#define FILTER_H

#include <string>
#include <vector>
#include <iostream>
#include <GLES2/gl2.h>
#include <fstream>
#include <sstream>
#include "program.h"

enum class filter_type {
    INPUT, ALGOFILTER, MEGERFILTER, OUTPUT
};

struct ProcessDate {
    GLuint srcTextureID;
    GLuint outputTextureID;
    GLuint frameBufferID;
    int textureWidth;
    int textureHeight;
};

class Filter {
public:
    virtual filter_type GetFilterType() = 0;

    virtual void Process(ProcessDate& data);

    virtual void DoProcess(ProcessDate& data) = 0;

    virtual void AddNextFilter(Filter* next);

    virtual void AddPreviousFilter(Filter* previous);

    virtual Filter* GetNextFilter(void);

    virtual Filter* GetPreviousFilter(void);

    virtual int GetInputNumber();

    virtual int GetOutputNumber();

    virtual int GetMaxInputNumber();

    virtual int GetMaxOutputNumber();
protected:
    int nextNum_ = 0;
    int preNum_ = 0;
    int nextPtrMax_ = 1;
    int prePtrMax_ = 1;
    Filter* previous_ = nullptr;
    Filter* next_ = nullptr;
};

#endif