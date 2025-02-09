#ifndef __L_Comm485_H__
#define __L_Comm485_H__


typedef struct _COMM_PARA_S_
{
	unsigned int	baudrate;	//������ 	COMM_BAUDRATE_E
	unsigned char	databit;	//����λ 	COMM_DATABIT_E
	unsigned char	stopbit;	//ֹͣλ 	COMM_CHECKTYPE_E
	unsigned char	checkType;	//�������� 	COMM_CHECKTYPE_E
	unsigned char	flowctrl; 	//������/Ӳ����/������		����	
}COMM_PARA_S;

//���ڴ���
typedef enum _COMM_ERR_E
{
	COMM_SUCCESS     =  0,		//��������
	COMM_PARAM_ERR   = -1,		//���ڲ�������
	COMM_OPEN_ERR    = -2,		//���ڴ򿪴���
	COMM_GET_OPT_ERR = -3,		//����ѡ�����
	COMM_SET_OPT_ERR = -4,		//��������ѡ�����
	COMM_WR_DATA_ERR = -5,		//����д���ݴ���
	COMM_RD_DATA_ERR = -6,		//���ڶ����ݴ���
	COMM_NOT_OPEN_ERR= -7,		//����û�д�
}COMM_ERR_S;

//������
typedef enum _COMM_BAUDRATE_E
{
	COMM_BAUDRATE_50    = 0,		//50bps
	COMM_BAUDRATE_75  	= 1,		//75bps
	COMM_BAUDRATE_110   = 2,		//110bps
	COMM_BAUDRATE_150 	= 3,		//150bps
	COMM_BAUDRATE_300 	= 4,		//300bps
	COMM_BAUDRATE_600 	= 5,		//600bps
	COMM_BAUDRATE_1200 	= 6,		//1200bps
	COMM_BAUDRATE_2400	= 7,		//2400bps
	COMM_BAUDRATE_4800	= 8,		//4800bps
	COMM_BAUDRATE_9600	= 9,		//9600bps
	COMM_BAUDRATE_19200	= 10,		//19200bps
	COMM_BAUDRATE_38400	= 11,		//38400bps
	COMM_BAUDRATE_57600	= 12,		//57600bps
	COMM_BAUDRATE_115200= 13,		//115200bps
}COMM_BAUDRATE_E;

//����λ
typedef enum _COMM_DATABIT_E
{
	COMM_DATABIT_5    = 0,		//5λ
	COMM_DATABIT_6    = 1,		//6λ
	COMM_DATABIT_7    = 2,		//7λ
	COMM_DATABIT_8	  = 3,		//8λ
}COMM_DATABIT_E;

//У������
typedef enum _COMM_CHECKTYPE_E
{
	COMM_CHECKTYPE_NONE   = 0,		//��
	COMM_CHECKTYPE_ODD    = 1,		//��У��
	COMM_CHECKTYPE_EVEN   = 2,		//żУ��
}COMM_CHECKTYPE_E;

//ֹͣλ
typedef enum _COMM_STOPBIT_E
{
	COMM_STOPBIT_ONE   = 0,		//һλ
	COMM_STOPBIT_TWO    = 1,	//��λ
}COMM_STOPBIT_E;


int comm485_prepare(COMM_PARA_S *param);
int comm485_write(int fd,char *writeMsg);
int comm485_read(int fd,char *readMsg);

#endif
