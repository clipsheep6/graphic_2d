/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "texgine/dynamic_file_font_provider.h"

#include "mock.h"
#include "texgine/utils/exlog.h"
#include "typeface.h"

namespace Texgine {
std::shared_ptr<DynamicFileFontProvider> DynamicFileFontProvider::Create() noexcept(true)
{
    return std::shared_ptr<DynamicFileFontProvider>(new DynamicFileFontProvider());
}

int DynamicFileFontProvider::LoadFont(const std::string &familyName, const std::string &path) noexcept(true)
{
    LOG2EX(DEBUG) << "loading font: '" << path << "'";
    std::error_code ec;
    auto ret = StdFilesystemExists(path, ec);
    if (ec) {
        LOG2EX(ERROR) << "open file failed: " << ec.message();
        return 1;
    }

    if (!ret) {
        LOG2EX(ERROR) << "file is not exists";
        return 1;
    }

    MockIFStream ifs(path);
    if (!ifs.StdFilesystemIsOpen()) {
        LOG2EX(ERROR) << "file open failed";
        return 1;
    }

    ifs.StdFilesystemSeekg(0, ifs.end);
    if (!ifs.good()) {
        LOG2EX(ERROR) << "seekg(0, ifs.end) failed!";
        return 1;
    }

    auto size = ifs.StdFilesystemTellg();
    if (ifs.fail()) {
        LOG2EX(ERROR) << "tellg failed!";
        return 1;
    }

    ifs.StdFilesystemSeekg(ifs.beg);
    if (!ifs.good()) {
        LOG2EX(ERROR) << "seekg(ifs.beg) failed!";
        return 1;
    }

    std::unique_ptr<char[]> buf = std::make_unique<char[]>(size);
    ifs.StdFilesystemRead(buf.get(), size);
    if (!ifs.good()) {
        LOG2EX(ERROR) << "read failed!";
        return 1;
    }

    return DynamicFontProvider::LoadFont(familyName, buf.get(), size);
}
} // namespace Texgine
