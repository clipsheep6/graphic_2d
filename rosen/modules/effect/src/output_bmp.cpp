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
			GLint iViewport[4]; // ��ͼ��С
			glGetIntegerv(GL_VIEWPORT, iViewport);
			ImageWidth = iViewport[2]; 
			ImageHeight = iViewport[3];
		}

		//������ɫ�����ڴ�
		RGBColor* ColorBuffer = new RGBColor[ImageWidth * ImageHeight];

		//glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		//��ȡ���أ�ע������ĸ�ʽ�� BGR��
		glReadPixels(0, 0, ImageWidth, ImageHeight, GL_BGR, GL_UNSIGNED_BYTE, ColorBuffer);

		//������д���ļ�
		WriteBMP(imageName_.c_str(), ColorBuffer, ImageWidth, ImageHeight);

		//������ɫ�����ڴ�
		delete[] ColorBuffer;
	}
}

void OutFilterBMP::WriteBMP(const char* FileName, RGBColor* ColorBuffer, int ImageWidth, int ImageHeight)
{
	//��ɫ�����ܳߴ磺
	const int ColorBufferSize = ImageHeight * ImageWidth * sizeof(RGBColor);
	//�ļ�ͷ
	BITMAPFILEHEADER fileHeader;
	fileHeader.bfType = 0x4D42;	//0x42��'B'��0x4D��'M'
	fileHeader.bfReserved1 = 0;
	fileHeader.bfReserved2 = 0;
	fileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + ColorBufferSize;
	fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	//��Ϣͷ
	BITMAPINFOHEADER bitmapHeader = { 0 };
	bitmapHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapHeader.biHeight = ImageHeight;
	bitmapHeader.biWidth = ImageWidth;
	bitmapHeader.biPlanes = 1;
	bitmapHeader.biBitCount = 24;
	bitmapHeader.biSizeImage = ColorBufferSize;
	bitmapHeader.biCompression = 0; //BI_RGB
	FILE* fp;//�ļ�ָ��

	//���ļ���û���򴴽���
	fopen_s(&fp, FileName, "wb");

	//д���ļ�ͷ����Ϣͷ
	fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	fwrite(&bitmapHeader, sizeof(BITMAPINFOHEADER), 1, fp);

	//д����ɫ����
	fwrite(ColorBuffer, ColorBufferSize, 1, fp);

	fclose(fp);
}

