#ifndef FRAMEWORKS_BOOTANIMATION_INCLUDE_LOG_H
#define FRAMEWORKS_BOOTANIMATION_INCLUDE_LOG_H

#include <hilog/log.h>

namespace OHOS {
static OHOS::HiviewDFX::HiLogLabel label_boot = { LOG_CORE, 0xD001402, "2DGraphics" };
inline constexpr const char *GetRawFileName(const char *path)
{
    char ch = '/';
    const char *start = path;
    // get the end of the string
    while (*start++) {
        ;
    }
    while (--start != path && *start != ch) {
        ;
    }
    return (*start == ch) ? ++start : path;
}
#define LXY_LOGD(fmt, ...) LOGPRINT(::OHOS::HiviewDFX::HiLog::Debug, fmt, ##__VA_ARGS__)
#define LXY_LOGI(fmt, ...) LOGPRINT(::OHOS::HiviewDFX::HiLog::Info, fmt, ##__VA_ARGS__)
#define LXY_LOGW(fmt, ...) LOGPRINT(::OHOS::HiviewDFX::HiLog::Warn, fmt, ##__VA_ARGS__)
#define LXY_LOGE(fmt, ...) LOGPRINT(::OHOS::HiviewDFX::HiLog::Error, fmt, ##__VA_ARGS__)
#define LOGPRINT(func, fmt, ...) func(label_boot, \
    "[%{public}s-(%{public}s:%{public}d)] lxy " fmt, __func__, GetRawFileName(__FILE__), __LINE__, ##__VA_ARGS__)
} // namespace OHOS
#endif // FRAMEWORKS_BOOTANIMATION_INCLUDE_LOG_H
