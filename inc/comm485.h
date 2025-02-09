#ifndef __L_Comm485_H__
#define __L_Comm485_H__


typedef struct _COMM_PARA_S_
{
	unsigned int	baudrate;	//波特率 	COMM_BAUDRATE_E
	unsigned char	databit;	//数据位 	COMM_DATABIT_E
	unsigned char	stopbit;	//停止位 	COMM_CHECKTYPE_E
	unsigned char	checkType;	//检验类型 	COMM_CHECKTYPE_E
	unsigned char	flowctrl; 	//软流控/硬流控/无流控		备用	
}COMM_PARA_S;

//串口错误
typedef enum _COMM_ERR_E
{
	COMM_SUCCESS     =  0,		//串口正常
	COMM_PARAM_ERR   = -1,		//串口参数错误
	COMM_OPEN_ERR    = -2,		//串口打开错误
	COMM_GET_OPT_ERR = -3,		//串口选择错误
	COMM_SET_OPT_ERR = -4,		//串口设置选择错误
	COMM_WR_DATA_ERR = -5,		//串口写数据错误
	COMM_RD_DATA_ERR = -6,		//串口读数据错误
	COMM_NOT_OPEN_ERR= -7,		//串口没有打开
}COMM_ERR_S;

//波特率
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

//数据位
typedef enum _COMM_DATABIT_E
{
	COMM_DATABIT_5    = 0,		//5位
	COMM_DATABIT_6    = 1,		//6位
	COMM_DATABIT_7    = 2,		//7位
	COMM_DATABIT_8	  = 3,		//8位
}COMM_DATABIT_E;

//校验类型
typedef enum _COMM_CHECKTYPE_E
{
	COMM_CHECKTYPE_NONE   = 0,		//无
	COMM_CHECKTYPE_ODD    = 1,		//奇校验
	COMM_CHECKTYPE_EVEN   = 2,		//偶校验
}COMM_CHECKTYPE_E;

//停止位
typedef enum _COMM_STOPBIT_E
{
	COMM_STOPBIT_ONE   = 0,		//一位
	COMM_STOPBIT_TWO    = 1,	//两位
}COMM_STOPBIT_E;


int comm485_prepare(COMM_PARA_S *param);
int comm485_write(int fd,char *writeMsg);
int comm485_read(int fd,char *readMsg);

#endif
