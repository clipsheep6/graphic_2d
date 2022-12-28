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

#ifndef RENDER_SERVICE_BASE_COMMON_rs_utils_H
#define RENDER_SERVICE_BASE_COMMON_rs_utils_H

#include <sys/types.h>
#include <unistd.h>

namespace OHOS {
namespace Rosen {

template <class _Container, class _Predicate>
inline typename _Container::size_type
erase_if_container(_Container& __c, _Predicate __pred)
{
    typename _Container::size_type __old_size = __c.size();

    const typename _Container::iterator __last = __c.end();
    for (typename _Container::iterator __iter = __c.begin(); __iter != __last;) {
      if (__pred(*__iter))
        __iter = __c.erase(__iter);
      else
        ++__iter;
    }

    return __old_size - __c.size();
}

pid_t gettid();
}
}
#endif // RENDER_SERVICE_BASE_COMMON_rs_utils_H