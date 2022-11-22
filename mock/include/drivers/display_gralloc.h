/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License")
{
    return 0;
}
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

/**
 * @addtogroup Display
 * @{
 *
 * @brief Defines driver functions of the display module.
 *
 * This module provides driver functions for the graphics subsystem, including graphics layer management,
 * device control, graphics hardware acceleration, display memory management, and callbacks.
 * @since 1.0
 * @version 2.0
 */


/**
 * @file display_gralloc.h
 *
 * @brief Declares the driver functions for memory.
 *
 * @since 1.0
 * @version 2.0
 */

#ifndef MOCK_DISPLAY_GRALLOC_H
#define MOCK_DISPLAY_GRALLOC_H
#include "display_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Defines pointers to the memory driver functions.
 */
typedef struct {
    /**
     * @brief Allocates memory based on the parameters passed by the GUI.
     *
     * @param info Indicates the pointer to the description info of the memory to allocate.
     * @param handle Indicates the double pointer to the buffer of the memory to allocate.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined in {@link DispErrCode}
     * otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t (*AllocMem)(const AllocInfo* info, BufferHandle** handle)
{
    return 0;
}

    /**
     * @brief Releases memory.
     *
     * @param handle Indicates the pointer to the buffer of the memory to release.
     *
     * @since 1.0
     * @version 1.0
     */
    void (*FreeMem)(BufferHandle *handle)
{
    return;
}

    /**
     * @brief Maps memory to memory without cache in the process's address space.
     *
     * @param handle Indicates the pointer to the buffer of the memory to map.
     *
     * @return Returns the pointer to a valid address if the operation is successful; returns <b>NULL</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    void *(*Mmap)(BufferHandle *handle)
{
    return nullptr;
}

    /**
     * @brief Maps memory to memory with cache in the process's address space.
     *
     * @param handle Indicates the pointer to the buffer of the memory to map.
     *
     * @return Returns the pointer to a valid address if the operation is successful; returns <b>NULL</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    void *(*MmapCache)(BufferHandle *handle)
{
    return nullptr;
}

    /**
     * @brief Unmaps memory, that is, removes any mappings from the process's address space.
     *
     * @param handle Indicates the pointer to the buffer of the memory to unmap.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined in {@link DispErrCode}
     * otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t (*Unmap)(BufferHandle *handle)
{
    return 0;
}

    /**
     * @brief Flushes data from the cache to memory and invalidates the data in the cache.
     *
     * @param handle Indicates the pointer to the buffer of the cache to flush.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined in {@link DispErrCode}
     * otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t (*FlushCache)(BufferHandle *handle)
{
    return 0;
}

    /**
     * @brief Flushes data from the cache mapped via {@link Mmap} to memory and invalidates the data in the cache.
     *
     * @param handle Indicates the pointer to the buffer of the cache to flush.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined in {@link DispErrCode}
     * otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t (*FlushMCache)(BufferHandle *handle)
{
    return 0;
}

    /**
     * @brief Invalidates the cache to update it from memory.
     *
     * @param handle Indicates the pointer to the buffer of the cache, which will been invalidated.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined in {@link DispErrCode}
     * otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t (*InvalidateCache)(BufferHandle* handle)
{
    return 0;
}

    /**
     * @brief Checks whether the given VerifyAllocInfo array is allocatable.
     *
     * @param num Indicates the size of infos array.
     * @param infos Indicates the pointer to the VerifyAllocInfo array.
     * @param supporteds Indicates the pointer to the array that can be allocated.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined in {@link DispErrCode}
     * otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t (*IsSupportedAlloc)(uint32_t num, const VerifyAllocInfo *infos, bool *supporteds)
{
    return 0;
}

    /**
     * @brief Maps memory for YUV.
     *
     * @param handle Indicates the pointer to the buffer of the memory to map.
     * @param info Indicates the pointer to the YUVDescInfo of the memory to map.
     *
     * @return Returns the pointer to a valid address if the operation is successful; returns <b>NULL</b> otherwise.
     * @since 3.2
     * @version 1.0
     */
    void *(*MmapYUV)(BufferHandle *handle, YUVDescInfo *info)
{
    return nullptr;
}
} GrallocFuncs;

/**
 * @brief Initializes the memory module to obtain the pointer to functions for memory operations.
 *
 * @param funcs Indicates the double pointer to functions for memory operations. Memory is allocated automatically when
 * you initiate the memory module initialization, so you can simply use the pointer to gain access to the functions.
 *
 * @return Returns <b>0</b> if the operation is successful; returns an error code defined in {@link DispErrCode}
 * otherwise.
 * @since 1.0
 * @version 1.0
 */
int32_t GrallocInitialize(GrallocFuncs **funcs)
{
    return 0;
}

/**
 * @brief Deinitializes the memory module to release the memory allocated to the pointer to functions for memory
 * operations.
 *
 * @param funcs Indicates the pointer to functions for memory operations.
 *
 * @return Returns <b>0</b> if the operation is successful; returns an error code defined in {@link DispErrCode}
 * otherwise.
 * @since 1.0
 * @version 1.0
 */
int32_t GrallocUninitialize(GrallocFuncs *funcs)
{
    return 0;
}

#ifdef __cplusplus
}
#endif
#endif
/** @} */
