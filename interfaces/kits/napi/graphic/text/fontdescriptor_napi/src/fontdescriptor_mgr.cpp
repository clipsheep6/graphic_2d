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


#include "fontdescriptor_mgr.h"
#include "js_fontdescriptor.h"

#include <mutex>

namespace OHOS::Rosen {
namespace {
std::mutex g_mtx;
} // namespace

FontDescriptorMgr* FontDescriptorMgr::GetInstance()
{
    if (instance_ == nullptr) {
        std::unique_lock<std::mutex> lock(g_mtx);
        if (instance_ == nullptr) {
            instance_ = new (std::nothrow) FontDescriptorMgr();
        }
    }
    return instance_;
}

FontDescriptorMgr::FontDescriptorMgr()
{
    ParserAllFontSource();
}

FontDescriptorMgr::~FontDescriptorMgr() {}

napi_value FontDescriptorMgr::CreateCallbackInfo(napi_env env, napi_value handle, sptr<CallbackInfo> cb)
{
    if (cb == nullptr) {
        return nullptr;
    }
    cb->env = env;
    napi_value promise = nullptr;
    if (handle == nullptr) {
        TEXT_ERROR_CHECK(napi_create_promise(env, &cb->deferred, &promise) == napi_ok, return nullptr,
            "napi_create_promise failed");
    } else {
        TEXT_ERROR_CHECK(napi_create_reference(env, handle, 1, &cb->ref) == napi_ok, return nullptr,
            "napi_create_reference failed");
    }
    return promise;
}

void FontDescriptorMgr::ParserAllFontSource()
{
    parserComplete_ = taskPool_.PushTask(std::bind(&ParserFontSource::ParserFontDirectory, &parser_, "/system/fonts"));
}

void FontDescriptorMgr::ClearFontFileCache()
{
    taskPool_.PushTask(std::bind(&ParserFontSource::ClearFontFileCache, &parser_));
}

void FontDescriptorMgr::MatchFontDescriptorTask(sptr<CallbackInfo> cb)
{
    std::set<FontDescriptorPtr> result;
    MatchingFontDescriptors(cb->uData.matchDesc, result);
    JsFontDescriptor::EmitMatchFontDescriptorsResult(cb, result);
}

napi_value FontDescriptorMgr::MatchingFontDescriptors(napi_env env, FontDescriptorPtr desc, napi_value handle)
{
    sptr<CallbackInfo> cb = new (std::nothrow) CallbackInfo();
    cb->uData.matchDesc = desc;
    napi_value ret = CreateCallbackInfo(env, handle, cb);
    auto callback = std::bind(&FontDescriptorMgr::MatchFontDescriptorTask, this, cb);
    taskPool_.PushTask(callback);
    return ret;
}

void FontDescriptorMgr::MatchingFontDescriptors(FontDescriptorPtr desc, std::set<FontDescriptorPtr>& descs)
{
    if (parserComplete_.valid()) {
        parserComplete_.get();
    }
    parser_.MatchFromFontDescriptor(desc, descs);
}
} // namespace OHOS::Rosen