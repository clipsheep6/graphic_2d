#include "input.h"

void Input::DoProcess(ProcessDate& data)
{
}

filter_type Input::GetFilterType()
{
	return filter_type::INPUT;
}

Input::Input(std::string imageURL)
{
	imageURL_ = imageURL;
}

void Input::LoadImageFromFileName(std::string imageURL)
{
	imageURL_ = imageURL;
}

int Input::GetImageWidth()
{
	return textureWidth_;
}

int Input::GetImageHeight()
{
	return textureHeight_;
}
