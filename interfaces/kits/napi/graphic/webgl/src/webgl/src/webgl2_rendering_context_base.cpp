/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "../include/context/webgl2_rendering_context_base.h"
#include "../../common/napi/n_func_arg.h"
#include "../../common/napi/n_class.h"
#include "../include/webgl/webgl_active_info.h"
#include "../include/webgl/webgl_shader.h"
#include "../include/webgl/webgl_buffer.h"
#include "../include/webgl/webgl_framebuffer.h"
#include "../include/webgl/webgl_program.h"
#include "../include/webgl/webgl_renderbuffer.h"
#include "../include/webgl/webgl_texture.h"
#include "../include/webgl/webgl_sync.h"
#include "../include/webgl/webgl_sampler.h"
#include "../include/webgl/webgl_query.h"
#include "../include/webgl/webgl_transform_feedback.h"
#include "../include/webgl/webgl_uniform_location.h"
#include "../include/webgl/webgl_vertex_array_object.h"
#include "../include/webgl/webgl_image_source.h"
#include "../include/util/log.h"
#include "../include/util/util.h"
#include "../include/util/buffer_data.h"

#ifdef __cplusplus
extern "C" {
#endif

namespace OHOS {
namespace Rosen {
namespace {
    const int UNIFORM_NAMES_MAX_LENGTH = 1000;
}

using namespace std;
static WebGLObjectManager *GetWebGLQueryMaps(napi_env env, napi_value thisVar)
{
    return WebGLObjectManager::GetWebGLObjectManager(env, thisVar, WebGLObjectManager::WEBGL_OBJECT_QUERY);
}
static WebGL2RenderingContextBase *GetWebGL2RenderingContextBase(napi_env env, napi_value thisVar)
{
    return reinterpret_cast<WebGL2RenderingContextBase *>(Util::GetContextObject(env, thisVar, "webgl2"));
}

static WebGLObjectManager *GetWebGLVertexArrayObjectMaps(napi_env env, napi_value thisVar)
{
    return WebGLObjectManager::GetWebGLObjectManager(env, thisVar, WebGLObjectManager::WEBGL_OBJECT_VERTEX_ARRAY);
}

napi_value WebGL2RenderingContextBase::DrawBuffers(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    ReadBufferData bufferData(env);
    napi_status status = bufferData.GetBufferData(funcArg[NARG_POS::FIRST], buffer_data_glenum);
    if (status != 0) {
        LOGE("WebGL glDrawBuffers failed to getbuffer data");
        return nullptr;
    }
    if (bufferData.GetBufferType() != buffer_array) {
        LOGE("WebGL glDrawBuffers invalid buffer type %d", bufferData.GetBufferType());
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    GLenum *data = reinterpret_cast<GLenum *>(bufferData.GetBuffer());
    GLsizei length = static_cast<GLsizei>(bufferData.GetBufferLength() / sizeof(GLenum));
    glDrawBuffers(length, data);
    return nullptr;
}

napi_value WebGL2RenderingContextBase::ClearBufferfv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::THREE, NARG_CNT::FOUR)) {
        return nullptr;
    }

    LOGI("WebGL2 clearBufferfv start");
    bool succ = false;
    int64_t buffer;
    tie(succ, buffer) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 clearBufferfv buffer = %{public}u", buffer);
    int64_t drawbuffer;
    tie(succ, drawbuffer) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 clearBufferfv drawbuffer = %{public}u", drawbuffer);

    if (funcArg[NARG_POS::THIRD] == nullptr) {
        return nullptr;
    }
    int64_t srcOffset = 0;
    if (funcArg[NARG_POS::FOURTH] != nullptr) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 clearBufferfv srcOffset = %{public}u", srcOffset);
    }
    ReadBufferData bufferData(env);
    napi_status status = bufferData.GetBufferData(funcArg[NARG_POS::THIRD], buffer_data_float32);
    if (status != 0) {
        LOGE("WebGL BufferData failed to getbuffer data");
        return nullptr;
    }
    if (bufferData.GetBufferDataType() != buffer_data_float32) {
        LOGE("WebGL BufferData invalid buffer data type %{public}d", bufferData.GetBufferDataType());
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    glClearBufferfv(static_cast<GLenum>(buffer), static_cast<GLint>(drawbuffer),
        reinterpret_cast<GLfloat *>(bufferData.GetBuffer() + srcOffset));
    LOGI("WebGL2 clearBufferfv typeArray end");
    return nullptr;
}

napi_value WebGL2RenderingContextBase::ClearBufferiv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::THREE, NARG_CNT::FOUR)) {
        return nullptr;
    }

    LOGI("WebGL2 clearBufferiv start");

    bool succ = false;
    int64_t buffer;
    tie(succ, buffer) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 clearBufferiv buffer = %{public}u", buffer);
    int64_t drawbuffer;
    tie(succ, drawbuffer) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 clearBufferiv drawbuffer = %{public}u", drawbuffer);

    if (funcArg[NARG_POS::THIRD] == nullptr) {
        return nullptr;
    }
    int64_t srcOffset = 0;
    if (funcArg[NARG_POS::FOURTH] != nullptr) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 clearBufferiv srcOffset = %{public}u", srcOffset);
    }

    ReadBufferData bufferData(env);
    napi_status status = bufferData.GetBufferData(funcArg[NARG_POS::THIRD], buffer_data_int32);
    if (status != 0) {
        LOGE("WebGL glClearBufferiv failed to getbuffer data");
        return nullptr;
    }
    if (bufferData.GetBufferDataType() != buffer_data_int32) {
        LOGE("WebGL glClearBufferiv invalid buffer data type %{public}d", bufferData.GetBufferDataType());
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    if (bufferData.GetBufferType() == buffer_array) {
        glClearBufferiv(static_cast<GLenum>(buffer), static_cast<GLint>(drawbuffer),
            reinterpret_cast<GLint*>(bufferData.GetBuffer()));
    } else if (bufferData.GetBufferType() == buffer_typedarray) {
        glClearBufferiv(static_cast<GLenum>(buffer), static_cast<GLint>(drawbuffer),
            reinterpret_cast<GLint *>(bufferData.GetBuffer() + srcOffset));
    } else {
        LOGE("WebGL glClearBufferiv not support buffer type %{public}d", bufferData.GetBufferType());
        return nullptr;
    }
    return nullptr;
}

napi_value WebGL2RenderingContextBase::ClearBufferuiv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::THREE, NARG_CNT::FOUR)) {
        return nullptr;
    }

    LOGI("WebGL2 clearBufferuiv start");

    bool succ = false;
    int64_t buffer;
    tie(succ, buffer) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 clearBufferuiv buffer = %{public}u", buffer);
    int64_t drawbuffer;
    tie(succ, drawbuffer) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 clearBufferuiv drawbuffer = %{public}u", drawbuffer);

    if (funcArg[NARG_POS::THIRD] == nullptr) {
        return nullptr;
    }
    int64_t srcOffset = 0;
    if (funcArg[NARG_POS::FOURTH] != nullptr) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 clearBufferuiv srcOffset = %{public}u", srcOffset);
    }

    ReadBufferData bufferData(env);
    napi_status status = bufferData.GetBufferData(funcArg[NARG_POS::THIRD], buffer_data_int32);
    if (status != 0) {
        LOGE("WebGL glClearBufferiv failed to getbuffer data");
        return nullptr;
    }
    if (bufferData.GetBufferDataType() != buffer_data_int32) {
        LOGE("WebGL glClearBufferiv invalid buffer data type %{public}d", bufferData.GetBufferDataType());
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    if (bufferData.GetBufferType() == buffer_array) {
        glClearBufferuiv(static_cast<GLenum>(buffer), static_cast<GLint>(drawbuffer),
            reinterpret_cast<GLuint*>(bufferData.GetBuffer()));
    } else if (bufferData.GetBufferType() == buffer_typedarray) {
        glClearBufferuiv(static_cast<GLenum>(buffer), static_cast<GLint>(drawbuffer),
            reinterpret_cast<GLuint *>(bufferData.GetBuffer() + srcOffset));
    } else {
        LOGE("WebGL glClearBufferiv not support buffer type %{public}d", bufferData.GetBufferType());
        return nullptr;
    }

    LOGI("WebGL2 clearBufferuiv typeArray end");
    return nullptr;
}

napi_value WebGL2RenderingContextBase::ClearBufferfi(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 clearBufferfi start");
    int64_t buffer;
    tie(succ, buffer) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 clearBufferfi buffer = %{public}u", buffer);
    int64_t drawbuffer;
    tie(succ, drawbuffer) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 clearBufferfi drawbuffer = %{public}u", drawbuffer);
    double depth;
    tie(succ, depth) = NVal(env, funcArg[NARG_POS::THIRD]).ToDouble();
    if (!succ) {
        return nullptr;
    }

    int64_t stencil;
    tie(succ, stencil) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 clearBufferfi stencil = %{public}u", stencil);
    glClearBufferfi(static_cast<GLenum>(buffer), static_cast<GLint>(drawbuffer),
        static_cast<GLfloat>((float) depth), static_cast<GLint>(stencil));
    LOGI("WebGL2 clearBufferfi end");
    return nullptr;
}

napi_value WebGL2RenderingContextBase::CreateQuery(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        return nullptr;
    }

    WebGLObjectManager *maps = GetWebGLQueryMaps(env, funcArg.GetThisVar());
    if (maps == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    WebGLQuery *webGlQuery = nullptr;
    napi_value objQuery = WebGLQuery::CreateObjectInstance(env, &webGlQuery).val_;
    if (objQuery == nullptr) {
        return NVal::CreateNull(env).val_;
    }

    unsigned int queryId;
    glGenQueries(1, &queryId);
    webGlQuery->SetQuery(queryId);
    maps->AddObject(queryId, objQuery);
    LOGI("WebGL2 createQuery queryId = %{public}u", queryId);
    return objQuery;
}

napi_value WebGL2RenderingContextBase::DeleteQuery(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    LOGI("WebGL2 deleteQuery start");
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }

    WebGLQuery *webGlQuery = nullptr;
    napi_status queryStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlQuery);
    if (queryStatus != napi_ok) {
        return nullptr;
    }
    unsigned int query = webGlQuery->GetQuery();
    glDeleteQueries(1, &query);

    WebGLObjectManager *maps = GetWebGLQueryMaps(env, funcArg.GetThisVar());
    if (maps == nullptr) {
        return nullptr;
    }
    maps->DeleteObject(query);
    LOGI("WebGL2 deleteQuery queryId = %{public}u", query);
    return nullptr;
}

napi_value WebGL2RenderingContextBase::IsQuery(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateBool(env, false).val_;
    }
    LOGI("WebGL2 isQuery start");
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return NVal::CreateBool(env, false).val_;
    }

    WebGLQuery *webGlQuery = nullptr;
    napi_status queryStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlQuery);
    if (queryStatus != napi_ok) {
        return NVal::CreateBool(env, false).val_;
    }
    unsigned int query = webGlQuery->GetQuery();
    GLboolean returnValue = glIsQuery(static_cast<GLuint>(query));
    bool res = static_cast<bool>(returnValue);
    LOGI("WebGL2 isQuery query %{public}u res %{public}d ", query, res);
    return NVal::CreateBool(env, res).val_;
}

napi_value WebGL2RenderingContextBase::BeginQuery(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }

    if (funcArg[NARG_POS::SECOND] == nullptr) {
        return nullptr;
    }

    WebGLQuery *webGlQuery = nullptr;
    napi_status queryStatus = napi_unwrap(env, funcArg[NARG_POS::SECOND], (void **) &webGlQuery);
    if (queryStatus != napi_ok) {
        return nullptr;
    }
    unsigned int query = webGlQuery->GetQuery();

    WebGL2RenderingContextBase *obj = GetWebGL2RenderingContextBase(env, funcArg.GetThisVar());
    if (obj == nullptr) {
        return nullptr;
    }

    LOGI("WebGL2 BeginQuery query %{public}u target %{public}u", query, target);
    obj->queryMaps.insert({ static_cast<GLenum>(target), static_cast<GLuint>(query) });
    glBeginQuery(static_cast<GLenum>(target), static_cast<GLuint>(query));
    return nullptr;
}

napi_value WebGL2RenderingContextBase::EndQuery(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    bool succ = false;
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    WebGL2RenderingContextBase *obj = GetWebGL2RenderingContextBase(env, funcArg.GetThisVar());
    if (obj == nullptr) {
        return nullptr;
    }
    LOGI("WebGL2 EndQuery target %{public}u", target);
    obj->queryMaps.erase(static_cast<GLenum>(target));
    glEndQuery(static_cast<GLenum>(target));
    return nullptr;
}

napi_value WebGL2RenderingContextBase::GetQuery(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 getQuery start");
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 getQuery target = %{public}u", target);

    int64_t pName;
    tie(succ, pName) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 getQuery pName = %{public}u", pName);

    WebGL2RenderingContextBase *obj = GetWebGL2RenderingContextBase(env, funcArg.GetThisVar());
    if (obj == nullptr) {
        return nullptr;
    }
    auto objects = obj->queryMaps;
    auto it = objects.find(static_cast<GLenum>(target));
    if (it == objects.end()) {
        return NVal::CreateNull(env).val_;
    }

    GLint params;
    glGetQueryiv(static_cast<GLenum>(target), static_cast<GLenum>(pName), &params);
    LOGI("WebGL2 getQuery params = %{public}u %{public}u", params, it->second);
    WebGLObjectManager *maps = GetWebGLQueryMaps(env, funcArg.GetThisVar());
    if (maps == nullptr) {
        return nullptr;
    }
    return maps->GetObject(it->second);
}

napi_value WebGL2RenderingContextBase::GetQueryParameter(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }

    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    LOGI("WebGL2 getQueryParameter start");
    WebGLQuery *webGLQuery = nullptr;
    napi_status status = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGLQuery);
    if (status != napi_ok) {
        return nullptr;
    }
    unsigned int queryId = webGLQuery->GetQuery();
    bool succ = false;
    int64_t pName;
    tie(succ, pName) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 getQueryParameter queryId %{public}u pName %{public}u", queryId, pName);
    GLuint params;
    glGetQueryObjectuiv(static_cast<GLuint>(queryId), static_cast<GLenum>(pName), &params);
    if (pName == GL_QUERY_RESULT) {
        int64_t res = static_cast<int64_t>(params);
        LOGI("WebGL2 getQueryParameter end");
        return NVal::CreateInt64(env, res).val_;
    } else if (pName == GL_QUERY_RESULT_AVAILABLE) {
        bool res = (params == GL_FALSE) ? false : true;
        LOGI("WebGL2 getQueryParameter end");
        return NVal::CreateBool(env, res).val_;
    } else {
        return nullptr;
    }
}

napi_value WebGL2RenderingContextBase::CreateSampler(napi_env env, napi_callback_info info)
{
    WebGLSampler *webGlSampler = nullptr;
    napi_value objSampler = WebGLSampler::CreateObjectInstance(env, &webGlSampler).val_;
    if (!objSampler) {
        return nullptr;
    }
    unsigned int samplerId;
    glGenSamplers(1, &samplerId);
    webGlSampler->SetSampler(samplerId);
    LOGI("WebGL2 createSampler samplerId = %{public}u", samplerId);
    return objSampler;
}

napi_value WebGL2RenderingContextBase::DeleteSampler(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }

    WebGLSampler *webGlSampler = nullptr;
    napi_status samplerStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlSampler);
    if (samplerStatus != napi_ok) {
        return nullptr;
    }
    unsigned int samplerId = webGlSampler->GetSampler();
    LOGI("WebGL2 deleteSampler samplerId = %{public}u", samplerId);
    glDeleteSamplers(1, &samplerId);
    return nullptr;
}

napi_value WebGL2RenderingContextBase::IsSampler(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateBool(env, false).val_;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return NVal::CreateBool(env, false).val_;
    }

    WebGLSampler *webGlSampler = nullptr;
    napi_status samplerStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlSampler);
    if (samplerStatus != napi_ok) {
        return NVal::CreateBool(env, false).val_;
    }
    unsigned int samplerId = webGlSampler->GetSampler();
    GLboolean returnValue = glIsSampler(static_cast<GLuint>(samplerId));
    bool res = static_cast<bool>(returnValue);
    LOGI("WebGL2 IsSampler samplerId = %{public}u res %{public}u", samplerId, res);
    return NVal::CreateBool(env, res).val_;
}

napi_value WebGL2RenderingContextBase::BindSampler(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    int64_t unit;
    tie(succ, unit) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    if (funcArg[NARG_POS::SECOND] == nullptr) {
        return nullptr;
    }

    WebGLSampler *webGlSampler = nullptr;
    napi_status samplerStatus = napi_unwrap(env, funcArg[NARG_POS::SECOND], (void **) &webGlSampler);
    if (samplerStatus != napi_ok) {
        return nullptr;
    }
    unsigned int samplerId = webGlSampler->GetSampler();
    glBindSampler(static_cast<GLuint>(unit), static_cast<GLuint>(samplerId));
    LOGI("WebGL2 bindSampler unit = %{public}u samplerId = %{public}u", unit, samplerId);
    return nullptr;
}

napi_value WebGL2RenderingContextBase::SamplerParameteri(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }

    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }

    WebGLSampler *webGlSampler = nullptr;
    napi_status samplerStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlSampler);
    if (samplerStatus != napi_ok) {
        return nullptr;
    }
    unsigned int samplerId = webGlSampler->GetSampler();
    bool succ = false;
    int64_t pName;
    tie(succ, pName) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 samplerParameteri pName = %{public}u", pName);
    int64_t param;
    tie(succ, param) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 samplerParameteri param = %{public}u samplerId = %{public}u", param, samplerId);
    glSamplerParameteri(static_cast<GLuint>(samplerId), static_cast<GLenum>(pName), static_cast<GLint>(param));
    return nullptr;
}

napi_value WebGL2RenderingContextBase::SamplerParameterf(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }

    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    LOGI("WebGL2 samplerParameterf start");
    WebGLSampler *webGlSampler = nullptr;
    napi_status samplerStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlSampler);
    if (samplerStatus != napi_ok) {
        return nullptr;
    }
    unsigned int samplerId = webGlSampler->GetSampler();

    bool succ = false;
    int64_t pName;
    tie(succ, pName) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 samplerParameterf pName = %{public}u", pName);
    double param;
    tie(succ, param) = NVal(env, funcArg[NARG_POS::THIRD]).ToDouble();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 glSamplerParameterf param = %{public}f samplerId = %{public}u", param, samplerId);
    glSamplerParameterf(static_cast<GLuint>(samplerId), static_cast<GLenum>(pName),
        static_cast<GLfloat>((float) param));
    return nullptr;
}

napi_value WebGL2RenderingContextBase::GetSamplerParameter(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }

    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    LOGI("WebGL2 getSamplerParameter start");
    WebGLSampler *webGlSampler = nullptr;
    napi_status samplerStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlSampler);
    if (samplerStatus != napi_ok) {
        return nullptr;
    }
    unsigned int samplerId = webGlSampler->GetSampler();

    bool succ = false;
    int64_t pName;
    tie(succ, pName) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 getSamplerParameter pName = %{public}u", pName);
    if (pName == GL_TEXTURE_MAX_LOD || pName == GL_TEXTURE_MIN_LOD) {
        GLfloat params;
        glGetSamplerParameterfv(static_cast<GLuint>(samplerId), static_cast<GLenum>(pName), &params);
        float res = static_cast<float>(params);
        LOGI("WebGL2 getSamplerParameter samplerId = %{public}u params %{public}f", samplerId, params);
        return NVal::CreateDouble(env, (double) res).val_;
    } else {
        GLint params;
        glGetSamplerParameteriv(static_cast<GLuint>(samplerId), static_cast<GLenum>(pName), &params);
        int64_t res = static_cast<int64_t>(params);
        LOGI("WebGL2 getSamplerParameter samplerId = %{public}u params %{public}u", samplerId, params);
        return NVal::CreateInt64(env, res).val_;
    }
}

napi_value WebGL2RenderingContextBase::FenceSync(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    int64_t condition;
    tie(succ, condition) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 fenceSync condition = %{public}u", condition);

    int64_t flags;
    tie(succ, flags) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 fenceSync flags = %{public}u", flags);

    WebGLSync *webGlSync = nullptr;
    napi_value objSync = WebGLSync::CreateObjectInstance(env, &webGlSync).val_;
    if (!objSync) {
        return nullptr;
    }
    GLsync returnValue = glFenceSync(static_cast<GLenum>(condition), static_cast<GLbitfield>(flags));
    long value = reinterpret_cast<long>(returnValue);
    webGlSync->SetSync(value);
    LOGI("WebGL2 fenceSync sync = %{public}u", value);
    return objSync;
}

napi_value WebGL2RenderingContextBase::IsSync(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateBool(env, false).val_;
    }
    LOGI("WebGL2 isSync start");
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return NVal::CreateBool(env, false).val_;
    }

    WebGLSync *webGlSync = nullptr;
    napi_status syncStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlSync);
    if (syncStatus != napi_ok) {
        return NVal::CreateBool(env, false).val_;
    }
    long sync = webGlSync->GetSync();

    GLboolean returnValue = glIsSync(reinterpret_cast<GLsync>(sync));
    bool res = static_cast<bool>(returnValue);
    LOGI("WebGL2 isSync sync = %{public}u res %{public}u", sync, res);
    return NVal::CreateBool(env, res).val_;
}

napi_value WebGL2RenderingContextBase::DeleteSync(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    LOGI("WebGL2 deleteSync start");
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }

    WebGLSync *webGlSync = nullptr;
    napi_status syncStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlSync);
    if (syncStatus != napi_ok) {
        return nullptr;
    }
    long sync = webGlSync->GetSync();

    glDeleteSync(reinterpret_cast<GLsync>(sync));
    LOGI("WebGL2 deleteSync sync = %{public}u ", sync);
    return nullptr;
}

napi_value WebGL2RenderingContextBase::ClientWaitSync(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    int res = 0;
    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return NVal::CreateInt64(env, res).val_;
    }

    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return NVal::CreateInt64(env, res).val_;
    }
    WebGLSync *webGlSync = nullptr;
    napi_status syncStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlSync);
    if (syncStatus != napi_ok) {
        return NVal::CreateInt64(env, res).val_;
    }
    long sync = webGlSync->GetSync();

    bool succ = false;
    int64_t flags;
    tie(succ, flags) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return NVal::CreateInt64(env, res).val_;
    }
    LOGI("WebGL2 clientWaitSync flags = %{public}u", flags);
    int64_t timeout;
    tie(succ, timeout) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return NVal::CreateInt64(env, res).val_;
    }
    LOGI("WebGL2 clientWaitSync timeout = %{public}u", timeout);
    GLenum returnValue = glClientWaitSync(reinterpret_cast<GLsync>(sync), static_cast<GLbitfield>(flags),
                                          static_cast<GLuint64>(timeout));
    res = static_cast<int>(returnValue);
    LOGI("WebGL2 clientWaitSync sync = %{public}u res %{public}u", sync, res);
    return NVal::CreateInt64(env, res).val_;
}

napi_value WebGL2RenderingContextBase::WaitSync(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }

    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    LOGI("WebGL2 waitSync start");
    WebGLSync *webGlSync = nullptr;
    napi_status syncStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlSync);
    if (syncStatus != napi_ok) {
        return nullptr;
    }
    long sync = webGlSync->GetSync();

    bool succ = false;
    int64_t flags;
    tie(succ, flags) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 waitSync flags = %{public}u", flags);
    int64_t timeout;
    tie(succ, timeout) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 waitSync timeout = %{public}u", timeout);
    glWaitSync(reinterpret_cast<GLsync>(sync), static_cast<GLbitfield>(flags),
        static_cast<GLuint64>(timeout));
    LOGI("WebGL2 waitSync sync = %{public}u", sync);
    return nullptr;
}

napi_value WebGL2RenderingContextBase::GetSyncParameter(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }

    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    LOGI("WebGL2 getSyncParameter start");
    WebGLSync *webGlSync = nullptr;
    napi_status syncStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlSync);
    if (syncStatus != napi_ok) {
        return nullptr;
    }
    long sync = webGlSync->GetSync();

    bool succ = false;
    int64_t pname;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 getSyncParameter sync = %{public}u pname = %{public}u ", sync, pname);
    GLint status;
    glGetSynciv(reinterpret_cast<GLsync>(sync), static_cast<GLenum>(pname),
        sizeof(GLint), nullptr, &status);
    int res = static_cast<int>(status);
    LOGI("WebGL2 getSyncParameter sync = %{public}u status = %{public}u ", sync, status);
    return NVal::CreateInt64(env, res).val_;
}

napi_value WebGL2RenderingContextBase::CreateTransformFeedback(napi_env env, napi_callback_info info)
{
    LOGI("WebGL2 createTransformFeedback start");
    WebGLTransformFeedback *webGlTransformFeedback = nullptr;
    napi_value objTransformFeedback = WebGLTransformFeedback::CreateObjectInstance(env, &webGlTransformFeedback).val_;
    if (!objTransformFeedback) {
        return nullptr;
    }
    unsigned int transformFeedbackId;
    glGenTransformFeedbacks(1, &transformFeedbackId);
    webGlTransformFeedback->SetTransformFeedback(transformFeedbackId);
    LOGI("WebGL2 createTransformFeedback transformFeedbackId %{public}u", transformFeedbackId);
    return objTransformFeedback;
}

napi_value WebGL2RenderingContextBase::DeleteTransformFeedback(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    LOGI("WebGL2 deleteTransformFeedback start");
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }

    WebGLTransformFeedback *webGlTransformFeedback = nullptr;
    napi_status transformFeedbackStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST],
        (void **)&webGlTransformFeedback);
    if (transformFeedbackStatus != napi_ok) {
        return nullptr;
    }
    unsigned int transformFeedback = static_cast<unsigned int>(webGlTransformFeedback->GetTransformFeedback());

    glDeleteTransformFeedbacks(1, &transformFeedback);
    LOGI("WebGL2 deleteTransformFeedback transformFeedbackId %{public}u", transformFeedback);
    return nullptr;
}

napi_value WebGL2RenderingContextBase::IsTransformFeedback(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateBool(env, false).val_;
    }
    LOGI("WebGL2 isTransformFeedback start");
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return NVal::CreateBool(env, false).val_;
    }

    WebGLTransformFeedback *webGlTransformFeedback = nullptr;
    napi_status transformFeedbackStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST],
        (void **) &webGlTransformFeedback);
    if (transformFeedbackStatus != napi_ok) {
        return NVal::CreateBool(env, false).val_;
    }
    unsigned int transformFeedback = static_cast<unsigned int>(webGlTransformFeedback->GetTransformFeedback());

    GLboolean returnValue = glIsTransformFeedback(static_cast<GLuint>(transformFeedback));
    bool res = static_cast<bool>(returnValue);
    LOGI("WebGL2 deleteTransformFeedback transformFeedbackId %{public}u %{public}u", transformFeedback, res);
    return NVal::CreateBool(env, res).val_;
}

napi_value WebGL2RenderingContextBase::BindTransformFeedback(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 bindTransformFeedback start");
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    WebGLTransformFeedback *webGlTransformFeedback = nullptr;
    napi_status transformFeedbackStatus = napi_unwrap(env, funcArg[NARG_POS::SECOND],
        (void **) &webGlTransformFeedback);
    if (transformFeedbackStatus != napi_ok) {
        return nullptr;
    }
    unsigned int transformFeedback = static_cast<unsigned int>(webGlTransformFeedback->GetTransformFeedback());

    glBindTransformFeedback(static_cast<GLenum>(target), static_cast<GLuint>(transformFeedback));
    LOGI("WebGL2 bindTransformFeedback transformFeedbackId %{public}u target %{public}u", transformFeedback, target);
    return nullptr;
}

napi_value WebGL2RenderingContextBase::BeginTransformFeedback(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 beginTransformFeedback start");
    int64_t primitiveMode;
    tie(succ, primitiveMode) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 beginTransformFeedback primitiveMode = %{public}u", primitiveMode);
    glBeginTransformFeedback(static_cast<GLenum>(primitiveMode));
    return nullptr;
}

napi_value WebGL2RenderingContextBase::EndTransformFeedback(napi_env env, napi_callback_info info)
{
    LOGI("WebGL2 endTransformFeedback start");
    glEndTransformFeedback();
    LOGI("WebGL2 cullendTransformFeedbackFace end");
    return nullptr;
}

napi_value WebGL2RenderingContextBase::TexStorage3D(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::SIX)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 texStorage3D start");
    int32_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::texStorage3D target = %{public}u", target);
    int32_t levels;
    tie(succ, levels) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::texStorage3D levels = %{public}u", levels);
    int32_t internalFormat;
    tie(succ, internalFormat) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::texStorage3D internalFormat = %{public}u", internalFormat);
    int32_t width;
    tie(succ, width) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::texStorage3D width = %{public}u", width);
    int32_t height;
    tie(succ, height) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::texStorage3D height = %{public}u", height);
    int32_t depth;
    tie(succ, depth) = NVal(env, funcArg[NARG_POS::SIXTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::texStorage3D depth = %{public}u", depth);
    glTexStorage3D(static_cast<GLenum>(target), static_cast<GLsizei>(levels),
                   static_cast<GLenum>(internalFormat),
                   static_cast<GLsizei>(width), static_cast<GLsizei>(height), static_cast<GLsizei>(depth));
    LOGI("WebGL2 texStorage3D end");
    return nullptr;
}

napi_value WebGL2RenderingContextBase::TexImage3D(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TEN, NARG_CNT::ELEVEN)) {
        return nullptr;
    }

    bool succ = false;
    WebGLImage3DArg  image3DArg = {};
    tie(succ, image3DArg.target) = NVal(env, funcArg[NARG_POS::FIRST]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    tie(succ, image3DArg.level) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    tie(succ, image3DArg.internalFormat) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    tie(succ, image3DArg.width) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    tie(succ, image3DArg.height) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    tie(succ, image3DArg.depth) = NVal(env, funcArg[NARG_POS::SIXTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    tie(succ, image3DArg.border) = NVal(env, funcArg[NARG_POS::SEVENTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    tie(succ, image3DArg.format) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToGLenum();
    if (!succ) {
        return nullptr;
    }
    tie(succ, image3DArg.type) = NVal(env, funcArg[NARG_POS::NINTH]).ToGLenum();
    if (!succ) {
        return nullptr;
    }

    if (NVal(env, funcArg[NARG_POS::TENTH]).IsDataView()) {
        size_t imageSize;
        void *pixels = nullptr;
        tie(succ, pixels, imageSize) = NVal(env, funcArg[NARG_POS::TENTH]).ToDataview();
        if (!succ) {
            return nullptr;
        }
        int64_t srcOffset = 0;
        if (funcArg[NARG_POS::ELEVENTH] != nullptr) {
            tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::ELEVENTH]).ToInt64();
            if (!succ) {
                return nullptr;
            }
        }
        image3DArg.source = reinterpret_cast<uint8_t *>((reinterpret_cast<uint8_t *>(pixels) + srcOffset));
    } else if (NVal(env, funcArg[NARG_POS::TENTH]).TypeIs(napi_number)) {
        int64_t pboOffset = 0;
        tie(succ, pboOffset) = NVal(env, funcArg[NARG_POS::TENTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGE("WebGl TexImage3D parse pboOffset %{public}x", pboOffset);
        image3DArg.source  = reinterpret_cast<uint8_t *>(pboOffset);
    } else {
        unique_ptr<WebGLImageSource> imageSource = WebGLImageSource::GetImageSource(env, funcArg[NARG_POS::TENTH]);
        if (imageSource != nullptr) {
            image3DArg.imageSource = imageSource.get();
            imageSource.reset();
        } else {
            return nullptr;
        }
    }

    if (image3DArg.imageSource != nullptr) {
        glTexImage3D(image3DArg.target, image3DArg.level, image3DArg.internalFormat,
            image3DArg.imageSource->width, image3DArg.imageSource->height,
            image3DArg.depth, image3DArg.border, image3DArg.format, image3DArg.type,
            static_cast<void*>(image3DArg.imageSource->source.get()));
    } else {
        glTexImage3D(image3DArg.target, image3DArg.level, image3DArg.internalFormat,
            image3DArg.width, image3DArg.height,
            image3DArg.depth, image3DArg.border, image3DArg.format, image3DArg.type,
            static_cast<void*>(image3DArg.source));
    }
    return nullptr;
}

napi_value WebGL2RenderingContextBase::TexSubImage3D(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ELEVEN, NARG_CNT::TWELVE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 texSubImage3D start");
    int32_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::texSubImage3D target = %{public}u", target);
    int32_t level;
    tie(succ, level) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::texSubImage3D level = %{public}u", level);
    int32_t xOffset;
    tie(succ, xOffset) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::texSubImage3D xOffset = %{public}u", xOffset);
    int32_t yOffset;
    tie(succ, yOffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::texSubImage3D yOffset = %{public}u", yOffset);
    int32_t zOffset;
    tie(succ, zOffset) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::texSubImage3D zOffset = %{public}u", zOffset);
    int32_t width;
    tie(succ, width) = NVal(env, funcArg[NARG_POS::SIXTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::texSubImage3D width = %{public}u", width);
    int32_t height;
    tie(succ, height) = NVal(env, funcArg[NARG_POS::SEVENTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::texSubImage3D height = %{public}u", height);
    int32_t depth;
    tie(succ, depth) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::texSubImage3D depth = %{public}u", depth);
    int32_t format;
    tie(succ, format) = NVal(env, funcArg[NARG_POS::NINTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::texSubImage3D format = %{public}u", format);
    int32_t type;
    tie(succ, type) = NVal(env, funcArg[NARG_POS::TENTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::texSubImage3D type = %{public}u", type);
    void *offset = nullptr;
    bool offsetsucc = NVal(env, funcArg[NARG_POS::TENTH]).TypeIs(napi_number);
    if (funcArg.InitArgs(NARG_CNT::ELEVEN)) {   // 十一个参数
        if (offsetsucc) {
            LOGI("WebGL WebGL2RenderingContextBase::texSubImage3D into eleven on GLint");
            char *offsetBase = nullptr;
            int32_t pboOffset;
            tie(succ, pboOffset) = NVal(env, funcArg[NARG_POS::ELEVENTH]).ToInt32();
            if (!succ) {
                napi_value teximagesource = funcArg[NARG_POS::ELEVENTH];
                napi_value resultobject = nullptr;
                napi_status statusobject = napi_coerce_to_object(env, teximagesource, &resultobject);
                if (statusobject != napi_ok) {
                    return nullptr;
                }
                LOGI("WebGL WebGLRenderContext::texSubImage3D resultobject = %{public}u", resultobject);
                napi_value resultwidth = nullptr;
                napi_status widthstatus = napi_get_named_property(env, resultobject, "width", &resultwidth);
                if (widthstatus != napi_ok) {
                    return nullptr;
                }
                int64_t dataWidth;
                tie(succ, dataWidth) = NVal(env, resultwidth).ToInt64();
                if (!succ) {
                    return nullptr;
                }
                LOGI("WebGL WebGLRenderContext::texSubImage3D dataWidth = %{public}u", dataWidth);
                napi_value resultheight = nullptr;
                napi_status heightstatus = napi_get_named_property(env, resultobject, "height", &resultheight);
                if (heightstatus != napi_ok) {
                    return nullptr;
                }
                int64_t dataHeight;
                tie(succ, dataHeight) = NVal(env, resultheight).ToInt64();
                if (!succ) {
                    return nullptr;
                }
                LOGI("WebGL WebGLRenderContext::texSubImage3D dataHeight = %{public}u", dataHeight);
                napi_value resultdata = nullptr;
                napi_status datares = napi_get_named_property(env, resultobject, "data", &resultdata);
                if (datares != napi_ok) {
                    return nullptr;
                }
                napi_value resultstr;
                napi_status rsuStatus = napi_coerce_to_string(env, resultdata, &resultstr);
                LOGI("WebGL WebGLRenderContext::texSubImage3D rsuStatus = %{public}u", rsuStatus);
                if (rsuStatus != napi_ok) {
                    return nullptr;
                }
                unique_ptr<char[]> name;
                tie(succ, name, ignore) = NVal(env, resultstr).ToUTF8String();
                if (!succ) {
                    return nullptr;
                }
                glTexSubImage3D(static_cast<GLenum>(target), static_cast<GLint>(level), static_cast<GLint>(xOffset),
                    static_cast<GLint>(yOffset), static_cast<GLint>(zOffset),
                    static_cast<GLsizei>((width != 0) ? width : dataWidth),
                    static_cast<GLsizei>((height != 0) ? height : dataHeight), static_cast<GLsizei>(depth),
                    static_cast<GLenum>(format),
                    static_cast<GLenum>(type), reinterpret_cast<GLvoid *>(name.get()));
                LOGI("WebGL texSubImage3D end");
                return nullptr;
            }
            LOGI("WebGL WebGLRenderContext::texSubImage3D pboOffset = %{public}u", pboOffset);
            offset = reinterpret_cast<GLvoid *>((offsetBase + pboOffset));
            LOGI("WebGL WebGLRenderContext::texSubImage3D offset = %{public}u", offset);
        } else {
            return nullptr;   // TexImageSource
        }
    } else if (funcArg.InitArgs(NARG_CNT::TWELVE)) {  // 十二个参数
        int32_t srcOffset;
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::TWELVETH]).ToInt32();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL WebGL2RenderingContextBase::texSubImage3D srcOffset = %{public}u", srcOffset);
        void *srcData = nullptr;
        size_t size;
        tie(succ, srcData, size) = NVal(env, funcArg[NARG_POS::ELEVENTH]).ToArraybuffer();
        char *srcDataBase = static_cast<char *>(srcData);
        offset = reinterpret_cast<GLvoid *>((srcDataBase + srcOffset));
    } else {
        return nullptr;
    }
    glTexSubImage3D(static_cast<GLenum>(target), static_cast<GLint>(level), static_cast<GLint>(xOffset),
                    static_cast<GLint>(yOffset), static_cast<GLint>(zOffset), static_cast<GLsizei>(width),
                    static_cast<GLsizei>(height), static_cast<GLsizei>(depth), static_cast<GLenum>(format),
                    static_cast<GLenum>(type), reinterpret_cast<GLvoid *>(offset));
    LOGI("WebGL texSubImage3D end");
    return nullptr;
}

napi_value WebGL2RenderingContextBase::CopyTexSubImage3D(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::NINE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 copyTexSubImage3D start");
    int32_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::copyTexSubImage3D target = %{public}u", target);
    int32_t level;
    tie(succ, level) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::copyTexSubImage3D level = %{public}u", level);
    int32_t xOffset;
    tie(succ, xOffset) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::copyTexSubImage3D xOffset = %{public}u", xOffset);
    int32_t yOffset;
    tie(succ, yOffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::copyTexSubImage3D yOffset = %{public}u", yOffset);
    int32_t zOffset;
    tie(succ, zOffset) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::copyTexSubImage3D zOffset = %{public}u", zOffset);
    int32_t x;
    tie(succ, x) = NVal(env, funcArg[NARG_POS::SIXTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::copyTexSubImage3D x = %{public}u", x);
    int32_t y;
    tie(succ, y) = NVal(env, funcArg[NARG_POS::SEVENTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::copyTexSubImage3D y = %{public}u", y);
    int32_t width;
    tie(succ, width) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::copyTexSubImage3D width = %{public}u", width);
    int32_t height;
    tie(succ, height) = NVal(env, funcArg[NARG_POS::NINTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::copyTexSubImage3D height = %{public}u", height);
    glCopyTexSubImage3D(static_cast<GLenum>(target), static_cast<GLint>(level), static_cast<GLint>(xOffset),
                        static_cast<GLint>(yOffset), static_cast<GLint>(zOffset), static_cast<GLint>(x),
                        static_cast<GLint>(y), static_cast<GLsizei>(width), static_cast<GLsizei>(height));
    LOGI("WebGL copyTexSubImage3D end");
    return nullptr;
}

napi_value WebGL2RenderingContextBase::CompressedTexImage3D(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::NINE, NARG_CNT::TEN)) {
        return nullptr;
    }

    bool succ = false;
    LOGI("WebGL compressedTexImage3D start");
    int32_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::compressedTexImage3D target = %{public}u", target);
    int32_t level;
    tie(succ, level) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::compressedTexImage3D level = %{public}u", level);
    int32_t internalFormat;
    tie(succ, internalFormat) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::compressedTexImage3D internalFormat = %{public}u", internalFormat);
    int32_t width;
    tie(succ, width) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::compressedTexImage3D width = %{public}u", width);
    int32_t height;
    tie(succ, height) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::compressedTexImage3D height = %{public}u", height);
    int32_t depth;
    tie(succ, depth) = NVal(env, funcArg[NARG_POS::SIXTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::compressedTexImage3D depth = %{public}u", depth);
    int32_t border;
    tie(succ, border) = NVal(env, funcArg[NARG_POS::SEVENTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::compressedTexImage3D border = %{public}u", border);
    bool imagesucc = NVal(env, funcArg[NARG_POS::EIGHTH]).TypeIs(napi_number);
    void *offset = nullptr;
    int32_t imageSize;
    if (imagesucc) {
        LOGI("WebGL WebGL2RenderingContextBase::compressedTexImage3D into nine");
        tie(succ, imageSize) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToInt32();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL WebGL2RenderingContextBase::compressedTexImage3D imageSize = %{public}u", imageSize);
        int32_t srcOffset;
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::NINTH]).ToInt32();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL WebGL2RenderingContextBase::compressedTexImage3D srcOffset = %{public}u", srcOffset);
        void *srcData = nullptr;
        char *srcDataBase = static_cast<char *>(srcData);
        offset = reinterpret_cast<GLvoid *>((srcDataBase + srcOffset));
        LOGI("WebGL WebGL2RenderingContextBase::compressedTexImage3D offset = %{public}u", offset);
    } else {
        int32_t srcOffset = 0;
        int32_t srcLengthOverride = 0;
        void *srcData = nullptr;
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::NINTH]).ToInt32();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL WebGL2RenderingContextBase::compressedTexImage3D srcOffset = %{public}u", srcOffset);
        tie(succ, srcLengthOverride) = NVal(env, funcArg[NARG_POS::TENTH]).ToInt32();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL WebGL2RenderingContextBase::compressedTexImage3D srcLengthOverride = %{public}u",
             srcLengthOverride);
        tie(succ, srcData, srcLengthOverride) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToArraybuffer();
        char *srcDataBase = static_cast<char *>(srcData);
        offset = reinterpret_cast<GLvoid *>((srcDataBase + srcOffset));
        imageSize = static_cast<int>(srcLengthOverride);
        LOGI("WebGL WebGL2RenderingContextBase::compressedTexImage3D offset = %{public}u", offset);
        LOGI("WebGL WebGL2RenderingContextBase::compressedTexImage3D imageSize = %{public}u", imageSize);
    }

    glCompressedTexImage3D(static_cast<GLenum>(target), static_cast<GLint>(level),
                           static_cast<GLenum>(internalFormat),
                           static_cast<GLsizei>(width), static_cast<GLsizei>(height),
                           static_cast<GLsizei>(depth),
                           static_cast<GLint>(border), static_cast<GLsizei>(imageSize),
                           reinterpret_cast<GLvoid *>(offset));
    LOGI("WebGL compressedTexImage3D end");
    return nullptr;
}

napi_value WebGL2RenderingContextBase::CompressedTexSubImage3D(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ELEVEN, NARG_CNT::TWELVE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL compressedTexSubImage3D start");
    int32_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::compressedTexSubImage3D target = %{public}u", target);
    int32_t level;
    tie(succ, level) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::compressedTexSubImage3D level = %{public}u", level);
    int32_t xOffset;
    tie(succ, xOffset) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::compressedTexSubImage3D xOffset = %{public}u", xOffset);
    int32_t yOffset;
    tie(succ, yOffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::compressedTexSubImage3D yOffset = %{public}u", yOffset);
    int32_t zOffset;
    tie(succ, zOffset) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::compressedTexSubImage3D zOffset = %{public}u", zOffset);
    int32_t width;
    tie(succ, width) = NVal(env, funcArg[NARG_POS::SIXTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::compressedTexSubImage3D width = %{public}u", width);
    int32_t height;
    tie(succ, height) = NVal(env, funcArg[NARG_POS::SEVENTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::compressedTexSubImage3D height = %{public}u", height);
    int32_t depth;
    tie(succ, depth) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::compressedTexSubImage3D depth = %{public}u", depth);
    int32_t format;
    tie(succ, format) = NVal(env, funcArg[NARG_POS::NINTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::compressedTexSubImage3D format = %{public}u", format);
    bool imagesucc = NVal(env, funcArg[NARG_POS::TENTH]).TypeIs(napi_number);
    void *offset = nullptr;
    int32_t imageSize;
    if (imagesucc) {
        // 是十一个参数
        tie(succ, imageSize) = NVal(env, funcArg[NARG_POS::TENTH]).ToInt32();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL WebGL2RenderingContextBase::compressedTexSubImage3D imageSize = %{public}u", imageSize);
        int32_t srcOffset;
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::ELEVENTH]).ToInt32();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL WebGL2RenderingContextBase::compressedTexSubImage3D srcOffset = %{public}u", srcOffset);
        void *srcData = nullptr;
        char *srcDataBase = static_cast<char *>(srcData);
        offset = reinterpret_cast<GLvoid *>((srcDataBase + srcOffset));
    } else {
        int32_t srcOffset = 0;
        int32_t srcLengthOverride = 0;
        void *srcData = nullptr;
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::ELEVENTH]).ToInt32();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL WebGL2RenderingContextBase::compressedTexSubImage3D srcOffset = %{public}u", srcOffset);
        tie(succ, srcLengthOverride) = NVal(env, funcArg[NARG_POS::TWELVETH]).ToInt32();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL WebGL2RenderingContextBase::compressedTexSubImage3D srcLengthOverride = %{public}u",
             srcLengthOverride);
        tie(succ, srcData, srcLengthOverride) = NVal(env, funcArg[NARG_POS::TENTH]).ToArraybuffer();
        char *srcDataBase = static_cast<char *>(srcData);
        offset = reinterpret_cast<GLvoid *>((srcDataBase + srcOffset));
        imageSize = static_cast<int>(srcLengthOverride);
        LOGI("WebGL WebGL2RenderingContextBase::compressedTexSubImage3D srcDataBase = %{public}u", srcDataBase);
        LOGI("WebGL WebGL2RenderingContextBase::compressedTexSubImage3D imageSize = %{public}u", imageSize);
    }
    glCompressedTexSubImage3D(static_cast<GLenum>(target), static_cast<GLint>(level),
                              static_cast<GLint>(xOffset),
                              static_cast<GLint>(yOffset), static_cast<GLint>(zOffset),
                              static_cast<GLsizei>(width), static_cast<GLsizei>(height),
                              static_cast<GLsizei>(depth),
                              static_cast<GLenum>(format), static_cast<GLsizei>(imageSize),
                              reinterpret_cast<GLvoid *>(offset));
    LOGI("WebGL compressedTexSubImage3D end");
    return nullptr;
}

napi_value WebGL2RenderingContextBase::GetFragDataLocation(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    LOGI("WebGL2 getFragDataLocation start");
    WebGLProgram *webGlProgram = nullptr;
    napi_status programStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlProgram);
    if (programStatus != napi_ok) {
        return nullptr;
    }
    bool succ = false;
    int program = webGlProgram->GetProgramId();
    unique_ptr<char[]> name;
    tie(succ, name, ignore) = NVal(env, funcArg[NARG_POS::SECOND]).ToUTF8String();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderingContextBase::getFragDataLocation name = %{public}s", name.get());
    int32_t res = static_cast<int32_t>(glGetFragDataLocation(static_cast<GLuint>(program),
                                                             const_cast<char *>(name.get())));
    LOGI("WebGL getFragDataLocation end");
    return NVal::CreateInt64(env, res).val_;
}

napi_value WebGL2RenderingContextBase::Uniform1ui(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 uniform1ui start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int location = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL WebGL2RenderingContextBase::uniform1ui location = %{public}u", location);
    int32_t v0;
    tie(succ, v0) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::uniform1ui v0 = %{public}u", v0);
    glUniform1ui(static_cast<GLint>(location), static_cast<GLuint>(v0));
    LOGI("WebGL uniform1ui end");
    return nullptr;
}

napi_value WebGL2RenderingContextBase::Uniform2ui(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 uniform2ui start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int location = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL WebGL2RenderingContextBase::uniform2ui location = %{public}u", location);
    int32_t v0;
    tie(succ, v0) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::uniform2ui v0 = %{public}u", v0);
    int32_t v1;
    tie(succ, v1) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::uniform2ui v1 = %{public}u", v1);
    glUniform2ui(static_cast<GLint>(location), static_cast<GLuint>(v0), static_cast<GLuint>(v1));
    LOGI("WebGL uniform2ui end");
    return nullptr;
}

napi_value WebGL2RenderingContextBase::Uniform3ui(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 uniform3ui start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int location = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL WebGL2RenderingContextBase::uniform3ui location = %{public}u", location);
    int32_t v0;
    tie(succ, v0) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::uniform3ui v0 = %{public}u", v0);
    int32_t v1;
    tie(succ, v1) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::uniform3ui v1 = %{public}u", v1);
    int32_t v2;
    tie(succ, v2) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::uniform3ui v2 = %{public}u", v2);
    glUniform3ui(static_cast<GLint>(location), static_cast<GLuint>(v0), static_cast<GLuint>(v1),
                 static_cast<GLuint>(v2));
    LOGI("WebGL uniform3ui end");
    return nullptr;
}

napi_value WebGL2RenderingContextBase::Uniform4ui(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FIVE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 uniform4ui start");
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int location = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL WebGL2RenderingContextBase::uniform4ui location = %{public}u", location);
    int32_t v0;
    tie(succ, v0) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::uniform4ui v0 = %{public}u", v0);
    int32_t v1;
    tie(succ, v1) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::uniform4ui v1 = %{public}u", v1);
    int32_t v2;
    tie(succ, v2) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::uniform4ui v2 = %{public}u", v2);
    int32_t v3;
    tie(succ, v3) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::uniform4ui v3 = %{public}u", v3);
    glUniform4ui(static_cast<GLint>(location), static_cast<GLuint>(v0), static_cast<GLuint>(v1),
                 static_cast<GLuint>(v2), static_cast<GLuint>(v3));
    LOGI("WebGL uniform4ui end");
    return nullptr;
}

napi_value WebGL2RenderingContextBase::VertexAttribI4i(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FIVE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL vertexAttribI4i start");
    int32_t index;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::vertexAttribI4i index = %{public}u", index);
    int32_t x;
    tie(succ, x) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::vertexAttribI4i x = %{public}u", x);
    int32_t y;
    tie(succ, y) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::vertexAttribI4i y = %{public}u", y);
    int32_t z;
    tie(succ, z) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::vertexAttribI4i z = %{public}u", z);
    int32_t w;
    tie(succ, w) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::vertexAttribI4i w = %{public}u", w);
    glVertexAttribI4i(static_cast<GLuint>(index), static_cast<GLint>(x), static_cast<GLint>(y),
                      static_cast<GLint>(z), static_cast<GLint>(w));
    LOGI("WebGL vertexAttribI4i end");
    return nullptr;
}

napi_value WebGL2RenderingContextBase::VertexAttribI4ui(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FIVE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL vertexAttribI4ui start");
    int32_t index;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::vertexAttribI4ui index = %{public}u", index);
    int32_t x;
    tie(succ, x) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::vertexAttribI4ui x = %{public}u", x);
    int32_t y;
    tie(succ, y) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::vertexAttribI4ui y = %{public}u", y);
    int32_t z;
    tie(succ, z) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::vertexAttribI4ui z = %{public}u", z);
    int32_t w;
    tie(succ, w) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::vertexAttribI4ui w = %{public}u", w);
    glVertexAttribI4ui(static_cast<GLuint>(index), static_cast<GLuint>(x), static_cast<GLuint>(y),
                       static_cast<GLuint>(z), static_cast<GLuint>(w));
    LOGI("WebGL vertexAttribI4ui end");
    return nullptr;
}

napi_value WebGL2RenderingContextBase::VertexAttribIPointer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FIVE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL vertexAttribIPointer start");
    int32_t index;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::vertexAttribIPointer index = %{public}u", index);
    int32_t size;
    tie(succ, size) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::vertexAttribIPointer size = %{public}u", size);
    int32_t type;
    tie(succ, type) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::vertexAttribIPointer type = %{public}u", type);
    int32_t stride;
    tie(succ, stride) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::vertexAttribIPointer stride = %{public}u", stride);
    int32_t offset;
    tie(succ, offset) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::vertexAttribIPointer offset = %{public}u", offset);
    glVertexAttribIPointer(static_cast<GLuint>(index), static_cast<GLuint>(size), static_cast<GLenum>(type),
                           static_cast<GLsizei>(stride), reinterpret_cast<GLvoid *>(static_cast<intptr_t>(offset)));
    LOGI("WebGL vertexAttribIPointer end");
    return nullptr;
}

napi_value WebGL2RenderingContextBase::VertexAttribDivisor(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 vertexAttribDivisor start");
    int32_t index;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::vertexAttribDivisor index = %{public}u", index);
    int32_t divisor;
    tie(succ, divisor) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::vertexAttribDivisor divisor = %{public}u", divisor);
    glVertexAttribDivisor(static_cast<GLuint>(index), static_cast<GLuint>(divisor));
    LOGI("WebGL vertexAttribDivisor end");
    return nullptr;
}

napi_value WebGL2RenderingContextBase::DrawArraysInstanced(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FOUR)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 drawArraysInstanced start");
    int32_t mode;
    tie(succ, mode) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::drawArraysInstanced mode = %{public}u", mode);
    int32_t first;
    tie(succ, first) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::drawArraysInstanced first = %{public}u", first);
    int32_t count;
    tie(succ, count) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::drawArraysInstanced count = %{public}u", count);
    int32_t instanceCount;
    tie(succ, instanceCount) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::drawArraysInstanced instanceCount = %{public}u", instanceCount);
    glDrawArraysInstanced(static_cast<GLenum>(mode), static_cast<GLint>(first), static_cast<GLsizei>(count),
                          static_cast<GLsizei>(instanceCount));
    LOGI("WebGL drawArraysInstanced end");
    return nullptr;
}

napi_value WebGL2RenderingContextBase::DrawElementsInstanced(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FIVE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 drawElementsInstanced start");
    int32_t mode;
    tie(succ, mode) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::drawElementsInstanced mode = %{public}u", mode);
    int32_t count;
    tie(succ, count) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::drawElementsInstanced count = %{public}u", count);
    int32_t type;
    tie(succ, type) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::drawElementsInstanced type = %{public}u", type);
    int32_t offset;
    tie(succ, offset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::drawElementsInstanced offset = %{public}u", offset);
    int32_t instanceCount;
    tie(succ, instanceCount) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::drawElementsInstanced instanceCount = %{public}u", instanceCount);
    glDrawElementsInstanced(static_cast<GLenum>(mode), static_cast<GLsizei>(count), static_cast<GLenum>(type),
                            reinterpret_cast<GLvoid *>(static_cast<intptr_t>(offset)),
                            static_cast<GLsizei>(instanceCount));
    LOGI("WebGL drawElementsInstanced end");
    return nullptr;
}

napi_value WebGL2RenderingContextBase::DrawRangeElements(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::SIX)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 drawRangeElements start");
    int32_t mode;
    tie(succ, mode) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::drawRangeElements mode = %{public}u", mode);
    int32_t start;
    tie(succ, start) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::drawRangeElements start = %{public}u", start);
    int32_t end;
    tie(succ, end) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::drawRangeElements end = %{public}u", end);
    int32_t count;
    tie(succ, count) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::drawRangeElements count = %{public}u", count);
    int32_t type;
    tie(succ, type) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::drawRangeElements type = %{public}u", type);
    int32_t offset;
    tie(succ, offset) = NVal(env, funcArg[NARG_POS::SIXTH]).ToInt32();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::drawRangeElements offset = %{public}u", offset);
    glDrawRangeElements(static_cast<GLenum>(mode), static_cast<GLuint>(start), static_cast<GLuint>(end),
                        static_cast<GLsizei>(count), static_cast<GLenum>(type),
                        reinterpret_cast<GLvoid *>(static_cast<intptr_t>(offset)));
    LOGI("WebGL drawRangeElements end");
    return nullptr;
}

napi_value WebGL2RenderingContextBase::CopyBufferSubData(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FIVE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL copyBufferSubData start");
    int64_t readtarget;
    tie(succ, readtarget) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::copyBufferSubData readtarget = %{public}u", readtarget);
    int64_t writetarget;
    tie(succ, writetarget) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::copyBufferSubData writetarget = %{public}u", writetarget);
    int64_t readoffset;
    tie(succ, readoffset) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::copyBufferSubData readoffset = %{public}u", readoffset);

    int64_t writeoffset;
    tie(succ, writeoffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::copyBufferSubData writeoffset = %{public}u", writeoffset);

    int64_t size;
    tie(succ, size) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::copyBufferSubData size = %{public}u", size);
    glCopyBufferSubData(static_cast<GLenum>(readtarget), static_cast<GLenum>(writetarget),
        static_cast<GLintptr>(readoffset), static_cast<GLintptr>(writeoffset), static_cast<GLsizeiptr>(size));
    LOGI("WebGL copyBufferSubData end");
    return nullptr;
}

napi_value WebGL2RenderingContextBase::GetBufferSubData(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    size_t argc = 5;
    napi_value args[5];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    int64_t target;
    napi_get_value_int64(env, args[0], &target);
    LOGI("WebGL WebGL2RenderingContextBase::getBufferSubData target = %{public}u", target);
    int64_t srcByteOffset;
    napi_get_value_int64(env, args[1], &srcByteOffset);
    LOGI("WebGL WebGL2RenderingContextBase::getBufferSubData srcByteOffset = %{public}u", srcByteOffset);
    LOGI("WebGL WebGL2RenderingContextBase::getBufferSubData funcArg[NARG_POS::THIRD] = %{public}u",
        funcArg[NARG_POS::THIRD]);
    size_t bufLen = 0;
    void *buf = nullptr;
    napi_value arraybuffer = nullptr;
    size_t byteoff = 0;
    int a = 2;
    napi_get_dataview_info(env, args[a], &bufLen, &buf, &arraybuffer, &byteoff);
    LOGI("WebGL WebGL2RenderingContextBase::getBufferSubData buf = %{public}u", buf);
    LOGI("WebGL WebGL2RenderingContextBase::getBufferSubData bufLen = %{public}u", bufLen);
    LOGI("WebGL WebGL2RenderingContextBase::getBufferSubData arraybuffer = %{public}u", arraybuffer);
    LOGI("WebGL WebGL2RenderingContextBase::getBufferSubData byteoff = %{public}u", byteoff);

    glBufferSubData(static_cast<GLenum>(target), static_cast<GLintptr>(srcByteOffset),
        static_cast<GLsizeiptr>(bufLen), static_cast<void*>(buf));
    LOGI("WebGL getBufferSubData end");
    return nullptr;
}

napi_value WebGL2RenderingContextBase::BlitFramebuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TEN)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL blitFramebuffer start");
    int64_t srcX0;
    tie(succ, srcX0) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::blitFramebuffer srcX0 = %{public}u", srcX0);
    int64_t srcY0;
    tie(succ, srcY0) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::blitFramebuffer srcY0 = %{public}u", srcY0);
    int64_t srcX1;
    tie(succ, srcX1) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::blitFramebuffer srcX1 = %{public}u", srcX1);
    int64_t srcY1;
    tie(succ, srcY1) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::blitFramebuffer srcY1 = %{public}u", srcY1);
    int64_t dstX0;
    tie(succ, dstX0) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::blitFramebuffer dstX0 = %{public}u", dstX0);
    int64_t dstY0;
    tie(succ, dstY0) = NVal(env, funcArg[NARG_POS::SIXTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::blitFramebuffer dstY0 = %{public}u", dstY0);
    int64_t dstX1;
    tie(succ, dstX1) = NVal(env, funcArg[NARG_POS::SEVENTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::blitFramebuffer dstX1 = %{public}u", dstX1);
    int64_t dstY1;
    tie(succ, dstY1) = NVal(env, funcArg[NARG_POS::EIGHTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::blitFramebuffer dstY1 = %{public}u", dstY1);
    int64_t mask;
    tie(succ, mask) = NVal(env, funcArg[NARG_POS::NINTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::blitFramebuffer mask = %{public}u", mask);
    int64_t filter;
    tie(succ, filter) = NVal(env, funcArg[NARG_POS::TENTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::blitFramebuffer filter = %{public}u", filter);
    glBlitFramebuffer(static_cast<GLint>(srcX0), static_cast<GLint>(srcY0), static_cast<GLint>(srcX1),
        static_cast<GLint>(srcY1), static_cast<GLint>(dstX0),
        static_cast<GLint>(dstY0), static_cast<GLint>(dstX1), static_cast<GLint>(dstY1),
        static_cast<GLbitfield>(mask), static_cast<GLenum>(filter));
    LOGI("WebGL blitFramebuffer end");
    return nullptr;
}

napi_value WebGL2RenderingContextBase::FramebufferTextureLayer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FIVE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL framebufferTextureLayer start");
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::framebufferTextureLayer target = %{public}u", target);
    int64_t attachment;
    tie(succ, attachment) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::framebufferTextureLayer attachment = %{public}u", attachment);
    if (funcArg[NARG_POS::THIRD] == nullptr) {
        return nullptr;
    }
    WebGLTexture *webGlTexture = nullptr;
    napi_status textureStatus = napi_unwrap(env, funcArg[NARG_POS::THIRD], (void **)&webGlTexture);
    if (textureStatus != napi_ok) {
        return nullptr;
    }
    unsigned int texture = webGlTexture->GetTexture();
    LOGI("WebGL WebGLRenderContext::WebGL2RenderingContextBase framebufferTextureLayer texture= %{public}u", texture);
    int64_t level;
    tie(succ, level) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::framebufferTextureLayer level = %{public}u", level);
    int64_t layer;
    tie(succ, layer) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::framebufferTextureLayer layer = %{public}u", layer);
    glFramebufferTextureLayer(static_cast<GLenum>(target), static_cast<GLenum>(attachment),
        static_cast<GLuint>(texture), static_cast<GLint>(level),
        static_cast<GLint>(layer));
    LOGI("WebGL framebufferTextureLayer end");
    return nullptr;
}

napi_value WebGL2RenderingContextBase::ReadBuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL readBuffer start");
    int64_t src;
    tie(succ, src) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::readBuffer src = %{public}u", src);
    glReadBuffer(static_cast<GLenum>(src));
    LOGI("WebGL readBuffer end");
    return nullptr;
}

napi_value WebGL2RenderingContextBase::RenderbufferStorageMultisample(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FIVE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL renderbufferStorageMultisample start");
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::renderbufferStorageMultisample target = %{public}u", target);
    int64_t samples;
    tie(succ, samples) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::renderbufferStorageMultisample samples = %{public}u", samples);
    int64_t internalformat;
    tie(succ, internalformat) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::renderbufferStorageMultisample internalformat end");
    int64_t width;
    tie(succ, width) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::renderbufferStorageMultisample level = %{public}u", width);
    int64_t height;
    tie(succ, height) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::renderbufferStorageMultisample height = %{public}u", height);
    glRenderbufferStorageMultisample(static_cast<GLenum>(target),
        static_cast<GLsizei>(samples), static_cast<GLenum>(internalformat),
        static_cast<GLsizei>(width), static_cast<GLsizei>(height));
    LOGI("WebGL renderbufferStorageMultisample end");
    return nullptr;
}

napi_value WebGL2RenderingContextBase::TexStorage2D(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FIVE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL texStorage2D start");
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::texStorage2D target = %{public}u", target);
    int64_t levels;
    tie(succ, levels) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::texStorage2D levels = %{public}u", levels);
    int64_t internalformat;
    tie(succ, internalformat) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGLRenderContext::WebGL2RenderingContextBase texStorage2D internalformat= %{public}u",
        internalformat);
    int64_t width;
    tie(succ, width) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::texStorage2D level = %{public}u", width);
    int64_t height;
    tie(succ, height) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::texStorage2D height = %{public}u", height);
    glTexStorage2D(static_cast<GLenum>(target), static_cast<GLsizei>(levels), static_cast<GLenum>(internalformat),
        static_cast<GLsizei>(width), static_cast<GLsizei>(height));
    LOGI("WebGL texStorage2D end");
    return nullptr;
}

napi_value WebGL2RenderingContextBase::GetTransformFeedbackVarying(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    napi_value res = nullptr;
    size_t argc = 2;
    napi_value args[2];
    napi_get_cb_info(env, info, &argc, args, &res, nullptr);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL GetTransformFeedbackVarying start");
    WebGLProgram *webGlProgram = nullptr;
    napi_status programStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlProgram);
    if (programStatus != napi_ok) {
        return nullptr;
    }
    int programId = webGlProgram->GetProgramId();
    LOGI("WebGL WebGL2RenderingContextBase::getTransformFeedbackVarying programId = %{public}u", programId);
    int64_t index;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }

    WebGLActiveInfo *webGLActiveInfo = nullptr;
    napi_value objActiveInfo = WebGLActiveInfo::CreateObjectInstance(env, &webGLActiveInfo).val_;
    if (!objActiveInfo) {
        return NVal::CreateNull(env).val_;
    }

    GLsizei bufSize = WEBGL_ACTIVE_INFO_NAME_MAX_LENGTH;
    int length;
    int size;
    unsigned int type;
    GLchar name[WEBGL_ACTIVE_INFO_NAME_MAX_LENGTH] = {0};
    LOGI("WebGL WebGL2RenderingContextBase::getTransformFeedbackVarying index = %{public}u", index);
    glGetTransformFeedbackVarying(static_cast<GLuint>(programId), static_cast<GLuint>(index),
        static_cast<GLsizei>(bufSize), static_cast<GLsizei*>(&length),
        static_cast<GLsizei*>(&size), static_cast<GLenum*>(&type), name);
    if (bufSize > WEBGL_ACTIVE_INFO_NAME_MAX_LENGTH) {
        LOGE("WebGL: GetTransformFeedbackVarying: [error] bufSize exceed!");
    }
    webGLActiveInfo->SetActiveName(name);
    webGLActiveInfo->SetActiveSize(size);
    webGLActiveInfo->SetActiveType(type);
    LOGI("WebGL getTransformFeedbackVarying end");
    return res;
}

napi_value WebGL2RenderingContextBase::PauseTransformFeedback(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    LOGI("WebGL pauseTransformFeedback start");
    glPauseTransformFeedback();
    LOGI("WebGL pauseTransformFeedback end");
    return nullptr;
}

napi_value WebGL2RenderingContextBase::ResumeTransformFeedback(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    LOGI("WebGL resumeTransformFeedback start");
    glResumeTransformFeedback();
    LOGI("WebGL resumeTransformFeedback end");
    return nullptr;
}

napi_value WebGL2RenderingContextBase::BindBufferBase(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL bindBufferBase start");
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::bindBufferBase target = %{public}u", target);
    int64_t index;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::bindBufferBase index = %{public}u", index);
    if (funcArg[NARG_POS::THIRD] == nullptr) {
        return nullptr;
    }
    WebGLBuffer *webGlBuffer = nullptr;
    napi_status bufferStatus = napi_unwrap(env, funcArg[NARG_POS::THIRD], (void **)&webGlBuffer);
    if (bufferStatus != napi_ok) {
        return nullptr;
    }
    unsigned int buffer = webGlBuffer->GetBuffer();
    LOGI("WebGL WebGLRenderContext::WebGL2RenderingContextBase bindBufferBase buffer= %{public}u", buffer);
    glBindBufferBase(static_cast<GLenum>(target), static_cast<GLuint>(index), static_cast<GLuint>(buffer));
    LOGI("WebGL bindBufferBase end");
    return nullptr;
}

napi_value WebGL2RenderingContextBase::BindBufferRange(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::FIVE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL bindBufferRange start");
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::bindBufferRange target = %{public}u", target);
    int64_t index;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::bindBufferRange index = %{public}u", index);
    if (funcArg[NARG_POS::THIRD] == nullptr) {
        return nullptr;
    }
    WebGLBuffer *webGlBuffer = nullptr;
    napi_status bufferStatus = napi_unwrap(env, funcArg[NARG_POS::THIRD], (void **)&webGlBuffer);
    if (bufferStatus != napi_ok) {
        return nullptr;
    }
    unsigned int buffer = webGlBuffer->GetBuffer();
    LOGI("WebGL WebGLRenderContext::WebGL2RenderingContextBase bindBufferRange buffer= %{public}u", buffer);
    int64_t offset;
    tie(succ, offset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::bindBufferRange offset = %{public}u", offset);
    int64_t size;
    tie(succ, size) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::bindBufferRange size = %{public}u", size);
    glBindBufferRange(static_cast<GLenum>(target), static_cast<GLuint>(index), static_cast<GLuint>(buffer),
        static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(size));
    LOGI("WebGL bindBufferRange end");
    return nullptr;
}

napi_value WebGL2RenderingContextBase::GetIndexedParameter(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL getIndexedParameter start");
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::getIndexedParameter target = %{public}u", target);
    int64_t index;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL WebGL2RenderingContextBase::getIndexedParameter index = %{public}u", index);
    if (target == GL_TRANSFORM_FEEDBACK_BUFFER_BINDING || target == GL_UNIFORM_BUFFER_BINDING) {
        return WebGLBuffer::GetWebGLBufferObj(env, funcArg.GetThisVar(), index);
    } else if (target == GL_TRANSFORM_FEEDBACK_BUFFER_SIZE || target == GL_UNIFORM_BUFFER_SIZE) {
        int64_t data;
        glGetInteger64i_v(target, index, &data);
        LOGI("WebGL getIndexedParameter end");
        return NVal::CreateInt64(env, data).val_;
    } else if (target == GL_UNIFORM_BUFFER_SIZE || target == GL_UNIFORM_BUFFER_START) {
        int64_t data;
        glGetInteger64i_v(target, index, &data);
        LOGI("WebGL getIndexedParameter end");
        return NVal::CreateInt64(env, data).val_;
    } else {
        LOGI("WebGL getIndexedParameter end");
        return nullptr;
    }
}

napi_value WebGL2RenderingContextBase::GetUniformBlockIndex(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 getUniformBlockIndex start");
    WebGLProgram *webGlProgram = nullptr;
    napi_status programStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGlProgram);
    if (programStatus != napi_ok) {
        return nullptr;
    }
    int programId = webGlProgram->GetProgramId();
    LOGI("WebGL2 getUniformBlockIndex programId = %{public}u", programId);
    unique_ptr<char[]> name;
    tie(succ, name, ignore) = NVal(env, funcArg[NARG_POS::SECOND]).ToUTF8String();
    if (!succ) {
        return nullptr;
    }
    const GLchar *uniformBlockName = name.get();
    LOGI("WebGL2 getUniformBlockIndex uniformBlockName = %{public}s", name.get());
    GLuint returnValue = glGetUniformBlockIndex(static_cast<GLuint>(programId), uniformBlockName);
    int64_t res = static_cast<int64_t>(returnValue);
    LOGI("WebGL2 getUniformBlockIndex end");
    return NVal::CreateInt64(env, res).val_;
}

napi_value WebGL2RenderingContextBase::UniformBlockBinding(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 uniformBlockBinding start");
    WebGLProgram *webGlProgram = nullptr;
    napi_status programStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGlProgram);
    if (programStatus != napi_ok) {
        return nullptr;
    }
    int programId = webGlProgram->GetProgramId();
    LOGI("WebGL2 uniformBlockBinding programId = %{public}u", programId);
    int64_t uniformBlockIndex;
    tie(succ, uniformBlockIndex) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 uniformBlockBinding uniformBlockIndex = %{public}u", uniformBlockIndex);
    int64_t uniformBlockBinding;
    tie(succ, uniformBlockBinding) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 uniformBlockBinding uniformBlockBinding = %{public}u", uniformBlockBinding);
    glUniformBlockBinding(static_cast<GLuint>(programId), static_cast<GLuint>(uniformBlockIndex),
                          static_cast<GLuint>(uniformBlockBinding));
    LOGI("WebGL2 uniformBlockBinding end");
    return nullptr;
}

napi_value WebGL2RenderingContextBase::CreateVertexArray(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        return NVal::CreateNull(env).val_;
    }

    WebGLObjectManager *maps = GetWebGLVertexArrayObjectMaps(env, funcArg.GetThisVar());
    if (maps == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    WebGLVertexArrayObject *webGLVertexArrayObject = nullptr;
    napi_value objVertexArrayObject = WebGLVertexArrayObject::CreateObjectInstance(env, &webGLVertexArrayObject).val_;
    if (!objVertexArrayObject) {
        return NVal::CreateNull(env).val_;
    }
    unsigned int vertexArraysId;
    glGenVertexArrays(1, &vertexArraysId);
    webGLVertexArrayObject->SetVertexArrays(vertexArraysId);
    LOGI("WebGL2 createVertexArray vertexArraysId = %{public}u", vertexArraysId);
    maps->AddObject(vertexArraysId, objVertexArrayObject);
    return objVertexArrayObject;
}

napi_value WebGL2RenderingContextBase::DeleteVertexArray(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    WebGLVertexArrayObject *webGLVertexArrayObject = nullptr;
    napi_status vertexArraysStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGLVertexArrayObject);
    if (vertexArraysStatus != napi_ok) {
        return nullptr;
    }
    unsigned int vertexArrays = webGLVertexArrayObject->GetVertexArrays();
    glDeleteVertexArrays(1, &vertexArrays);
    LOGI("WebGL2 createVertexArray deleteVertexArrays = %{public}u", vertexArrays);
    WebGLObjectManager *maps = GetWebGLVertexArrayObjectMaps(env, funcArg.GetThisVar());
    if (maps != nullptr) {
        maps->DeleteObject(vertexArrays);
    }
    return nullptr;
}

napi_value WebGL2RenderingContextBase::IsVertexArray(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    bool res = false;
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return NVal::CreateBool(env, res).val_;
    }
    LOGI("WebGL isVertexArray start");
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return NVal::CreateBool(env, res).val_;
    }
    WebGLVertexArrayObject *webGlVertexArrayObject = nullptr;
    napi_status vertexArraysStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGlVertexArrayObject);
    if (vertexArraysStatus != napi_ok) {
        return NVal::CreateBool(env, res).val_;
    }
    unsigned int vertexArrays = webGlVertexArrayObject->GetVertexArrays();
    GLboolean returnValue = glIsVertexArray(static_cast<GLuint>(vertexArrays));
    res = static_cast<bool>(returnValue);
    LOGI("WebGL2 createVertexArray deleteVertexArrays = %{public}u %{public}u", vertexArrays, res);
    return NVal::CreateBool(env, res).val_;
}

napi_value WebGL2RenderingContextBase::BindVertexArray(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    WebGLVertexArrayObject *webGlVertexArrayObject = nullptr;
    napi_status vertexArraysStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGlVertexArrayObject);
    if (vertexArraysStatus != napi_ok) {
        return nullptr;
    }
    unsigned int vertexArrays = webGlVertexArrayObject->GetVertexArrays();
    glBindVertexArray(static_cast<GLuint>(vertexArrays));
    LOGI("WebGL2 createVertexArray bindVertexArray = %{public}u ", vertexArrays);
    return nullptr;
}

napi_value WebGL2RenderingContextBase::Uniform1uiv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::THREE, NARG_CNT::FOUR)) {
        return nullptr;
    }

    LOGI("WebGL2 uniform1uiv start");
    bool succ = false;
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int location = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL2 WebGLRenderContext::uniform1uiv location = %{public}u", location);
    if (funcArg[NARG_POS::THIRD] == nullptr) {
        return nullptr;
    }
    int64_t srcOffset = 0;
    if (funcArg[NARG_POS::THIRD] != nullptr) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 uniform1uiv srcOffset = %{public}u", srcOffset);
    }
    int64_t srcLength = 0;
    if (funcArg[NARG_POS::FOURTH] != nullptr) {
        tie(succ, srcLength) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 uniform1uiv srcLength = %{public}u", srcLength);
    }
    napi_value array = funcArg[NARG_POS::SECOND];
    bool isArray = false;
    tie(succ, isArray) = NVal(env, array).IsArray();
    if (isArray) {
        LOGI("WebGL2 uniform1uiv is Array");
        uint32_t length;
        napi_status lengthStatus = napi_get_array_length(env, array, &length);
        if (lengthStatus != napi_ok) {
            return nullptr;
        }
        float uniform1uiv[length];
        uint32_t i;
        for (i = 0; i < length; i++) {
            napi_value element;
            napi_status eleStatus = napi_get_element(env, array, i, &element);
            if (eleStatus != napi_ok) {
                return nullptr;
            }
            double ele;
            napi_status doubleStatus = napi_get_value_double(env, element, &ele);
            if (doubleStatus != napi_ok) {
                return nullptr;
            }
            uniform1uiv[i] = static_cast<float>(ele);
        }
        glUniform1uiv(static_cast<GLint>(location), static_cast<GLsizei>(srcLength),
            reinterpret_cast<GLuint *>(uniform1uiv + srcOffset));
        LOGI("WebGL2 uniform1uiv array end");
        return nullptr;
    }
    bool isTypedarray = false;
    tie(succ, isTypedarray) = NVal(env, array).IsTypeArray();
    if (!isTypedarray || !succ) {
        return nullptr;
    }
    void *data = nullptr;
    size_t length;
    napi_typedarray_type type;
    tie(succ, type, data, length) = NVal(env, array).ToTypedArray();
    if (!succ) {
        return nullptr;
    }
    if (type == napi_float32_array) {
        float inputFloat[INPUTFLOAT_LENGTH] = {0};
        errno_t ret = memcpy_s(inputFloat, sizeof(inputFloat), reinterpret_cast<uint8_t*>(data) + srcOffset, length);
        if (ret != EOK) {
            LOGE("WebGL2 uniform1uiv memcpy_s failed");
            return nullptr;
        }
        glUniform1uiv(static_cast<GLint>(location), static_cast<GLsizei>(srcLength),
            reinterpret_cast<GLuint *>(inputFloat));
    }
    LOGI("WebGL2 uniform1uiv typeArray end");
    return nullptr;
}

napi_value WebGL2RenderingContextBase::Uniform2uiv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::THREE, NARG_CNT::FOUR)) {
        return nullptr;
    }

    LOGI("WebGL2 uniform2uiv start");
    bool succ = false;
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int location = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL2 WebGLRenderContext::uniform2uiv location = %{public}u", location);
    if (funcArg[NARG_POS::THIRD] == nullptr) {
        return nullptr;
    }
    int64_t srcOffset = 0;
    if (funcArg[NARG_POS::FOURTH] != nullptr) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 uniform2uiv srcOffset = %{public}u", srcOffset);
    }
    int64_t srcLength = 0;
    if (funcArg[NARG_POS::FOURTH] != nullptr) {
        tie(succ, srcLength) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 uniform2uiv srcLength = %{public}u", srcLength);
    }
    napi_value array = funcArg[NARG_POS::SECOND];
    bool isArray = false;
    tie(succ, isArray) = NVal(env, array).IsArray();
    if (isArray) {
        LOGI("WebGL2 uniform2uiv is Array");
        uint32_t length;
        napi_status lengthStatus = napi_get_array_length(env, array, &length);
        if (lengthStatus != napi_ok) {
            return nullptr;
        }
        float uniform2uiv[length];
        uint32_t i;
        for (i = 0; i < length; i++) {
            napi_value element;
            napi_status eleStatus = napi_get_element(env, array, i, &element);
            if (eleStatus != napi_ok) {
                return nullptr;
            }
            double ele;
            napi_status doubleStatus = napi_get_value_double(env, element, &ele);
            if (doubleStatus != napi_ok) {
                return nullptr;
            }
            uniform2uiv[i] = static_cast<float>(ele);
        }
        glUniform2uiv(static_cast<GLint>(location), static_cast<GLsizei>(srcLength),
            reinterpret_cast<GLuint *>(uniform2uiv + srcOffset));
        LOGI("WebGL2 uniform2uiv array end");
        return nullptr;
    }
    bool isTypedarray = false;
    tie(succ, isTypedarray) = NVal(env, array).IsTypeArray();
    if (!isTypedarray || !succ) {
        return nullptr;
    }
    void *data = nullptr;
    size_t length;
    napi_typedarray_type type;
    tie(succ, type, data, length) = NVal(env, array).ToTypedArray();
    if (!succ) {
        return nullptr;
    }
    if (type == napi_float32_array) {
        float inputFloat[INPUTFLOAT_LENGTH] = {0};
        errno_t ret = memcpy_s(inputFloat, sizeof(inputFloat), reinterpret_cast<uint8_t*>(data) + srcOffset, length);
        if (ret != EOK) {
            LOGE("WebGL2 uniform2uiv memcpy_s failed");
            return nullptr;
        }
        glUniform2uiv(static_cast<GLint>(location), static_cast<GLsizei>(srcLength),
            reinterpret_cast<GLuint *>(inputFloat));
    }
    LOGI("WebGL2 uniform2uiv typeArray end");
    return nullptr;
}

napi_value WebGL2RenderingContextBase::Uniform3uiv(napi_env env, napi_callback_info info)
{
        NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::THREE, NARG_CNT::FOUR)) {
        return nullptr;
    }

    LOGI("WebGL2 uniform3uiv start");
    bool succ = false;
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int location = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL2 WebGLRenderContext::uniform3uiv location = %{public}u", location);
    if (funcArg[NARG_POS::THIRD] == nullptr) {
        return nullptr;
    }
    int64_t srcOffset = 0;
    if (funcArg[NARG_POS::FOURTH] != nullptr) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 uniform3uiv srcOffset = %{public}u", srcOffset);
    }
    int64_t srcLength = 0;
    if (funcArg[NARG_POS::FOURTH] != nullptr) {
        tie(succ, srcLength) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 uniform3uiv srcLength = %{public}u", srcLength);
    }
    napi_value array = funcArg[NARG_POS::SECOND];
    bool isArray = false;
    tie(succ, isArray) = NVal(env, array).IsArray();
    if (isArray) {
        LOGI("WebGL2 uniform3uiv is Array");
        uint32_t length;
        napi_status lengthStatus = napi_get_array_length(env, array, &length);
        if (lengthStatus != napi_ok) {
            return nullptr;
        }
        float uniform3uiv[length];
        uint32_t i;
        for (i = 0; i < length; i++) {
            napi_value element;
            napi_status eleStatus = napi_get_element(env, array, i, &element);
            if (eleStatus != napi_ok) {
                return nullptr;
            }
            double ele;
            napi_status doubleStatus = napi_get_value_double(env, element, &ele);
            if (doubleStatus != napi_ok) {
                return nullptr;
            }
            uniform3uiv[i] = static_cast<float>(ele);
        }
        glUniform3uiv(static_cast<GLint>(location), static_cast<GLsizei>(srcLength),
            reinterpret_cast<GLuint *>(uniform3uiv + srcOffset));
        LOGI("WebGL2 uniform3uiv array end");
        return nullptr;
    }
    bool isTypedarray = false;
    tie(succ, isTypedarray) = NVal(env, array).IsTypeArray();
    if (!isTypedarray || !succ) {
        return nullptr;
    }
    void *data = nullptr;
    size_t length;
    napi_typedarray_type type;
    tie(succ, type, data, length) = NVal(env, array).ToTypedArray();
    if (!succ) {
        return nullptr;
    }
    if (type == napi_float32_array) {
        float inputFloat[INPUTFLOAT_LENGTH] = {0};
        errno_t ret = memcpy_s(inputFloat, sizeof(inputFloat), reinterpret_cast<uint8_t*>(data) + srcOffset, length);
        if (ret != EOK) {
            LOGE("WebGL2 uniform3uiv memcpy_s failed");
            return nullptr;
        }
        glUniform3uiv(static_cast<GLint>(location), static_cast<GLsizei>(srcLength),
            reinterpret_cast<GLuint *>(inputFloat));
    }
    LOGI("WebGL2 uniform3uiv typeArray end");
    return nullptr;
}

napi_value WebGL2RenderingContextBase::Uniform4uiv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::THREE, NARG_CNT::FOUR)) {
        return nullptr;
    }

    LOGI("WebGL2 uniform4uiv start");
    bool succ = false;
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int location = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL2 WebGLRenderContext::uniform4uiv location = %{public}u", location);
    if (funcArg[NARG_POS::THIRD] == nullptr) {
        return nullptr;
    }
    int64_t srcOffset = 0;
    if (funcArg[NARG_POS::FOURTH] != nullptr) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 uniform4uiv srcOffset = %{public}u", srcOffset);
    }
    int64_t srcLength = 0;
    if (funcArg[NARG_POS::FOURTH] != nullptr) {
        tie(succ, srcLength) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 uniform4uiv srcLength = %{public}u", srcLength);
    }
    napi_value array = funcArg[NARG_POS::SECOND];
    bool isArray = false;
    tie(succ, isArray) = NVal(env, array).IsArray();
    if (isArray) {
        LOGI("WebGL2 uniform4uiv is Array");
        uint32_t length;
        napi_status lengthStatus = napi_get_array_length(env, array, &length);
        if (lengthStatus != napi_ok) {
            return nullptr;
        }
        float uniform4uiv[length];
        uint32_t i;
        for (i = 0; i < length; i++) {
            napi_value element;
            napi_status eleStatus = napi_get_element(env, array, i, &element);
            if (eleStatus != napi_ok) {
                return nullptr;
            }
            double ele;
            napi_status doubleStatus = napi_get_value_double(env, element, &ele);
            if (doubleStatus != napi_ok) {
                return nullptr;
            }
            uniform4uiv[i] = static_cast<float>(ele);
        }
        glUniform4uiv(static_cast<GLint>(location), static_cast<GLsizei>(srcLength),
            reinterpret_cast<GLuint *>(uniform4uiv + srcOffset));
        LOGI("WebGL2 uniform4uiv array end");
        return nullptr;
    }
    bool isTypedarray = false;
    tie(succ, isTypedarray) = NVal(env, array).IsTypeArray();
    if (!isTypedarray || !succ) {
        return nullptr;
    }
    void *data = nullptr;
    size_t length;
    napi_typedarray_type type;
    tie(succ, type, data, length) = NVal(env, array).ToTypedArray();
    if (!succ) {
        return nullptr;
    }
    if (type == napi_float32_array) {
        float inputFloat[INPUTFLOAT_LENGTH] = {0};
        errno_t ret = memcpy_s(inputFloat, sizeof(inputFloat), reinterpret_cast<uint8_t*>(data) + srcOffset, length);
        if (ret != EOK) {
            LOGE("WebGL2 uniform4uiv memcpy_s failed");
            return nullptr;
        }
        glUniform4uiv(static_cast<GLint>(location), static_cast<GLsizei>(srcLength),
            reinterpret_cast<GLuint *>(inputFloat));
    }
    LOGI("WebGL2 uniform4uiv typeArray end");
    return nullptr;
}

napi_value WebGL2RenderingContextBase::UniformMatrix3x2fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::THREE, NARG_CNT::FIVE)) {
        return nullptr;
    }

    LOGI("WebGL2 uniformMatrix3x2fv start");
    bool succ = false;
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int location = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL2 WebGLRenderContext::uniformMatrix3x2fv location = %{public}u", location);
    if (funcArg[NARG_POS::SECOND] == nullptr) {
        return nullptr;
    }
    bool transpose = false;
    tie(succ, transpose) = NVal(env, funcArg[NARG_POS::SECOND]).ToBool();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 uniformMatrix3x2fv transpose = %{public}u", transpose);

    int64_t srcOffset = 0;
    if (funcArg[NARG_POS::FOURTH] != nullptr) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 uniformMatrix3x2fv srcOffset = %{public}u", srcOffset);
    }
    int64_t srcLength = 0;
    if (funcArg[NARG_POS::FIFTH] != nullptr) {
        tie(succ, srcLength) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 uniformMatrix3x2fv srcLength = %{public}u", srcLength);
    }
    napi_value array = funcArg[NARG_POS::THIRD];
    bool isArray = false;
    tie(succ, isArray) = NVal(env, array).IsArray();
    if (isArray) {
        LOGI("WebGL2 uniformMatrix3x2fv is Array");
        uint32_t length;
        napi_status lengthStatus = napi_get_array_length(env, array, &length);
        if (lengthStatus != napi_ok) {
            return nullptr;
        }
        float uniformMatrix3x2fv[length];
        uint32_t i;
        for (i = 0; i < length; i++) {
            napi_value element;
            napi_status eleStatus = napi_get_element(env, array, i, &element);
            if (eleStatus != napi_ok) {
                return nullptr;
            }
            double ele;
            napi_status doubleStatus = napi_get_value_double(env, element, &ele);
            if (doubleStatus != napi_ok) {
                return nullptr;
            }
            uniformMatrix3x2fv[i] = static_cast<float>(ele);
        }
        glUniformMatrix3x2fv(static_cast<GLuint>(location), static_cast<GLsizei>(srcLength),
            static_cast<GLboolean>(transpose), reinterpret_cast<GLfloat*>(uniformMatrix3x2fv + srcOffset));
        LOGI("WebGL2 uniformMatrix3x2fv array end");
        return nullptr;
    }
    bool isTypedarray = false;
    tie(succ, isTypedarray) = NVal(env, array).IsTypeArray();
    if (!isTypedarray || !succ) {
        return nullptr;
    }
    void *data = nullptr;
    size_t length;
    napi_typedarray_type type;
    tie(succ, type, data, length) = NVal(env, array).ToTypedArray();
    if (!succ) {
        return nullptr;
    }
    if (type == napi_float32_array) {
        float inputFloat[INPUTFLOAT_LENGTH] = {0};
        errno_t ret = memcpy_s(inputFloat, sizeof(inputFloat), reinterpret_cast<uint8_t*>(data) + srcOffset, length);
        if (ret != EOK) {
            LOGE("WebGL2 uniformMatrix3x2fv memcpy_s failed");
            return nullptr;
        }
        glUniformMatrix3x2fv(static_cast<GLuint>(location), static_cast<GLsizei>(srcLength),
            static_cast<GLboolean>(transpose), reinterpret_cast<GLfloat*>(inputFloat));
    }
    LOGI("WebGL2 uniformMatrix3x2fv typeArray end");
    return nullptr;
}

napi_value WebGL2RenderingContextBase::UniformMatrix4x2fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::THREE, NARG_CNT::FIVE)) {
        return nullptr;
    }

    LOGI("WebGL2 uniformMatrix4x2fv start");
    bool succ = false;
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int location = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL2 WebGLRenderContext::uniformMatrix4x2fv location = %{public}u", location);
    if (funcArg[NARG_POS::SECOND] == nullptr) {
        return nullptr;
    }
    bool transpose = false;
    tie(succ, transpose) = NVal(env, funcArg[NARG_POS::SECOND]).ToBool();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 uniformMatrix4x2fv transpose = %{public}u", transpose);

    int64_t srcOffset = 0;
    if (funcArg[NARG_POS::FOURTH] != nullptr) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 uniformMatrix4x2fv srcOffset = %{public}u", srcOffset);
    }
    int64_t srcLength = 0;
    if (funcArg[NARG_POS::FIFTH] != nullptr) {
        tie(succ, srcLength) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 uniformMatrix4x2fv srcLength = %{public}u", srcLength);
    }
    napi_value array = funcArg[NARG_POS::THIRD];
    bool isArray = false;
    tie(succ, isArray) = NVal(env, array).IsArray();
    if (isArray) {
        LOGI("WebGL2 uniformMatrix4x2fv is Array");
        uint32_t length;
        napi_status lengthStatus = napi_get_array_length(env, array, &length);
        if (lengthStatus != napi_ok) {
            return nullptr;
        }
        float uniformMatrix4x2fv[length];
        uint32_t i;
        for (i = 0; i < length; i++) {
            napi_value element;
            napi_status eleStatus = napi_get_element(env, array, i, &element);
            if (eleStatus != napi_ok) {
                return nullptr;
            }
            double ele;
            napi_status doubleStatus = napi_get_value_double(env, element, &ele);
            if (doubleStatus != napi_ok) {
                return nullptr;
            }
            uniformMatrix4x2fv[i] = static_cast<float>(ele);
        }
        glUniformMatrix4x2fv(static_cast<GLuint>(location), static_cast<GLsizei>(srcLength),
            static_cast<GLboolean>(transpose), reinterpret_cast<GLfloat*>(uniformMatrix4x2fv + srcOffset));
        LOGI("WebGL2 uniformMatrix4x2fv array end");
        return nullptr;
    }
    bool isTypedarray = false;
    tie(succ, isTypedarray) = NVal(env, array).IsTypeArray();
    if (!isTypedarray || !succ) {
        return nullptr;
    }
    void *data = nullptr;
    size_t length;
    napi_typedarray_type type;
    tie(succ, type, data, length) = NVal(env, array).ToTypedArray();
    if (!succ) {
        return nullptr;
    }
    if (type == napi_float32_array) {
        float inputFloat[INPUTFLOAT_LENGTH] = {0};
        errno_t ret = memcpy_s(inputFloat, sizeof(inputFloat), reinterpret_cast<uint8_t*>(data) + srcOffset, length);
        if (ret != EOK) {
            LOGE("WebGL2 uniformMatrix4x2fv memcpy_s failed");
            return nullptr;
        }
        glUniformMatrix4x2fv(static_cast<GLuint>(location), static_cast<GLsizei>(srcLength),
            static_cast<GLboolean>(transpose), reinterpret_cast<GLfloat*>(inputFloat));
    }
    LOGI("WebGL2 uniformMatrix4x2fv typeArray end");
    return nullptr;
}

napi_value WebGL2RenderingContextBase::UniformMatrix2x3fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::THREE, NARG_CNT::FIVE)) {
        return nullptr;
    }

    LOGI("WebGL2 uniformMatrix2x3fv start");
    bool succ = false;
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int location = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL2 WebGLRenderContext::uniformMatrix2x3fv location = %{public}u", location);
    if (funcArg[NARG_POS::SECOND] == nullptr) {
        return nullptr;
    }
    bool transpose = false;
    tie(succ, transpose) = NVal(env, funcArg[NARG_POS::SECOND]).ToBool();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 uniformMatrix2x3fv transpose = %{public}u", transpose);

    int64_t srcOffset = 0;
    if (funcArg[NARG_POS::FOURTH] != nullptr) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 uniformMatrix2x3fv srcOffset = %{public}u", srcOffset);
    }
    int64_t srcLength = 0;
    if (funcArg[NARG_POS::FIFTH] != nullptr) {
        tie(succ, srcLength) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 uniformMatrix2x3fv srcLength = %{public}u", srcLength);
    }
    napi_value array = funcArg[NARG_POS::THIRD];
    bool isArray = false;
    tie(succ, isArray) = NVal(env, array).IsArray();
    if (isArray) {
        LOGI("WebGL2 uniformMatrix2x3fv is Array");
        uint32_t length;
        napi_status lengthStatus = napi_get_array_length(env, array, &length);
        if (lengthStatus != napi_ok) {
            return nullptr;
        }
        float uniformMatrix2x3fv[length];
        uint32_t i;
        for (i = 0; i < length; i++) {
            napi_value element;
            napi_status eleStatus = napi_get_element(env, array, i, &element);
            if (eleStatus != napi_ok) {
                return nullptr;
            }
            double ele;
            napi_status doubleStatus = napi_get_value_double(env, element, &ele);
            if (doubleStatus != napi_ok) {
                return nullptr;
            }
            uniformMatrix2x3fv[i] = static_cast<float>(ele);
        }
        glUniformMatrix2x3fv(static_cast<GLuint>(location), static_cast<GLsizei>(srcLength),
            static_cast<GLboolean>(transpose), reinterpret_cast<GLfloat*>(uniformMatrix2x3fv + srcOffset));
        LOGI("WebGL2 uniformMatrix2x3fv array end");
        return nullptr;
    }
    bool isTypedarray = false;
    tie(succ, isTypedarray) = NVal(env, array).IsTypeArray();
    if (!isTypedarray || !succ) {
        return nullptr;
    }
    void *data = nullptr;
    size_t length;
    napi_typedarray_type type;
    tie(succ, type, data, length) = NVal(env, array).ToTypedArray();
    if (!succ) {
        return nullptr;
    }
    if (type == napi_float32_array) {
        float inputFloat[INPUTFLOAT_LENGTH] = {0};
        errno_t ret = memcpy_s(inputFloat, sizeof(inputFloat), reinterpret_cast<uint8_t*>(data) + srcOffset, length);
        if (ret != EOK) {
            LOGE("WebGL2 uniformMatrix2x3fv memcpy_s failed");
            return nullptr;
        }
        glUniformMatrix2x3fv(static_cast<GLuint>(location), static_cast<GLsizei>(srcLength),
            static_cast<GLboolean>(transpose), reinterpret_cast<GLfloat*>(inputFloat));
    }
    LOGI("WebGL2 uniformMatrix2x3fv typeArray end");
    return nullptr;
}

napi_value WebGL2RenderingContextBase::UniformMatrix4x3fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FIVE)) {
        return nullptr;
    }
    LOGI("WebGL2 uniformMatrix4x3fv start");
    bool succ = false;
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int location = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL2 WebGLRenderContext::uniformMatrix4x3fv location = %{public}u", location);
    if (funcArg[NARG_POS::SECOND] == nullptr) {
        return nullptr;
    }
    bool transpose = false;
    tie(succ, transpose) = NVal(env, funcArg[NARG_POS::SECOND]).ToBool();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 uniformMatrix4x3fv transpose = %{public}u", transpose);

    int64_t srcOffset = 0;
    if (funcArg[NARG_POS::FOURTH] != nullptr) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 uniformMatrix4x3fv srcOffset = %{public}u", srcOffset);
    }
    int64_t srcLength = 0;
    if (funcArg[NARG_POS::FIFTH] != nullptr) {
        tie(succ, srcLength) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 uniformMatrix4x3fv srcLength = %{public}u", srcLength);
    }
    napi_value array = funcArg[NARG_POS::THIRD];
    bool isArray = false;
    tie(succ, isArray) = NVal(env, array).IsArray();
    if (isArray) {
        LOGI("WebGL2 uniformMatrix4x3fv is Array");
        uint32_t length;
        napi_status lengthStatus = napi_get_array_length(env, array, &length);
        if (lengthStatus != napi_ok) {
            return nullptr;
        }
        float uniformMatrix4x3fv[length];
        uint32_t i;
        for (i = 0; i < length; i++) {
            napi_value element;
            napi_status eleStatus = napi_get_element(env, array, i, &element);
            if (eleStatus != napi_ok) {
                return nullptr;
            }
            double ele;
            napi_status doubleStatus = napi_get_value_double(env, element, &ele);
            if (doubleStatus != napi_ok) {
                return nullptr;
            }
            uniformMatrix4x3fv[i] = static_cast<float>(ele);
        }
        glUniformMatrix4x3fv(static_cast<GLuint>(location), static_cast<GLsizei>(srcLength),
            static_cast<GLboolean>(transpose), reinterpret_cast<GLfloat*>(uniformMatrix4x3fv + srcOffset));
        LOGI("WebGL2 uniformMatrix4x3fv array end");
        return nullptr;
    }
    bool isTypedarray = false;
    tie(succ, isTypedarray) = NVal(env, array).IsTypeArray();
    if (!isTypedarray || !succ) {
        return nullptr;
    }
    void *data = nullptr;
    size_t length;
    napi_typedarray_type type;
    tie(succ, type, data, length) = NVal(env, array).ToTypedArray();
    if (!succ) {
        return nullptr;
    }
    if (type == napi_float32_array) {
        float inputFloat[INPUTFLOAT_LENGTH] = {0};
        errno_t ret = memcpy_s(inputFloat, sizeof(inputFloat), reinterpret_cast<uint8_t*>(data) + srcOffset, length);
        if (ret != EOK) {
            LOGE("WebGL2 uniformMatrix4x3fv memcpy_s failed");
            return nullptr;
        }
        glUniformMatrix4x3fv(static_cast<GLuint>(location), static_cast<GLsizei>(srcLength),
            static_cast<GLboolean>(transpose), reinterpret_cast<GLfloat*>(inputFloat));
    }
    LOGI("WebGL2 uniformMatrix4x3fv typeArray end");
    return nullptr;
}

napi_value WebGL2RenderingContextBase::UniformMatrix2x4fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FIVE)) {
        return nullptr;
    }

    LOGI("WebGL2 uniformMatrix2x4fv start");
    bool succ = false;
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int location = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL2 WebGLRenderContext::uniformMatrix2x4fv location = %{public}u", location);
    if (funcArg[NARG_POS::SECOND] == nullptr) {
        return nullptr;
    }
    bool transpose = false;
    tie(succ, transpose) = NVal(env, funcArg[NARG_POS::SECOND]).ToBool();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 uniformMatrix2x4fv transpose = %{public}u", transpose);

    int64_t srcOffset = 0;
    if (funcArg[NARG_POS::FOURTH] != nullptr) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 uniformMatrix2x4fv srcOffset = %{public}u", srcOffset);
    }
    int64_t srcLength = 0;
    if (funcArg[NARG_POS::FIFTH] != nullptr) {
        tie(succ, srcLength) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 uniformMatrix2x4fv srcLength = %{public}u", srcLength);
    }
    napi_value array = funcArg[NARG_POS::THIRD];
    bool isArray = false;
    tie(succ, isArray) = NVal(env, array).IsArray();
    if (isArray) {
        LOGI("WebGL2 uniformMatrix2x4fv is Array");
        uint32_t length;
        napi_status lengthStatus = napi_get_array_length(env, array, &length);
        if (lengthStatus != napi_ok) {
            return nullptr;
        }
        float uniformMatrix2x4fv[length];
        uint32_t i;
        for (i = 0; i < length; i++) {
            napi_value element;
            napi_status eleStatus = napi_get_element(env, array, i, &element);
            if (eleStatus != napi_ok) {
                return nullptr;
            }
            double ele;
            napi_status doubleStatus = napi_get_value_double(env, element, &ele);
            if (doubleStatus != napi_ok) {
                return nullptr;
            }
            uniformMatrix2x4fv[i] = static_cast<float>(ele);
        }
        glUniformMatrix2x4fv(static_cast<GLuint>(location), static_cast<GLsizei>(srcLength),
            static_cast<GLboolean>(transpose), reinterpret_cast<GLfloat*>(uniformMatrix2x4fv + srcOffset));
        LOGI("WebGL2 uniformMatrix2x4fv array end");
        return nullptr;
    }
    bool isTypedarray = false;
    tie(succ, isTypedarray) = NVal(env, array).IsTypeArray();
    if (!isTypedarray || !succ) {
        return nullptr;
    }
    void *data = nullptr;
    size_t length;
    napi_typedarray_type type;
    tie(succ, type, data, length) = NVal(env, array).ToTypedArray();
    if (!succ) {
        return nullptr;
    }
    if (type == napi_float32_array) {
        float inputFloat[INPUTFLOAT_LENGTH] = {0};
        errno_t ret = memcpy_s(inputFloat, sizeof(inputFloat), reinterpret_cast<uint8_t*>(data) + srcOffset, length);
        if (ret != EOK) {
            LOGE("WebGL2 uniformMatrix2x4fv memcpy_s failed");
            return nullptr;
        }
        glUniformMatrix2x4fv(static_cast<GLuint>(location), static_cast<GLsizei>(srcLength),
            static_cast<GLboolean>(transpose), reinterpret_cast<GLfloat*>(inputFloat));
    }
    LOGI("WebGL2 uniformMatrix2x4fv typeArray end");
    return nullptr;
}

napi_value WebGL2RenderingContextBase::UniformMatrix3x4fv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::FIVE)) {
        return nullptr;
    }
    LOGI("WebGL2 uniformMatrix3x4fv start");
    bool succ = false;
    WebGLUniformLocation *webGLUniformLocation = nullptr;
    napi_status locationStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGLUniformLocation);
    if (locationStatus != napi_ok) {
        return nullptr;
    }
    int location = webGLUniformLocation->GetUniformLocationId();
    LOGI("WebGL2 WebGLRenderContext::uniformMatrix3x4fv location = %{public}u", location);
    if (funcArg[NARG_POS::SECOND] == nullptr) {
        return nullptr;
    }
    bool transpose = false;
    tie(succ, transpose) = NVal(env, funcArg[NARG_POS::SECOND]).ToBool();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 uniformMatrix3x4fv transpose = %{public}u", transpose);

    int64_t srcOffset = 0;
    if (funcArg[NARG_POS::FOURTH] != nullptr) {
        tie(succ, srcOffset) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 uniformMatrix3x4fv srcOffset = %{public}u", srcOffset);
    }
    int64_t srcLength = 0;
    if (funcArg[NARG_POS::FIFTH] != nullptr) {
        tie(succ, srcLength) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 uniformMatrix3x4fv srcLength = %{public}u", srcLength);
    }
    napi_value array = funcArg[NARG_POS::THIRD];
    bool isArray = false;
    tie(succ, isArray) = NVal(env, array).IsArray();
    if (isArray) {
        LOGI("WebGL2 uniformMatrix3x4fv is Array");
        uint32_t length;
        napi_status lengthStatus = napi_get_array_length(env, array, &length);
        if (lengthStatus != napi_ok) {
            return nullptr;
        }
        float uniformMatrix3x4fv[length];
        uint32_t i;
        for (i = 0; i < length; i++) {
            napi_value element;
            napi_status eleStatus = napi_get_element(env, array, i, &element);
            if (eleStatus != napi_ok) {
                return nullptr;
            }
            double ele;
            napi_status doubleStatus = napi_get_value_double(env, element, &ele);
            if (doubleStatus != napi_ok) {
                return nullptr;
            }
            uniformMatrix3x4fv[i] = static_cast<float>(ele);
        }
        glUniformMatrix3x4fv(static_cast<GLuint>(location), static_cast<GLsizei>(srcLength),
            static_cast<GLboolean>(transpose), reinterpret_cast<GLfloat*>(uniformMatrix3x4fv + srcOffset));
        LOGI("WebGL2 uniformMatrix3x4fv array end");
        return nullptr;
    }
    bool isTypedarray = false;
    tie(succ, isTypedarray) = NVal(env, array).IsTypeArray();
    if (!isTypedarray || !succ) {
        return nullptr;
    }
    void *data = nullptr;
    size_t length;
    napi_typedarray_type type;
    tie(succ, type, data, length) = NVal(env, array).ToTypedArray();
    if (!succ) {
        return nullptr;
    }
    if (type == napi_float32_array) {
        float inputFloat[INPUTFLOAT_LENGTH] = {0};
        errno_t ret = memcpy_s(inputFloat, sizeof(inputFloat), reinterpret_cast<uint8_t*>(data) + srcOffset, length);
        if (ret != EOK) {
            LOGE("WebGL2 uniformMatrix3x4fv memcpy_s failed");
            return nullptr;
        }
        glUniformMatrix3x4fv(static_cast<GLuint>(location), static_cast<GLsizei>(srcLength),
            static_cast<GLboolean>(transpose), reinterpret_cast<GLfloat*>(inputFloat));
    }
    LOGI("WebGL2 uniformMatrix3x4fv typeArray end");
    return nullptr;
}

napi_value WebGL2RenderingContextBase::VertexAttribI4iv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    LOGI("WebGL2 vertexAttribI4iv start");
    bool succ = false;
    int64_t index;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 vertexAttribI4iv index = %{public}u", index);
    napi_value array = funcArg[NARG_POS::SECOND];
    bool isArray = false;
    tie(succ, isArray) = NVal(env, array).IsArray();
    if (isArray) {
        LOGI("WebGL2 vertexAttribI4iv is Array");
        uint32_t length;
        napi_status lengthStatus = napi_get_array_length(env, array, &length);
        if (lengthStatus != napi_ok) {
            return nullptr;
        }
        float vertexAttribI4iv[length];
        uint32_t i;
        for (i = 0; i < length; i++) {
            napi_value element;
            napi_status eleStatus = napi_get_element(env, array, i, &element);
            if (eleStatus != napi_ok) {
                return nullptr;
            }
            double ele;
            napi_status doubleStatus = napi_get_value_double(env, element, &ele);
            if (doubleStatus != napi_ok) {
                return nullptr;
            }
            vertexAttribI4iv[i] = static_cast<float>(ele);
        }
        glVertexAttribI4iv(static_cast<GLuint>(index), reinterpret_cast<GLint *>(vertexAttribI4iv));
        LOGI("WebGL2 vertexAttribI4iv array end");
        return nullptr;
    }
    bool isTypedarray = false;
    tie(succ, isTypedarray) = NVal(env, array).IsTypeArray();
    if (!isTypedarray || !succ) {
        return nullptr;
    }
    void *data = nullptr;
    size_t length;
    napi_typedarray_type type;
    tie(succ, type, data, length) = NVal(env, array).ToTypedArray();
    if (!succ) {
        return nullptr;
    }
    if (type == napi_float32_array) {
        float inputFloat[INPUTFLOAT_LENGTH] = {0};
        errno_t ret = memcpy_s(inputFloat, sizeof(inputFloat), data, length);
        if (ret != EOK) {
            LOGE("WebGL2 vertexAttribI4iv memcpy_s failed");
            return nullptr;
        }
        glVertexAttribI4iv(static_cast<GLuint>(index), reinterpret_cast<GLint *>(inputFloat));
    }
    LOGI("WebGL2 vertexAttribI4iv typeArray end");
    return nullptr;
}

napi_value WebGL2RenderingContextBase::VertexAttribI4uiv(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }

    LOGI("WebGL2 vertexAttribI4uiv start");
    bool succ = false;
    int64_t index;
    tie(succ, index) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 vertexAttribI4uiv index = %{public}u", index);
    napi_value array = funcArg[NARG_POS::SECOND];
    bool isArray = false;
    tie(succ, isArray) = NVal(env, array).IsArray();
    if (isArray) {
        LOGI("WebGL2 vertexAttribI4uiv is Array");
        uint32_t length;
        napi_status lengthStatus = napi_get_array_length(env, array, &length);
        if (lengthStatus != napi_ok) {
            return nullptr;
        }
        float vertexAttribI4uiv[length];
        uint32_t i;
        for (i = 0; i < length; i++) {
            napi_value element;
            napi_status eleStatus = napi_get_element(env, array, i, &element);
            if (eleStatus != napi_ok) {
                return nullptr;
            }
            double ele;
            napi_status doubleStatus = napi_get_value_double(env, element, &ele);
            if (doubleStatus != napi_ok) {
                return nullptr;
            }
            vertexAttribI4uiv[i] = static_cast<float>(ele);
        }
        glVertexAttribI4uiv(static_cast<GLuint>(index), reinterpret_cast<const GLuint *>(vertexAttribI4uiv));
        LOGI("WebGL2 vertexAttribI4uiv array end");
        return nullptr;
    }
    bool isTypedarray = false;
    tie(succ, isTypedarray) = NVal(env, array).IsTypeArray();
    if (!isTypedarray || !succ) {
        return nullptr;
    }
    void *data = nullptr;
    size_t length;
    napi_typedarray_type type;
    tie(succ, type, data, length) = NVal(env, array).ToTypedArray();
    if (!succ) {
        return nullptr;
    }
    if (type == napi_float32_array) {
        int *inputFloat = reinterpret_cast<int*>(reinterpret_cast<uint8_t*>(data));
        glVertexAttribI4uiv(static_cast<GLuint>(index), reinterpret_cast<const GLuint *>(inputFloat));
    }
    LOGI("WebGL2 vertexAttribI4uiv typeArray end");
    return nullptr;
}

napi_value WebGL2RenderingContextBase::InvalidateFramebuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
            return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 invalidateFramebuffer start");
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 invalidateFramebuffer target = %{public}u", target);
    napi_value uniformarray = funcArg[NARG_POS::SECOND];
    bool isUniformArray = false;
    tie(succ, isUniformArray) = NVal(env, uniformarray).IsArray();
    if (isUniformArray) {
        LOGI("WebGL2 invalidateFramebuffer is isUniformArray");
        uint32_t numAttachments;
        napi_status countStatus = napi_get_array_length(env, uniformarray, &numAttachments);
        if (countStatus != napi_ok) {
            return nullptr;
        }
        LOGI("WebGL2 invalidateFramebuffer numAttachments = %{public}u", numAttachments);
        int64_t attachments[numAttachments];
        uint32_t i;
        for (i = 0; i < numAttachments; i++) {
            napi_value element;
            napi_status eleStatus = napi_get_element(env, uniformarray, i, &element);
            if (eleStatus != napi_ok) {
                return nullptr;
            }
            int64_t ele;
            napi_status doubleStatus = napi_get_value_int64(env, element, &ele);
            if (doubleStatus != napi_ok) {
                return nullptr;
            }
            LOGI("WebGL2 invalidateFramebuffer ele = %{public}u", ele);
            attachments[i] = ele;
        }
        glInvalidateFramebuffer(static_cast<GLenum>(target),
            static_cast<GLsizei>(numAttachments), reinterpret_cast<GLenum*>(attachments));
        LOGI("WebGL2 invalidateFramebuffer invalidateFramebuffer end");
        return nullptr;
    }
    LOGI("WebGL2 invalidateFramebuffer end");
    return nullptr;
}

napi_value WebGL2RenderingContextBase::InvalidateSubFramebuffer(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::SIX)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 invalidateSubFramebuffer start");
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 invalidateSubFramebuffer target = %{public}u", target);
    napi_value uniformarray = funcArg[NARG_POS::SECOND];
    uint32_t numAttachments;
    void *attachments;
    bool isUniformArray = false;
    tie(succ, isUniformArray) = NVal(env, uniformarray).IsArray();
    if (isUniformArray) {
        LOGI("WebGL2 invalidateSubFramebuffer is isUniformArray");
        napi_status countStatus = napi_get_array_length(env, uniformarray, &numAttachments);
        if (countStatus != napi_ok) {
            return nullptr;
        }
        LOGI("WebGL2 invalidateSubFramebuffer numAttachments = %{public}u", numAttachments);
        int64_t attachments1[numAttachments];
        uint32_t i;
        for (i = 0; i < numAttachments; i++) {
            napi_value element;
            napi_status eleStatus = napi_get_element(env, uniformarray, i, &element);
            if (eleStatus != napi_ok) {
                return nullptr;
            }
            int64_t ele;
            napi_status doubleStatus = napi_get_value_int64(env, element, &ele);
            if (doubleStatus != napi_ok) {
                return nullptr;
            }
            LOGI("WebGL2 invalidateSubFramebuffer ele = %{public}u", ele);
            attachments1[i] = ele;
        }
        attachments = attachments1;
    } else {
        return nullptr;
    }

    int64_t x;
    tie(succ, x) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 invalidateSubFramebuffer x = %{public}u", x);
    int64_t y;
    tie(succ, y) = NVal(env, funcArg[NARG_POS::FOURTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 invalidateSubFramebuffer y = %{public}u", y);
    int64_t width;
    tie(succ, width) = NVal(env, funcArg[NARG_POS::FIFTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 invalidateSubFramebuffer width = %{public}u", width);
    int64_t height;
    tie(succ, height) = NVal(env, funcArg[NARG_POS::SIXTH]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 invalidateSubFramebuffer height = %{public}u", height);
    glInvalidateSubFramebuffer(static_cast<GLenum>(target), static_cast<GLsizei>(numAttachments),
        reinterpret_cast<GLenum*>(attachments), static_cast<GLint>(x),
        static_cast<GLint>(y), static_cast<GLsizei>(width), static_cast<GLsizei>(height));
    LOGI("WebGL2 invalidateSubFramebuffer end");
    return nullptr;
}

napi_value WebGL2RenderingContextBase::GetInternalformatParameter(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 getInternalformatParameter start");
    int64_t target;
    tie(succ, target) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 getInternalformatParameter target = %{public}u", target);
    int64_t internalformat;
    tie(succ, internalformat) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 getInternalformatParameter internalformat = %{public}u", internalformat);
    int64_t pname;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 getInternalformatParameter pname = %{public}u", pname);

    napi_value retures = nullptr;
    static GLint* params = nullptr;
    if (pname == GL_SAMPLES) {
        LOGI("WebGL2 getInternalformatParameter GL_SAMPLES start");
        GLint length = -1;
        glGetInternalformativ(static_cast<GLenum>(target), static_cast<GLenum>(internalformat), GL_NUM_SAMPLE_COUNTS,
            1, &length);
        LOGI("WebGL2 getInternalformatParameter length = %{public}u", length);
        if (length <= 0) {
            return nullptr;
        }
        int a = static_cast<int>(length);
        params = new GLint[a];
        glGetInternalformativ(static_cast<GLenum>(target), static_cast<GLenum>(internalformat),
            static_cast<GLenum>(pname), static_cast<GLsizei>(length), static_cast<GLint *>(params));
        LOGI("WebGL2 getInternalformatParameter params = %{public}u", params);
        int res[length];
        for (int i = 0; i < length; i++) {
            res[i] = static_cast<int>(params[i]);
        }
        napi_value outputBuffer = nullptr;
        napi_create_external_arraybuffer(env, res, sizeof(res),
                                         [](napi_env env, void *finalize_data, void *finalize_hint) {  },
                                         NULL, &outputBuffer);
        napi_create_typedarray(env, napi_int32_array, sizeof(res) / sizeof(int), outputBuffer, 0, &retures);
        delete []params;
    }
    LOGI("WebGL2 getInternalformatParameter end");
    return retures;
}

napi_value WebGL2RenderingContextBase::TransformFeedbackVaryings(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 TransformFeedbackVaryings start");
    WebGLProgram *webGlProgram = nullptr;
    napi_status programStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlProgram);
    if (programStatus != napi_ok) {
        return nullptr;
    }
    int programId = webGlProgram->GetProgramId();
    LOGI("WebGL2 transformFeedbackVaryings programId = %{public}u", programId);

    napi_value transformFeedbackarray = funcArg[NARG_POS::SECOND];
    uint32_t count;
    bool istransformFeedbackArray = false;
    tie(succ, istransformFeedbackArray) = NVal(env, transformFeedbackarray).IsArray();
    if (istransformFeedbackArray) {
        LOGI("WebGL2 transformFeedbackVaryings is isUniformArray");
        napi_status countStatus = napi_get_array_length(env, transformFeedbackarray, &count);
        if (countStatus != napi_ok) {
            return nullptr;
        }
        LOGI("WebGL2 transformFeedbackVaryings count = %{public}u", count);
        char *varyings[count];
        uint32_t i;
        for (i = 0; i < count; i++) {
            napi_value element;
            napi_status eleStatus = napi_get_element(env, transformFeedbackarray, i, &element);
            LOGI("WebGL2 transformFeedbackVaryings element = %{public}u", element);
            if (eleStatus != napi_ok) {
                return nullptr;
            }
            napi_value result;
            napi_status rsuStatus = napi_coerce_to_string(env, element, &result);
            if (rsuStatus != napi_ok) {
                return nullptr;
            }
            unique_ptr<char[]> name;
            tie(succ, name, ignore) = NVal(env, result).ToUTF8String();
            if (!succ) {
                return nullptr;
            }
            LOGI("WebGL2 transformFeedbackVaryings name = %{public}s", name.get());
            varyings[i] = name.get();
        }
        int64_t bufferMode;
        tie(succ, bufferMode) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
        if (!succ) {
            return nullptr;
        }
        LOGI("WebGL2 transformFeedbackVaryings bufferMode = %{public}u", bufferMode);
        glTransformFeedbackVaryings(static_cast<GLuint>(programId), static_cast<GLsizei>(count),
            varyings, static_cast<GLenum>(bufferMode));
        LOGI("WebGL2 transformFeedbackVaryings end");
    }
    return nullptr;
}

napi_value WebGL2RenderingContextBase::GetUniformIndices(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 getUniformIndices start");
    WebGLProgram *webGlProgram = nullptr;
    napi_status programStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlProgram);
    if (programStatus != napi_ok) {
        return nullptr;
    }
    int programId = webGlProgram->GetProgramId();
    LOGI("WebGL2 getUniformIndices programId = %{public}u", programId);
    napi_value transformFeedbackarray = funcArg[NARG_POS::SECOND];
    uint32_t uniformCount;
    char** uniformNames = NULL;
    bool istransformFeedbackArray = false;
    tie(succ, istransformFeedbackArray) = NVal(env, transformFeedbackarray).IsArray();
    if (istransformFeedbackArray) {
        LOGI("WebGL2 transformFeedbackVaryings is isUniformArray");
        napi_status countStatus = napi_get_array_length(env, transformFeedbackarray, &uniformCount);
        if (countStatus != napi_ok) {
            return nullptr;
        }
        LOGI("WebGL2 WebGLRenderingContextOverloads::getUniformIndices count = %{public}u", uniformCount);
        if (uniformCount > UNIFORM_NAMES_MAX_LENGTH) {
            LOGE("WebGL2 WebGLRenderingContextOverloads::getUniformIndicesuniformCount is invalid");
            return nullptr;
        }
        uniformNames = (char**)malloc(uniformCount * sizeof(char*));
        if (uniformNames == NULL) {
            LOGE("WebGL2 WebGLRenderingContextOverloads::getUniformIndices malloc failed");
            return nullptr;
        }
        uint32_t i;
        for (i = 0; i < uniformCount; i++) {
            napi_value element;
            napi_status eleStatus = napi_get_element(env, transformFeedbackarray, i, &element);
            if (eleStatus != napi_ok) {
                free(uniformNames);
                return nullptr;
            }
            napi_value result;
            napi_status rsuStatus = napi_coerce_to_string(env, element, &result);
            if (rsuStatus != napi_ok) {
                free(uniformNames);
                return nullptr;
            }
            unique_ptr<char[]> name;
            tie(succ, name, ignore) = NVal(env, result).ToUTF8String();
            if (!succ) {
                free(uniformNames);
                return nullptr;
            }
            LOGI("WebGL2 WebGLRenderingContextOverloads::getUniformIndices str = %{public}s", name.get());
            uniformNames[i] = name.get();
        }
    } else {
        return nullptr;
    }
    LOGI("WebGL2 getUniformIndices uniformCount = %{public}u", uniformCount);
    GLuint* uniformIndices = new GLuint[uniformCount];
    glGetUniformIndices(static_cast<GLuint>(programId), static_cast<GLsizei>(uniformCount),
        const_cast<const GLchar **>(uniformNames), static_cast<GLuint *>(uniformIndices));
    napi_value ret;
    napi_create_array(env, &ret);
    for (uint32_t i = 0; i < uniformCount; i++) {
        uint32_t a = static_cast<GLuint>(uniformIndices[i]);
        LOGI("WebGL2 getUniformIndices a = %{public}u", a);
        napi_value result;
        napi_status status = napi_create_uint32(env, a, &result);
        if (status != napi_ok) {
            delete []uniformIndices;
            free(uniformNames);
            return nullptr;
        }
        napi_set_element(env, ret, i, result);
    }
    LOGI("WebGL2 getUniformIndices end");
    delete []uniformIndices;
    free(uniformNames);
    return ret;
}

napi_value WebGL2RenderingContextBase::GetActiveUniforms(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 getActiveUniforms start");
    WebGLProgram *webGlProgram = nullptr;
    napi_status programStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **) &webGlProgram);
    if (programStatus != napi_ok) {
        return nullptr;
    }
    int programId = webGlProgram->GetProgramId();
    LOGI("WebGL2 getActiveUniforms programId = %{public}u", programId);
    napi_value activeUniforms  = funcArg[NARG_POS::SECOND];
    uint32_t uniformCount;
    void *uniformIndices;
    bool istransformFeedbackArray = false;
    int* transformFeedback = nullptr;
    tie(succ, istransformFeedbackArray) = NVal(env, activeUniforms).IsArray();
    if (istransformFeedbackArray) {
        LOGI("WebGL2 getActiveUniforms is isUniformArray");
        napi_status countStatus = napi_get_array_length(env, activeUniforms, &uniformCount);
        if (countStatus != napi_ok) {
            return nullptr;
        }
        LOGI("WebGL2 getActiveUniforms count = %{public}u", uniformCount);
        if (uniformCount == 0) {
            return nullptr;
        }
        transformFeedback = new int[uniformCount];
        uint32_t i;
        for (i = 0; i < uniformCount; i++) {
            napi_value element;
            napi_status eleStatus = napi_get_element(env, activeUniforms, i, &element);
            if (eleStatus != napi_ok) {
                delete []transformFeedback;
                return nullptr;
            }
            napi_value result;
            napi_status rsuStatus = napi_coerce_to_number(env, element, &result);
            if (rsuStatus != napi_ok) {
                delete []transformFeedback;
                return nullptr;
            }
            int64_t ele;
            napi_status doubleStatus = napi_get_value_int64(env, result, &ele);
            if (doubleStatus != napi_ok) {
                delete []transformFeedback;
                return nullptr;
            }
            transformFeedback[i] = ele;
        }
        uniformIndices = transformFeedback;
    } else {
        return nullptr;
    }
    LOGI("WebGL2 getActiveUniforms uniformCount = %{public}u", uniformCount);
    LOGI("WebGL2 getActiveUniforms uniformIndices = %{public}u", uniformIndices);
    int64_t pname;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        delete []transformFeedback;
        return nullptr;
    }
    LOGI("WebGL2 getActiveUniforms pname = %{public}u", pname);
    napi_value res = nullptr;
    napi_create_array(env, &res);
    if (pname == GL_UNIFORM_TYPE) {
        LOGI("WebGL2 getActiveUniforms GL_UNIFORM_TYPE start");
        auto params = new unsigned int[uniformCount];
        glGetActiveUniformsiv(static_cast<GLuint>(programId), static_cast<GLsizei>(uniformCount),
                              static_cast<GLuint *>(uniformIndices), static_cast<GLenum>(pname),
                              reinterpret_cast<GLint *>(params));
        LOGI("WebGL2 getActiveUniforms params = %{public}u", params);
        for (uint32_t i = 0; i < uniformCount; i++) {
            int64_t a = (reinterpret_cast<int64_t*>(params))[i];
            napi_value result;
            napi_status status = napi_create_int64(env, a, &result);
            if (status != napi_ok) {
                delete []transformFeedback;
                delete []params;
                return nullptr;
            }
            napi_set_element(env, res, i, result);
        }
        delete []params;
    } else if (pname == GL_UNIFORM_SIZE) {
        LOGI("WebGL2 getActiveUniforms GL_UNIFORM_SIZE start");
        auto params = new unsigned int[uniformCount];
        glGetActiveUniformsiv(static_cast<GLuint>(programId), static_cast<GLsizei>(uniformCount),
                              static_cast<GLuint *>(uniformIndices), static_cast<GLenum>(pname),
                              reinterpret_cast<GLint *>(params));
        LOGI("WebGL2 getActiveUniforms params = %{public}u", params);
        for (uint32_t i = 0; i < uniformCount; i++) {
            uint32_t a = (static_cast<uint32_t*>(params))[i];
            LOGI("WebGL2 getActiveUniforms a = %{public}u", a);
            napi_value result;
            napi_status status = napi_create_uint32(env, a, &result);
            if (status != napi_ok) {
                delete []transformFeedback;
                delete []params;
                return nullptr;
            }
            napi_set_element(env, res, i, result);
        }
        delete []params;
    } else if (pname == GL_UNIFORM_BLOCK_INDEX || pname == GL_UNIFORM_OFFSET || pname == GL_UNIFORM_ARRAY_STRIDE
               || pname == GL_UNIFORM_MATRIX_STRIDE) {
        LOGI("WebGL2 getActiveUniforms GL_UNIFORM_BLOCK_INDEX start");
        auto params = new int[uniformCount];
        glGetActiveUniformsiv(static_cast<GLuint>(programId), static_cast<GLsizei>(uniformCount),
                              static_cast<GLuint *>(uniformIndices), static_cast<GLenum>(pname),
                              reinterpret_cast<GLint *>(params));
        LOGI("WebGL2 getActiveUniforms params = %{public}u", params);
        for (uint32_t i = 0; i < uniformCount; i++) {
            int32_t a = (static_cast<int32_t*>(params))[i];
            napi_value result;
            napi_status status = napi_create_int32(env, a, &result);
            if (status != napi_ok) {
                delete []transformFeedback;
                delete []params;
                return nullptr;
            }
            napi_set_element(env, res, i, result);
        }
        delete []params;
    } else if (pname == GL_UNIFORM_IS_ROW_MAJOR) {
        LOGI("WebGL2 getActiveUniforms GL_UNIFORM_IS_ROW_MAJOR start");
        auto params = new unsigned int[uniformCount];
        glGetActiveUniformsiv(static_cast<GLuint>(programId), static_cast<GLsizei>(uniformCount),
                              static_cast<GLuint *>(uniformIndices), static_cast<GLenum>(pname),
                              reinterpret_cast<GLint *>(params));
        LOGI("WebGL2 getActiveUniforms params = %{public}u", params);
        for (uint32_t i = 0; i < uniformCount; i++) {
            bool a = (reinterpret_cast<bool*>(params))[i];
            napi_value result = nullptr;
            napi_status status = napi_get_boolean(env, a, &result);
            if (status != napi_ok) {
                delete []transformFeedback;
                delete []params;
                return nullptr;
            }
            napi_set_element(env, res, i, result);
        }
        delete []params;
    }
    LOGI("WebGL2 getActiveUniforms end");
    delete []transformFeedback;
    return res;
}

napi_value WebGL2RenderingContextBase::GetActiveUniformBlockParameter(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::THREE)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 getActiveUniformBlockParameter start");
    WebGLProgram *webGlProgram = nullptr;
    napi_status programStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGlProgram);
    if (programStatus != napi_ok) {
        return nullptr;
    }
    int programId = webGlProgram->GetProgramId();
    LOGI("WebGL2 getActiveUniformBlockParameter programId = %{public}u", programId);
    int64_t uniformBlockIndex;
    tie(succ, uniformBlockIndex) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 getActiveUniformBlockParameter uniformBlockIndex = %{public}u",
         uniformBlockIndex);
    int64_t pname;
    tie(succ, pname) = NVal(env, funcArg[NARG_POS::THIRD]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 getActiveUniformBlockParameter pname = %{public}u", pname);
    GLint params;
    glGetActiveUniformBlockiv(static_cast<GLuint>(programId), static_cast<GLuint>(uniformBlockIndex),
                              static_cast<GLenum>(pname), &params);
    if (pname == GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER ||
        pname == GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER) {
        bool res = (params == GL_FALSE) ? false : true;
        LOGI("WebGL2 getActiveUniformBlockParameter end");
        return NVal::CreateBool(env, res).val_;
    } else if (pname == GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES) {
        GLint uniformCount = 1;
        LOGI("WebGL2 getActiveUniformBlockParameter uniform_count = %{public}d",
             GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS);
        glGetActiveUniformBlockiv(static_cast<GLuint>(programId), static_cast<GLuint>(uniformBlockIndex),
                                  GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &uniformCount);
        LOGI("WebGL2 getActiveUniformBlockParameter uniform_count = %{public}d", uniformCount);
        GLint params[uniformCount];
        glGetActiveUniformBlockiv(static_cast<GLuint>(programId), static_cast<GLuint>(uniformBlockIndex),
                                  static_cast<GLenum>(pname), static_cast<GLint *>(params));
        static uint32_t *res = new uint32_t[uniformCount];
        int i;
        for (i = 0; i < uniformCount; i++) {
            res[i] = static_cast<uint32_t>(params[i]);
        }
        napi_value outputBuffer = nullptr;
        napi_create_external_arraybuffer(env, res, sizeof(params),
                                         [](napi_env env, void *finalize_data, void *finalize_hint) {  },
                                         NULL, &outputBuffer);
        napi_value outputArray = nullptr;
        napi_create_typedarray(env, napi_uint32_array, sizeof(params) / sizeof(uint32_t),
                               outputBuffer, 0, &outputArray);
        LOGI("WebGL2 getActiveUniformBlockParameter end");
        delete []res;
        return outputArray;
    } else if (pname == GL_UNIFORM_BLOCK_BINDING ||
               pname == GL_UNIFORM_BLOCK_DATA_SIZE ||
               pname == GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS) {
        int64_t res = static_cast<int64_t>(params);
        LOGI("WebGL2 getActiveUniformBlockParameter end");
        return NVal::CreateInt64(env, res).val_;
    } else {
        LOGI("WebGL getActiveUniformBlockParameter : pname is wrong");
        return nullptr;
    }
}

napi_value WebGL2RenderingContextBase::GetActiveUniformBlockName(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        return nullptr;
    }
    if (funcArg[NARG_POS::FIRST] == nullptr) {
        return nullptr;
    }
    bool succ = false;
    LOGI("WebGL2 getActiveUniformBlockName start");
    WebGLProgram *webGlProgram = nullptr;
    napi_status programStatus = napi_unwrap(env, funcArg[NARG_POS::FIRST], (void **)&webGlProgram);
    if (programStatus != napi_ok) {
        return nullptr;
    }
    int programId = webGlProgram->GetProgramId();
    LOGI("WebGL2 getActiveUniformBlockName programId = %{public}u", programId);
    int64_t uniformBlockIndex;
    tie(succ, uniformBlockIndex) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt64();
    if (!succ) {
        return nullptr;
    }
    LOGI("WebGL2 getActiveUniformBlockName uniformBlockIndex = %{public}u", uniformBlockIndex);
    GLint length = 0;
    GLsizei size = 0;
    glGetProgramiv(static_cast<GLuint>(programId), GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &length);
    std::unique_ptr<char[]> buf = std::make_unique<char[]>(length);
    if (buf == nullptr) {
        return nullptr;
    }
    glGetActiveUniformBlockName(programId, uniformBlockIndex, length, &size, buf.get());
    string str = buf.get();
    return NVal::CreateUTF8String(env, str).val_;
}
} // namespace Rosen
} // namespace OHOS

#ifdef __cplusplus
}
#endif
