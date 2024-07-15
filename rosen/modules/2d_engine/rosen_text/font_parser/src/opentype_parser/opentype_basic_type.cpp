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

#include <rosen_text/opentype_parser/opentype_basic_type.h>

namespace OHOS {
namespace Rosen {
namespace TextEngine {
namespace OpenTypeBasicType {
#define MAX_NUM 65536.0
#define ZERO 0
#define FIRST 0
#define SECOND 1
#define THIRD 2
#define FORTH 3
#define FIFTH 4
#define ARRAY_END 5
#define ONE_BYTE 8
#define THREE_BYTES 24

constexpr union {
    unsigned int i;
    unsigned char big;
} G_ENDIAN{0x01};

const std::string Tag::Get() const
{
    // 5 is the size of open type table tags length
    int size = ARRAY_END;
    char tagsWithZero[size];
    tagsWithZero[FIRST] = tags[FIRST];  // numbers means array subscripts
    tagsWithZero[SECOND] = tags[SECOND];
    tagsWithZero[THIRD] = tags[THIRD];
    tagsWithZero[FORTH] = tags[FORTH];
    tagsWithZero[FIFTH] = ZERO;
    return tagsWithZero;
}

int16_t Int16::Get() const
{
    if (G_ENDIAN.big != '\0') {
        return ((data & 0x0000ff00) >> ONE_BYTE) | ((data & 0x000000ff) << ONE_BYTE);
    } else {
        return data;
    }
}

uint16_t Uint16::Get() const
{
    if (G_ENDIAN.big != '\0') {
        return ((static_cast<uint32_t>(data) & 0x0000ff00) >> ONE_BYTE) |
               ((static_cast<uint32_t>(data) & 0x000000ff) << ONE_BYTE);
    } else {
        return data;
    }
}

int32_t Int32::Get() const
{
    if (G_ENDIAN.big != '\0') {
        return ((static_cast<uint32_t>(data) & 0xff000000) >> THREE_BYTES) |
            ((static_cast<uint32_t>(data) & 0x00ff0000) >> ONE_BYTE) |
            ((static_cast<uint32_t>(data) & 0x0000ff00) << ONE_BYTE) |
            ((static_cast<uint32_t>(data) & 0x000000ff) << THREE_BYTES);
    } else {
        return data;
    }
}

uint32_t Uint32::Get() const
{
    if (G_ENDIAN.big != '\0') {
        return ((data & 0xff000000) >> THREE_BYTES) | ((data & 0x00ff0000) >> ONE_BYTE) |
            ((data & 0x0000ff00) << ONE_BYTE) | ((data & 0x000000ff) << THREE_BYTES);
    } else {
        return data;
    }
}

float Fixed::Get() const
{
    return data.Get() / MAX_NUM;
}
} // namespace OpenTypeBasicType
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
