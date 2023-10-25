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

#include "webgl/webgl_texture.h"

#include "napi/n_class.h"
#include "napi/n_func_arg.h"
#include "util/util.h"

namespace OHOS {
namespace Rosen {
using namespace std;

napi_value WebGLTexture::Constructor(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        return nullptr;
    }

    unique_ptr<WebGLTexture> webGlTexture = make_unique<WebGLTexture>();
    if (!NClass::SetEntityFor<WebGLTexture>(env, funcArg.GetThisVar(), move(webGlTexture))) {
        return nullptr;
    }
    return funcArg.GetThisVar();
}

bool WebGLTexture::Export(napi_env env, napi_value exports)
{
    vector<napi_property_descriptor> props = {};

    string className = GetClassName();
    bool succ = false;
    napi_value clas = nullptr;
    tie(succ, clas) = NClass::DefineClass(exports_.env_, className, WebGLTexture::Constructor, std::move(props));
    if (!succ) {
        return false;
    }
    succ = NClass::SaveClass(exports_.env_, className, clas);
    if (!succ) {
        return false;
    }

    return exports_.AddProp(className, clas);
}

string WebGLTexture::GetClassName()
{
    return WebGLTexture::className;
}

int64_t WebGLTexture::ComputeTextureLevel(int64_t width, int64_t height, int64_t depth)
{
    // return 1 + log2Floor(std::max(width, height));
    int64_t max = std::max(std::max(width, height), depth);
    if (max <= 0)
        return 0;

    double result = log2(max);
    return static_cast<int64_t>(result - 0.5) + 1;
}

GLint WebGLTexture::GetMaxTextureLevelForTarget(GLenum target)
{
    static GLint maxTextureLevel = 0;
    static GLint maxCubeMapTextureLevel = 0;
    GLint max = 0;
    switch (target) {
        case GL_TEXTURE_2D: {
            if (maxTextureLevel != 0) {
                return maxTextureLevel;
            }

            glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
            maxTextureLevel = ComputeTextureLevel(max, max, 1);
            return maxTextureLevel;
        }
        case GL_TEXTURE_CUBE_MAP:
        case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
        case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
        case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z: {
            if (maxCubeMapTextureLevel != 0) {
                return maxCubeMapTextureLevel;
            }
            glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &max);
            maxCubeMapTextureLevel = ComputeTextureLevel(max, max, 1);
            return maxCubeMapTextureLevel;
        }
    }
    return 0;
}

WebGLTexture::~WebGLTexture()
{
    m_textureLevelInfos.clear();
}

bool WebGLTexture::SetTarget(GLenum target)
{
    if (m_target) {
        return true;
    }
    m_target = target;
    int maxTextureLevelCtrl = 1;
    switch (target) {
        case GL_TEXTURE_2D:
        case GL_TEXTURE_2D_ARRAY:
        case GL_TEXTURE_3D:
            m_target = target;
            maxTextureLevelCtrl = 1;
            break;
        case GL_TEXTURE_CUBE_MAP:
            m_target = target;
            maxTextureLevelCtrl = 6; // 6 for GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
            break;
        default:
            return false;
    }
    m_textureLevelInfos.resize(maxTextureLevelCtrl);
    for (int i = 0; i < maxTextureLevelCtrl; ++i) {
        m_textureLevelInfos[i].Init(target);
    }
    return true;
}

const TextureLevelInfo* WebGLTexture::GetTextureLevel(GLenum target, GLint level) const
{
    int index = -1;
    if (m_target == GL_TEXTURE_2D) {
        index = target - m_target;
    } else if (m_target == GL_TEXTURE_CUBE_MAP) {
        index = target - GL_TEXTURE_CUBE_MAP_POSITIVE_X;
    } else if (m_target == GL_TEXTURE_3D) {
        index = target - m_target;
    } else if (m_target == GL_TEXTURE_2D_ARRAY) {
        index = target - m_target;
    }
    if (index < 0 || index >= static_cast<GLint>(m_textureLevelInfos.size())) {
        return nullptr;
    }
    return m_textureLevelInfos[index].GetTextureLevel(level);
}

bool WebGLTexture::SetTextureLevel(const TexImageArg* arg)
{
    TextureLevelInfo* levelInfo = const_cast<TextureLevelInfo*>(GetTextureLevel(arg->target, arg->level));
    if (levelInfo) {
        levelInfo->valid = true;
        levelInfo->internalFormat = arg->internalFormat;
        levelInfo->width = arg->width;
        levelInfo->height = arg->height;
        levelInfo->depth = arg->depth;
        levelInfo->type = arg->type;
    }
    return true;
}

bool WebGLTexture::SetTexStorageInfo(const TexStorageArg* arg, GLenum type)
{
    for (size_t i = 0; i < m_textureLevelInfos.size(); ++i) {
        GLsizei levelWidth = arg->width;
        GLsizei levelHeight = arg->height;
        GLsizei levelDepth = arg->depth;
        for (GLint level = 0; level < arg->levels; ++level) {
            TextureLevelInfo* levelInfo = const_cast<TextureLevelInfo*>(GetTextureLevel(arg->target, level));
            if (levelInfo != nullptr) {
                levelInfo->width = levelWidth;
                levelInfo->height = levelHeight;
                levelInfo->depth = levelDepth;
                levelInfo->type = type;
            }
            levelWidth = std::max(1, levelWidth >> 1);
            levelHeight = std::max(1, levelHeight >> 1);
            levelDepth = std::max(1, levelDepth >> 1);
        }
    }
    m_immutable = true;
    return true;
}
/*
GLenum WebGLTexture::GetTypeFromInternalFormat(GLenum internalFormat)
{
    switch (internalFormat) {
        case GL_RGBA4:
            return GL_UNSIGNED_SHORT_4_4_4_4;
        case GL_RGB565:
            return GL_UNSIGNED_SHORT_5_6_5;
        case GL_RGB5_A1:
            return GL_UNSIGNED_SHORT_5_5_5_1;
        case GL_DEPTH_COMPONENT16:
            return GL_UNSIGNED_SHORT;
        case GL_R8:
            return GL_UNSIGNED_BYTE;
        case GL_R8_SNORM:
            return GL_BYTE;
        case GL_R16F:
            return GL_HALF_FLOAT;
        case GL_R32F:
            return GL_FLOAT;
        case GL_R8UI:
            return GL_UNSIGNED_BYTE;
        case GL_R8I:
            return GL_BYTE;
        case GL_R16UI:
            return GL_UNSIGNED_SHORT;
        case GL_R16I:
            return GL_SHORT;
        case GL_R32UI:
            return GL_UNSIGNED_INT;
        case GL_R32I:
            return GL_INT;
        case GL_RG8:
            return GL_UNSIGNED_BYTE;
        case GL_RG8_SNORM:
            return GL_BYTE;
        case GL_RG16F:
            return GL_HALF_FLOAT;
        case GL_RG32F:
            return GL_FLOAT;
        case GL_RG8UI:
            return GL_UNSIGNED_BYTE;
        case GL_RG8I:
            return GL_BYTE;
        case GL_RG16UI:
            return GL_UNSIGNED_SHORT;
        case GL_RG16I:
            return GL_SHORT;
        case GL_RG32UI:
            return GL_UNSIGNED_INT;
        case GL_RG32I:
            return GL_INT;
        case GL_RGB8:
            return GL_UNSIGNED_BYTE;
        case GL_SRGB8:
            return GL_UNSIGNED_BYTE;
        case GL_RGB8_SNORM:
            return GL_BYTE;
        case GL_R11F_G11F_B10F:
            return GL_UNSIGNED_INT_10F_11F_11F_REV;
        case GL_RGB9_E5:
            return GL_UNSIGNED_INT_5_9_9_9_REV;
        case GL_RGB16F:
            return GL_HALF_FLOAT;
        case GL_RGB32F:
            return GL_FLOAT;
        case GL_RGB8UI:
            return GL_UNSIGNED_BYTE;
        case GL_RGB8I:
            return GL_BYTE;
        case GL_RGB16UI:
            return GL_UNSIGNED_SHORT;
        case GL_RGB16I:
            return GL_SHORT;
        case GL_RGB32UI:
            return GL_UNSIGNED_INT;
        case GL_RGB32I:
            return GL_INT;
        case GL_RGBA8:
            return GL_UNSIGNED_BYTE;
        case GL_SRGB8_ALPHA8:
            return GL_UNSIGNED_BYTE;
        case GL_RGBA8_SNORM:
            return GL_BYTE;
        case GL_RGB10_A2:
            return GL_UNSIGNED_INT_2_10_10_10_REV;
        case GL_RGBA16F:
            return GL_HALF_FLOAT;
        case GL_RGBA32F:
            return GL_FLOAT;
        case GL_RGBA8UI:
            return GL_UNSIGNED_BYTE;
        case GL_RGBA8I:
            return GL_BYTE;
        case GL_RGB10_A2UI:
            return GL_UNSIGNED_INT_2_10_10_10_REV;
        case GL_RGBA16UI:
            return GL_UNSIGNED_SHORT;
        case GL_RGBA16I:
            return GL_SHORT;
        case GL_RGBA32I:
            return GL_INT;
        case GL_RGBA32UI:
            return GL_UNSIGNED_INT;

        case GL_DEPTH_COMPONENT24:
            return GL_UNSIGNED_INT;
        case GL_DEPTH_COMPONENT32F:
            return GL_FLOAT;
        case GL_DEPTH24_STENCIL8:
            return GL_UNSIGNED_INT_24_8;
        case GL_DEPTH32F_STENCIL8:
            return GL_FLOAT_32_UNSIGNED_INT_24_8_REV;
        // Compressed types.
        case GL_ATC_RGB_AMD:
        case GL_ATC_RGBA_EXPLICIT_ALPHA_AMD:
        case GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD:
        case GL_ETC1_RGB8_OES:
        case GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG:
        case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
        case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
        case GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG:
        case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG:
        case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
        case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
        case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
            return GL_UNSIGNED_BYTE;
        default:
            return GL_NONE;
    }
}
*/

GLenum WebGLTexture::GetInternalFormat(GLenum target, GLint level) const
{
    const TextureLevelInfo* levelInfo = GetTextureLevel(target, level);
    if (levelInfo) {
        return levelInfo->internalFormat;
    }
    return 0;
}

GLenum WebGLTexture::GetType(GLenum target, GLint level) const
{
    const TextureLevelInfo* levelInfo = GetTextureLevel(target, level);
    if (levelInfo) {
        return levelInfo->type;
    }
    return 0;
}

GLsizei WebGLTexture::GetWidth(GLenum target, GLint level) const
{
    const TextureLevelInfo* levelInfo = GetTextureLevel(target, level);
    if (levelInfo) {
        return levelInfo->width;
    }
    return 0;
}

GLsizei WebGLTexture::GetHeight(GLenum target, GLint level) const
{
    const TextureLevelInfo* levelInfo = GetTextureLevel(target, level);
    if (levelInfo) {
        return levelInfo->height;
    }
    return 0;
}

GLsizei WebGLTexture::GetDepth(GLenum target, GLint level) const
{
    const TextureLevelInfo* levelInfo = GetTextureLevel(target, level);
    if (levelInfo) {
        return levelInfo->depth;
    }
    return 0;
}

bool WebGLTexture::CheckValid(GLenum target, GLint level) const
{
    const TextureLevelInfo* levelInfo = GetTextureLevel(target, level);
    if (levelInfo) {
        return levelInfo->valid;
    }
    return 0;
}

void TextureLevelCtrl::Init(GLenum target)
{
    GLint maxLevel = WebGLTexture::GetMaxTextureLevelForTarget(target);
    if (maxLevel > 0) {
        m_textureInfos.resize(maxLevel);
    }
}

void TexImageArg::Dump(const std::string& info) const
{
    LOGI("%s target %{public}u %{public}d internalFormat %{public}u format %{public}u %{public}u", info.c_str(), target,
        level, internalFormat, format, type);
    LOGI("%s width %{public}d height %{public}d border %{public}d depth %{public}d", info.c_str(), width, height,
        border, depth);
}

void TexStorageArg::Dump(const std::string& info) const
{
    LOGI("%s target %{public}u %{public}d internalFormat %{public}u ", info.c_str(), target, levels, internalFormat);
    LOGI("%s width %{public}d height %{public}d depth %{public}d", info.c_str(), width, height, depth);
}

} // namespace Rosen
} // namespace OHOS
