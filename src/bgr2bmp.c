/*
 * save bgr to bmp picture
 * 20200825: by liyouxing: liyx@szzhst.com
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

//#include "turbojpeg.h"


#ifndef PAD
#define PAD(v, p)  ((v + (p) - 1) & (~((p) - 1)))
#endif

typedef struct	/****?BMP?file?header?structure?****/
{
	unsigned int bfSize;			/*?Size?of?file?*/
	unsigned short bfReserved1;		/*?Reserved?*/
	unsigned short bfReserved2;
	unsigned int bfOffBits;			/*?Offset?to?bitmap?data?*/
}BMP_FILE_HDR_S;

typedef struct						/****?BMP?file?info?structure?****/
{
	unsigned int biSize;			/*?Size?of?info?header?*/
	int biWidth;					/*?Width?of?image?*/
	int biHeight;					/*?Height?of?image?*/
	unsigned short biPlanes;		/*?Number?of?color?planes?*/
	unsigned short biBitCount;		/*?Number?of?bits?per?pixel?*/
	unsigned int biCompression;		/*?Type?of?compression?to?use?*/
	unsigned int biSizeImage;		/*?Size?of?image?data?*/
	int biXPelsPerMeter;			/*?X?pixels?per?meter?*/
	int biYPelsPerMeter;			/*?Y?pixels?per?meter?*/
	unsigned int biClrUsed;			/*?Number?of?colors?used?*/
	unsigned int biClrImportant;	/*?Number?of?important?colors?*/
}BMP_FILE_INFO_S;

/*
 * 注意：因为坐标系的原因，BGR保存为BMP图像后是上下翻转的
 */
void SaveBgrToBmpFile(
		const char *filename, unsigned char *bgr_buf, 
		int width, int height)
{
	//char szFilename[128] = {0};
	int i = 0;
	int bmp_data_size = 0;

	int line_size = 0;
	int line_data_size = 0;
	int padding_size = 0;
	unsigned char  aucPadding[8] = {0};
	
	BMP_FILE_HDR_S stBmpFileHdr;
	BMP_FILE_INFO_S stBmpFileInfo;
	/*?Magic?number?for?file.?It?does?not?fit?in?the?header?structure?due?to?alignment?requirements,?so?put?it?outside?*/
	unsigned short bfType=0x4d42;

	line_data_size  = ((width*24 + 7) / 8); 
	line_size  = ((line_data_size + 3) / 4 *4); // microsoft bmp shoule 4'bytes' aligned	
	padding_size    = line_size - line_data_size;
	if (padding_size <0 || padding_size >= 4)
	{
		printf("padding size(%u) error.\n", padding_size);
		return;
	}	
	bmp_data_size   = line_size * height;


	printf("bmp_data_size(%d) line_data_size(%d) padding_size(%d) wh(%d, %d)\n", 
			bmp_data_size, line_data_size, padding_size, width, height);
	
	stBmpFileHdr.bfSize = 2+sizeof(stBmpFileHdr) + sizeof(stBmpFileInfo) + bmp_data_size;
	stBmpFileHdr.bfReserved1 = 0;
	stBmpFileHdr.bfReserved2 = 0;
	stBmpFileHdr.bfOffBits = 0x36;
	
	stBmpFileInfo.biSize = sizeof(BMP_FILE_INFO_S);
	stBmpFileInfo.biWidth = width;
	stBmpFileInfo.biHeight = height;
	stBmpFileInfo.biPlanes = 1;
	stBmpFileInfo.biBitCount = 24;
	stBmpFileInfo.biCompression = 0;
	stBmpFileInfo.biSizeImage = bmp_data_size;
	stBmpFileInfo.biXPelsPerMeter = 5000;
	stBmpFileInfo.biYPelsPerMeter = 5000;
	stBmpFileInfo.biClrUsed = 0;
	stBmpFileInfo.biClrImportant = 0;

	//snprintf(szFilename, sizeof(szFilename), "%s.bmp", filename);
	//FILE *fp = fopen(szFilename, "wb+");
	FILE *fp = fopen(filename, "wb+");
	if (!fp)
	{
		printf("Could not open file(%s), err %d(%s)\n", filename, errno, strerror(errno));
		return;
	}

	/*Write headers*/
	//写入文件中
	fwrite(&bfType, sizeof(bfType), 1, fp);
	fwrite(&stBmpFileHdr, sizeof(stBmpFileHdr), 1, fp);
	fwrite(&stBmpFileInfo, sizeof(stBmpFileInfo), 1, fp);
	
	for (i = 0; i < height; ++i)
	{
		fwrite(bgr_buf + (line_data_size*i), line_data_size, 1, fp);
		if (padding_size > 0)
		{
			fwrite(aucPadding, padding_size, 1, fp);
		}
	}
	
	fclose(fp);
}

#if 0
int main(int argc, char *argv[])
{
    FILE* fp = NULL;
    unsigned char* pucRead = NULL;
    
    int i = 0;
    unsigned char* pucBgr  = NULL;
    
    pucRead = (unsigned char*) malloc(416*416*6);
    if (NULL == pucRead)
    {
        return -1;    
    }
    
    fp = fopen("bgr_0_package.bgr", "r");
    if (NULL != fp)
    {
        fread(pucRead + (416*416*3), 416*416*3, 1, fp);
        pucBgr   = pucRead;
        pucRead += (416*416*3);
        for (i = 0; i < 416*416; ++i)
        {
            /* 平面 ->  交错*/
            pucBgr[i*3 + 0] = pucRead[i + (416*416*0)];
            pucBgr[i*3 + 1] = pucRead[i + (416*416*1)];
            pucBgr[i*3 + 2] = pucRead[i + (416*416*2)];
            
            /* 交错  -> 平面 */
            // pucBgr[i + (416*416*0)] = pucRead[i*3 + 0];
            // pucBgr[i + (416*416*1)] = pucRead[i*3 + 1];
            // pucBgr[i + (416*416*2)] = pucRead[i*3 + 2];
            
            /* 直接拷贝 */
            // pucBgr[i*3 + 0] = pucRead[i*3 + 0];
            // pucBgr[i*3 + 1] = pucRead[i*3 + 1];
            // pucBgr[i*3 + 2] = pucRead[i*3 + 2];
        }
        SaveBgrToBmpFile("bgr_0_package", pucBgr, 416, 416);
        
        fclose(fp);
    }
    
    return 0;
}
#endif