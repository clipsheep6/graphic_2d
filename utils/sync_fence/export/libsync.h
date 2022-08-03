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

#ifndef _LIBSYNC_H
#define _LIBSYNC_H

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <unistd.h>

#if defined(__cplusplus)
extern "C" {
#endif

#ifndef SYNC_IOC_MERGE
/* duplicated from linux/sync_file.h to avoid build-time dependency
 * on new (v4.7) kernel headers.  Once distro's are mostly using
 * something newer than v4.7 drop this and #include <linux/sync_file.h>
 * instead.
 */
struct sync_merge_data {
    char     name[32];
    int32_t  fd2;
    int32_t  fence;
    uint32_t flags;
    uint32_t pad;
};
#define SYNC_IOC_MAGIC    '>'
#define SYNC_IOC_MERGE    _IOWR(SYNC_IOC_MAGIC, 3, struct sync_merge_data)
#endif

static inline int sync_wait(int fd, int timeout)
{
    struct pollfd fds = {0};
    int ret;

    fds.fd = fd;
    fds.events = POLLIN;

    do {
        ret = poll(&fds, 1, timeout);
        if (ret > 0) {
            if (fds.revents & (POLLERR | POLLNVAL)) {
                errno = EINVAL;
                return -1;
            }
            return 0;
        } else if (ret == 0) {
            errno = ETIME;
            return -1;
        }
    } while (ret == -1 && (errno == EINTR || errno == EAGAIN));

    return ret;
}

static inline int sync_merge(const char *name, int fd1, int fd2)
{
    struct sync_merge_data data = {0};
    int ret;

    data.fd2 = fd2;
    strncpy(data.name, name, sizeof(data.name));

    do {
        ret = ioctl(fd1, SYNC_IOC_MERGE, &data);
    } while (ret == -1 && (errno == EINTR || errno == EAGAIN));

    if (ret < 0) {
        return ret;
    }

    return data.fence;
}

#if defined(__cplusplus)
}
#endif

#endif
