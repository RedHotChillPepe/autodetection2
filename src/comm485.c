#include <stdio.h>       //标准输入输出定义
#include <stdlib.h>      //标准库函数定义
#include <unistd.h>      //UNIX标准函数定义
#include <sys/types.h>  //基本系统数据类型
#include <sys/stat.h>  //获取一些文件相关的信息
#include <fcntl.h>    //文件控制定义
#include <errno.h>  //错误号定义
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <termios.h>
#include "memmap.h"
#include "comm485.h"
#include "gsf_base.h"

#define GSF_RETURN_FAIL -1
#define GSF_RETURN_OK    0


int gDefRs485CommNo = 1;  //默认串口1(ttyAMA1) 作为RS485串口
unsigned int nChipType = 0;

static const int speed_arr[14] = {
	B50,   B75,   B110,  B150,   B300,   B600,   B1200,  
	B2400, B4800, B9600, B19200, B38400, B57600, B115200
};

int gsf_gpio_read_reg(int addr)
{
	void* pMem  = NULL;
    int len = sizeof(int);
	unsigned int data = 0;
	pMem = memmap(addr, len);
	if (NULL == pMem)
	{
		printf("memmap addr %x err\n", addr);	
		return -1;
	}

	data = *((int *)(pMem));
	memunmap(pMem);
	return data;
}

int gsf_gpio_write_reg(int addr, int value)
{
	void* pMem  = NULL;
    int len = sizeof(int);
	pMem = memmap(addr, len);
	if (NULL == pMem)
	{
		printf("memmap addr %x err\n", addr);	
		return -1;
	}

	*((int *)(pMem)) = value;
	memunmap(pMem);
	return 0;
}

int gsf_gpio_set_uart_rts(int nUartNo, int nWrite)
{
	if(nChipType == GSF_CHIP_3516AV3 || nChipType == GSF_CHIP_3516DV3)
	{
		unsigned int u32RegAddr = 0x120d8040; 
		unsigned int u32Reg = 0;

		u32RegAddr = 0x120d8040; 
		u32Reg = gsf_gpio_read_reg(u32RegAddr); // GPIO8_4
		if (nWrite == 0 &&(( u32Reg & (1 << 4))	== 0))
			return 0;
		if (nWrite == 1 && (( u32Reg & (1 << 4)) != 0))
			return 0;
		 
		if (nWrite == 0)
			u32Reg &= (~(1 << 4)); //rts信号模式取正常模式 ：正常模式为读
		else
			u32Reg |= (1 << 4); //rts信号模式取反模式 ：取反模式为写
		gsf_gpio_write_reg(u32RegAddr, u32Reg);		
			
		usleep(100);
		return 0;
	}else if(nChipType == GSF_CHIP_3516EV3 || nChipType == GSF_CHIP_3516EV2)
	{
		GSF_U32 u32RegAddr = 0x120b1200; 
		GSF_U32 u32Reg = 0;
		
   		u32Reg = gsf_gpio_read_reg(u32RegAddr); // GPIO1_7
		if (nWrite == 0 &&(( u32Reg & (1 << 7))	== 0))
			return 0;
		if (nWrite == 1 && (( u32Reg & (1 << 7)) != 0))
			return 0;
		 
		if (nWrite == 0)
			u32Reg &= (~(1 << 7)); //rts信号模式取正常模式 ：正常模式为读
		else
			u32Reg |= (1 << 7); //rts信号模式取反模式 ：取反模式为写
		gsf_gpio_write_reg(u32RegAddr, u32Reg);		
			
		usleep(100);
		return 0;
	}else if(nChipType == GSF_CHIP_7205V3 || nChipType == GSF_CHIP_7205V2 || nChipType == GSF_CHIP_7605V1)
	{
		GSF_U32 u32RegAddr = 0x120b1200; 
		GSF_U32 u32Reg = 0;

   		u32Reg = gsf_gpio_read_reg(u32RegAddr); // GPIO1_7
		if (nWrite == 0 &&(( u32Reg & (1 << 7))	== 0))
			return 0;
		if (nWrite == 1 && (( u32Reg & (1 << 7)) != 0))
			return 0;
		 
		if (nWrite == 0)
			u32Reg &= (~(1 << 7)); //rts信号模式取正常模式 ：正常模式为读
		else
			u32Reg |= (1 << 7); //rts信号模式取反模式 ：取反模式为写
		gsf_gpio_write_reg(u32RegAddr, u32Reg);		

		usleep(100);
		return 0;
	}else if(nChipType == GSF_CHIP_SS928)
	{
		GSF_U32 u32RegAddr = 0x11095100;  // GPIO5_6
		GSF_U32 u32Reg = 0;

   		u32Reg = gsf_gpio_read_reg(u32RegAddr); // GPIO1_7
		if (nWrite == 0 &&(( u32Reg & (1 << 6))	== 0))
			return 0;
		if (nWrite == 1 && (( u32Reg & (1 << 6)) != 0))
			return 0;
		 
		if (nWrite == 0)
			u32Reg &= (~(1 << 6)); //rts信号模式取正常模式 ：正常模式为读
		else
			u32Reg |= (1 << 6); //rts信号模式取反模式 ：取反模式为写
		gsf_gpio_write_reg(u32RegAddr, u32Reg);		

		usleep(100);
		return 0;
	}else if(nChipType == GSF_CHIP_3519DV5 || nChipType == GSF_CHIP_3516DV5)
	{
		GSF_U32 u32RegAddr = 0x11098080;  // GPIO8_5
		GSF_U32 u32Reg = 0;

   		u32Reg = gsf_gpio_read_reg(u32RegAddr); // GPIO1_7
		if (nWrite == 0 &&(( u32Reg & (1 << 5))	== 0))
			return 0;
		if (nWrite == 1 && (( u32Reg & (1 << 5)) != 0))
			return 0;
		 
		if (nWrite == 0)
			u32Reg &= (~(1 << 5)); //rts信号模式取正常模式 ：正常模式为读
		else
			u32Reg |= (1 << 5); //rts信号模式取反模式 ：取反模式为写
		gsf_gpio_write_reg(u32RegAddr, u32Reg);		

		usleep(100);
		return 0;
	}

	return 0;
}
//设置参数
int set_opt(int fd, COMM_PARA_S *pStComm, int nVTime, int nVMin)
{
	struct termios opt_new;
	if (tcgetattr (fd, &opt_new) != 0)
	{
		//perror("tcsetattr fd1");
		return COMM_GET_OPT_ERR;
	}
	
	printf("baudrate(%d)--databit(%d)--checkType(%d)--stopbit(%d)--\n",pStComm->baudrate,pStComm->databit,pStComm->checkType,pStComm->stopbit);
	
	tcflush(fd,  TCIOFLUSH);	//刷新输入输出缓冲
	cfsetispeed(&opt_new, speed_arr[pStComm->baudrate%14]);	//这里分别设置                 
	cfsetospeed(&opt_new, speed_arr[pStComm->baudrate%14]); 

	opt_new.c_cflag |= CLOCAL | CREAD;
	opt_new.c_cflag &= ~CSIZE;	//这是设置c_cflag选项不按位数据位掩码
	switch (pStComm->databit) /*设置数据位数*/                     
	{
	case 0:
		opt_new.c_cflag |= CS5;					//设置c_cflag选项数据位为5位
		break;
	case 1:
		opt_new.c_cflag |= CS6;					//设置c_cflag选项数据位为6位
		break;
	case 2:
		opt_new.c_cflag |= CS7;					//设置c_cflag选项数据位为7位
		break;
	case 3:
		opt_new.c_cflag |= CS8;					//设置c_cflag选项数据位为8位
		break;
	default:
	    opt_new.c_cflag |= CS8;			//设置c_cflag选项数据位为8位
	    break;
    }

	switch (pStComm->checkType)								//设置奇偶校验，c_cflag和c_iflag有效
    {   
	case 0:	//'n'								//无校验  当然都不选
		opt_new.c_cflag &= ~PARENB;				/* Clear parity enable */
		opt_new.c_iflag &= ~INPCK;				/* Enable parity checking */
		break;
	case 1:		   								//奇校验 其中PARENB校验位有效；PARODD奇校验,INPCK检查校验
		opt_new.c_cflag |= (PARODD | PARENB);	/* 设置为奇效验*/ 
		opt_new.c_iflag |= INPCK;				/* Disnable parity checking */
		break;
		
	case 2:									//偶校验，奇校验不选就是偶校验了
		opt_new.c_cflag |= PARENB;				/* Enable parity */
		opt_new.c_cflag &= ~PARODD;				/* 转换为偶效验*/  
		opt_new.c_iflag |= INPCK;				/* Disnable parity checking */
		break;
	default:
		opt_new.c_cflag &= ~PARENB;				/* Clear parity enable */
		opt_new.c_iflag &= ~INPCK;				/* Enable parity checking */
		break;
    }	

	/* 设置停止位*/   
	switch (pStComm->stopbit)			//这是设置停止位数，影响的标志是c_cflag
    {
	case 0:
		opt_new.c_cflag &= ~CSTOPB;//不指明表示一位停止位
		break;
	case 1:
		opt_new.c_cflag |= CSTOPB;//指明CSTOPB表示两位，只有两种可能
		break;
	default:
		opt_new.c_cflag &= ~CSTOPB;	//不指明表示一位停止位
		break;
    }
	//这个地方是用来设置控制字符和超时参数的，一般默认即可。
	//稍微要注意的是c_cc数组的VSTART 和 VSTOP 元素被设定成DC1 和 DC3,
	//ASCII 标准的XON和XOFF字符。所以如果在传输这两个字符的时候就传不过去，
	//这时需要把软件流控制屏蔽 options.c_iflag &= ~(IXON | IXOFF | IXANY);
    opt_new.c_iflag &= ~(IGNBRK | IGNCR | INLCR | ICRNL | IUCLC | IXANY | IXON | IXOFF | INPCK | ISTRIP);
    opt_new.c_iflag |= (BRKINT | IGNPAR);
    opt_new.c_oflag &= ~OPOST;
    opt_new.c_lflag &= ~(XCASE|ECHONL|NOFLSH);
    opt_new.c_lflag &= ~(ICANON | ISIG | ECHO);	
//    opt_new.c_lflag |= CRTSCTS;
	
	opt_new.c_cc[VTIME] = nVTime;
	opt_new.c_cc[VMIN]  = nVMin;

	tcflush (fd, TCIFLUSH);
	if ((tcsetattr (fd, TCSANOW, &opt_new)) != 0)
	{
		printf("[%s %d]comm_set_opt set error\n",__func__,__LINE__);
		return COMM_SET_OPT_ERR;
	}	
	return 0;
	
}

//写入完成
int gsf_gpio_wait_uart_write_ok(int nUartNo)
{
	int count = 100;
	int nRegAddr = 0;
	unsigned int u32Reg = 0;
	do
	{
		usleep(100);
#if (defined GSFSS928_ARCH)
		if (nUartNo == 1)
			nRegAddr = 0x11041018;
		else
			nRegAddr = 0x11044018;
		u32Reg = gsf_gpio_read_reg(nRegAddr);	// UART1 UART_FR
#elif (defined GSF3519DV5_ARCH)
		if (nUartNo == 1)
			nRegAddr = 0x11041018;
		else
			nRegAddr = 0x11045018;
		u32Reg = gsf_gpio_read_reg(nRegAddr);	// UART1 UART_FR
#endif
		count--;
	}while ((u32Reg & 0x00000088) != 0x80 && count);	//busy=0，txfe=1则发送完成

	usleep(100);
	return (count == 0)?-1:0;
}

int comm485_prepare(COMM_PARA_S *param)
{
	int fd = -1;
	int nRet = -1;
	char szDev[32] = "";
	GSF_DEV_CFG_S devCfg = {0};

	if (0 != (nRet = DEVSDK_ParamConfig(0, GSF_GET_PARAM_MSG, GSF_VER_PARAM, &devCfg, sizeof(GSF_DEV_CFG_S))))
	{
		printf("[%s %d] get dev param err(%d)\n", __func__, __LINE__, nRet);
		return -1;
	}
	//printf("chipType=%#x\n",devCfg.u32ChipType);
	nChipType = devCfg.u32ChipType;
	
	if( nChipType == GSF_CHIP_3516AV3 || nChipType == GSF_CHIP_3516DV3 || nChipType == GSF_CHIP_SS928 || 
		nChipType == GSF_CHIP_3516DV5 || nChipType == GSF_CHIP_3519DV5 )
		strcpy(szDev,"/dev/ttyAMA1");
	else if(nChipType == GSF_CHIP_3516AV2 || nChipType == GSF_CHIP_3519)
		strcpy(szDev,"/dev/ttyAMA3");
	else
		strcpy(szDev,"/dev/ttyAMA2");

	if(0 > (fd = open(szDev, O_RDWR | O_NOCTTY | O_NDELAY)))
	{
		printf("[%s %d]comm open err fd(%d)!!!\n",__func__,__LINE__,fd);
		return GSF_RETURN_FAIL;
	}
	if(0 != set_opt(fd, param, 0, 0))
	{
		printf("[%s %d]comm param set err!!!\n",__func__,__LINE__);
		return GSF_RETURN_FAIL;
	}

	return fd;
}

int comm485_write(int fd ,char *writeMsg)
{
	gsf_gpio_set_uart_rts(gDefRs485CommNo,1);
	if(write(fd,writeMsg,strlen(writeMsg)) < 0)
	{
		printf("[%s %d]comm write err!!!\n",__func__,__LINE__);
		return -1;
	}
	printf("WriteBuff=%s\n",writeMsg);
	if (0 == gsf_gpio_wait_uart_write_ok(gDefRs485CommNo))
	{
		printf("[%s %d]comm write finsh!!!\n",__func__,__LINE__);
		gsf_gpio_set_uart_rts(gDefRs485CommNo,0);
	}
	return 0;
}

int comm485_read(int fd ,char *readMsg)
{
	int ret = 0;
	gsf_gpio_set_uart_rts(gDefRs485CommNo,0);
	ret = gsf_select(&fd , 0x1, 0x1, 0);
	if(ret == (fd | 0x10000))
	{
		if(read(fd,readMsg,sizeof(readMsg)) < 0)
		{
			printf("[%s %d]comm read err!!!\n",__func__,__LINE__);
			return -1;
		}
	}
	return 0;
}

