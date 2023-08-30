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

#ifndef ROSENRENDER_ROSEN_WEBGL_IMAGE_SOURCE
#define ROSENRENDER_ROSEN_WEBGL_IMAGE_SOURCE

#include <memory>
#include <map>
#include "../../../common/napi/n_exporter.h"

namespace OHOS {
namespace Rosen {
struct WebGLImageSource
{
public:
    GLsizei width;
    GLsizei height;
    std::unique_ptr<char[]> source;

    static std::unique_ptr<WebGLImageSource> GetImageSource(napi_env env, napi_value texImageSource);
};

struct WebGLImage2DArg {
    GLenum target;
    GLint level;
    GLint internalFormat;
    GLsizei width;
    GLsizei height;
    GLint border;
    GLenum format;
    GLenum type;
    uint8_t *source;
    WebGLImageSource *imageSource;
    void Dump();
};

struct WebGLSubImage2DArg {
    GLenum target;
    GLint level;
    GLint xoffset;
    GLint yoffset;
    GLsizei width;
    GLsizei height;
    GLenum format;
    GLenum type;
    uint8_t *source;
    GLintptr pboOffset;
    WebGLImageSource *imageSource;

    void Dump();
};

struct WebGLImage3DArg {
    GLenum target;
    GLint level;
    GLint internalFormat;
    GLsizei width;
    GLsizei height;
    GLsizei depth;
    GLint border;
    GLenum format;
    GLenum type;
    uint8_t *source;
    WebGLImageSource *imageSource;
};

} // namespace Rosen
} // namespace OHOS

#endif // ROSENRENDER_ROSEN_WEBGL_IMAGE_SOURCE