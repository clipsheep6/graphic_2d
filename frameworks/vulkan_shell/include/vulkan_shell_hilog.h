#ifndef VULKAN_SHELL_H
#define VULKAN_SHELL_H
#include <hilog/log.h>

#define LOGD(fmt, ...)               \
    ::OHOS::HiviewDFX::HiLog::Debug( \
        ::OHOS::HiviewDFX::HiLogLabel { LOG_CORE, 0xD001400, "VulkanShell" }, "%{public}s: " fmt, __func__, ##__VA_ARGS__)
#define LOGI(fmt, ...)              \
    ::OHOS::HiviewDFX::HiLog::Info( \
        ::OHOS::HiviewDFX::HiLogLabel { LOG_CORE, 0xD001400, "VulkanShell" }, "%{public}s: " fmt, __func__, ##__VA_ARGS__)
#define LOGW(fmt, ...)              \
    ::OHOS::HiviewDFX::HiLog::Warn( \
        ::OHOS::HiviewDFX::HiLogLabel { LOG_CORE, 0xD001400, "VulkanShell" }, "%{public}s: " fmt, __func__, ##__VA_ARGS__)
#define LOGE(fmt, ...)               \
    ::OHOS::HiviewDFX::HiLog::Error( \
        ::OHOS::HiviewDFX::HiLogLabel { LOG_CORE, 0xD001400, "VulkanShell" }, "%{public}s: " fmt, __func__, ##__VA_ARGS__)

#endif