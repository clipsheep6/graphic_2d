#ifndef VWETICAL_BLUR_FILTER_H
#define VWETICAL_BLUR_FILTER_H

#include "base_blur_filter.h"

class VerticalBlurFilter :
	public BaseBlurFilter
{
public:
	VerticalBlurFilter();
	std::string GetFragmentShader() override;
	std::string GetVertexShader() override;
};

#endif

