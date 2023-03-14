/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef GRAPHIC_COMPOSER_DISPLAY_TYPE_H
#define GRAPHIC_COMPOSER_DISPLAY_TYPE_H
#include "buffer_handle.h"
#include "surface_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PROPERTY_NAME_LEN  50

/*
 * @brief Defines property object which contains name, property id and value.
 */
typedef struct {
    char name[PROPERTY_NAME_LEN]; /**< Name of the property */
    uint32_t propId;              /**< Property id which was decided in the DRM internal */
    uint64_t value;               /**< the value of property  */
} GraphicPropertyObject;

/*
 * @brief Enumerates interface types.
 */
typedef enum {
    GRAPHIC_DISP_INTF_HDMI = 0,       /**< HDMI interface */
    GRAPHIC_DISP_INTF_LCD,            /**< LCD interface */
    GRAPHIC_DISP_INTF_BT1120,         /**< BT1120 interface */
    GRAPHIC_DISP_INTF_BT656,          /**< BT656 interface */
    GRAPHIC_DISP_INTF_YPBPR,          /**< YPBPR interface */
    GRAPHIC_DISP_INTF_RGB,            /**< RGB interface */
    GRAPHIC_DISP_INTF_CVBS,           /**< CVBS interface */
    GRAPHIC_DISP_INTF_SVIDEO,         /**< SVIDEO interface */
    GRAPHIC_DISP_INTF_VGA,            /**< VGA interface */
    GRAPHIC_DISP_INTF_MIPI,           /**< MIPI interface */
    GRAPHIC_DISP_INTF_PANEL,          /**< PANEL interface */
    GRAPHIC_DISP_INTF_BUTT,
} GraphicInterfaceType;

/**
 * @brief Defines the capability of the output.
 */
typedef struct {
    char name[PROPERTY_NAME_LEN];              /**< Name of the display device */
    GraphicInterfaceType type;                 /**< Interface type of panel */
    uint32_t phyWidth;                         /**< Physical width */
    uint32_t phyHeight;                        /**< Physical height */
    uint32_t supportLayers;                    /**< Number of supported layers */
    uint32_t virtualDispCount;                 /**< Count of virtual displays supported */
    bool supportWriteBack;                     /**< Whether writeback is supported */
    uint32_t propertyCount;                    /**< Count of properties */
    std::vector<GraphicPropertyObject> props;  /**< Vector of property objects */
} GraphicDisplayCapability;

/*
 * @brief Defines output mode info.
 */
typedef struct {
    int32_t width;      /**< Width in pixel */
    int32_t height;     /**< Height in pixel */
    uint32_t freshRate; /**< Fresh rate per second */
    int32_t id;         /**< ID of the mode */
} GraphicDisplayModeInfo;

typedef enum {
    GRAPHIC_POWER_STATUS_ON,              /**< The power status is on. */
    GRAPHIC_POWER_STATUS_STANDBY,         /**< The power status is standby. */
    GRAPHIC_POWER_STATUS_SUSPEND,         /**< The power status is suspended. */
    GRAPHIC_POWER_STATUS_OFF,             /**< The power status is off. */
    GRAPHIC_POWER_STATUS_BUTT
} GraphicDispPowerStatus;

/**
 * @brief Enumerates the color gamut maps.
 *
 */
typedef enum {
    GRAPHIC_GAMUT_MAP_CONSTANT = 0,
    GRAPHIC_GAMUT_MAP_EXPANSION = 1,
    GRAPHIC_GAMUT_MAP_HDR_CONSTANT = 2,
    GRAPHIC_GAMUT_MAP_HDR_EXPANSION = 3,
} GraphicGamutMap;

/*
 * @brief Enumerates the HDR formats.
 */
typedef enum {
    GRAPHIC_NOT_SUPPORT_HDR = 0,
    GRAPHIC_DOLBY_VISION = 1,
    GRAPHIC_HDR10 = 2,
    GRAPHIC_HLG = 3,
    GRAPHIC_HDR10_PLUS = 4,
    GRAPHIC_HDR_VIVID = 5,
} GraphicHDRFormat;

/*
 * @brief Defines the HDR capability.
 */
typedef struct {
    uint32_t formatCount;
    std::vector<GraphicHDRFormat> formats;
    float maxLum;
    float maxAverageLum;
    float minLum;
} GraphicHDRCapability;

/*
 * @brief Called when a hot plug event occurs.
 *
 * This callback must be registered by calling <b>RegHotPlugCallback</b>.
 *
 * @param devId Indicates the ID of the display device.
 * @param connected Indicates the connection status of the display device. The value <b>true</b> means that
 *  the display device is connected, and <b>false</b> means the opposite.
 * @param data Indicates the private data carried by the graphics service.
 */
typedef void (*HotPlugCallback)(uint32_t devId, bool connected, void *data);

/*
 * @brief Called when a VBLANK event occurs.
 *
 * This callback must be registered by calling <b>RegDisplayVBlankCallback</b>.
 *
 * @param sequence Indicates the VBLANK sequence, which is an accumulated value.
 * @param ns Indicates the timestamp (in ns) of the VBLANK event.
 * @param data Indicates the pointer to the private data carried by the graphics service.
 */
typedef void (*VBlankCallback)(unsigned int sequence, uint64_t ns, void *data);

#ifdef __cplusplus
}
#endif
#endif // GRAPHIC_COMPOSER_DISPLAY_TYPE_H
