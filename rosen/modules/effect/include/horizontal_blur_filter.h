#ifndef HORIZONTAL_BLUR_FILTER_H
#define HORIZONTAL_BLUR_FILTER_H

#include "base_blur_filter.h"

class HorizontalBlurFilter :
	public BaseBlurFilter
{
public:
	HorizontalBlurFilter();
	std::string GetFragmentShader() override;
	std::string GetVertexShader() override;
};

#endif // !HORIZONTAL_BLUR_FILTER_H

