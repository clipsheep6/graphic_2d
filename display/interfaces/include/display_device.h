/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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
 * @file display_device.h
 *
 * @brief Declares control functions of the display device.
 *
 * @since 1.0
 * @version 2.0
 */

#ifndef DISPLAY_DEVICE_H
#define DISPLAY_DEVICE_H
#include "display_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/* *
 * @brief Called when a hot plug event occurs.
 *
 * This callback must be registered by calling <b>RegHotPlugCallback</b>.
 *
 * @param devId Indicates the ID of the display device. This ID is generated by the HDI implementation layer and 
 * transferred to the graphics service through the current callback. It identifies the display device to connect.
 * @param connected Indicates the connection status of the display device. The value <b>true</b> means that the 
 * display device is connected, and <b>false</b> means the opposite.
 * @param data Indicates the private data carried by the graphics service. This parameter carries the private data
 * address transferred when <b>RegHotPlugCallback</b> is called. For details, see {@link RegHotPlugCallback}.
 *  
 *
 * @since 1.0
 * @version 1.0
 */
typedef void (*HotPlugCallback)(uint32_t devId, bool connected, void *data);

/* *
 * @brief Called when a VBLANK event occurs.
 *
 * This callback must be registered by calling <b>RegDisplayVBlankCallback</b>.
 *
 * @param sequence Indicates the VBLANK sequence, which is an accumulated value.
 * @param ns Indicates the timestamp (in ns) of the VBLANK event.
 * @param data Indicates the pointer to the private data carried by the graphics service. This parameter carries
 * the address passed when <b>RegDisplayVBlankCallback</b> is called.
 *
 * @since 1.0
 * @version 1.0
 */
typedef void (*VBlankCallback)(unsigned int sequence, uint64_t ns, void *data);

/* *
 * @brief Called when the graphics service needs to refresh data frames.
 *
 * This callback must be registered by calling <b>RegDisplayRefreshCallback</b>.
 *
 * @param devId Indicates the ID of the display device.
 * @param data Indicates the pointer to the private data carried by the graphics service. This parameter carries
 * the address passed when <b>RegDisplayRefreshCallback</b> is called.
 *
 * @since 1.0
 * @version 1.0
 */
typedef void (*RefreshCallback)(uint32_t devId, void *data);

/**
 * @brief  Defines pointers to the functions of the display device.
 */
typedef struct {
    /* *
     * @brief Registers the callback to be invoked when a hot plug event occurs.
     *
     * @param callback Indicates the instance used to notify the graphics service of a hot plug event occurred.
     * @param data Indicates the pointer to the private data returned to the graphics service in the
     * <b>HotPlugCallback</b> callback.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined
     * in {@link DispErrCode} otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t (*RegHotPlugCallback)(HotPlugCallback callback, void *data);

    /* *
     * @brief Registers the callback to be invoked when a VBLANK event occurs.
     *
     * @param devId Indicates the ID of the display device.
     * @param callback Indicates the instance used to notify the graphics service of the VBLANK event occurred when
     * <b>DisplayVsync</b> is enabled.
     * @param data Indicates the pointer to the private data returned to the graphics service in the
     * <b>VBlankCallback</b> callback.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined
     * in {@link DispErrCode} otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t (*RegDisplayVBlankCallback)(uint32_t devId, VBlankCallback callback, void *data);

    /* *
     * @brief Called when the graphics service needs to refresh data frames.
     *
     * @param devId Indicates the ID of the display device.
     * @param callback Indicates the instance used to notify the graphics service of the request for refreshing
     * data frames.
     * @param data Indicates the pointer to the private data returned to the graphics service in the
     * <b>RefreshCallback</b> callback.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined
     * in {@link DispErrCode} otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t (*RegDisplayRefreshCallback)(uint32_t devId, RefreshCallback callback, void *data);

    /* *
     * @brief Obtains the capabilities of a display device.
     *
     * @param devId Indicates the ID of the display device.
     * @param info Indicates the pointer to the capabilities supported by the display device. For details,
     * see {@link DisplayCapability}.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined
     * in {@link DispErrCode} otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t (*GetDisplayCapability)(uint32_t devId, DisplayCapability *info);

    /* *
     * @brief Obtains the display modes supported by a display device.
     *
     * @param devId Indicates the ID of the display device.
     * @param num Indicates the pointer to the number of modes supported by the display device.
     * @param modes Indicates the pointer to the information about all modes supported by the display device,
     * including all supported resolutions and refresh rates. Each mode has an ID, which will be used when
     * the mode is set or obtained. For details, see {@link DisplayModeInfo}.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined
     * in {@link DispErrCode} otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t (*GetDisplaySuppportedModes)(uint32_t devId, int *num, DisplayModeInfo *modes);

    /* *
     * @brief Obtains the current display mode of a display device.
     *
     * @param devId Indicates the ID of the display device.
     * @param modeId indicates the pointer to the ID of the current display mode of the device. The display mode ID
     * is written by this API.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*GetDisplayMode)(uint32_t devId, uint32_t *modeId);

    /* *
     * @brief Sets the display mode of a display device.
     *
     * @param devId Indicates the ID of the display device.
     * @param modeId Indicates the ID of the display mode. The device is switched to the display mode specified by
     * this parameter in this interface.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined
     * in {@link DispErrCode} otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t (*SetDisplayMode)(uint32_t devId, uint32_t modeId);

    /* *
     * @brief Obtains the power status of a display device.
     *
     * @param devId Indicates the ID of the display device.
     * @param status Indicates the pointer to the power status of the device. The status is written by this interface.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined
     * in {@link DispErrCode} otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t (*GetDisplayPowerStatus)(uint32_t devId, DispPowerStatus *status);

    /* *
     * @brief Sets the power status of a display device.
     *
     * @param devId Indicates the ID of the display device.
     * @param status Indicates the power status to set.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined
     * in {@link DispErrCode} otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t (*SetDisplayPowerStatus)(uint32_t devId, DispPowerStatus status);

    /* *
     * @brief Obtains the backlight value of a display device.
     *
     * @param devId Indicates the ID of the display device.
     * @param level Indicates the pointer to the backlight value of the device. The backlight value is written
     * by this interface. 
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined
     * in {@link DispErrCode} otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t (*GetDisplayBacklight)(uint32_t devId, uint32_t *level);

    /* *
     * @brief Sets the backlight value for a display device.
     *
     * @param devId Indicates the ID of the display device.
     * @param level Indicates the backlight value to set.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined
     * in {@link DispErrCode} otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t (*SetDisplayBacklight)(uint32_t devId, uint32_t level);

    /* *
     * @brief Obtains the property for a display device.
     *
     * @param devId Indicates the ID of the display device.
     * @param id Indicates the property ID returned by <b>GetDisplayCapability</b>.
     * @param level Indicates the pointer to the property corresponding to the property ID. The property value is
     * written by this interface. 
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined
     * in {@link DispErrCode} otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t (*GetDisplayProperty)(uint32_t devId, uint32_t id, uint64_t *value);

    /* *
     * @brief Sets the property for a display device.
     *
     * @param devId Indicates the ID of the display device.
     * @param id Indicates the property ID returned by <b>GetDisplayCapability</b>.
     * @param value Indicates the property to set.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined
     * in {@link DispErrCode} otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t (*SetDisplayProperty)(uint32_t devId, uint32_t id, uint64_t value);

    /* *
    * @brief Prepares for the composition to be performed by a display device.
    *
    * Before the composition, the graphics service needs to notify the display device of the preparation to be made
    * through this interface.
    *
    * @param devId Indicates the ID of the display device.
    * @param needFlushFb Indicates the pointer that specifies whether the graphics service needs to reset the display
    * framebuffer by using <b>SetDisplayClientBuffer</b> before the commit operation.
    * The value <b>true</b> means that the framebuffer needs to be reset, and <b>false</b> means the opposite.
    *
    * @return Returns <b>0</b> if the operation is successful; returns an error code defined
    * in {@link DispErrCode} otherwise.
    * @since 1.0
    * @version 1.0
    */
    int32_t (*PrepareDisplayLayers)(uint32_t devId, bool *needFlushFb);

    /* *
     * @brief Obtains the layers whose composition types have changed.
     *
     * In the preparation for composition, the display device changes the composition type for each layer based on
     * the composition capability of the device. This function returns the layers whose composition types have changed.
     *
     * @param devId Indicates the ID of the display device.
     * @param num Indicates the pointer to the number of layers whose composition types have changed.
     * @param Layers Indicates the pointer to the start address of the layer array.
     * @param Layers Indicates the pointer to the start address of the composition type array.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined
     * in {@link DispErrCode} otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t (*GetDisplayCompChange)(uint32_t devId, uint32_t *num, uint32_t *Layers, int32_t *type);

    /* *
     * @brief Sets the cropped region for a display device.
     *
     * You can use this interface to set the cropped region of the client buffer of the display device.
     * The cropped region cannot exceed the size of the client buffer.
     *
     * @param devId Indicates the ID of the display device.
     * @param rect Indicates the pointer to the cropped region of the client buffer.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined
     * in {@link DispErrCode} otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t (*SetDisplayClientCrop)(uint32_t devId, IRect *rect);

    /* *
     * @brief Sets the display region for a display device.
     *
     * @param devId Indicates the ID of the display device.
     * @param rect Indicates the pointer to the display region.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined
     * in {@link DispErrCode} otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t (*SetDisplayClientDestRect)(uint32_t devId, IRect *rect);

    /* *
     * @brief Sets the display buffer for a display device.
     *
     * The display buffer stores the hardware composition result of the display device.
     *
     * @param devId Indicates the ID of the display device.
     * @param buffer Indicates the pointer to the display buffer.
     * @param fence Indicates the sync fence that specifies whether the display buffer can be accessed. The display
     * buffer is created and released by the graphics service. It can be accessed only when the sync fence is in the
     * signaled state.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined
     * in {@link DispErrCode} otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t (*SetDisplayClientBuffer)(uint32_t devId, const BufferHandle *buffer, int32_t fence);

    /* *
     * @brief Sets the dirty region for a display device.
     *
     * The dirty region consists of multiple rectangular regions. The rectangular regions can be refreshed based on
     * the settings.
     *
     * @param devId Indicates the ID of the display device.
     * @param num Indicates the number of rectangles.
     * @param rect Indicates the pointer to the start address of the rectangle array.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined
     * in {@link DispErrCode} otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t (*SetDisplayClientDamage)(uint32_t devId, uint32_t num, IRect *rect);

    /* *
     * @brief Enables or disables the vertical sync signal.
     *
     * When the vertical sync signal is generated, the <b>VBlankCallback</b> callback registered
     * by <b>RegDisplayVBlankCallback</b> will be invoked. The vertical sync signal must be enabled when the graphics
     * service needs to refresh the display, and disabled when display refresh is not required. The display does not
     * need to refresh when <b>VBlankCallback</b> is invoked and the graphics service composes layers and sends the
     * composition result to the device for display.
     * 
     * @param devId Indicates the ID of the display device.
     * @param enabled Specifies whether to enable the vertical sync signal. The value <b>true</b> means to enable the
     * vertical sync signal, and <b>false</b> means to disable it.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined
     * in {@link DispErrCode}otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t (*SetDisplayVsyncEnabled)(uint32_t devId, bool enabled);

    /* *
     * @brief Obtains the fences of the display layers after the commit operation.
     *
     * @param devId Indicates the ID of the display device.
     * @param num Indicates the pointer to the number of layers.
     * @param layers Indicates the pointer to the start address of the layer array.
     * @param fences Indicates the pointer to the start address of the fence array.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined
     * in {@link DispErrCode} otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t (*GetDisplayReleaseFence)(uint32_t devId, uint32_t *num, uint32_t *layers, int32_t *fences);

    /* *
     * @brief Commits the request for composition and display.
     *
     * If there is a hardware composition layer, the composition is performed and the composition result is sent to
     * the hardware for display.
     *
     * @param devId Indicates the ID of the display device.
     * @param num Indicates the pointer to the number of layers.
     * @param layers Indicates the pointer to the start address of the layer array.
     * @param fences Indicates the pointer to the start address of the fence array.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined
     * in {@link DispErrCode} otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t (*Commit)(uint32_t devId, int32_t *fence);

    /* *
     * @brief Invokes the display device commands.
     *
     * This function extends the APIs between the graphics service and implementation layer, eliminating the need
     * to add new APIs.
     *
     * @param devId Indicates the ID of the display device.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined
     * in {@link DispErrCode} otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t (*InvokeDisplayCmd)(uint32_t devId, ...);

    /* *
     * @brief Creates a virtual display device.
     *
     * @param width Indicates the pixel width of the display device.
     * @param height Indicates the pixel height of the display device.
     * @param format Indicates the pointer to the pixel format of the display device.
     * For details, see {@link PixelFormat}. The <b>format</b> can be modified based on hardware requirements and
     * returned to the graphics service.
     * @param devId Indicates the pointer to the ID of the virtual display device created.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined
     * in {@link DispErrCode} otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t (*CreateVirtualDisplay)(uint32_t width, uint32_t height, int32_t *format, uint32_t *devId);

    /* *
     * @brief Destroys a virtual display device.
     * 
     * @param devId Indicates the ID of the display device.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined
     * in {@link DispErrCode} otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t (*DestroyVirtualDisplay)(uint32_t devId);

    /* *
     * @brief Sets the output buffer for a virtual display device.
     *
     * This buffer stores the output of the virtual display device. The buffer can be used only after the sync fence
     * is in the signaled state.
     *
     * @param devId Indicates the ID of the display device.
     * @param buffer Indicates the pointer to the output buffer.
     * @param fence Indicates the sync fence.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined
     * in {@link DispErrCode} otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t (*SetVirtualDisplayBuffer)(uint32_t devId, BufferHandle *buffer, int32_t fence);

    /* *
     * @brief Obtains the writeback frame of a display device.
     *
     * This function is used to obtain data of the writeback endpoint specified by <b>devId</b>. The data is written
     * to the specified buffer by this interface.
     *
     * @param devId Indicates the ID of the display device.
     * @param buffer Indicates the pointer to the buffer of the writeback endpoint data.
     * @param fence Indicates the pointer to the sync fence. When calling this interface, the graphics service needs
     * to pass the <b>release fence</b> of the buffer to
     * specify whether data can be written to the buffer. Then, <b>acquire fence</b> of the buffer needs to be written
     * and sent to the graphics service to specify whether the writeback data has been written to the buffer.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined
     * in {@link DispErrCode} otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t (*GetWriteBackFrame)(uint32_t devId, BufferHandle *buffer, int32_t *fence);

    /* *
     * @brief Creates a writeback endpoint for a display device.
     *
     * If the number of writeback endpoints exceeds the limit, a failure message will be returned.
     *
     * @param devId Indicates the pointer to the ID of the display device. After a writeback endpoint is created, the
     * device ID of the writeback endpoint is written in this parameter and returned to the graphics service.
     * @param width Indicates the writeback pixel width.
     * @param height Indicates the writeback pixel height.
     * @param format Indicates the pointer to the writeback point data format. For details, see {@link PixelFormat}.
     * The <b>format</b> can be modified based on hardware requirements and returned to the graphics service.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined
     * in {@link DispErrCode} otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t (*CreateWriteBack)(uint32_t *devId, uint32_t width, uint32_t height, int32_t *format);

    /* *
     * @brief Destroys the writeback endpoint of a display device.
     *
     * @param devId Indicates the ID of the display device.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined
     * in {@link DispErrCode} otherwise.
     * @since 1.0
     * @version 1.0
     */
    int32_t (*DestroyWriteBack)(uint32_t devId);
} DeviceFuncs;

/**
 * @brief Initializes the control functions of a display device. You can apply for resources for
 * using control functions and then operate the display device by using the control functions.
 *
 * @param funcs Indicates the double pointer to the control functions of the display device. The caller obtains
 * the double pointer to operate the display device. The memory is allocated during initialization, and therefore
 * the caller does not need to allocate the memory.
 *
 * @return Returns <b>0</b> if the operation is successful; returns an error code defined
 * in {@link DispErrCode} otherwise.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t DeviceInitialize(DeviceFuncs **funcs);

/**
 * @brief Uninitializes control functions of the display device. The resources used by the control functions will be
 * released. In other words, the memory allocated during initialization of the control functions will be released.
 *
 * @param funcs Indicates the double pointer to the control functions of the display device.
 *
 * @return Returns <b>0</b> if the operation is successful; returns an error code defined
 * in {@link DispErrCode} otherwise.
 * @since 1.0
 * @version 1.0
 */
int32_t DeviceUninitialize(DeviceFuncs *funcs);

#ifdef __cplusplus
}
#endif
#endif
/* @} */
