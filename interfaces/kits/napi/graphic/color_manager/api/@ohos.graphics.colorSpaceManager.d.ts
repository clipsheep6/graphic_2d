/*
* Copyright (C) 2022-2023 Huawei Device Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

import { AsyncCallback } from './@ohos.base';

/**
 * Color space manager.
 *
 * @namespace colorSpaceManager
 * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
 * @since 9
 */
declare namespace colorSpaceManager {
  /**
   * Enumerates color space types.
   * @enum { number } ColorSpace
   * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
   * @since 9
   */
  enum ColorSpace {
    /**
     * Indicates an unknown color space.
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     * @since 9
     */
    UNKNOWN = 0,

    /**
     * Indicates the color space based on Adobe RGB (1998).
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     * @since 9
     */
    ADOBE_RGB_1998 = 1,

    /**
     * Indicates the color space based on SMPTE RP 431-2-2007 and IEC 61966-2.1:1999.
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     * @since 9
     */
    DCI_P3 = 2,

    /**
     * Indicates the color space based on SMPTE RP 431-2-2007 and IEC 61966-2.1:1999.
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     * @since 9
     */
    DISPLAY_P3 = 3,

    /**
     * Indicates the standard red green blue (SRGB) color space based on IEC 61966-2.1:1999.
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     * @since 9
     */
    SRGB = 4,

    /**
     * Indicates the color space based on ITU-R BT.709.
     * PRIMARIES_BT709 | TRANSFUNC_BT709 | RANGE_FULL
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     * @since 11
     */
    BT709 = 6,

    /**
     * Indicates the color space based on ITU-R BT.601.
     * PRIMARIES_BT601_P | TRANSFUNC_BT709 | RANGE_FULL
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     * @since 11
     */
    BT601_EBU = 7,

    /**
     * Indicates the color space based on ITU-R BT.601.
     * PRIMARIES_BT601_N | TRANSFUNC_BT709 | RANGE_FULL
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     * @since 11
     */
    BT601_SMPTE_C = 8,

    /**
     * Indicates the color space based on ITU-R BT.2020.
     * PRIMARIES_BT2020 | TRANSFUNC_HLG | RANGE_FULL
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     * @since 11
     */
    BT2020_HLG = 9,

    /**
     * Indicates the color space based on ITU-R BT.2020.
     * PRIMARIES_BT2020 | TRANSFUNC_PQ | RANGE_FULL
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     * @since 11
     */
    BT2020_PQ = 10,

    /**
     * PRIMARIES_P3_D65 | TRANSFUNC_HLG | RANGE_FULL
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     * @since 11
     */
    P3_HLG = 11,

    /**
     * PRIMARIES_P3_D65 | TRANSFUNC_PQ | RANGE_FULL
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     * @since 11
     */
    P3_PQ = 12,

    /**
     * PRIMARIES_ADOBE_RGB | TRANSFUNC_ADOBE_RGB | RANGE_LIMIT
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     * @since 11
     */
    ADOBE_RGB_1998_LIMIT = 13,

    /**
     * PRIMARIES_P3_D65 | TRANSFUNC_SRGB | RANGE_LIMIT
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     * @since 11
     */
    DISPLAY_P3_LIMIT = 14,

    /**
     * PRIMARIES_SRGB | TRANSFUNC_SRGB | RANGE_LIMIT
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     * @since 11
     */
    SRGB_LIMIT = 15,

    /**
     * PRIMARIES_BT709 | TRANSFUNC_BT709 | RANGE_LIMIT
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     * @since 11
     */
    BT709_LIMIT = 16,

    /**
     * PRIMARIES_BT601_P | TRANSFUNC_BT709 | RANGE_LIMIT
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     * @since 11
     */
    BT601_EBU_LIMIT = 17,

    /**
     * PRIMARIES_BT601_N | TRANSFUNC_BT709 | RANGE_LIMIT
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     * @since 11
     */
    BT601_SMPTE_C_LIMIT = 18,

    /**
     * PRIMARIES_BT2020 | TRANSFUNC_HLG | RANGE_LIMIT
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     * @since 11
     */
    BT2020_HLG_LIMIT = 19,

    /**
     * PRIMARIES_BT2020 | TRANSFUNC_PQ | RANGE_LIMIT
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     * @since 11
     */
    BT2020_PQ_LIMIT = 20,

    /**
     * PRIMARIES_P3_D65 | TRANSFUNC_HLG | RANGE_LIMIT
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     * @since 11
     */
    P3_HLG_LIMIT = 21,

    /**
     * PRIMARIES_P3_D65 | TRANSFUNC_PQ | RANGE_LIMIT
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     * @since 11
     */
    P3_PQ_LIMIT = 22,

    /**
     * PRIMARIES_P3_D65 | TRANSFUNC_LINEAR
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     * @since 11
     */
    LINEAR_P3 = 23,

    /**
     * PRIMARIES_SRGB | TRANSFUNC_LINEAR
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     * @since 11
     */
    LINEAR_SRGB = 24,

    /**
     * PRIMARIES_BT709 | TRANSFUNC_LINEAR
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     * @since 11
     */
    LINEAR_BT709 = LINEAR_SRGB,

    /**
     * PRIMARIES_BT2020 | TRANSFUNC_LINEAR
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     * @since 11
     */
    LINEAR_BT2020 = 25,

    /**
     * PRIMARIES_SRGB | TRANSFUNC_SRGB | RANGE_FULL
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     * @since 11
     */
    DISPLAY_SRGB = SRGB,

    /**
     * PRIMARIES_P3_D65 | TRANSFUNC_SRGB | RANGE_FULL
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     * @since 11
     */
    DISPLAY_P3_SRGB = DISPLAY_P3,

    /**
     * PRIMARIES_P3_D65 | TRANSFUNC_HLG | RANGE_FULL
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     * @since 11
     */
    DISPLAY_P3_HLG = P3_HLG,

    /**
     * PRIMARIES_DISPLAY_P3 | TRANSFUNC_PQ | RANGE_FULL
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     * @since 11
     */
    DISPLAY_P3_PQ = P3_PQ,

    /**
     * Indicates a customized color space.
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     * @since 9
     */
    CUSTOM = 5,
  }

  /**
   * Describes the primary colors red, green, blue and white point coordinated as (x, y)
   * in color space, in terms of real world chromaticities.
   * @typedef ColorSpacePrimaries
   * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
   * @since 9
   */
  interface ColorSpacePrimaries {
    /**
     * Coordinate value x of red color
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     * @since 9
     */
    redX: number;

    /**
     * Coordinate value y of red color
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     * @since 9
     */
    redY: number;

    /**
     * Coordinate value x of green color
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     * @since 9
     */
    greenX: number;

    /**
     * Coordinate value y of green color
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     * @since 9
     */
    greenY: number;

    /**
     * Coordinate value x of blue color
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     * @since 9
     */
    blueX: number;

    /**
     * Coordinate value y of blue color
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     * @since 9
     */
    blueY: number;

    /**
     * Coordinate value x of white point
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     * @since 9
     */
    whitePointX: number;

    /**
     * Coordinate value y of white point
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     * @since 9
     */
    whitePointY: number;
  }

  /**
   * Defines a color space object and manages its key information
   * @interface ColorSpaceManager
   * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
   * @since 9
   */
  interface ColorSpaceManager {
    /**
     * Get the name of color space type.
     * @returns { ColorSpace } Returns the name of color space type.
     * @throws { BusinessError } 18600001 - Parameter value is abnormal.
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     * @since 9
     */
    getColorSpaceName(): ColorSpace;

    /**
     * Get white point(x, y) of color space.
     * @returns { Array<number> } Returns the white point value of color space.
     * @throws { BusinessError } 18600001 - Parameter value is abnormal.
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     * @since 9
     */
    getWhitePoint(): Array<number>;

    /**
     * Get gamma value of color space.
     * @returns { number } Returns the gamma value of color space.
     * @throws { BusinessError } 18600001 - Parameter value is abnormal.
     * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
     * @since 9
     */
    getGamma(): number;
  }

  /**
   * Create a color space manager by provided color space type.
   * @param { ColorSpace } colorSpaceName - Indicates the type of color space
   * @returns { ColorSpaceManager } Returns a color space manager object created by provided type.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 18600001 - Parameter value is abnormal.
   * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
   * @since 9
   */
  function create(colorSpaceName: ColorSpace): ColorSpaceManager;

  /**
   * Create a customized color space manager by its color primaries and gamma value
   * @param { ColorSpacePrimaries } primaries - Indicates the customized color primaries
   * @param { number } gamma - Indicates display gamma value
   * @returns { ColorSpaceManager } Returns a color space manager object created by customized parameters.
   * @throws { BusinessError } 401 - Parameter error.
   * @throws { BusinessError } 18600001 - Parameter value is abnormal.
   * @syscap SystemCapability.Graphic.Graphic2D.ColorManager.Core
   * @since 9
   */
  function create(primaries: ColorSpacePrimaries, gamma: number): ColorSpaceManager;
}

export default colorSpaceManager;