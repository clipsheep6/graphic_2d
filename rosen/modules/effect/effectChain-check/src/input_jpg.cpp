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

#include "input_jpg.h"

InputJPG::InputJPG(std::string imageURL):Input(imageURL)
{
}

void InputJPG::BindImageOnTexture(GLuint TextureID)
{
    glBindTexture(GL_TEXTURE_2D, TextureID);
    stbi_set_flip_vertically_on_load(true);
    int nrChannels;
    std::string imagePath = imageURL_.c_str();
    unsigned char* data = stbi_load(imageURL_.c_str(), &textureWidth, &textureHeight, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
}