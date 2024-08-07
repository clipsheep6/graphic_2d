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
#include "parser_font_source.h"

namespace OHOS::Rosen {
using FontDescriptorPtr = std::shared_ptr<TextEngine::FontParser::FontDescriptor>;

class FontDescriptorMgr {
public:
    static FontDescriptorMgr* GetInstance();
    virtual ~FontDescriptorMgr();
    DISALLOW_COPY_AND_MOVE(FontDescriptorMgr);

    void ParserAllFontSource();
    void MatchingFontDescriptors(FontDescriptorPtr desc, std::set<FontDescriptorPtr>& descs);
    void ClearFontFileCache();

private:
    FontDescriptorMgr();

private:
    static inline FontDescriptorMgr* instance_{nullptr};
    ParserFontSource parser_;
};
} // namespace OHOS::Rosen
#define FontDescriptorMgrIns OHOS::Rosen::FontDescriptorMgr::GetInstance()
#endif // OHOS_ROSEN_FONTDESCRIPTOR_MGR_H