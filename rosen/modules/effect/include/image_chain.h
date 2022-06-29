#ifndef IMAGE_CHAIN_H
#define IMAGE_CHAIN_H

#include "input.h"
#include "algo_filter.h"
#include "cJSON.h"

class ImageChain {
public:
    ImageChain(std::vector< Input* > inputs);
    void Render();
private:
    void CreatTexture(unsigned int& TextureID);
    void SeriesRendering();
    void ParallelRendering();
    bool _flagSeries = false;
    unsigned int _frameBuffer;
    unsigned int _srcTexture;
    unsigned int _outputTexture;
    unsigned int _resultTexture;
    unsigned int _frameBufferTexSrc;
    unsigned int _frameBufferTexDst;
    std::vector< Input* > myInputs_;
    cJSON* pJSON;
};

#endif
