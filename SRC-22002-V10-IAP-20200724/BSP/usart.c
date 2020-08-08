/*******************************************************************************
* File Name: usart.c
*
* Description:
* 本文件提供USART1驱动的配置函数，请根据需要修改参数
* Note:
*
********************************************************************************/
#include "usart.h"
#include "ads8698.h"
#include "string.h"

u8 g_usartTxByte[200]={0,0};
u8 g_usartRxByte[200]={0,0};
u8 RXCounter=0;
u8 g_usartRxFlag =0;
extern u8 Sensor1_flag;
extern u8 Sensor2_flag;
u8 Comman_state;


/*************************************************************
函数名称:USART_Config
函数原型:void USART_Config(void)
功    能: 初始化串口2，PA9为tx,PA10为rx, 115200波特率，8个数据位，1个停止位�
奁媾夹Ｑ�,无硬件流控制。
参    数:无
返 回 值:无
调用函数:无
*************************************************************/
void USART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;


	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//使能GPIOA的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能USART1的时钟
	USART_DeInit(USART1);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	//初始化PA2为复用推挽输出 USART TX

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	//初始化PA3为浮空输入 USART RX

	
	USART_InitStructure.USART_BaudRate = BAUDRATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl= USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	

	//⑤初始化 NVIC
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级 0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; //子优先级 1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ 通道使能
	NVIC_Init(&NVIC_InitStructure); //根据指定的参数初始化 VIC 寄存器
  USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//开启空闲中断
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断
  USART_ClearITPendingBit(USART1,USART_IT_RXNE); //清除接收中断标志
  USART_ClearFlag(USART1, USART_FLAG_TC);//清除发送中断标志
	USART_Cmd(USART1, ENABLE);
}



/*************************************************************
函数名称:USART2_IRQHandler
函数原型:void USART2_IRQHandler(void)
功    能:
USART1中断服务函数，接收到USART1数据后，将数据储存在g_RxMessage中
全局变量：g_RxMessage,g_CAN_RX_FLAG
参    数:无
返 回 值:无
*************************************************************/
void USART1_IRQHandler(void)
{ 
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(USART1,USART_IT_RXNE); //清除中断标志
		g_usartRxByte[RXCounter++] = USART_ReceiveData(USART1);//(USART1->DR); //读取接收到的数据	
	}
		//下面根据自己需要设置标识位，存储数据。请勿在中断中处理数据。				
	
    else if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
    {
        USART1->SR;
        USART1->DR;
        g_usartRxFlag = 1;
		
    }     
}




/*************************************************************
函数名称:USART_SendString
函数原型:void USART_SendString(u8 *str)
功    能: 从USART1将str字符串输出
参    数:str字符串
返 回 值:无
调用函数:无
*************************************************************/
void USART_SendString(char *str)
{
  while(*str)
  {
    USART_ClearFlag(USART1, USART_FLAG_TC);  
  	USART_SendData(USART1,*str);
  	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待
	str++;
  }
}


/*************************************************************
函数名称:USART2_Send
函数原型:void USART2_SendS(u8 *data,u8 Lenth)
参    剩：需要发送的数据u8 *data；发送数据的长度u8 Lenth
返 回 值:无
调用函数:无
*************************************************************/
void USART1_Send(u8 *data,u8 Lenth)
{
  u8 i;
	for(i=0;i<Lenth;i++)
        {
        USART_ClearFlag(USART1, USART_FLAG_TC);  
        USART_SendData(USART1,*data);
        data++;
        while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待
        }
}
/*************************************************************
函数名称:CRC16
函数原型:unsigned int CRC16(unsigned char *buf, unsigned int length)
参    数：数据unsigned char *buf；数据长度unsigned char length
返 回 值:unsigned int crc
调用函数:无
*************************************************************/
unsigned int CRC16(unsigned char *buf, unsigned char length)
{
    unsigned int i;
    unsigned int j;
    unsigned int c;
    unsigned int crc = 0xFFFF;   
    for (i=0; i<length; i++)
    {
			c = *(buf+i) & 0x00FF;
			crc^=c;
			for (j=0; j<8; j++)
			{
				 if (crc & 0x0001)
				 {
						crc >>= 1;
						crc ^= 0xA001;
				 }
				 else
				 { 
						crc >>= 1;
				 }
			}
   }
    return(crc);
}

/*************************************************************
函数名称:DataCheck
函数原型:u8  DataCheck(void)
参    数：无
返 回 值:0数据帧正常 ；1数据帧有误
调用函数:unsigned int CRC16(unsigned char *buf, unsigned int length)
*************************************************************/
u16 Crc16=0X00;
u8  CrcL,CrcH;
u8 DataCheck(void)
{
	if((g_usartRxByte[0] ==FH)&&(g_usartRxByte[3] ==(RXCounter-7))&&(g_usartRxByte[RXCounter-1] ==FHE)&&(ID==g_usartRxByte[1]))//判断数据帧，包括帧头、帧尾、数据长度
	{
		Crc16=CRC16(g_usartRxByte, RXCounter-3);
	  CrcL=(u8)Crc16;
		CrcH=(u8)(Crc16>>8);
		if((g_usartRxByte[RXCounter-2]==CrcH)&&(g_usartRxByte[RXCounter-3]==CrcL))
      return 0;
	}
    return 1;
}
/*************************************************************
函数名称:Data_Analysis
函数原型:void Data_Analysis(void)
参    数：无
命令解析:上位机的命令格式：按照字节来说：帧头、地址、命令号、数据长度、数据域、CRC校验（两字节，低字节在前）、帧尾
     下位机发送的命令格式：按照字节来说：帧头、地址、状态号、数据长度、数据域、CRC校验（两字节，低字节在前）、帧尾
命令号：0x01  读取杯子各通道的值，数据域第一个字节是：Cup_number 读取第几个杯子
				0x02	读取传感器的状态，数据域的第一个字节是：Sennor_number 读取第几个传感器
状态号：0x64  成功    
				0x32  失败
*************************************************************/
void Data_Analysis(void)
{
    u8 TX_Lenth=0;   //发送数据长度
    g_usartTxByte[0]=FH;   //帧头
    g_usartTxByte[1]=ID;   //地址
	  Comman_state=Command_OK;//默认是成功
	 if(DataCheck()==0x00)
		{			
			if(g_usartRxByte[2]<=2)
			  {
					switch(g_usartRxByte[2])
						{
							case Cup_Value  ://读取杯子的值，共计82个杯子
							{
								if(g_usartRxByte[4]<=82)
								{
									u8 i;
									for(i=0;i<16;i++)
									{
										g_usartTxByte[4+i]=ADS8698_MaxValue[g_usartRxByte[4]][i];									
									}
								}
								else
									Comman_state=Command_ERROR;									
								TX_Lenth=23;//计算回复帧总长，
								g_usartTxByte[2]=Comman_state; //回复帧命令号
								g_usartTxByte[3]=16; //回复数据长度
								Crc16=CRC16(g_usartTxByte, TX_Lenth-3);
								g_usartTxByte[TX_Lenth-3]=(u8)Crc16;//校验低八位
								g_usartTxByte[TX_Lenth-2]=(u8)(Crc16>>8);//校验高八位
								g_usartTxByte[TX_Lenth-1]=FHE;//帧尾								
							}break;																
							case Sensor_Status  :
							{
								switch (g_usartRxByte[4])
								{
									case 0:
										g_usartTxByte[4]=Sensor1_flag;
									break;
									case 1:
										g_usartTxByte[4]=Sensor2_flag;
									break;
									default:
										Comman_state=Command_ERROR;	
									break;								
								}
									TX_Lenth=8;//计算回复帧总长，
									g_usartTxByte[2]=Comman_state; //回复帧命令号
									g_usartTxByte[3]=1; //回复数据长度
									Crc16=CRC16(g_usartTxByte, TX_Lenth-3);
									g_usartTxByte[TX_Lenth-3]=(u8)Crc16;//校验低八位
									g_usartTxByte[TX_Lenth-2]=(u8)(Crc16>>8);//校验高八位
									g_usartTxByte[TX_Lenth-1]=FHE;//帧尾									
								
							}break;			
						 default :
						 break;	
						}							                        								 
				   }
			  }else 
					{
						TX_Lenth=8;
						g_usartTxByte[2]=Command_ERROR; 
						g_usartTxByte[3]=0X01; 
						Crc16=CRC16(g_usartTxByte, TX_Lenth-3);
						g_usartTxByte[TX_Lenth-3]=(u8)Crc16;
						g_usartTxByte[TX_Lenth-2]=(u8)(Crc16>>8);
						g_usartTxByte[TX_Lenth-1]=FHE;
					}		
				USART1_Send(g_usartTxByte,TX_Lenth);
				memset(g_usartTxByte,0,200);
				memset(g_usartRxByte,0,200);
			}
		
			
								

		


/****END OF FILE****/
