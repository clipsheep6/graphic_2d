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

#ifndef BUILDER_H
#define BUILDER_H

#include <unordered_map>
#include <vector>
#include "cJSON.h"
#include "image_chain.h"
#include "gaussian_blur_filter.h"
#include "input_jpg.h"
#include "input_png.h"
#include "output_bmp.h"

class Builder
{
public:
	ImageChain* CreatFromConfig(std::string path);
private:
	void AnalyseFilters(cJSON* filters);
	Filter* CreatBlurFilters(cJSON* filters, std::string filterName);
	Filter* CreatFilters(cJSON* filters);
	void ConnectPipeline(cJSON* connect);
	std::unordered_map<std::string, std::string> nameType_;
	std::unordered_map<std::string, Filter*> nameFilter_;
	std::vector<Input*> myInputs_;
	cJSON* filters_ = nullptr;
	cJSON* connect_ = nullptr;
	const int RADIUS = 3;
};

#endif
