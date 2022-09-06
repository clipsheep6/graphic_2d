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

#ifndef RENDER_SERVICE_CHANGE_CORE_UTIL_H
#define RENDER_SERVICE_CHANGE_CORE_UTIL_H

#include <fcntl.h>
#include <pthread.h>
#include <cstdio>
#include <sys/resource.h>
#include <sys/ioctl.h>
#include <thread>
#include <unistd.h>
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
class RSChangeCoreUtil {
    enum {
        GET_SCHED_STAT = 1,
        SET_TASK_UTIL,
        GET_IPA_STAT,
        GET_DDR_FLUX,
        GET_RELATED_TID,
        GET_DEV_FREQ,
        GET_THERMAL_CDEV_POWER,
        SET_FRAME_RATE,
        SET_FRAME_MARGIN,
        SET_FRAME_STATUS,
        SET_TASK_RTG,
        SET_RTG_CPUS,
        SET_RTG_FREQ,
        SET_RTG_FREQ_UPDATE_INTERVAL,
        SET_RTG_UTIL_INVALID_INTERVAL,
        GET_GPU_FENCE,
        // render related
        INIT_RENDER_PID,
        GET_RENDER_RT,
        STOP_RENDER_RT,
        GET_RENDER_HT,
        DESTROY_RENDER_RT,
        SET_RTG_LOAD_MODE,
        SET_RTG_ED_PARAMS,
        GET_DEV_CAP,
        SET_VIP_PRIO,
        SET_FAVOR_SMALL_CAP,
        SET_TASK_RTG_MIN_FREQ,
        SET_TASK_MIN_UTIL,
        GET_GPU_BUFFER_SIZE,
        SET_LB_POLICY,
        RESET_LB_POLICY,
        GET_LB_LITE_INFO,
        ENABLE_GPU_LB,
        GET_CPU_BUSY_TIME,
        SET_TASK_MAX_UTIL,
        GET_TASK_YIELD_TIME,
        SET_TASK_L3C_PART,
        PERF_CTRL_MAX_NR,
    };

    #define PERF_CTRL_MAGIC 'x'

    struct task_config {
        pid_t pid;
        unsigned int value;
    };
    #define PERF_CTRL_SET_TASK_MIN_UTIL \
        _IOW(PERF_CTRL_MAGIC, SET_TASK_MIN_UTIL, struct task_config)
    mutable std::mutex mutex_;

public:
    static RSChangeCoreUtil& Instance()
    {
        static RSChangeCoreUtil coreUtil;
        return coreUtil;
    }

    void set_core_level(int coreLevel)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        task_config data;
        data.pid = gettid();
        data.value = coreLevel;
        char fileName[] = "/dev/hisi_perf_ctrl";
        int fd = open(fileName, O_RDWR);
        if (fd < 0) {
            RS_LOGE("Main Thread trival open rtg node failed");
        }
        int ret = ioctl(fd, PERF_CTRL_SET_TASK_MIN_UTIL, &data);
        if (ret < 0) {
            RS_LOGE("Main Thread ioctl failed");
        }
        close(fd);
    }
};
}
}

#endif // RENDER_SERVICE_CHANGE_CORE_UTIL_H