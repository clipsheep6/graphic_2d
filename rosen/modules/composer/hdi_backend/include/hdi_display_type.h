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

#ifndef HDI_BACKEND_HDI_DISPLAY_TYPE_H
#define HDI_BACKEND_HDI_DISPLAY_TYPE_H

namespace OHOS {
namespace Rosen {
/**
 * @brief Enumerates Color Gamut Mode of display.
 *
 */
typedef enum {
    CGM_INVALID = -1,            /**< invalid */
    CGM_NATIVE = 0,              /**< native or default */
    CGM_SATNDARD_BT601 = 1,      /**< standard BT601 */
    CGM_STANDARD_BT709 = 2,      /**< standard BT709 */
    CGM_DCI_P3 = 3,              /**< DCI P3 */
    CGM_SRGB = 4,                /**< SRGB */
    CGM_ADOBE_RGB = 5,           /**< Adobe RGB */
    CGM_DISPLAY_P3 = 6,          /**< display P3 */
    CGM_BT2020 = 7,              /**< BT2020 */
    CGM_BT2100_PQ = 8,           /**< BT2100 PQ */
    CGM_BT2100_HLG = 9,          /**< BT2100 HLG */
    CGM_DISPLAY_BT2020 = 10,     /**< display BT2020 */
} ColorGamutMode;

/**
 * @brief Enumerates Color Gamut Map.
 *
 */
typedef enum {
    GM_COLORIMETRIC = 0,
    GM_ENHANCE = 1,
    GM_HDR_COLORIMETRIC = 2,
    GM_HDR_ENHANCE = 3,
} GamutMap;

/**
 * @brief Enumerates Color Data Space.
 *
 */

typedef enum {
    UNKNOWN = 0,
    COLOR_MODE_BT601 = 0x00000001,
    COLOR_MODE_BT709 = 0x00000002,
    COLOR_MODE_DCI_P3 = 0x00000003,
    COLOR_MODE_SRGB = 0x00000004,
    COLOR_MODE_ADOBE_RGB = 0x00000005,
    COLOR_MODE_DISPLAY_P3 = 0x00000006,
    COLOR_MODE_BT2020 = 0x00000007,
    COLOR_MODE_BT2100_PQ = 0x00000008,
    COLOR_MODE_BT2100_HLG = 0x00000009,
    COLOR_MODE_DISPLAY_BT2020 = 0x0000000a,
    TF_UNSPECIFIED = 0x00000100,
    TF_LINEAR = 0x00000200,
    TF_SRGB = 0x00000300,
    TF_SMPTE_170M = 0x00000400,
    TF_GM2_2 = 0x00000500,
    TF_GM2_6 = 0x00000600,
    TF_GM2_8 = 0x00000700,
    TF_ST2084 = 0x00000800,
    TF_HLG = 0x00000900,
    PRECISION_UNSPECIFIED = 0x00010000,
    PRECISION_FULL = 0x00020000,
    PRESION_LIMITED = 0x00030000,
    PRESION_EXTENDED = 0x00040000,
    BT601_SMPTE170M_FULL = COLOR_MODE_BT601 | TF_SMPTE_170M | PRECISION_FULL,
    BT601_SMPTE170M_LIMITED = COLOR_MODE_BT601 | TF_SMPTE_170M | PRESION_LIMITED,
    BT709_LINEAR_FULL = COLOR_MODE_BT709 | TF_LINEAR | PRECISION_FULL,
    BT709_LINEAR_EXTENDED = COLOR_MODE_BT709 | TF_LINEAR | PRESION_EXTENDED,
    BT709_SRGB_FULL = COLOR_MODE_BT709 | TF_SRGB | PRECISION_FULL,
    BT709_SRGB_EXTENDED = COLOR_MODE_BT709 | TF_SRGB | PRESION_EXTENDED,
    BT709_SMPTE170M_LIMITED = COLOR_MODE_BT709 | TF_SMPTE_170M | PRESION_LIMITED,
    DCI_P3_LINEAR_FULL = COLOR_MODE_DCI_P3 | TF_LINEAR | PRECISION_FULL,
    DCI_P3_GAMMA26_FULL = COLOR_MODE_DCI_P3 | TF_GM2_6 | PRECISION_FULL,
    DISPLAY_P3_LINEAR_FULL = COLOR_MODE_DISPLAY_P3 | TF_LINEAR | PRECISION_FULL,
    DCI_P3_SRGB_FULL = COLOR_MODE_DCI_P3 | TF_SRGB | PRECISION_FULL,
    ADOBE_RGB_GAMMA22_FULL = COLOR_MODE_ADOBE_RGB | TF_GM2_2 | PRECISION_FULL,
    BT2020_LINEAR_FULL = COLOR_MODE_BT2020 | TF_LINEAR | PRECISION_FULL,
    BT2020_SRGB_FULL = COLOR_MODE_BT2020 | TF_SRGB | PRECISION_FULL,
    BT2020_SMPTE170M_FULL = COLOR_MODE_BT2020 | TF_SMPTE_170M | PRECISION_FULL,
    BT2020_ST2084_FULL = COLOR_MODE_BT2020 | TF_ST2084 | PRECISION_FULL,
    BT2020_HLG_FULL = COLOR_MODE_BT2020 | TF_HLG | PRECISION_FULL,
    BT2020_ST2084_LIMITED = COLOR_MODE_BT2020 | TF_ST2084 | PRESION_LIMITED,
} ColorDataSpace;

/**
 * @brief Enumerates HDR format.
 *
 */
typedef enum {
    NOT_SUPPORT_HDR = 0,
    DOLBY_VISION = 1,
    HDR10 = 2,
    HLG = 3,
    HDR10_PLUS = 4,
    HDR_VIVID = 5,
} HDRFormat;

/**
 * @brief Defines Disply HDR capability.
 *
 */
typedef struct {
    HDRFormat format;
    float maxLum;
    float maxAverageLum;
    float minLum;
} HDRCapability;

/**
 * @brief Enumerates metadatakey.
 *
 */
typedef enum {
    MATAKEY_RED_PRIMARY_X = 0,
    MATAKEY_RED_PRIMARY_Y = 1,
    MATAKEY_GREEN_PRIMARY_X = 2,
    MATAKEY_GREEN_PRIMARY_Y = 3,
    MATAKEY_BLUE_PRIMARY_X = 4,
    MATAKEY_BLUE_PRIMARY_Y = 5,
    MATAKEY_WHITE_PRIMARY_X = 6,
    MATAKEY_WHITE_PRIMARY_Y = 7,
    MATAKEY_MAX_LUMINANCE = 8,
    MATAKEY_MIN_LUMINANCE = 9,
    MATAKEY_MAX_CONTENT_LIGHT_LEVEL = 10,
    MATAKEY_MAX_FRAME_AVERAGE_LIGHT_LEVEL = 11,
    MATAKEY_HDR10_PLUS = 12,
    MATAKEY_HDR_VIVID = 13,
} MetadataKey;

/**
 * @brief Defines MetaData.
 *
 */
typedef struct {
    MetadataKey key;
    float value;
} MataData;

} // namespace Rosen
} // namespace OHOS
#endif