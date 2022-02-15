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
#include "src/core/SkVM.h"
#include "color_space_convertor.h"

namespace OHOS {
namespace ColorManager {

ColorSpaceConvertor::ColorSpaceConvertor(ColorSpaceName src, ColorSpaceName dst, GamutMappingMode mapingMode)
	: srcColorSpace(src), dstColorSpace(dst), mappingMode(mapingMode){
	ColorSpace srcCS = ColorSpace(src, 3);
	ColorSpace dstCS = ColorSpace(dst, 3);
	// OH ColorSpace can be transfer to skColorSpace by 
	// func colorSpace() & alphaType()
	SkConvertor = SkColorSpaceXformSteps(srcCS, dstCS);
};

std::vector<float> ColorSpaceConvertor::Convert(float r, float g, float b){
	
	float rgba[4] = {r, g, b, 0.0f}; 
	SkConvertor.apply(rgba);
	std::vector<float> dstColor(3);
	
	dstColor[0] = rgba[0];
	dstColor[1] = rgba[1];
	dstColor[2] = rgba[2];

	return dstColor;
};

}
}