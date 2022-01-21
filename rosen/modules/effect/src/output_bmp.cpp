#include "output_bmp.h"

OutFilterBMP::OutFilterBMP():Output()
{
}

void OutFilterBMP::DoProcess(ProcessDate& data)
{
	RenderOnScreen(data.srcTextureID_);
	if (saveResult_) 
	{
		//SaveRenderResultToBMP(data.textureWidth_, data.textureHeight_);
		SaveRenderResultToBMP(imageWidth_, imageHeight_);
		saveResult_ = false;
	}
	Filter* end = nullptr;
	AddNextFilter(end);
}

void OutFilterBMP::SaveRenderResultToBMP(int ImageWidth, int ImageHeight)
{
	if (!imageName_.empty())
	{
		imageName_ += ".bmp";
		
		if (ImageWidth == -1 || imageHeight_ == -1) {
			GLint iViewport[4]; // 视图大小
			glGetIntegerv(GL_VIEWPORT, iViewport);
			ImageWidth = iViewport[2]; 
			ImageHeight = iViewport[3];
		}

		//申请颜色数据内存
		RGBColor* ColorBuffer = new RGBColor[ImageWidth * ImageHeight];

		//glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		//读取像素（注意这里的格式是 BGR）
		glReadPixels(0, 0, ImageWidth, ImageHeight, GL_BGR, GL_UNSIGNED_BYTE, ColorBuffer);

		//将数据写入文件
		WriteBMP(imageName_.c_str(), ColorBuffer, ImageWidth, ImageHeight);

		//清理颜色数据内存
		delete[] ColorBuffer;
	}
}

void OutFilterBMP::WriteBMP(const char* FileName, RGBColor* ColorBuffer, int ImageWidth, int ImageHeight)
{
	//颜色数据总尺寸：
	const int ColorBufferSize = ImageHeight * ImageWidth * sizeof(RGBColor);
	//文件头
	BITMAPFILEHEADER fileHeader;
	fileHeader.bfType = 0x4D42;	//0x42是'B'；0x4D是'M'
	fileHeader.bfReserved1 = 0;
	fileHeader.bfReserved2 = 0;
	fileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + ColorBufferSize;
	fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	//信息头
	BITMAPINFOHEADER bitmapHeader = { 0 };
	bitmapHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapHeader.biHeight = ImageHeight;
	bitmapHeader.biWidth = ImageWidth;
	bitmapHeader.biPlanes = 1;
	bitmapHeader.biBitCount = 24;
	bitmapHeader.biSizeImage = ColorBufferSize;
	bitmapHeader.biCompression = 0; //BI_RGB
	FILE* fp;//文件指针

	//打开文件（没有则创建）
	fopen_s(&fp, FileName, "wb");

	//写入文件头和信息头
	fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	fwrite(&bitmapHeader, sizeof(BITMAPINFOHEADER), 1, fp);

	//写入颜色数据
	fwrite(ColorBuffer, ColorBufferSize, 1, fp);

	fclose(fp);
}

