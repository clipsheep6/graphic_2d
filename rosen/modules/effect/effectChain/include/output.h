/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "image_source.h"
#include "image_packer.h"

namespace OHOS {
namespace Rosen {
#pragma pack(2)

struct RGBAColor {
    unsigned char R;
    unsigned char G;
    unsigned char B;
    unsigned char A;
};

#pragma pack()

class Output : public AlgoFilter {
public:
    Output();
    virtual ~Output();
    void SetValue(char* key, void *value, int size) override;
    void DoProcess(ProcessData& data) override;
    std::string GetVertexShader() override;
    std::string GetFragmentShader() override;
    virtual FILTER_TYPE GetFilterType() override;

protected:
    const char* format_ = nullptr;
    const char* dstImagePath_ = nullptr;
    RGBAColor* colorBuffer = nullptr;

private:
    void LoadFilterParams() override {};
};
} // namespace Rosen
} // namespace OHOS
#endif

