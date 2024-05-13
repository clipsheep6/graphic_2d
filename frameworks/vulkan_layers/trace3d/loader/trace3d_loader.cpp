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

void* GetCaptureEntryLibHandle() {
    for (auto &lib : captureLibs) {
        if (lib.handle && lib.mainEntry)
            return lib.handle;
    }
    return nullptr;
}

void* CaptureInit() {
    ShmCaptureInit();

    void *libHandle = GetCaptureEntryLibHandle();
    if (!libHandle) {
        for (auto &lib : captureLibs) { 
            if (lib.handle)
                continue;
            size_t foundSize = TestBundledSharedLibrary(lib.name);

            if (foundSize > 0) {
                TRACE3D_LOGI("%s:%d found bundle:'%s', size:%d\n", __FUNCTION__, __LINE__, lib.name, foundSize);

                if (!lib.handle && lib.mainEntry) {
                    libHandle = lib.handle = DlopenBundledSharedLibrary(lib.name);
                }
            } else {
                foundSize = TestStoredSharedLibrary(lib.name);

                if (foundSize > 0) {
                    TRACE3D_LOGI("%s:%d found stored:'%s', size:%d\n", __FUNCTION__, __LINE__, lib.name, foundSize);
                }

                const std::string shmFullName = std::string(TRACE3D_SHM_URI) + lib.shmName;
                const size_t shmSize = GetFileSize(shmFullName.c_str());

                if (shmSize > 0 && foundSize != shmSize) {
                    std::vector<uint8_t> blob;
        
                    if (ReadFileData(shmFullName.c_str(), blob) > 0) {
                        StoreSharedLibrary(lib.name, blob);
                    }
                    foundSize = TestStoredSharedLibrary(lib.name);

                    if (foundSize > 0) {
                        TRACE3D_LOGI("%s:%d found stored:'%s', size:%d\n", __FUNCTION__, __LINE__, lib.name, foundSize);
                    }
                }
                if (foundSize > 0) {
                    if (!lib.handle && lib.mainEntry) {
                        libHandle = lib.handle = DlopenStoredSharedLibrary(lib.name);
                    }
                }
            }
        }
    }

    return libHandle;
}

void CaptureCleanup() {
    for (auto &lib : captureLibs) {
        if (lib.handle) {
            dlclose(lib.handle);
            lib.handle = nullptr;
        }
    }
    ShmCaptureCleanup();
}

} // namespace trace3d

__attribute__((destructor)) static void CleanupCaptureLoaderLib() {
    trace3d::CaptureCleanup();
}
