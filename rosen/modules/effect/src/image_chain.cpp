//#define STB_IMAGE_IMPLEMENTATION
#include "image_chain.h"
ImageChain::ImageChain(std::vector<Input*> inputs) : myInputs_(inputs)
{
	CreatTexture(_srcTexture);
	CreatTexture(_outputTexture);

	glGenFramebuffers(1, &_frameBuffer);
	if (inputs.size() == 1) {
		_flagSeries = true;
	}
	else
	{
		CreatTexture(_resultTexture);
	}
}

void ImageChain::Render() {
	if (_flagSeries) {
		myInputs_.at(0)->BindImageOnTexture(_srcTexture);
		SeriesRendering();
	}

	//if (!_flagSeries) {
	//	std::cout << "ParallelRendering" << std::endl;
	//	ParallelRendering();
	//}
}


void ImageChain::SeriesRendering() {
	int width = myInputs_.at(0)->GetImageWidth();
	int height = myInputs_.at(0)->GetImageHeight();
	ProcessDate data{ _srcTexture, _outputTexture, _frameBuffer, width, height};
	myInputs_.at(0)->Process(data);
}

void ImageChain::ParallelRendering() {
}

void ImageChain::CreatTexture(unsigned int& TextureID) {
	glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_2D, TextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
}
