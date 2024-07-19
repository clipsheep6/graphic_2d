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

#ifndef OHOS_ROSEN_FONTDESCRIPTOR_MGR_H
#define OHOS_ROSEN_FONTDESCRIPTOR_MGR_H

#include <nocopyable.h>

#include "font_parser.h"
#include "fontdescriptor_util.h"
#include "parser_font_source.h"
#include "thread_pool.h"

namespace OHOS::Rosen {
using FontDescriptorPtr = std::shared_ptr<TextEngine::FontParser::FontDescriptor>;

class FontDescriptorMgr {
public:
    static FontDescriptorMgr* GetInstance();
    virtual ~FontDescriptorMgr();
    DISALLOW_COPY_AND_MOVE(FontDescriptorMgr);

    void ParserAllFontSource();
    napi_value MatchingFontDescriptors(napi_env env, FontDescriptorPtr desc, napi_value handle = nullptr);
    void MatchingFontDescriptors(FontDescriptorPtr desc, std::set<FontDescriptorPtr>& descs);
    void ClearFontFileCache();

private:
    FontDescriptorMgr();
    napi_value CreateCallbackInfo(napi_env env, napi_value handle, sptr<CallbackInfo> cb);
    void MatchFontDescriptorTask(sptr<CallbackInfo> cb);

private:
    static inline FontDescriptorMgr* instance_{nullptr};
    ParserFontSource parser_;
    ThreadPool taskPool_ {1};
    std::future<void> parserComplete_;
};
} // namespace OHOS::Rosen
#define FontDescriptorMgrIns OHOS::Rosen::FontDescriptorMgr::GetInstance()
#endif // OHOS_ROSEN_FONTDESCRIPTOR_MGR_H