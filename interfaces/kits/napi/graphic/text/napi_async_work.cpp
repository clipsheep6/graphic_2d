/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "napi_async_work.h"
#include "js_text_utils.h"

namespace OHOS::Rosen {
ContextBase::~ContextBase()
{
    ROSEN_LOGE("no memory leak after callback or promise[resolved/rejected]");
    if (env == nullptr) {
        return;
    }
    if (work != nullptr) {
        napi_delete_async_work(env, work);
    }
    if (callbackRef != nullptr) {
        napi_delete_reference(env, callbackRef);
    }
    napi_delete_reference(env, selfRef);
    env = nullptr;
    callbackRef = nullptr;
    selfRef = nullptr;
}

void ContextBase::GetCbInfo(napi_env envi, napi_callback_info info, NapiCbInfoParser parser, bool sync)
{
    env = envi;
    size_t argc = ARGC_MAX;
    napi_value argv[ARGC_MAX] = {nullptr};
    status = napi_get_cb_info(env, info, &argc, argv, &self, nullptr);
    if (status != napi_ok) {
        AssignmentContextBaseArguments(*this, "napi_get_cb_info failed!", DrawingErrorCode::ERROR, false);
        return;
    }
    if (argc > ARGC_MAX) {
        AssignmentContextBaseArguments(*this, "too many arguments!", DrawingErrorCode::ERROR_INVALID_PARAM);
        return;
    }
    if (self == nullptr) {
        AssignmentContextBaseArguments(*this, "no JavaScript this argument!", DrawingErrorCode::ERROR_INVALID_PARAM);
        return;
    }

    napi_create_reference(env, self, 1, &selfRef);

    status = napi_unwrap(env, self, &native);
    if (status != napi_ok) {
        AssignmentContextBaseArguments(*this, "self unwrap failed!", DrawingErrorCode::ERROR, false);
        return;
    }

    if (!sync && (argc > 0)) {
        // get the last arguments :: <callback>
        size_t index = argc - 1;
        napi_valuetype type = napi_undefined;
        napi_status tyst = napi_typeof(env, argv[index], &type);
        if ((tyst == napi_ok) && (type == napi_function)) {
            status = napi_create_reference(env, argv[index], 1, &callbackRef);
            if (status != napi_ok) {
                AssignmentContextBaseArguments(*this, "ref callback failed!", DrawingErrorCode::ERROR, false);
                return;
            }
            argc = index;
            ROSEN_LOGD("async callback, no promise");
        } else {
            ROSEN_LOGD("no callback, async promise");
        }
    }

    if (parser) {
        parser(argc, argv);
    } else {
        if (!(argc == 0)) {
            AssignmentContextBaseArguments(*this, "required no arguments!", DrawingErrorCode::ERROR_INVALID_PARAM);
        }
    }
}

napi_value NapiAsyncWork::Enqueue(napi_env env, std::shared_ptr<ContextBase> contextBase, const std::string& name,
                                  NapiAsyncExecute execute, NapiAsyncComplete complete)
{
    if (contextBase == nullptr) {
        ROSEN_LOGE("NapiAsyncWork::Enqueue contextBase nullptr!");
        return nullptr;
    }
    contextBase->execute = std::move(execute);
    contextBase->complete = std::move(complete);
    napi_value promise = nullptr;
    if (contextBase->callbackRef == nullptr) {
        napi_create_promise(contextBase->env, &contextBase->deferred, &promise);
    } else {
        napi_get_undefined(contextBase->env, &promise);
    }

    napi_value resource = nullptr;
    napi_create_string_utf8(contextBase->env, name.c_str(), NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(
        contextBase->env, nullptr, resource,
        [](napi_env env, void* data) {
            if (data == nullptr) {
                ROSEN_LOGE("NapiAsyncWork::Enqueue napi_async_execute_callback nullptr! ");
                return;
            }
            auto contextBase = reinterpret_cast<ContextBase*>(data);
            if (contextBase && contextBase->execute && contextBase->status == napi_ok) {
                contextBase->execute();
            }
        },
        [](napi_env env, napi_status status, void* data) {
            if (data == nullptr) {
                ROSEN_LOGE("NapiAsyncWork::Enqueue napi_async_complete_callback nullptr! ");
                return;
            }
            auto contextBase = reinterpret_cast<ContextBase*>(data);
            if (!contextBase) {
                ROSEN_LOGE("NapiAsyncWork::Enqueue contextBase nullptr! ");
                return;
            }

            if ((status != napi_ok) && contextBase && (contextBase->status == napi_ok)) {
                contextBase->status = status;
            }
            if (contextBase && (contextBase->complete) && (status == napi_ok) && (contextBase->status == napi_ok)) {
                contextBase->complete(contextBase->output);
            }
            GenerateOutput(*contextBase);
        },
        reinterpret_cast<void*>(contextBase.get()), &contextBase->work);
    napi_queue_async_work_with_qos(contextBase->env, contextBase->work, napi_qos_user_initiated);
    contextBase->hold = contextBase; // save crossing-thread contextBase.
    return promise;
}

void NapiAsyncWork::GenerateOutput(ContextBase& contextBase)
{
    napi_value result[RESULT_ALL] = {nullptr};
    if (contextBase.status == napi_ok) {
        napi_get_undefined(contextBase.env, &result[RESULT_ERROR]);
        if (contextBase.output == nullptr) {
            napi_get_undefined(contextBase.env, &contextBase.output);
        }
        result[RESULT_DATA] = contextBase.output;
    } else {
        napi_value message = nullptr;
        napi_value code = nullptr;
        napi_create_string_utf8(contextBase.env, contextBase.errMessage.c_str(), NAPI_AUTO_LENGTH, &message);
        napi_create_error(contextBase.env, nullptr, message, &result[RESULT_ERROR]);
        napi_create_int32(contextBase.env, contextBase.errCode, &code);
        napi_set_named_property(contextBase.env, result[RESULT_ERROR], "code", code);
        napi_get_undefined(contextBase.env, &result[RESULT_DATA]);
    }
    if (contextBase.deferred != nullptr) {
        if (contextBase.status == napi_ok) {
            ROSEN_LOGD("deferred promise resolved");
            napi_resolve_deferred(contextBase.env, contextBase.deferred, result[RESULT_DATA]);
        } else {
            ROSEN_LOGD("deferred promise rejected");
            napi_reject_deferred(contextBase.env, contextBase.deferred, result[RESULT_ERROR]);
        }
    } else {
        napi_value callback = nullptr;
        napi_get_reference_value(contextBase.env, contextBase.callbackRef, &callback);
        napi_value callbackResult = nullptr;
        ROSEN_LOGD("call callback function");
        napi_call_function(contextBase.env, nullptr, callback, RESULT_ALL, result, &callbackResult);
    }

    contextBase.hold.reset(); // release contextBase.
}
}