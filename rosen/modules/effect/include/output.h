#ifndef OUTPUT_H
#define OUTPUT_H

#include "algo_filter.h"

class Output :public AlgoFilter
{
public:
    Output();
    std::string GetVertexShader() override;
    std::string GetFragmentShader() override;
    virtual filter_type GetFilterType()override;
    void SaveRenderResultToImage(std::string name, int width = -1, int height = -1);
protected:
    void RenderOnScreen(GLuint& RenderTextureID);
    bool saveResult_ = false;
    int imageWidth_;
    int imageHeight_;
    std::string imageName_;
};

#endif

