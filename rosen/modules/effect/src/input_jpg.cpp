#include "input_jpg.h"
InputJPG::InputJPG(std::string imageURL):Input(imageURL)
{
}

void InputJPG::BindImageOnTexture(GLuint TextureID)
{
	glBindTexture(GL_TEXTURE_2D, TextureID);
	stbi_set_flip_vertically_on_load(true);
	int nrChannels;
	std::string imagePath = imageURL_.c_str();
	unsigned char* data = stbi_load(imageURL_.c_str(), &textureWidth_, &textureHeight_, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth_, textureHeight_, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
}


