#ifndef INPUT_H
#define INPUT_H

#include "filter.h"
#include "stb_image.h"
#include "mesh.h"

#include <vector>
#include <algorithm>

class Input :public Filter
{
public:
	Input() = default;
	Input(std::string imageURL);
	void LoadImageFromFileName(std::string imageURL);
	int GetImageWidth();
	int GetImageHeight();
	void DoProcess(ProcessDate& data) override;
	virtual filter_type GetFilterType()override;
protected:
	virtual void BindImageOnTexture(GLuint TextureID) = 0;
	std::string imageURL_;
	int textureWidth_ = -1;
	int textureHeight_ = -1;
private:
	friend class ImageChain;
};
#endif
