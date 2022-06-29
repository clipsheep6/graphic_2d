#ifndef INPUT_JPG_H
#define INPUT_JPG_H

#include "input.h"

class InputJPG :public Input
{
public:
	InputJPG() = default;
	InputJPG(std::string imageURL);
private:
	void BindImageOnTexture(GLuint TextureID) override;
};

#endif
