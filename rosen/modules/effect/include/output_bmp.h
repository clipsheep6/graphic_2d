#ifndef OUTPUT_BMP_H
#define OUTPUT_BMP_H

#include "output.h"
#pragma pack(2)//Ӱ���ˡ����롱������ʵ��ǰ�� sizeof(BITMAPFILEHEADER) �Ĳ��
typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;
typedef long    LONG;

//BMP�ļ�ͷ��
struct BITMAPFILEHEADER
{
    WORD  bfType;		//�ļ����ͱ�ʶ������ΪASCII�롰BM��
    DWORD bfSize;		//�ļ��ĳߴ磬��byteΪ��λ
    WORD  bfReserved1;	//�����֣�����Ϊ0
    WORD  bfReserved2;	//�����֣�����Ϊ0
    DWORD bfOffBits;	//һ����byteΪ��λ��ƫ�ƣ���BITMAPFILEHEADER�ṹ�忪ʼ��λͼ����
};

//BMP��Ϣͷ��
struct BITMAPINFOHEADER
{
    DWORD biSize;			//����ṹ��ĳߴ�
    LONG  biWidth;			//λͼ�Ŀ��
    LONG  biHeight;			//λͼ�ĳ���
    WORD  biPlanes;			//The number of planes for the target device. This value must be set to 1.
    WORD  biBitCount;		//һ�������м�λ
    DWORD biCompression;    //0����ѹ����1��RLE8��2��RLE4
    DWORD biSizeImage;      //4�ֽڶ����ͼ�����ݴ�С
    LONG  biXPelsPerMeter;  //������/�ױ�ʾ��ˮƽ�ֱ���
    LONG  biYPelsPerMeter;  //������/�ױ�ʾ�Ĵ�ֱ�ֱ���
    DWORD biClrUsed;        //ʵ��ʹ�õĵ�ɫ����������0��ʹ�����еĵ�ɫ������
    DWORD biClrImportant;	//��Ҫ�ĵ�ɫ����������0�����еĵ�ɫ����������Ҫ
};

//һ�����ص���ɫ��Ϣ
struct RGBColor
{
    char B;		//��
    char G;		//��
    char R;		//��
    char A;		//͸��
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
