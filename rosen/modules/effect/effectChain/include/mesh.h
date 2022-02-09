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

#ifndef MESH_H
#define MESH_H

#include <iostream>
#include <GLES3/gl32.h>

namespace OHOS {
namespace Rosen {
class Mesh {
public:
    static constexpr int DEFAULT_STRIDE = 5;
    static constexpr int DEFAULT_OFFSET = 3;
    static constexpr int DEFAULT_VERTEX_POINT_SIZE = 3;
    static constexpr int DEFAULT_TEXTURE_POINT_SIZE = 2;

    Mesh();
    ~Mesh();
    void Use();
    void Delete();
    unsigned int VAO_;

private:
    unsigned int VBO_;
    unsigned int EBO_;
    int verticesSize_;
    int indicesSize_;
    int verticesNumber_;
    int indicesNumber_;
    float* mVertices_ = nullptr;
    unsigned int* mIndices_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS
#endif

