#include "gaussian_blur_filter.h"
GaussianBlurFilter::GaussianBlurFilter()
{
	narrowFilter_ = new ScaleFilter();
	narrowFilter_->SetScale(0.5);
	enlargeFilter_ = new ScaleFilter();
	enlargeFilter_->SetScale(2.0);
	horizontalBlurFilter_ = new HorizontalBlurFilter();
	verticalBlurFilter_ = new VerticalBlurFilter();
}

GaussianBlurFilter::~GaussianBlurFilter()
{
	delete narrowFilter_;
	delete enlargeFilter_;
	delete horizontalBlurFilter_;
	delete verticalBlurFilter_;
}

void GaussianBlurFilter::DoProcess(ProcessDate& data)
{
	narrowFilter_->Process(data);
	std::swap(data.srcTextureID_, data.outputTextureID_);
	horizontalBlurFilter_->Process(data);
	std::swap(data.srcTextureID_, data.outputTextureID_);
	verticalBlurFilter_->Process(data);
	std::swap(data.srcTextureID_, data.outputTextureID_);
	enlargeFilter_->Process(data);
	std::swap(data.srcTextureID_, data.outputTextureID_);
}

void GaussianBlurFilter::SetValue(std::string key, void* value, int size)
{
	horizontalBlurFilter_->SetValue(key, value, size);
	verticalBlurFilter_->SetValue(key, value, size);
}

std::string GaussianBlurFilter::GetVertexShader()
{
	return std::string();
}

std::string GaussianBlurFilter::GetFragmentShader()
{
	return std::string();
}
