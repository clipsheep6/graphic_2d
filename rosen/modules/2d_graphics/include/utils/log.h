/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef DRAWING_LOG_H
#define DRAWING_LOG_H

#include <chrono>
#include <cstdint>
#include <ctime>
#include <functional>
#ifdef OHOS_PLATFORM
#include <hilog/log.h>
#endif

namespace OHOS {
namespace Rosen {

#ifndef LOGD
#ifdef OHOS_PLATFORM

#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD001402

#undef LOG_TAG
#define LOG_TAG "2DGraphics"

class LogUtils {
public:
    static inline bool CanPrint()
    {
        uint64_t curTime = GetCurrentTime();
        if (curTime < baseTime || (curTime - baseTime) >= LOG_LIMIT_DURATION_IN_SECOND) {
            baseTime = curTime;
            currentLogNum = 0;
        }
        return currentLogNum++ < MAX_LOG_NUM_IN_DURATION;
    }

private:
    static inline uint64_t GetCurrentTime()
    {
        struct timespec current = {0};
        int ret = clock_gettime(CLOCK_REALTIME, &current);
        if (ret != 0) {
            return 0;
        }
        return (uint64_t) current.tv_sec;
    }

    static constexpr uint8_t LOG_LIMIT_DURATION_IN_SECOND = 60;
    static constexpr uint8_t MAX_LOG_NUM_IN_DURATION = 6;
    static uint8_t currentLogNum;
    static uint64_t baseTime;
};

#define HILOG_WITH_CHECK(func, fmt, ...)                                    \
    do {                                                                    \
        if (LogUtils::CanPrint()) {                                         \
            func(LOG_CORE, "%{public}s: " fmt, __func__, ##__VA_ARGS__);    \
        }                                                                   \
    } while (0)

#define LOGD(fmt, ...)              \
    HILOG_WITH_CHECK(HILOG_DEBUG, fmt, ##__VA_ARGS__)
#define LOGI(fmt, ...)              \
    HILOG_WITH_CHECK(HILOG_INFO, fmt, ##__VA_ARGS__)
#define LOGW(fmt, ...)              \
    HILOG_WITH_CHECK(HILOG_WARN, fmt, ##__VA_ARGS__)
#define LOGE(fmt, ...)              \
    HILOG_WITH_CHECK(HILOG_ERROR, fmt, ##__VA_ARGS__)
#else
#define LOGD(fmt, ...)
#define LOGI(fmt, ...)
#define LOGW(fmt, ...)
#define LOGE(fmt, ...)
#endif
#endif
} // namespace Rosen
} // namespace OHOS
#endif
