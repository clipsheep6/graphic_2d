#ifndef ALGO_FILTER_FACTORY_H
#define ALGO_FILTER_FACTORY_H

#include "gaussian_blur_filter.h"

class AlgoFilterFactory
{
public:
	static AlgoFilter* GetFilter(std::string name);
};

#endif


