#ifndef OUTPUT_BMP_H
#define OUTPUT_BMP_H

#include "output.h"
#pragma pack(2)//影响了“对齐”。可以实验前后 sizeof(BITMAPFILEHEADER) 的差别
typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;
typedef long    LONG;

//BMP文件头：
struct BITMAPFILEHEADER
{
    WORD  bfType;		//文件类型标识，必须为ASCII码“BM”
    DWORD bfSize;		//文件的尺寸，以byte为单位
    WORD  bfReserved1;	//保留字，必须为0
    WORD  bfReserved2;	//保留字，必须为0
    DWORD bfOffBits;	//一个以byte为单位的偏移，从BITMAPFILEHEADER结构体开始到位图数据
};

//BMP信息头：
struct BITMAPINFOHEADER
{
    DWORD biSize;			//这个结构体的尺寸
    LONG  biWidth;			//位图的宽度
    LONG  biHeight;			//位图的长度
    WORD  biPlanes;			//The number of planes for the target device. This value must be set to 1.
    WORD  biBitCount;		//一个像素有几位
    DWORD biCompression;    //0：不压缩，1：RLE8，2：RLE4
    DWORD biSizeImage;      //4字节对齐的图像数据大小
    LONG  biXPelsPerMeter;  //用象素/米表示的水平分辨率
    LONG  biYPelsPerMeter;  //用象素/米表示的垂直分辨率
    DWORD biClrUsed;        //实际使用的调色板索引数，0：使用所有的调色板索引
    DWORD biClrImportant;	//重要的调色板索引数，0：所有的调色板索引都重要
};

//一个像素的颜色信息
struct RGBColor
{
    char B;		//蓝
    char G;		//绿
    char R;		//红
    char A;		//透明
};
class OutFilterBMP :public Output
{
public:
    OutFilterBMP();
    void DoProcess(ProcessDate& data) override;
private:
    void SaveRenderResultToBMP(int ImageWidth, int ImageHeight);
    void WriteBMP(const char* FileName, RGBColor* ColorBuffer, int ImageWidth, int ImageHeight);
};

#endif
