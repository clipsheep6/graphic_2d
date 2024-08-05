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

void FontDescriptorMgr::ParserAllFontSource()
{
    parser_.ParserSystemFonts();
}

void FontDescriptorMgr::ClearFontFileCache()
{
    parser_.ClearFontFileCache();
}

void FontDescriptorMgr::MatchingFontDescriptors(FontDescriptorPtr desc, std::set<FontDescriptorPtr>& descs)
{
    parser_.MatchFromFontDescriptor(desc, descs);
}
} // namespace OHOS::Rosen