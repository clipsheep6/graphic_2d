#ifndef BUILDER_H
#define BUILDER_H

#include "image_chain.h"
#include "gaussian_blur_filter.h"
#include "input_jpg.h"
#include "input_png.h"
#include "output_bmp.h"

#include <unordered_map>
#include <vector>

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
