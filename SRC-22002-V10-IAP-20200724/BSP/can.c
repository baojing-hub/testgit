/*******************************************************************************
* File Name: can.c
*
* Description:
* 本文件提供CAN1驱动的配置函数，请根据需要修改参数
* Note:
* 注意设置自身CANID和处理标识位。
********************************************************************************/

#include "can.h"

#define RCC_APB1Periph_CANx RCC_APB1Periph_CAN1
#define RCC_APB2Periph_GPIOx RCC_APB2Periph_GPIOA
#define CAN_IO_PORT GPIOA
#define PIN_CAN_RX GPIO_Pin_11
#define PIN_CAN_TX GPIO_Pin_12
#define CanAllID	0xff

CanRxMsg g_RxMessage;

u8  g_SelfCanId = 20;	//自身CANID
u8  g_CAN_RX_FLAG = 0; //CAN接收数据状态标识
u8  g_CAN_TX_FLAG = 0;  //CAN发送状态标识位
u32 g_CAN_ERR_CNT = 0;  //CAN发送错误计数
u32 g_CAN_ERR_MSG = 0;	//CAN错误信息
/*************************************************************
函数名称:CAN_Config
函数原型:void Can_Config(void)
功    能:
CAN 初始化: 普通模式，1M波特率，只接受自身CANID数据，配置CAN中断
全局变量：g_SelfCanId
参    数:无
返 回 值:无
*************************************************************/
void CAN_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	CAN_InitTypeDef CAN_InitStructure;
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOx, ENABLE);//使能 PORT 时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CANx, ENABLE);//使能 CAN 时钟
	
	GPIO_InitStructure.GPIO_Pin = PIN_CAN_TX;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽
	GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化 IO
	
	GPIO_InitStructure.GPIO_Pin = PIN_CAN_RX;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化 IO
	
	
	
	//CAN 单元设置
	CAN_InitStructure.CAN_TTCM=DISABLE; //非时间触发通信模式
	CAN_InitStructure.CAN_ABOM=DISABLE; ///软件自动离线管理
	CAN_InitStructure.CAN_AWUM=DISABLE; //睡眠模式通过软件唤醒
	CAN_InitStructure.CAN_NART=DISABLE; //打开报文自动重发 //
	CAN_InitStructure.CAN_RFLM=DISABLE; //报文不锁定,新的覆盖旧的 //
	CAN_InitStructure.CAN_TXFP=DISABLE; //优先级由报文标识符决定 //
	CAN_InitStructure.CAN_Mode= CAN_Mode_Normal ; //模式设置： 0,普通模式;1,回环模式;
	
	/*设置波特率
	tsjw:重新同步跳跃时间单元. CAN_SJW_1tq~CAN_SJW_4tq
	tbs2:时间段 2 的时间单元.
	tbs1:时间段 1 的时间单元 CAN_BS1_1tq ~CAN_BS1_16tq
	brp :波特率分频器.范围:1~1024;(实际要加 1,也就是 1~1024) tq=(brp)*tpclk1
	注意以上参数任何一个都不能设为 0,否则会乱.
	波特率=Fpclk1/((tsjw+tbs1+tbs2)*brp);
	Fpclk1 的时钟在初始化的时候设置为 36M
	则波特率为:36M/((1+4+1)*6)=1M
	*/
	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq; //重新同步跳跃宽度(Tsjw)
	CAN_InitStructure.CAN_BS1=CAN_BS1_4tq; //时间段 4 占用时间单位
	CAN_InitStructure.CAN_BS2=CAN_BS2_1tq;// 时间段 1 占用时间单位
	CAN_InitStructure.CAN_Prescaler=6; //分频系数(Fdiv)为 brp+1 //	
	CAN_Init(CAN1, &CAN_InitStructure); // 初始化 CAN1
	
	
	//设置滤波器,只接收CAN ID为slave_id的标准数据帧
	CAN_FilterInitStructure.CAN_FilterNumber=0; //过滤器 0
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32 位
	CAN_FilterInitStructure.CAN_FilterIdHigh=(((u32)g_SelfCanId<<21)&0xffff0000)>>16;////32 位 ID
	CAN_FilterInitStructure.CAN_FilterIdLow=(((u32)g_SelfCanId<<21)|CAN_ID_STD|CAN_RTR_DATA)&0xffff;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0xFFFF;//32 位 MASK
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0xFFFF;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;// FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //激活过滤器 0
	CAN_FilterInit(&CAN_FilterInitStructure);//滤波器初始化
	
		
	/* CAN 过滤器设置 ：获取广播包*/
	CAN_FilterInitStructure.CAN_FilterNumber=1;
	CAN_FilterInitStructure.CAN_FilterMode= CAN_FilterMode_IdMask;//CAN_FilterMode_IdList;//
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh=(((u32)CanAllID<<21)&0xFFFF0000)>>16;// 
	CAN_FilterInitStructure.CAN_FilterIdLow=(((u32)CanAllID<<21)|CAN_ID_STD|CAN_RTR_DATA)&0xFFFF;// 
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=(((u32)CanAllID<<21)&0xFFFF0000)>>16;//
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=(((u32)CanAllID<<21)|CAN_ID_STD|CAN_RTR_DATA)&0xFFFF;// 
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_FIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
	//CAN接收中断配置

	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);//FIFO0 消息挂号中断允许.
	#ifdef  STM32F10X_CL
	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
	#else
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn ;
	#endif /* DATA_IN_ExtSRAM */
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 主优先级为 1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // 次优先级为 0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	CAN_ITConfig(CAN1,CAN_IT_ERR | CAN_IT_LEC, ENABLE);//CAN错误计数中断
	NVIC_InitStructure.NVIC_IRQChannel=CAN1_SCE_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}



/*************************************************************
函数名称:CAN1_SCE_IRQHandler
函数原型:CAN1_SCE_IRQHandler()
功    能:
CAN1错误中断服务函数，实现发送数据时CAN错误计数
全局变量：g_CAN_TX_FLAG，g_CAN_ERR_CNT
参    数:无
返 回 值:无
*************************************************************/
void CAN1_SCE_IRQHandler(void)
{
	if(g_CAN_TX_FLAG==1)		//判断是否为发送状态
	{
		g_CAN_ERR_CNT++;		//CAN错误软件计数
		g_CAN_ERR_MSG=CAN1->ESR;	//CAN1错误状态寄存器值
	}
 	CAN_ClearITPendingBit(CAN1,CAN_IT_LEC);	//清中断标志
}




/*************************************************************
函数名称:CAN1_RX0_IRQHandler 或 USB_LP_CAN1_RX0_IRQHandler
函数原型:void CAN1_RX0_IRQHandler(void) 或 void USB_LP_CAN1_RX0_IRQHandler(void) 
功    能:
CAN1中断服务函数，接收到CAN1数据后，将数据储存在g_RxMessage中
全局变量：g_RxMessage,g_CAN_RX_FLAG
参    数:无
返 回 值:无
*************************************************************/
#ifdef  STM32F10X_CL
void CAN1_RX0_IRQHandler(void)
#else
void USB_LP_CAN1_RX0_IRQHandler(void) 
#endif
{
	
	CAN_ClearITPendingBit(CAN1,CAN_IT_FF0);
	CAN_Receive(CAN1, CAN_FIFO0, &g_RxMessage);
	g_CAN_RX_FLAG = 1;
}



/*************************************************************
函数名称:SendCanMessage
函数原型:u8 SendCanMessage(u16 stdid ,u8 statusno,u8 orderno,u8 *candata )
功    能:
通过CAN1向目标ID发送一帧CAN数据
全局变量：g_SelfCanId g_CAN_TX_FLAG
参    数:
targetID  目标ID
statusNO  状态号
orderNO   命令号
canData   4字节数组
返 回 值:
0 发送失败
1 发送成功
*************************************************************/
u8 Send_CanMessage(u16 targetID ,u8 statusNO,u8 orderNO,u8 *canData )
{
	u8 mBox = 0;
	u16 i = 0;
	CanTxMsg TxMessage;
	TxMessage.StdId = targetID; // 标准标识符
	TxMessage.IDE = CAN_Id_Standard; // 使用标准标识符
	TxMessage.RTR = CAN_RTR_Data; // 消息类型为数据帧，
	TxMessage.DLC = CANDATALENGTH;
	
	TxMessage.Data[0] = targetID;
	TxMessage.Data[1] = g_SelfCanId;
	TxMessage.Data[2] = statusNO;
	TxMessage.Data[3] = orderNO;
	TxMessage.Data[4] = canData[3];
	TxMessage.Data[5] = canData[2];
	TxMessage.Data[6] = canData[1];
	TxMessage.Data[7] = canData[0];
	g_CAN_TX_FLAG = 1;

	mBox= CAN_Transmit(CAN1, &TxMessage);
	i=0;
	while((CAN_TransmitStatus(CAN1, mBox)!=CAN_TxStatus_Ok)&&(i!=0xfff))
	{
	   i++; //等待发送完成
	}
	//发送结束
	g_CAN_TX_FLAG = 0;

	if(i>=0xfff)
	return 1;
	else
	return 0;
}


/****END OF FILE****/
