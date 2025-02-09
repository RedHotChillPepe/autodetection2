/*
 * ��ץ��ͨ���л�ȡJPEGͼƬ
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <malloc.h>
#include <sys/select.h>
#include "hi_math.h"
#include "hi_mpi_venc.h"

#define VENC_JPG_CHN 2
#define VENC_JPG_MAX_PACKET 8
#define VENC_JPG_MAX_BUF_SIZE 2097152 //most of time 2M Buffer is enought, becareful. 

char *pPackInfoBuf = NULL;
char *pJpegBuf = NULL;
//��ʼ��jpeg����
int get_jpeg_init_buffer()
{
	if ((pPackInfoBuf = malloc(VENC_JPG_MAX_PACKET * sizeof(hi_venc_pack))) == NULL)	
	{
		printf("get jpeg malloc pack info buffer fail\n");
		return -1;	
	}	
	
	if ((pJpegBuf = malloc(VENC_JPG_MAX_BUF_SIZE)) == NULL)	
	{
		printf("get jpeg malloc jpg buffer fail\n");
		return -1;	
	}		
	return 0;
}
//����jpeg����
int get_jpeg_destroy_buffer()
{
	if (pPackInfoBuf != NULL)
	{
		free(pPackInfoBuf);
	}	
	
	if (pJpegBuf != NULL)
	{
		free(pJpegBuf);
	}		
	return 0;	
}
//��ȡjpeg׼��
int get_jpeg_prepare()
{
	hi_venc_start_param recv_param = {0};
	recv_param.recv_pic_num = -1;
	int s32Ret = 0;

	if (get_jpeg_init_buffer() < 0)
  		return -1;  	

  	if ((s32Ret = hi_mpi_venc_reset_chn(VENC_JPG_CHN)) != HI_SUCCESS)	//��λJPGͨ��
	{
		printf("venc chn %d hi_mpi_venc_reset_chn err %#x.\n", VENC_JPG_CHN, s32Ret);
		return s32Ret;
	}		
	
	if ((s32Ret =  hi_mpi_venc_start_chn(VENC_JPG_CHN, &recv_param)) != HI_SUCCESS)	//����JPG����ͨ����������ͼ��
	{
	    printf("start venc chn %d hi_mpi_venc_start_chn is err: %#x.\n", VENC_JPG_CHN, s32Ret);
      return s32Ret;
	}	
	
	
	return 0;
}
//��ȡjpeg����
int get_jpeg_finish()
{   
	int s32Ret = 0;	
	if ((s32Ret =  hi_mpi_venc_stop_chn(VENC_JPG_CHN)) != HI_SUCCESS)	//ֹͣjpeg����ͨ����������ͼ��
	{
	    printf("start venc chn %d hi_mpi_venc_stop_chn is err: %#x.\n", VENC_JPG_CHN, s32Ret);
      return s32Ret;
	}    	
	
	if ((s32Ret = hi_mpi_venc_reset_chn(VENC_JPG_CHN)) != HI_SUCCESS)	//��λjpegͨ��
	{
		printf("venc chn %d hi_mpi_venc_reset_chn err %#x.\n", VENC_JPG_CHN, s32Ret);
		return s32Ret;
	}		
	get_jpeg_destroy_buffer();	//����jpeg����
	return 0;
}
//�ӱ������л�ȡͼƬ
int get_jpeg_pic_from_encoder()
{
	int nVencFd = hi_mpi_venc_get_fd(VENC_JPG_CHN);	//��ȡ����ͨ����Ӧ���豸�ļ����
	fd_set			read_fds;
	struct timeval	TimeoutVal; 
	hi_venc_chn_status status = {0};
	hi_venc_stream	  stream = {0};
	int nDataOffset = 0; 
	int index = 0;
	int s32Ret = 0;	
	
	FD_ZERO(&read_fds);
	FD_SET(nVencFd, &read_fds);
	
	TimeoutVal.tv_sec  = 1;
	TimeoutVal.tv_usec = 0;
    
	if((s32Ret = select(nVencFd + 1, &read_fds, NULL, NULL, &TimeoutVal)) <= 0)
	{
     printf("Venc get stream select nVencChn:%d. s32Ret %0#x\n", VENC_JPG_CHN, s32Ret);
		return s32Ret;
	}

	if((FD_ISSET(nVencFd, &read_fds) == 0))
	{
	   printf("FD_ISSET err\n");
		return -1;
  }
    
	memset(&stream, 0, sizeof(hi_venc_stream));
	memset(&status,   0, sizeof(status));
	if ((s32Ret = hi_mpi_venc_query_status(VENC_JPG_CHN, &status)) != HI_SUCCESS) //��ѯ����ͨ��״̬
	{
		 printf("Venc Chn %d Venc Query Err %#x\n", VENC_JPG_CHN, s32Ret);	
     usleep(10000);
		 return s32Ret;
	}
	
	if (status.cur_packs > VENC_JPG_MAX_PACKET)
	{
			printf("Venc Chn %d Venc Query pack number %d too big\n", VENC_JPG_CHN, status.cur_packs);	
			return s32Ret;
	}	
	
	//---------------------------------
	//��ȡ������Ϣ
	stream.pack = (hi_venc_pack *)(pPackInfoBuf);
	stream.pack_cnt = status.cur_packs;

	if ((s32Ret = hi_mpi_venc_get_stream(VENC_JPG_CHN, &stream, HI_TRUE)) != HI_SUCCESS) //��ȡjpeg����
	{
		printf("Venc Chn %d GetStream Err %#x\n", VENC_JPG_CHN, s32Ret);
		return s32Ret;
	}
	//---------------------------------
	//��������

    for (index = 0; index < stream.pack_cnt; index++)
    {
        if (stream.pack[index].len > 0)
        {
        		memcpy(pJpegBuf+nDataOffset, 
        					(char *)(stream.pack[index].addr + stream.pack[index].offset),
        					stream.pack[index].len - stream.pack[index].offset);
        		nDataOffset += stream.pack[index].len - stream.pack[index].offset;			         
        }
    }   
    if (nDataOffset > 2)
			printf("get jpeg picture size %d -- 0x%x 0x%x 0x%x 0x%x\n", 
					nDataOffset, pJpegBuf[0], pJpegBuf[1], pJpegBuf[nDataOffset-2], pJpegBuf[nDataOffset-1]);
		else
			printf("get jpeg picture fail\n");
	//------------------------------------------------------
	//�ͷ�����
	if ((s32Ret = hi_mpi_venc_release_stream(VENC_JPG_CHN, &stream)) != HI_SUCCESS) //�ͷ���������
	{
		printf("Venc Chn %d ReleaseStream Err %#x\n", VENC_JPG_CHN, s32Ret);
		return s32Ret;
	}
	 /*
	FILE *fp = NULL;
	static int i = 0;
	char szName[32] = {0};
	
	if (++i % 10 == 0)
	{	
		sprintf(szName, "/tmp/%d.jpg", i);
	
		if ((fp = fopen(szName, "wb")) != NULL)
		{
			fwrite(pJpegBuf, nDataOffset, 1, fp);	
		}	
		fclose(fp);
	}*/
	return 0;
}
