#ifndef INPUT_PNG_H
#define INPUT_PNG_H

#include "input.h"

class InputPNG :
	public Input
{
public:
	InputPNG() = default;
	InputPNG(std::string imageURL);
private:
	void BindImageOnTexture(GLuint TextureID) override;
};

#endif

