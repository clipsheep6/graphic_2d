/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "../include/context/webgl2_rendering_context_impl.h"

#include "../../common/napi/n_class.h"
#include "../../common/napi/n_func_arg.h"
#include "../include/context/webgl2_rendering_context_base.h"
#include "../include/context/webgl_context_attributes.h"
#include "../include/context/webgl_rendering_context.h"
#include "../include/context/webgl_rendering_context_base.h"
#include "../include/context/webgl_rendering_context_basic_base.h"
#include "../include/util/egl_manager.h"
#include "../include/util/log.h"
#include "../include/util/object_source.h"
#include "../include/util/util.h"
#include "../include/webgl/webgl_query.h"
#include "../include/webgl/webgl_sampler.h"
#include "../include/webgl/webgl_shader.h"
#include "../include/webgl/webgl_sync.h"
#include "../include/webgl/webgl_transform_feedback.h"
#include "../include/webgl/webgl_vertex_array_object.h"

namespace OHOS {
namespace Rosen {
namespace Impl {
using namespace std;

#define SET_ERROR(error, ...) \
    do {                      \
        LOGE(__VA_ARGS__);    \
        SetError(error);      \
    } while (0)

napi_value WebGL2RenderingContextImpl::CreateQuery(napi_env env)
{
    WebGLQuery* webGlQuery = nullptr;
    napi_value objQuery = WebGLQuery::CreateObjectInstance(env, &webGlQuery).val_;
    if (!objQuery) {
        return NVal::CreateNull(env).val_;
    }

    unsigned int queryId;
    glGenQueries(1, &queryId);
    webGlQuery->SetQuery(queryId);
    AddObject<WebGLQuery>(env, queryId, objQuery);
    LOGI("WebGL2 createQuery queryId = %{public}u", queryId);
    return objQuery;
}

napi_value WebGL2RenderingContextImpl::DeleteQuery(napi_env env, napi_value object)
{
    uint32_t queryId = 0;
    WebGLQuery* webGlQuery = WebGLObject::GetObjectInstance<WebGLQuery>(env, object);
    if (webGlQuery == nullptr) {
        return nullptr;
    }
    queryId = webGlQuery->GetQuery();

    glDeleteQueries(1, &queryId);

    DeleteObject<WebGLQuery>(env, queryId);
    LOGI("WebGL2 deleteQuery queryId = %{public}u", queryId);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::IsQuery(napi_env env, napi_value object)
{
    uint32_t queryId = 0;
    WebGLQuery* webGlQuery = WebGLObject::GetObjectInstance<WebGLQuery>(env, object);
    if (webGlQuery == nullptr) {
        return NVal::CreateBool(env, false).val_;
    }
    queryId = webGlQuery->GetQuery();

    GLboolean returnValue = glIsQuery(static_cast<GLuint>(queryId));
    bool res = static_cast<bool>(returnValue);
    LOGI("WebGL2 isQuery query %{public}u res %{public}d ", queryId, res);
    return NVal::CreateBool(env, res).val_;
}

napi_value WebGL2RenderingContextImpl::GetQuery(napi_env env, GLenum target, GLenum pName)
{
    LOGI("WebGL2 getQuery target %{public}u %{public}u", target, pName);
    if (pName != WebGL2RenderingContextBase::CURRENT_QUERY) {
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    uint32_t index = 0;
    if (CheckQueryTarget(env, target, index)) {
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }

    GLint params;
    glGetQueryiv(target, pName, &params);
    LOGI("WebGL2 getQuery params = %{public}u %{public}u", params, currentQuery[index]);
    return GetObject<WebGLQuery>(env, params);
}

napi_value WebGL2RenderingContextImpl::BeginQuery(napi_env env, GLenum target, napi_value object)
{
    LOGI("WebGL2 beginQuery target %{public}u", target);
    GLuint queryId = 0;
    WebGLQuery* webGlQuery = WebGLObject::GetObjectInstance<WebGLQuery>(env, object);
    if (webGlQuery == nullptr) {
        SetError(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    queryId = webGlQuery->GetQuery();

    if (webGlQuery->GetTarget() && webGlQuery->GetTarget() != target) {
        SetError(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    uint32_t index = 0;
    if (CheckQueryTarget(env, target, index)) {
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }

    if (currentQuery[index] && currentQuery[index] != queryId) {
        SetError(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    webGlQuery->SetTarget(target);

    glBeginQuery(target, queryId);
    LOGI("WebGL2 beginQuery target %{public}u %{public}u", target, queryId);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::EndQuery(napi_env env, GLenum target)
{
    LOGI("WebGL2 endQuery target %{public}u", target);
    uint32_t index = 0;
    if (CheckQueryTarget(env, target, index)) {
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    if (currentQuery[index]) {
        SetError(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    currentQuery[index] = 0;
    glEndQuery(target);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::GetQueryParameter(napi_env env, napi_value queryObj, GLenum pName)
{
    GLuint queryId = 0;
    WebGLQuery* webGlQuery = WebGLObject::GetObjectInstance<WebGLQuery>(env, queryObj);
    if (webGlQuery == nullptr) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateNull(env).val_;
    }
    queryId = webGlQuery->GetQuery();

    GLuint params;
    if (pName == GL_QUERY_RESULT) {
        glGetQueryObjectuiv(queryId, pName, &params);
        int64_t res = static_cast<int64_t>(params);
        LOGI("WebGL2 getQueryParameter params %{public}u", params);
        return NVal::CreateInt64(env, res).val_;
    } else if (pName == GL_QUERY_RESULT_AVAILABLE) {
        glGetQueryObjectuiv(queryId, pName, &params);
        bool res = (params == GL_FALSE) ? false : true;
        LOGI("WebGL2 getQueryParameter params %{public}u", params);
        return NVal::CreateBool(env, res).val_;
    } else {
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }
}

napi_value WebGL2RenderingContextImpl::CreateSampler(napi_env env)
{
    WebGLSampler* webGlSampler = nullptr;
    napi_value objSampler = WebGLSampler::CreateObjectInstance(env, &webGlSampler).val_;
    if (!objSampler) {
        return NVal::CreateNull(env).val_;
    }
    GLuint samplerId;
    glGenSamplers(1, &samplerId);
    webGlSampler->SetSampler(samplerId);
    LOGI("WebGL2 createSampler samplerId = %{public}u", samplerId);
    AddObject<WebGLSampler>(env, samplerId, objSampler);
    return objSampler;
}

napi_value WebGL2RenderingContextImpl::DeleteSampler(napi_env env, napi_value samplerObj)
{
    WebGLSampler* sampler = WebGLObject::GetObjectInstance<WebGLSampler>(env, samplerObj);
    if (sampler == nullptr) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    GLuint samplerId = sampler->GetSampler();
    LOGI("WebGL2 deleteSampler samplerId = %{public}u", samplerId);
    glDeleteSamplers(1, &samplerId);
    DeleteObject<WebGLSampler>(env, samplerId);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::IsSampler(napi_env env, napi_value samplerObj)
{
    WebGLSampler* sampler = WebGLObject::GetObjectInstance<WebGLSampler>(env, samplerObj);
    if (sampler == nullptr) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateBool(env, false).val_;
    }
    GLuint samplerId = sampler->GetSampler();

    GLboolean returnValue = glIsSampler(samplerId);
    bool res = static_cast<bool>(returnValue);
    LOGI("WebGL2 IsSampler samplerId = %{public}u res %{public}u", samplerId, res);
    return NVal::CreateBool(env, res).val_;
}

napi_value WebGL2RenderingContextImpl::BindSampler(napi_env env, GLuint unit, napi_value samplerObj)
{
    WebGLSampler* sampler = WebGLObject::GetObjectInstance<WebGLSampler>(env, samplerObj);
    if (sampler == nullptr) {
        SetError(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    GLuint samplerId = sampler->GetSampler();

    glBindSampler(unit, samplerId);
    LOGI("WebGL2 bindSampler unit = %{public}u samplerId = %{public}u", unit, samplerId);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::SamplerParameter(
    napi_env env, napi_value samplerObj, GLenum pName, bool isFloat, void* param)
{
    LOGI("WebGL2 samplerParameteri pname %{public}u param %{public}u", pName, param);
    WebGLSampler* sampler = WebGLObject::GetObjectInstance<WebGLSampler>(env, samplerObj);
    if (sampler == nullptr) {
        SetError(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    GLuint samplerId = sampler->GetSampler();
    switch (pName) {
        case GL_TEXTURE_COMPARE_FUNC:
        case GL_TEXTURE_COMPARE_MODE:
        case GL_TEXTURE_MAG_FILTER:
        case GL_TEXTURE_MIN_FILTER:
        case GL_TEXTURE_WRAP_R:
        case GL_TEXTURE_WRAP_S:
        case GL_TEXTURE_WRAP_T:
        case GL_TEXTURE_MAX_LOD:
        case GL_TEXTURE_MIN_LOD:
            break;
        default:
            SetError(WebGLRenderingContextBase::INVALID_ENUM);
            return nullptr;
    }
    if (isFloat) {
        GLfloat v = *static_cast<GLfloat*>(param);
        glSamplerParameterf(samplerId, pName, v);
    } else {
        GLint v = *static_cast<GLint*>(param);
        glSamplerParameteri(samplerId, pName, v);
    }
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::GetSamplerParameter(napi_env env, napi_value samplerObj, GLenum pName)
{
    WebGLSampler* sampler = WebGLObject::GetObjectInstance<WebGLSampler>(env, samplerObj);
    if (sampler == nullptr) {
        SetError(WebGLRenderingContextBase::INVALID_OPERATION);
        return NVal::CreateNull(env).val_;
    }
    GLuint samplerId = sampler->GetSampler();
    LOGI("WebGL2 getSamplerParameter samplerId %{public}u params %{public}u", samplerId, pName);

    switch (pName) {
        case GL_TEXTURE_COMPARE_FUNC:
        case GL_TEXTURE_COMPARE_MODE:
        case GL_TEXTURE_MAG_FILTER:
        case GL_TEXTURE_MIN_FILTER:
        case GL_TEXTURE_WRAP_R:
        case GL_TEXTURE_WRAP_S:
        case GL_TEXTURE_WRAP_T: {
            GLint params;
            glGetSamplerParameteriv(static_cast<GLuint>(samplerId), static_cast<GLenum>(pName), &params);
            int64_t res = static_cast<int64_t>(params);
            LOGI("WebGL2 getSamplerParameter samplerId %{public}u params %{public}u", samplerId, params);
            return NVal::CreateInt64(env, res).val_;
        }
        case GL_TEXTURE_MAX_LOD:
        case GL_TEXTURE_MIN_LOD: {
            GLfloat params;
            glGetSamplerParameterfv(static_cast<GLuint>(samplerId), static_cast<GLenum>(pName), &params);
            float res = static_cast<float>(params);
            LOGI("WebGL2 getSamplerParameter samplerId %{public}u params %{public}f", samplerId, params);
            return NVal::CreateDouble(env, (double)res).val_;
        }
        default:
            SetError(WebGLRenderingContextBase::INVALID_ENUM);
            return NVal::CreateNull(env).val_;
    }
}

napi_value WebGL2RenderingContextImpl::CreateVertexArray(napi_env env)
{
    WebGLVertexArrayObject* webGLVertexArrayObject = nullptr;
    napi_value objVertexArrayObject = WebGLVertexArrayObject::CreateObjectInstance(env, &webGLVertexArrayObject).val_;
    if (!objVertexArrayObject) {
        return NVal::CreateNull(env).val_;
    }
    unsigned int vertexArraysId;
    glGenVertexArrays(1, &vertexArraysId);

    webGLVertexArrayObject->SetVertexArrays(vertexArraysId);
    LOGI("WebGL2 createVertexArray vertexArraysId = %{public}u", vertexArraysId);
    AddObject<WebGLVertexArrayObject>(env, vertexArraysId, objVertexArrayObject);
    return objVertexArrayObject;
}

napi_value WebGL2RenderingContextImpl::DeleteVertexArray(napi_env env, napi_value object)
{
    unsigned int vertexArrays = WebGLVertexArrayObject::DEFAULT_VERTEX_ARRAY_OBJECT;
    WebGLVertexArrayObject* webGLVertexArrayObject =
        WebGLObject::GetObjectInstance<WebGLVertexArrayObject>(env, object);
    if (webGLVertexArrayObject == nullptr) {
        return nullptr;
    }
    vertexArrays = webGLVertexArrayObject->GetVertexArrays();

    glDeleteVertexArrays(1, &vertexArrays);
    LOGI("WebGL2 createVertexArray deleteVertexArrays = %{public}u", vertexArrays);
    DeleteObject<WebGLVertexArrayObject>(env, vertexArrays);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::IsVertexArray(napi_env env, napi_value object)
{
    GLuint vertexArrayId = WebGLVertexArrayObject::DEFAULT_VERTEX_ARRAY_OBJECT;
    WebGLVertexArrayObject* webGLVertexArrayObject =
        WebGLObject::GetObjectInstance<WebGLVertexArrayObject>(env, object);
    if (webGLVertexArrayObject == nullptr) {
        return NVal::CreateBool(env, false).val_;
    }
    vertexArrayId = webGLVertexArrayObject->GetVertexArrays();
    GLboolean returnValue = glIsVertexArray(vertexArrayId);
    LOGI("WebGL2 isVertexArray %{public}u %{public}u", vertexArrayId, returnValue);
    return NVal::CreateBool(env, returnValue).val_;
}

napi_value WebGL2RenderingContextImpl::BindVertexArray(napi_env env, napi_value object)
{
    GLuint vertexArrayId = WebGLVertexArrayObject::DEFAULT_VERTEX_ARRAY_OBJECT;
    WebGLVertexArrayObject* webGLVertexArrayObject =
        WebGLObject::GetObjectInstance<WebGLVertexArrayObject>(env, object);
    if (webGLVertexArrayObject == nullptr) {
        return nullptr;
    }
    vertexArrayId = webGLVertexArrayObject->GetVertexArrays();
    glBindVertexArray(vertexArrayId);
    LOGI("WebGL2 bindVertexArray %{public}u ", vertexArrayId);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::FenceSync(napi_env env, GLenum condition, GLbitfield flags)
{
    LOGI("WebGL2 fenceSync condition  %{public}u flags %{public}u", condition, flags);
    WebGLSync* webGlSync = nullptr;
    napi_value objSync = WebGLSync::CreateObjectInstance(env, &webGlSync).val_;
    if (!objSync) {
        return NVal::CreateNull(env).val_;
    }
    GLsync returnValue = glFenceSync(condition, flags);
    webGlSync->SetSync(reinterpret_cast<int64_t>(returnValue));
    LOGI("WebGL2 fenceSync syncId %{public}p", returnValue);
    return objSync;
}

napi_value WebGL2RenderingContextImpl::IsSync(napi_env env, napi_value syncObj)
{
    WebGLSync* webGlSync = WebGLObject::GetObjectInstance<WebGLSync>(env, syncObj);
    if (webGlSync == nullptr) {
        return NVal::CreateBool(env, false).val_;
    }
    int64_t syncId = webGlSync->GetSync();
    GLboolean returnValue = glIsSync(reinterpret_cast<GLsync>(syncId));
    LOGI("WebGL2 isSync syncId = %{public}ld res %{public}u", syncId, returnValue);
    return NVal::CreateBool(env, static_cast<bool>(returnValue)).val_;
}

napi_value WebGL2RenderingContextImpl::DeleteSync(napi_env env, napi_value syncObj)
{
    WebGLSync* webGlSync = WebGLObject::GetObjectInstance<WebGLSync>(env, syncObj);
    if (webGlSync == nullptr) {
        return nullptr;
    }
    int64_t syncId = webGlSync->GetSync();

    glDeleteSync(reinterpret_cast<GLsync>(syncId));
    LOGI("WebGL2 deleteSync syncId %{public}ld ", syncId);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::ClientWaitSync(
    napi_env env, napi_value syncObj, GLbitfield flags, GLint64 timeout)
{
    WebGLSync* webGlSync = WebGLObject::GetObjectInstance<WebGLSync>(env, syncObj);
    if (webGlSync == nullptr) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateInt64(env, WebGL2RenderingContextBase::WAIT_FAILED).val_;
    }
    int64_t syncId = webGlSync->GetSync();
    if (timeout < WebGL2RenderingContextBase::TIMEOUT_IGNORED) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateInt64(env, WebGL2RenderingContextBase::WAIT_FAILED).val_;
    }

    GLuint64 timeout64 = static_cast<GLuint64>(timeout);
    GLint maxTimeout = 0;
    glGetIntegerv(WebGL2RenderingContextBase::MAX_CLIENT_WAIT_TIMEOUT_WEBGL, &maxTimeout);
    LOGI("WebGL2 clientWaitSync maxTimeout %{public}u ", maxTimeout);
    if (timeout64 > static_cast<GLuint64>(maxTimeout)) {
        timeout64 = maxTimeout;
    }

    GLenum returnValue = glClientWaitSync(reinterpret_cast<GLsync>(syncId), flags, timeout64);
    LOGI("WebGL2 clientWaitSync syncId = %{public}u result %{public}u", syncId, returnValue);
    return NVal::CreateInt64(env, static_cast<int64_t>(returnValue)).val_;
}

napi_value WebGL2RenderingContextImpl::WaitSync(napi_env env, napi_value syncObj, GLbitfield flags, GLint64 timeout)
{
    WebGLSync* webGlSync = WebGLObject::GetObjectInstance<WebGLSync>(env, syncObj);
    if (webGlSync == nullptr) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    int64_t syncId = webGlSync->GetSync();
    if (timeout < WebGL2RenderingContextBase::TIMEOUT_IGNORED) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    GLuint64 timeout64 = static_cast<GLuint64>(timeout);
    GLint maxTimeout = 0;
    glGetIntegerv(WebGL2RenderingContextBase::MAX_CLIENT_WAIT_TIMEOUT_WEBGL, &maxTimeout);
    LOGI("WebGL2 clientWaitSync maxTimeout %{public}u ", maxTimeout);
    if (timeout64 > static_cast<GLuint64>(maxTimeout)) {
        timeout64 = maxTimeout;
    }

    glWaitSync(reinterpret_cast<GLsync>(syncId), flags, timeout64);
    LOGI("WebGL2 waitSync syncId %{public}u", syncId);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::GetSyncParameter(napi_env env, napi_value syncObj, GLenum pname)
{
    WebGLSync* webGlSync = WebGLObject::GetObjectInstance<WebGLSync>(env, syncObj);
    if (webGlSync == nullptr) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateNull(env).val_;
    }
    int64_t syncId = webGlSync->GetSync();
    LOGI("WebGL2 getSyncParameter syncId %{public}u pname %{public}u ", syncId, pname);
    if (CheckInList(env, pname, { GL_OBJECT_TYPE, GL_SYNC_STATUS, GL_SYNC_CONDITION, GL_SYNC_FLAGS })) {
        GLint value = 0;
        GLsizei length = -1;
        glGetSynciv(reinterpret_cast<GLsync>(syncId), pname, 1, &length, &value);
        LOGI("WebGL2 getSyncParameter syncId = %{public}u value %{public}d ", syncId, value);
        return NVal::CreateInt64(env, value).val_;
    } else {
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
}

napi_value WebGL2RenderingContextImpl::CreateTransformFeedback(napi_env env)
{
    WebGLTransformFeedback* webGlTransformFeedback = nullptr;
    napi_value objTransformFeedback = WebGLTransformFeedback::CreateObjectInstance(env, &webGlTransformFeedback).val_;
    if (!objTransformFeedback) {
        return NVal::CreateNull(env).val_;
    }
    GLuint transformFeedbackId;
    glGenTransformFeedbacks(1, &transformFeedbackId);
    webGlTransformFeedback->SetTransformFeedback(transformFeedbackId);
    LOGI("WebGL2 createTransformFeedback transformFeedbackId %{public}u", transformFeedbackId);
    AddObject<WebGLTransformFeedback>(env, transformFeedbackId, objTransformFeedback);
    return objTransformFeedback;
}

napi_value WebGL2RenderingContextImpl::DeleteTransformFeedback(napi_env env, napi_value obj)
{
    uint32_t transformFeedbackId = WebGLTransformFeedback::DEFAULT_TRANSFORM_FEEDBACK;
    WebGLTransformFeedback* webGlTransformFeedback = WebGLObject::GetObjectInstance<WebGLTransformFeedback>(env, obj);
    if (webGlTransformFeedback == nullptr) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    transformFeedbackId = webGlTransformFeedback->GetTransformFeedback();
    DeleteObject<WebGLTransformFeedback>(env, transformFeedbackId);
    glDeleteTransformFeedbacks(1, &transformFeedbackId);
    LOGI("WebGL2 deleteTransformFeedback transformFeedbackId %{public}u", transformFeedbackId);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::IsTransformFeedback(napi_env env, napi_value obj)
{
    GLuint transformFeedbackId = WebGLTransformFeedback::DEFAULT_TRANSFORM_FEEDBACK;
    WebGLTransformFeedback* webGlTransformFeedback = WebGLObject::GetObjectInstance<WebGLTransformFeedback>(env, obj);
    if (webGlTransformFeedback == nullptr) {
        return NVal::CreateBool(env, false).val_;
    }
    transformFeedbackId = webGlTransformFeedback->GetTransformFeedback();
    GLboolean returnValue = glIsTransformFeedback(transformFeedbackId);
    LOGI("WebGL2 isTransformFeedback transformFeedbackId %{public}u %{public}u", transformFeedbackId, returnValue);
    return NVal::CreateBool(env, returnValue).val_;
}

napi_value WebGL2RenderingContextImpl::BindTransformFeedback(napi_env env, napi_value obj, GLenum target)
{
    GLuint transformFeedbackId = WebGLTransformFeedback::DEFAULT_TRANSFORM_FEEDBACK;
    WebGLTransformFeedback* webGlTransformFeedback = WebGLObject::GetObjectInstance<WebGLTransformFeedback>(env, obj);
    if (webGlTransformFeedback == nullptr) {
        SetError(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    transformFeedbackId = webGlTransformFeedback->GetTransformFeedback();
    if (target != GL_TRANSFORM_FEEDBACK) {
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    if (boundTransformFeedback && boundTransformFeedback != transformFeedbackId) {
        LOGE("WebGL2 bindTransformFeedback has been bound %{public}u", boundTransformFeedback);
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    boundTransformFeedback = transformFeedbackId;
    glBindTransformFeedback(target, transformFeedbackId);
    LOGI("WebGL2 bindTransformFeedback transformFeedbackId %{public}u target %{public}u", transformFeedbackId, target);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::BeginTransformFeedback(napi_env env, GLenum primitiveMode)
{
    LOGI("WebGL2 beginTransformFeedback primitiveMode %{public}u", primitiveMode);
    if (!CheckInList(env, primitiveMode,
            { WebGLRenderingContextBase::POINTS, WebGLRenderingContextBase::LINES,
                WebGLRenderingContextBase::TRIANGLES })) {
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    glBeginTransformFeedback(primitiveMode);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::EndTransformFeedback(napi_env env)
{
    LOGI("WebGL2 endTransformFeedback");
    glEndTransformFeedback();
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::GetTransformFeedbackVarying(napi_env env, napi_value programObj, GLuint index)
{
    GLuint programId = WebGLProgram::DEFAULT_PROGRAM_ID;
    WebGLProgram* webGLProgram = WebGLProgram::GetObjectInstance(env, programObj);
    if (webGLProgram == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    programId = webGLProgram->GetProgramId();

    WebGLActiveInfo* webGLActiveInfo = nullptr;
    napi_value objActiveInfo = WebGLActiveInfo::CreateObjectInstance(env, &webGLActiveInfo).val_;
    if (!objActiveInfo) {
        return NVal::CreateNull(env).val_;
    }

    GLsizei bufSize = WEBGL_ACTIVE_INFO_NAME_MAX_LENGTH;
    GLsizei length;
    GLsizei size;
    GLenum type;
    GLchar name[WEBGL_ACTIVE_INFO_NAME_MAX_LENGTH] = { 0 };
    LOGI("WebGL2 getTransformFeedbackVarying index = %{public}u", index);
    glGetTransformFeedbackVarying(programId, index, bufSize, &length, &size, &type, name);
    if (length > WEBGL_ACTIVE_INFO_NAME_MAX_LENGTH) {
        LOGE("WebGL: GetTransformFeedbackVarying: [error] bufSize exceed!");
    }
    webGLActiveInfo->SetActiveName(name);
    webGLActiveInfo->SetActiveSize(size);
    webGLActiveInfo->SetActiveType(type);
    return objActiveInfo;
}

napi_value WebGL2RenderingContextImpl::TexStorage3D(napi_env env, const TexStorageArg& arg)
{
    if (arg.target != GL_TEXTURE_3D && arg.target != GL_TEXTURE_2D_ARRAY) {
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    GLenum result = CheckTexStorage(env, arg);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        SetError(result);
        return nullptr;
    }
    glTexStorage3D(arg.target, arg.levels, arg.internalFormat, arg.width, arg.height, arg.depth);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::TexImage3D(napi_env env, const TexImageArg* info, napi_value source)
{
    TexImageArg* imgArg = const_cast<TexImageArg*>(info);
    imgArg->Dump("WebGL2 texImage3D");

    GLvoid* data = nullptr;
    WebGLImageSource imageSource;
    if (!NVal(env, source).IsNull()) {
        bool succ = imageSource.GenImageSource(env, source);
        if (!succ) {
            SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE, "texImage2D Image source invalid");
            return nullptr;
        }
        data = imageSource.GetImageSourceData(info->format, info->type, unpackFlipY_, unpackPremultiplyAlpha_);
        imgArg->width = imageSource.GetWidth();
        imgArg->height = imageSource.GetHeight();
    }

    glTexImage3D(imgArg->target, imgArg->level, imgArg->internalFormat, imgArg->width, imgArg->height, imgArg->depth,
        imgArg->border, imgArg->format, imgArg->type, data);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::TexImage3D(
    napi_env env, const TexImageArg* imgArg, napi_value dataObj, GLuint srcOffset)
{
    imgArg->Dump("WebGL2 texImage3D");

    WebGLReadBufferArg bufferData(env);
    GLvoid* data = nullptr;
    if (!NVal(env, dataObj).IsNull()) {
        napi_status status = bufferData.GenBufferData(dataObj);
        if (status != napi_ok) {
            SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE, "texImage2D get buffer fail");
            return nullptr;
        }
        data = reinterpret_cast<void*>(bufferData.GetBuffer());
    }
    glTexImage3D(imgArg->target, imgArg->level, imgArg->internalFormat, imgArg->width, imgArg->height, imgArg->depth,
        imgArg->border, imgArg->format, imgArg->type, data);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::TexImage3D(napi_env env, const TexImageArg* imgArg, GLintptr pboOffset)
{
    imgArg->Dump("WebGL2 texImage3D");

    glTexImage3D(imgArg->target, imgArg->level, imgArg->internalFormat, imgArg->width, imgArg->height, imgArg->depth,
        imgArg->border, imgArg->format, imgArg->type, reinterpret_cast<void*>(pboOffset));
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::TexSubImage3D(napi_env env, const TexSubImage3DArg* info, napi_value source)
{
    TexSubImage3DArg* imgArg = const_cast<TexSubImage3DArg*>(info);
    imgArg->Dump("WebGL2 texSubImage3D");

    GLvoid* data = nullptr;
    WebGLImageSource imageSource;
    if (!NVal(env, source).IsNull()) {
        bool succ = imageSource.GenImageSource(env, source);
        if (!succ) {
            SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE, "texImage2D Image source invalid");
            return nullptr;
        }
        data = imageSource.GetImageSourceData(info->format, info->type, unpackFlipY_, unpackPremultiplyAlpha_);
        imgArg->width = imageSource.GetWidth();
        imgArg->height = imageSource.GetHeight();
    }

    glTexSubImage3D(imgArg->target, imgArg->level, imgArg->xOffset, imgArg->yOffset, imgArg->zOffset, imgArg->width,
        imgArg->height, imgArg->depth, imgArg->format, imgArg->type, data);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::TexSubImage3D(
    napi_env env, const TexSubImage3DArg* info, napi_value dataObj, GLuint srcOffset)
{
    TexSubImage3DArg* imgArg = const_cast<TexSubImage3DArg*>(info);
    imgArg->Dump("WebGL2 texSubImage3D");

    WebGLReadBufferArg bufferData(env);
    GLvoid* data = nullptr;
    if (!NVal(env, dataObj).IsNull()) {
        napi_status status = bufferData.GenBufferData(dataObj);
        if (status != napi_ok) {
            SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE, "texImage2D get buffer fail");
            return nullptr;
        }
        data = reinterpret_cast<GLvoid*>(bufferData.GetBuffer());
    }
    glTexSubImage3D(imgArg->target, imgArg->level, imgArg->xOffset, imgArg->yOffset, imgArg->zOffset, imgArg->width,
        imgArg->height, imgArg->depth, imgArg->format, imgArg->type, data);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::TexSubImage3D(napi_env env, const TexSubImage3DArg* imgArg, GLintptr pboOffset)
{
    imgArg->Dump("WebGL2 texSubImage3D");

    glTexSubImage3D(imgArg->target, imgArg->level, imgArg->xOffset, imgArg->yOffset, imgArg->zOffset, imgArg->width,
        imgArg->height, imgArg->depth, imgArg->format, imgArg->type, reinterpret_cast<void*>(pboOffset));
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::CopyTexSubImage3D(napi_env env, const CopyTexSubImage3DArg* imgArg)
{
    imgArg->Dump("WebGL2 copyTexSubImage3D");
    glCopyTexSubImage3D(imgArg->target, imgArg->level, imgArg->xOffset, imgArg->yOffset, imgArg->zOffset, imgArg->x,
        imgArg->y, imgArg->width, imgArg->height);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::CompressedTexImage3D(
    napi_env env, const TexImageArg* imgArg, GLsizei imageSize, GLintptr offset)
{
    imgArg->Dump("WebGL2 compressedTexImage3D");
    glCompressedTexImage3D(imgArg->target, imgArg->level, imgArg->internalFormat, imgArg->width, imgArg->height,
        imgArg->depth, imgArg->border, imageSize, reinterpret_cast<void*>(offset));
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::CompressedTexImage3D(
    napi_env env, const TexImageArg* imgArg, napi_value data, GLuint srcOffset, GLuint srcLengthOverride)
{
    imgArg->Dump("WebGL2 compressedTexImage3D");
    glCompressedTexImage3D(imgArg->target, imgArg->level, imgArg->internalFormat, imgArg->width, imgArg->height,
        imgArg->depth, imgArg->border, 0, 0);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::CompressedTexSubImage3D(
    napi_env env, const TexSubImage3DArg* imgArg, GLsizei imageSize, GLintptr offset)
{
    imgArg->Dump("WebGL2 compressedTexSubImage3D");
    glCompressedTexSubImage3D(imgArg->target, imgArg->level, imgArg->xOffset, imgArg->yOffset, imgArg->zOffset,
        imgArg->width, imgArg->height, imgArg->depth, imgArg->format, imageSize, reinterpret_cast<void*>(offset));
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::CompressedTexSubImage3D(
    napi_env env, const TexSubImage3DArg* imgArg, napi_value data, GLuint srcOffset, GLuint srcLengthOverride)
{
    imgArg->Dump("WebGL2 compressedTexSubImage3D");
    glCompressedTexSubImage3D(imgArg->target, imgArg->level, imgArg->xOffset, imgArg->yOffset, imgArg->zOffset,
        imgArg->width, imgArg->height, imgArg->depth, imgArg->format, 0, 0);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::ClearBufferV(
    napi_env env, GLenum buffer, GLint drawBuffer, napi_value value, int64_t srcOffset, BufferDataType type)
{
    LOGI("WebGL2 clearBuffer buffer %{public}u %{public}d srcOffset %{public}u", buffer, drawBuffer, srcOffset);
    WebGLReadBufferArg bufferData(env);
    napi_status status = bufferData.GenBufferData(value, type);
    if (status != 0) {
        LOGE("WebGL2 clearBuffer failed to getbuffer data");
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    if (bufferData.GetBufferDataType() != type) {
        LOGE("WebGL2 clearBuffer invalid buffer data type %{public}d", bufferData.GetBufferDataType());
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }

    GLenum result = CheckClearBuffer(env, buffer, bufferData);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        LOGE("WebGL2 clearBuffer invalid clear buffer");
        SetError(result);
        return nullptr;
    }

    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    switch (type) {
        case BUFFER_DATA_FLOAT_32:
            glClearBufferfv(buffer, drawBuffer, reinterpret_cast<GLfloat*>(bufferData.GetBuffer() + srcOffset));
            break;
        case BUFFER_DATA_INT_32:
            glClearBufferiv(buffer, drawBuffer, reinterpret_cast<GLint*>(bufferData.GetBuffer() + srcOffset));
            break;
        case BUFFER_DATA_UINT_32:
            glClearBufferuiv(buffer, drawBuffer, reinterpret_cast<GLuint*>(bufferData.GetBuffer() + srcOffset));
            break;
        default:
            break;
    }
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::ClearBufferfi(
    napi_env env, GLenum buffer, GLint drawBuffer, GLfloat depth, GLint stencil)
{
    LOGE("WebGL2 clearBuffer buffer %{public}u %{public}d depth %{public}f %{public}d", buffer, drawBuffer, depth,
        stencil);
    if (buffer != WebGLRenderingContextBase::DEPTH_STENCIL) {
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    glClearBufferfi(buffer, drawBuffer, depth, stencil);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::GetIndexedParameter(napi_env env, GLenum target, GLuint index)
{
    LOGI("WebGL2 getIndexedParameter index = %{public}u", index);
    if (target == GL_TRANSFORM_FEEDBACK_BUFFER_BINDING || target == GL_UNIFORM_BUFFER_BINDING) {
        return GetObject<WebGLBuffer>(env, index);
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

napi_value WebGL2RenderingContextImpl::GetFragDataLocation(napi_env env, napi_value programObj, const std::string& name)
{
    GLuint program = WebGLProgram::DEFAULT_PROGRAM_ID;
    WebGLProgram* webGLProgram = WebGLProgram::GetObjectInstance(env, programObj);
    if (webGLProgram == nullptr) {
        return NVal::CreateInt64(env, -1).val_;
        ;
    }
    program = webGLProgram->GetProgramId();

    LOGI("WebGL WebGLRenderingContextBase::getFragDataLocation name %{public}s", name.c_str());
    GLint res = glGetFragDataLocation(program, const_cast<char*>(name.c_str()));
    LOGI("WebGL getFragDataLocation result %{public}d", res);
    return NVal::CreateInt64(env, res).val_;
}

napi_value WebGL2RenderingContextImpl::VertexAttribI4i(napi_env env, GLuint index, GLint* data)
{
    VertexAttribInfo *info = GetVertexAttribInfo(index);
    if (info == nullptr) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    glVertexAttribI4i(index, data[0], data[1], data[2], data[3]); // 2, 3 index for data
    info->type = BUFFER_DATA_INT_32;
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::VertexAttribI4ui(napi_env env, GLuint index, GLuint* data)
{
    VertexAttribInfo *info = GetVertexAttribInfo(index);
    if (info == nullptr) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    glVertexAttribI4ui(index, data[0], data[1], data[2], data[3]); // 2, 3 index for data
    info->type = BUFFER_DATA_UINT_32;
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::VertexAttribI4iv(napi_env env, GLuint index, napi_value data)
{
    VertexAttribInfo *info = GetVertexAttribInfo(index);
    if (info == nullptr) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }

    WebGLReadBufferArg bufferData(env);
    napi_status status = bufferData.GenBufferData(data, BUFFER_DATA_INT_32);
    if (status != 0) {
        LOGE("WebGL vertexAttribI4iv failed to getbuffer data");
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    if (bufferData.GetBufferDataType() != BUFFER_DATA_INT_32) {
        LOGE("WebGL vertexAttribI4iv invalid data type %{public}u", bufferData.GetBufferDataType());
        return nullptr;
    }
    glVertexAttribI4iv(index, reinterpret_cast<GLint*>(bufferData.GetBuffer()));
    info->type = BUFFER_DATA_INT_32;
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::VertexAttribI4uiv(napi_env env, GLuint index, napi_value data)
{
    VertexAttribInfo *info = GetVertexAttribInfo(index);
    if (info == nullptr) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    WebGLReadBufferArg bufferData(env);
    napi_status status = bufferData.GenBufferData(data, BUFFER_DATA_UINT_32);
    if (status != 0) {
        LOGE("WebGL vertexAttribI4uiv failed to getbuffer data");
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    if (bufferData.GetBufferDataType() != BUFFER_DATA_UINT_32) {
        LOGE("WebGL2 vertexAttribI4uiv invalid data type %{public}u", bufferData.GetBufferDataType());
        return nullptr;
    }
    glVertexAttribI4uiv(index, reinterpret_cast<const GLuint*>(bufferData.GetBuffer()));
    info->type = BUFFER_DATA_INT_32;
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::VertexAttribIPointer(napi_env env, const VertexAttribArg* vertexInfo)
{
    LOGI("WebGL vertexAttribPointer index %{public}u size %{public}u type %{public}u", vertexInfo->index,
        vertexInfo->size, vertexInfo->type);

    if (!CheckGLenum(env, vertexInfo->type,
            { GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_INT, GL_UNSIGNED_INT }, {})) {
        LOGE("WebGL vertexAttribPointer invalid type %{public}d", vertexInfo->type);
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
    }

    if (!CheckVertexAttribPointer(env, vertexInfo)) {
        return nullptr;
    }

    glVertexAttribIPointer(vertexInfo->index, vertexInfo->size, vertexInfo->type, vertexInfo->stride,
        reinterpret_cast<GLvoid*>(vertexInfo->offset));
    LOGI("WebGL vertexAttribPointer index %{public}u offset %{public}u", vertexInfo->index, vertexInfo->offset);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::VertexAttribDivisor(napi_env env, GLuint index, GLuint divisor)
{
    if (index >= GetMaxVertexAttribs()) {
        LOGE("WebGL2 vertexAttribDivisor invalid index %{public}d", index);
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    glVertexAttribDivisor(index, divisor);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::DrawBuffers(napi_env env, napi_value dataObj)
{
    WebGLReadBufferArg bufferData(env);
    napi_status status = bufferData.GenBufferData(dataObj, BUFFER_DATA_GLENUM);
    if (status != 0) {
        LOGE("WebGL glDrawBuffers failed to getbuffer data");
        return nullptr;
    }
    if (bufferData.GetBufferType() != BUFFER_ARRAY) {
        LOGE("WebGL glDrawBuffers invalid buffer type %d", bufferData.GetBufferType());
        return nullptr;
    }

    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    GLenum* data = reinterpret_cast<GLenum*>(bufferData.GetBuffer());
    GLsizei length = static_cast<GLsizei>(bufferData.GetBufferLength() / sizeof(GLenum));
    glDrawBuffers(length, data);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::DrawArraysInstanced(napi_env env,
    GLenum mode, GLint first, GLsizei count, GLsizei instanceCount)
{
    glDrawArraysInstanced(mode, first, count, instanceCount);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::DrawElementsInstanced(napi_env env, const DrawElementArg &arg, GLsizei instanceCount)
{
    glDrawElementsInstanced(arg.mode, arg.count, arg.type, reinterpret_cast<GLvoid*>(arg.offset), instanceCount);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::DrawRangeElements(napi_env env, const DrawElementArg &arg, GLuint start, GLuint end)
{
    glDrawRangeElements(arg.mode, start, end, arg.count, arg.type, reinterpret_cast<GLvoid*>(arg.offset));
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::CopyBufferSubData(
    napi_env env, GLenum targets[2], GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size)
{
    glCopyBufferSubData(targets[0], targets[1], readOffset, writeOffset, size);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::GetBufferSubData(
    napi_env env, GLenum target, GLintptr offset, napi_value data, const BufferExt &ext)
{
    WebGLReadBufferArg bufferData(env);
    napi_status status = bufferData.GenBufferData(data);
    if (status != 0) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }

    GLsizeiptr dstSize = (ext.length == 0) ? static_cast<GLsizeiptr>(bufferData.GetBufferLength()) :
        static_cast<GLsizeiptr>(ext.length * bufferData.GetBufferDataSize());
    GLuint dstOffset = static_cast<GLuint>(ext.dstOffset * bufferData.GetBufferDataSize());
    glBufferSubData(target, offset, dstSize, reinterpret_cast<void*>(bufferData.GetBuffer() + dstOffset));
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::BlitFrameBuffer(napi_env env, GLint data[8], GLbitfield mask, GLenum filter)
{
    glBlitFramebuffer(data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], mask, filter);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::FrameBufferTextureLayer(
    napi_env env, GLenum target, GLenum attachment, napi_value textureObj, const TextureLayerArg &layer)
{
    GLuint textureId = WebGLTexture::DEFAULT_TEXTURE;
    WebGLTexture* webGlTexture = WebGLTexture::GetObjectInstance(env, textureObj);
    if (webGlTexture == nullptr) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    textureId = webGlTexture->GetTexture();
    LOGI("WebGL frameBufferTextureLayer texture %{public}u", textureId);
    glFramebufferTextureLayer(target, attachment, textureId, layer.level, layer.layer);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::ReadBuffer(napi_env env, GLenum mode)
{
    LOGI("WebGL2 readBuffer mode %{public}u", mode);
    if (!(mode == WebGLRenderingContextBase::BACK || mode == WebGLRenderingContextBase::NONE ||
            mode >= WebGLRenderingContextBase::COLOR_ATTACHMENT0 ||
            mode <= (WebGLRenderingContextBase::COLOR_ATTACHMENT0 + GetMaxColorAttachments()))) {
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }

    WebGLFramebuffer* readFrameBuffer = GetBoundFrameBuffer(env, WebGL2RenderingContextBase::READ_FRAMEBUFFER);
    if (!readFrameBuffer) {
        if (mode != WebGLRenderingContextBase::BACK && mode != WebGLRenderingContextBase::NONE) {
            SetError(WebGLRenderingContextBase::INVALID_OPERATION);
            return nullptr;
        }
        // TODO
        // m_readBufferOfDefaultFramebuffer = mode;
        // translate GL_BACK to GL_COLOR_ATTACHMENT0, because the default
        // framebuffer for WebGL is not fb 0, it is an internal fbo.
        if (mode == WebGLRenderingContextBase::BACK)
            mode = WebGLRenderingContextBase::COLOR_ATTACHMENT0;
    } else {
        if (mode == WebGLRenderingContextBase::BACK) {
            SetError(WebGLRenderingContextBase::INVALID_OPERATION);
            return nullptr;
        }
    }
    glReadBuffer(mode);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::RenderBufferStorageMultiSample(
    napi_env env, const TexStorageArg& arg, GLsizei samples)
{
    LOGI("WebGL renderbufferStorageMultisample target %{public}u %{public}u %{public}d %{public}d",
        arg.target, arg.internalFormat, arg.width, arg.height);
    WebGLRenderbuffer* renderBuffer = CheckRenderBufferStorage(env, arg);
    if (renderBuffer == nullptr) {
        return nullptr;
    }

    if (CheckInList(env, arg.internalFormat, {
        WebGL2RenderingContextBase::R8UI,
        WebGL2RenderingContextBase::R8I,
        WebGL2RenderingContextBase::R16UI,
        WebGL2RenderingContextBase::R16I,
        WebGL2RenderingContextBase::R32UI,
        WebGL2RenderingContextBase::R32I,
        WebGL2RenderingContextBase::RG8UI,
        WebGL2RenderingContextBase::RG8I,
        WebGL2RenderingContextBase::RG16UI,
        WebGL2RenderingContextBase::RG16I,
        WebGL2RenderingContextBase::RG32UI,
        WebGL2RenderingContextBase::RG32I,
        WebGL2RenderingContextBase::RGBA8UI,
        WebGL2RenderingContextBase::RGBA8I,
        WebGL2RenderingContextBase::RGB10_A2UI,
        WebGL2RenderingContextBase::RGBA16UI,
        WebGL2RenderingContextBase::RGBA16I,
        WebGL2RenderingContextBase::RGBA32I,
        WebGL2RenderingContextBase::RGBA32UI,
    })) {
        if (samples > 0) {
            SetError(WebGLRenderingContextBase::INVALID_OPERATION);
            return nullptr;
        }
    } else if (CheckInList(env, arg.internalFormat, {
        WebGL2RenderingContextBase::R8,
        WebGL2RenderingContextBase::RG8,
        WebGL2RenderingContextBase::RGB8,
        WebGLRenderingContextBase::RGB565,
        WebGL2RenderingContextBase::RGBA8,
        WebGL2RenderingContextBase::SRGB8_ALPHA8,
        WebGLRenderingContextBase::RGB5_A1,
        WebGLRenderingContextBase::RGBA4,
        WebGL2RenderingContextBase::RGB10_A2,
        WebGLRenderingContextBase::DEPTH_COMPONENT16,
        WebGL2RenderingContextBase::DEPTH_COMPONENT24,
        WebGL2RenderingContextBase::DEPTH_COMPONENT32F,
        WebGL2RenderingContextBase::DEPTH24_STENCIL8,
        WebGL2RenderingContextBase::DEPTH32F_STENCIL8,
        WebGLRenderingContextBase::STENCIL_INDEX8
    })) {
        if (samples == 0) {
            glRenderbufferStorage(arg.target, arg.internalFormat, arg.width, arg.height);
        } else {
            glRenderbufferStorageMultisample(arg.target, samples, arg.internalFormat, arg.width, arg.height);
        }
    } else if (arg.internalFormat == GL_DEPTH_STENCIL) {
        if (samples > 0) {
            SetError(WebGLRenderingContextBase::INVALID_ENUM);
            return nullptr;
        }
        glRenderbufferStorage(arg.target, GL_DEPTH24_STENCIL8, arg.width, arg.height);
    } else {
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    renderBuffer->SetInternalFormat(arg.internalFormat);
    renderBuffer->SetSize(arg.width, arg.height);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::BindBufferBase(napi_env env, const BufferBaseArg &arg, napi_value bufferObj)
{
    GLuint bufferId = WebGLBuffer::DEFAULT_BUFFER;
    WebGLBuffer* webGlBuffer = WebGLBuffer::GetObjectInstance(env, bufferObj);
    if (webGlBuffer != nullptr) {
        bufferId = webGlBuffer->GetBufferId();
    }
    GLenum result = CheckBufferBindTarget(arg.target, arg.index, webGlBuffer);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        SetError(result);
        return nullptr;
    }
    LOGI("WebGL2 bindBufferBase target %{public}u %{public}u %{public}u", arg.target, arg.index, bufferId);
    glBindBufferBase(arg.target, arg.index, bufferId);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::BindBufferRange(
    napi_env env, const BufferBaseArg &arg, napi_value bufferObj, GLintptr offset, GLsizeiptr size)
{
    GLuint bufferId = WebGLBuffer::DEFAULT_BUFFER;
    WebGLBuffer* webGlBuffer = WebGLBuffer::GetObjectInstance(env, bufferObj);
    if (webGlBuffer != nullptr) {
        bufferId = webGlBuffer->GetBufferId();
    }
    GLenum result = CheckBufferBindTarget(arg.target, arg.index, webGlBuffer);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        SetError(result);
        return nullptr;
    }
    LOGI("WebGL2 bindBufferRange target %{public}u %{public}u %{public}u", arg.target, arg.index, bufferId);
    glBindBufferRange(arg.target, arg.index, bufferId, offset, size);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::TexStorage2D(napi_env env, const TexStorageArg& arg)
{
    LOGI("WebGL2 texStorage2D target %{public}u", arg.target);
    if (arg.target != GL_TEXTURE_2D && arg.target != GL_TEXTURE_CUBE_MAP) {
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    GLenum result = CheckTexStorage(env, arg);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        SetError(result);
        return nullptr;
    }
    glTexStorage2D(arg.target, arg.levels, arg.internalFormat, arg.width, arg.height);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::GetUniformBlockIndex(
    napi_env env, napi_value programObj, const std::string& uniformBlockName)
{
    GLuint programId = WebGLProgram::DEFAULT_PROGRAM_ID;
    WebGLProgram* webGLProgram = WebGLProgram::GetObjectInstance(env, programObj);
    if (webGLProgram == nullptr) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateInt64(env, -1).val_;
        ;
    }
    programId = webGLProgram->GetProgramId();
    LOGI("WebGL2 getUniformBlockIndex programId %{public}u", programId);

    GLuint returnValue = glGetUniformBlockIndex(programId, uniformBlockName.c_str());
    return NVal::CreateInt64(env, returnValue).val_;
}

napi_value WebGL2RenderingContextImpl::UniformBlockBinding(
    napi_env env, napi_value programObj, GLuint uniformBlockIndex, GLuint uniformBlockBinding)
{
    GLuint programId = WebGLProgram::DEFAULT_PROGRAM_ID;
    WebGLProgram* webGLProgram = WebGLProgram::GetObjectInstance(env, programObj);
    if (webGLProgram == nullptr) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    programId = webGLProgram->GetProgramId();
    LOGI("WebGL2 uniformBlockBinding programId %{public}u %{public}u %{public}u", programId, uniformBlockIndex,
        uniformBlockBinding);
    glUniformBlockBinding(programId, uniformBlockIndex, uniformBlockBinding);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::InvalidateFrameBuffer(napi_env env, GLenum target, napi_value data)
{
    LOGE("WebGL2 invalidateFramebuffer target %{public}u", target);
    WebGLReadBufferArg bufferData(env);
    napi_status status = bufferData.GenBufferData(data, BUFFER_DATA_GLENUM);
    if (status != 0) {
        LOGE("WebGL2 invalidateFramebuffer failed to getbuffer data");
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    if (bufferData.GetBufferDataType() != BUFFER_DATA_GLENUM) {
        LOGE("WebGL2 invalidateFramebuffer invalid data type %{public}u", bufferData.GetBufferDataType());
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    if (bufferData.GetBufferType() != BUFFER_ARRAY) {
        LOGE("WebGL2 invalidateFramebuffer invalid type %{public}u", bufferData.GetBufferType());
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    glInvalidateFramebuffer(target, static_cast<GLsizei>(bufferData.GetBufferLength() / sizeof(GLenum)),
        reinterpret_cast<GLenum*>(bufferData.GetBuffer()));
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::InvalidateSubFrameBuffer(
    napi_env env, GLenum target, napi_value data, const BufferPosition& position, const BufferSize& size)
{
    WebGLReadBufferArg bufferData(env);
    napi_status status = bufferData.GenBufferData(data, BUFFER_DATA_GLENUM);
    if (status != 0) {
        LOGE("WebGL2 invalidateFramebuffer failed to getbuffer data");
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    if (bufferData.GetBufferDataType() != BUFFER_DATA_GLENUM) {
        LOGE("WebGL2 invalidateFramebuffer invalid data type %{public}u", bufferData.GetBufferDataType());
        return nullptr;
    }
    if (bufferData.GetBufferType() != BUFFER_ARRAY) {
        LOGE("WebGL2 invalidateFramebuffer invalid type %{public}u", bufferData.GetBufferType());
        return nullptr;
    }

    glInvalidateSubFramebuffer(target, static_cast<GLsizei>(bufferData.GetBufferLength() / sizeof(GLenum)),
        reinterpret_cast<GLenum*>(bufferData.GetBuffer()), position.x, position.y, size.width, size.height);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::GetInternalFormatParameter(
    napi_env env, GLenum target, GLenum internalFormat, GLenum pname)
{
    if (target != WebGLRenderingContextBase::RENDERBUFFER) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateNull(env).val_;
    }
    LOGI("WebGL2 getInternalformatParameter target %{public}u %{public}u %{public}u", target, internalFormat, pname);
    WebGLWriteBufferArg writeBuffer(env);
    if (pname == GL_SAMPLES) {
        GLint length = -1;
        glGetInternalformativ(target, internalFormat, GL_NUM_SAMPLE_COUNTS, 1, &length);
        LOGI("WebGL2 getInternalformatParameter length %{public}u", length);
        if (length <= 0) {
            return NVal::CreateNull(env).val_;
        }
        GLint* params = reinterpret_cast<GLint*>(writeBuffer.AllocBuffer(length * sizeof(GLint)));
        if (params == nullptr) {
            return NVal::CreateNull(env).val_;
        }
        glGetInternalformativ(target, internalFormat, pname, length, params);
        return writeBuffer.ToExternalArray(BUFFER_DATA_INT_32);
    }
    return NVal::CreateNull(env).val_;
}

napi_value WebGL2RenderingContextImpl::TransformFeedbackVaryings(
    napi_env env, napi_value programObj, napi_value data, GLenum bufferMode)
{
    GLuint programId = WebGLProgram::DEFAULT_PROGRAM_ID;
    WebGLProgram* webGLProgram = WebGLProgram::GetObjectInstance(env, programObj);
    if (webGLProgram == nullptr) {
        return nullptr;
    }
    programId = webGLProgram->GetProgramId();

    std::vector<char*> list = {};
    bool succ = GetStringList(env, data, list);
    if (!succ) {
        FreeStringList(list);
        return nullptr;
    }
    glTransformFeedbackVaryings(programId, static_cast<GLsizei>(list.size()), list.data(), bufferMode);
    FreeStringList(list);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::GetUniformIndices(napi_env env, napi_value programObj, napi_value data)
{
    GLuint programId = WebGLProgram::DEFAULT_PROGRAM_ID;
    WebGLProgram* webGLProgram = WebGLProgram::GetObjectInstance(env, programObj);
    if (webGLProgram == nullptr) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateNull(env).val_;
    }
    programId = webGLProgram->GetProgramId();

    std::vector<char*> list = {};
    bool succ = GetStringList(env, data, list);
    if (!succ) {
        FreeStringList(list);
        return NVal::CreateNull(env).val_;
    }
    LOGI("WebGL2 getUniformIndices uniformCount %{public}u", list.size());

    WebGLWriteBufferArg writeBuffer(env);
    napi_value result = NVal::CreateNull(env).val_;
    GLuint* uniformIndices = reinterpret_cast<GLuint*>(writeBuffer.AllocBuffer(list.size() * sizeof(GLuint)));
    if (uniformIndices != nullptr) {
        glGetUniformIndices(programId, static_cast<GLsizei>(list.size()), const_cast<const GLchar**>(list.data()),
            static_cast<GLuint*>(uniformIndices));
        result = writeBuffer.ToNormalArray(BUFFER_DATA_UINT_32, BUFFER_DATA_UINT_32);
    }
    FreeStringList(list);
    return result;
}

napi_value WebGL2RenderingContextImpl::GetActiveUniforms(
    napi_env env, napi_value programObj, napi_value data, GLenum pname)
{
    GLuint programId = WebGLProgram::DEFAULT_PROGRAM_ID;
    WebGLProgram* webGLProgram = WebGLProgram::GetObjectInstance(env, programObj);
    if (webGLProgram == nullptr) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateNull(env).val_;
    }
    if (pname == GL_UNIFORM_NAME_LENGTH) {
        SetError(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }

    programId = webGLProgram->GetProgramId();
    LOGI("WebGL2 getActiveUniforms programId %{public}u pname %{public}u", programId, pname);
    WebGLReadBufferArg bufferData(env);
    napi_status status = bufferData.GenBufferData(data, BUFFER_DATA_UINT_32);
    if (status != 0) {
        LOGE("WebGL2 invalidateFramebuffer failed to getbuffer data");
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    GLsizei size = static_cast<GLsizei>(bufferData.GetBufferLength() / bufferData.GetBufferDataSize());

    WebGLWriteBufferArg writeBuffer(env);
    GLint* params = reinterpret_cast<GLint*>(writeBuffer.AllocBuffer(size * sizeof(GLint)));
    if (params == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    glGetActiveUniformsiv(
        programId, size, reinterpret_cast<GLuint*>(bufferData.GetBuffer()), pname, reinterpret_cast<GLint*>(params));
    if (pname == GL_UNIFORM_TYPE) {
        return writeBuffer.ToNormalArray(BUFFER_DATA_INT_32, BUFFER_DATA_BIGUINT_64);
    } else if (pname == GL_UNIFORM_SIZE) {
        return writeBuffer.ToNormalArray(BUFFER_DATA_INT_32, BUFFER_DATA_BIGUINT_64);
    } else if (pname == GL_UNIFORM_BLOCK_INDEX || pname == GL_UNIFORM_OFFSET || pname == GL_UNIFORM_ARRAY_STRIDE ||
               pname == GL_UNIFORM_MATRIX_STRIDE) {
        return writeBuffer.ToNormalArray(BUFFER_DATA_INT_32, BUFFER_DATA_INT_32);
    } else if (pname == GL_UNIFORM_IS_ROW_MAJOR) {
        return writeBuffer.ToNormalArray(BUFFER_DATA_INT_32, BUFFER_DATA_BOOLEAN);
    }
    return NVal::CreateNull(env).val_;
}

napi_value WebGL2RenderingContextImpl::GetActiveUniformBlockParameter(
    napi_env env, napi_value programObj, GLuint uniformBlockIndex, GLenum pname)
{
    GLuint programId = WebGLProgram::DEFAULT_PROGRAM_ID;
    WebGLProgram* webGLProgram = WebGLProgram::GetObjectInstance(env, programObj);
    if (webGLProgram == nullptr) {
        return NVal::CreateInt64(env, -1).val_;
    }
    programId = webGLProgram->GetProgramId();
    LOGI("WebGL2 getActiveUniformBlockParameter programId %{public}u %{public}u %{public}u", programId,
        uniformBlockIndex, pname);
    switch (pname) {
        case GL_UNIFORM_BLOCK_BINDING:
        case GL_UNIFORM_BLOCK_DATA_SIZE:
        case GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS: {
            GLint params;
            glGetActiveUniformBlockiv(programId, uniformBlockIndex, pname, &params);
            return NVal::CreateInt64(env, params).val_;
        }
        case GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER:
        case GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER: {
            GLint params;
            glGetActiveUniformBlockiv(programId, uniformBlockIndex, pname, &params);
            return NVal::CreateBool(env, params != GL_FALSE).val_;
        }
        case GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES: {
            WebGLWriteBufferArg writeBuffer(env);
            GLint uniformCount = 1;
            glGetActiveUniformBlockiv(programId, uniformBlockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &uniformCount);
            LOGI("WebGL2 getActiveUniformBlockParameter uniformCount %{public}d", uniformCount);
            GLint* params = reinterpret_cast<GLint*>(writeBuffer.AllocBuffer(uniformCount * sizeof(GLint)));
            if (params == nullptr) {
                return nullptr;
            }
            glGetActiveUniformBlockiv(programId, uniformBlockIndex, pname, params);
            return writeBuffer.ToNormalArray(BUFFER_DATA_INT_32, BUFFER_DATA_UINT_32);
        }
        default:
            SetError(WebGLRenderingContextBase::INVALID_ENUM);
            return nullptr;
    }
    return NVal::CreateInt64(env, -1).val_;
}

napi_value WebGL2RenderingContextImpl::GetActiveUniformBlockName(
    napi_env env, napi_value programObj, GLuint uniformBlockIndex)
{
    GLuint programId = WebGLProgram::DEFAULT_PROGRAM_ID;
    WebGLProgram* webGLProgram = WebGLProgram::GetObjectInstance(env, programObj);
    if (webGLProgram == nullptr) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateUTF8String(env, "").val_;
    }
    programId = webGLProgram->GetProgramId();

    GLint length = 0;
    GLsizei size = 0;
    glGetProgramiv(programId, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &length);
    if (length <= 0) {
        SetError(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateUTF8String(env, "").val_;
    }
    std::unique_ptr<char[]> buf = std::make_unique<char[]>(length + 1);
    if (buf == nullptr) {
        return nullptr;
    }
    glGetActiveUniformBlockName(programId, uniformBlockIndex, length, &size, buf.get());
    LOGI("WebGL2 getActiveUniformBlockName programId %{public}u name %{public}s size %{public}d", programId, buf.get(),
        size);
    string str(buf.get(), size);
    return NVal::CreateUTF8String(env, str).val_;
}

GLenum WebGL2RenderingContextImpl::CheckClearBuffer(napi_env env, GLenum buffer, const WebGLReadBufferArg& bufferData)
{
    size_t size = bufferData.GetBufferLength() / bufferData.GetBufferDataSize();
    switch (buffer) {
        case WebGL2RenderingContextBase::COLOR:
            /*
            case WebGLRenderingContextBase::FRONT:
            case WebGLRenderingContextBase::BACK:
            case WebGLRenderingContextBase::FRONT_AND_BACK:
            */
            if (size < 4) {
                return WebGLRenderingContextBase::INVALID_VALUE;
            }
            break;
        case WebGL2RenderingContextBase::DEPTH:
        case WebGL2RenderingContextBase::STENCIL:
        case WebGLRenderingContextBase::DEPTH_STENCIL:
            if (size < 1) {
                return WebGLRenderingContextBase::INVALID_VALUE;
            }
            break;
        default:
            return WebGLRenderingContextBase::INVALID_ENUM;
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

bool WebGL2RenderingContextImpl::CheckQueryTarget(napi_env env, GLenum target, uint32_t& index)
{
    switch (target) {
        case WebGL2RenderingContextBase::ANY_SAMPLES_PASSED:
        case WebGL2RenderingContextBase::ANY_SAMPLES_PASSED_CONSERVATIVE:
            index = BoundQueryType::ANY_SAMPLES_PASSED;
            break;
        case WebGL2RenderingContextBase::TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN:
            index = BoundQueryType::TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN;
            break;
        default:
            return false;
    }
    return true;
}

GLenum WebGL2RenderingContextImpl::CheckTexStorage(napi_env env, const TexStorageArg& arg)
{
    WebGLTexture* texture = GetBoundTexture(env, arg.target, false);
    if (!texture) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }

    // internalFormat check

    if (arg.width <= 0 || arg.height <= 0 || arg.depth <= 0) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }

    if (arg.levels <= 0) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }

    if (arg.target == GL_TEXTURE_3D) {
        if (arg.levels > log2(std::max(std::max(arg.width, arg.height), arg.depth)) + 1) {
            return WebGLRenderingContextBase::INVALID_OPERATION;
        }
    } else {
        if (arg.levels > log2(std::max(arg.width, arg.height)) + 1) {
            return WebGLRenderingContextBase::INVALID_OPERATION;
        }
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

GLenum WebGL2RenderingContextImpl::CheckBufferBindTarget(GLenum target, GLuint index, WebGLBuffer* buffer)
{
    if (target != GL_TRANSFORM_FEEDBACK_BUFFER && target != GL_UNIFORM_BUFFER) {
        return WebGLRenderingContextBase::INVALID_ENUM;
    }

    return WebGLRenderingContextBase::NO_ERROR;
}

bool WebGL2RenderingContextImpl::GetStringList(napi_env env, napi_value array, std::vector<char*>& list)
{
    bool succ = false;
    uint32_t count = 0;
    napi_status status = napi_get_array_length(env, array, &count);
    if (status != napi_ok) {
        return false;
    }
    uint32_t i;
    for (i = 0; i < count; i++) {
        napi_value element;
        status = napi_get_element(env, array, i, &element);
        if (status != napi_ok) {
            return false;
        }
        napi_value result;
        status = napi_coerce_to_string(env, element, &result);
        if (status != napi_ok) {
            return false;
        }
        unique_ptr<char[]> name;
        tie(succ, name, ignore) = NVal(env, result).ToUTF8String();
        if (!succ) {
            return false;
        }
        LOGI("WebGL2 GetStringList = %{public}s", name.get());
        list.emplace_back(name.get());
        name.release();
    }
    return true;
}

void WebGL2RenderingContextImpl::FreeStringList(std::vector<char*>& list)
{
    for (size_t i = 0; i < list.size(); i++) {
        if (list[i] != nullptr) {
            delete[] list[i];
        }
    }
    list.clear();
}

GLint WebGL2RenderingContextImpl::GetMaxColorAttachments()
{
    if (!maxColorAttachments) {
        glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &maxColorAttachments);
    }
    return maxColorAttachments;
}
} // namespace Impl
} // namespace Rosen
} // namespace OHOS
