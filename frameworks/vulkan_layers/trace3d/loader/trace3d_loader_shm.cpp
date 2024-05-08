//
// Copyright (c) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
//

//
// Desc: This file is a part of the Trace3D Capture Gles/Vk Layer Loader
//
// Author: Dmitry Zhuk Z00858759 <zhuk.dmitry@huawei.com>
//

#include "trace3d_loader_shm.h"
#include "trace3d_loader_helper.h"

namespace trace3d {

static bool ShmOpenLibrary(const std::string &libPath, CaptureLib &lib) {
    if (lib.shmFd < 0) {
        std::string libFullName = libPath + lib.name;
        std::vector<uint8_t> blob;

        size_t sizeBytes = GetFileSize(libFullName.c_str());
        sizeBytes = sizeBytes > 0 ? ReadFileData(libFullName.c_str(), blob) : 0;

        if (sizeBytes > 0 && blob.size() == sizeBytes) {
            size_t len = sizeBytes;

            lib.shmFd = shm_open(lib.shmName, O_CREAT | O_RDWR, 0644);
            if (lib.shmFd >= 0) {
                ftruncate(lib.shmFd, len);

                void *mem = mmap(NULL, len, PROT_WRITE, MAP_SHARED, lib.shmFd, 0);
                if (mem) {
                    memcpy(mem, blob.data(), len);
                    munmap(mem, len);

                    TRACE3D_LOGI("%s:%d shm_open '%s' fd:%d\n", __FUNCTION__, __LINE__, lib.shmName, lib.shmFd);
                } else {
                    shm_unlink(lib.shmName);
                    close(lib.shmFd);
                    lib.shmFd = -1;

                    TRACE3D_LOGE("%s:%d ERROR: mmap\n", __FUNCTION__, __LINE__);
                }
            } else {
                TRACE3D_LOGE("%s:%d ERROR: shm_open '%s'\n", __FUNCTION__, __LINE__, lib.shmName);
            }
        }
    }
    return lib.shmFd >= 0;
}

void ShmCaptureInit() {
    for (auto &lib : captureLibs) {
        size_t foundSize = TestBundledSharedLibrary(lib.name);

        if (foundSize > 0) {
            TRACE3D_LOGI("%s:%d found bundle:'%s', size:%d\n", __FUNCTION__, __LINE__, lib.name, foundSize);

            if (!ShmOpenLibrary(TRACE3D_APP_LIB_URI, lib)) {
                //if (!ShmOpenLibrary(TRACE3D_APP_LIB_URI, lib)) {
                //    ShmOpenLibrary(TRACE3D_SYSTEM_LIB_URI, lib);
                //}
            }
        }
    }
}

void ShmCaptureCleanup() {
    for (auto &lib : captureLibs) {
        if (lib.shmFd >= 0) {
            shm_unlink(lib.shmName);
            close(lib.shmFd);
            lib.shmFd = -1;
        }
    }
}

} // namespace trace3d
