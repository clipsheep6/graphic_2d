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

#ifndef ROSEN_MODULES_TEXGINE_SRC_MOCK_H
#define ROSEN_MODULES_TEXGINE_SRC_MOCK_H

#include <fstream>
#include <system_error>

namespace Texgine {
class MockIFStream : public std::ifstream {
public:
    explicit MockIFStream(const std::string &filename,
                std::ios_base::openmode mode = ios_base::in);

    bool StdFilesystemIsOpen() const;

    std::istream &StdFilesystemSeekg(pos_type pos);

    std::istream &StdFilesystemSeekg(off_type off, std::ios_base::seekdir dir);

    pos_type StdFilesystemTellg();

    std::istream &StdFilesystemRead(char_type *s, std::streamsize count);
};

bool StdFilesystemExists(const std::string &p, std::error_code &ec);
} // namespace Texgine

#endif // ROSEN_MODULES_TEXGINE_SRC_MOCK_H
