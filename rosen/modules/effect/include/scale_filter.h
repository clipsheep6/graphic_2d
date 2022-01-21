#ifndef SCALE_FILTER_H
#define SCALE_FILTER_H

#include "algo_filter.h"

class ScaleFilter :public AlgoFilter
{
public:
    ScaleFilter();
    std::string GetVertexShader() override;
    std::string GetFragmentShader() override;
    void SetScale(float scale);
private:
    virtual void Prepare(ProcessDate& data) override;
    float scale_ = 1.0f;
    friend class GaussianBlurFilter;
    void DoProcess(ProcessDate& data) override;
};

#endif

