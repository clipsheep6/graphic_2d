#ifndef GAUSSIAN_BLUR_FILTER_H
#define GAUSSIAN_BLUR_FILTER_H

#include "algo_filter.h"
#include "horizontal_blur_filter.h"
#include "vertical_blur_filter.h"
#include "scale_filter.h"

#include <algorithm>

class GaussianBlurFilter : public AlgoFilter
{
public:
	GaussianBlurFilter();
	~GaussianBlurFilter();
	void SetValue(std::string key, void* value, int size = -1) override;
private:
	std::string GetVertexShader() override;
	std::string GetFragmentShader() override;
	void DoProcess(ProcessDate& data) override;
	ScaleFilter* enlargeFilter_ = nullptr;
	ScaleFilter* narrowFilter_ = nullptr;
	HorizontalBlurFilter* horizontalBlurFilter_ = nullptr;
	VerticalBlurFilter* verticalBlurFilter_ = nullptr;
};

#endif // !GAUSSIAN_BLUR_FILTER_H

