#include "algo_filter_factory.h"

AlgoFilter* AlgoFilterFactory::GetFilter(std::string name)
{
    if (name.compare("GaussianBlur") == 0) {
        return new GaussianBlurFilter();
    }

    if (name.compare("HorizontalBlur") == 0) {
        return new HorizontalBlurFilter();
    }

    if (name.compare("VerticalBlur") == 0) {
        return new VerticalBlurFilter();
    }

    if (name.compare("Scale") == 0) {
        return new ScaleFilter();
    }
    return nullptr;
}
