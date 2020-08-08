/*******************************************************************************
* File Name: can.c
*
* Description:
* ���ļ��ṩCAN1���������ú������������Ҫ�޸Ĳ���
* Note:
* ע����������CANID�ʹ����ʶλ��
********************************************************************************/

#include "can.h"

#define RCC_APB1Periph_CANx RCC_APB1Periph_CAN1
#define RCC_APB2Periph_GPIOx RCC_APB2Periph_GPIOA
#define CAN_IO_PORT GPIOA
#define PIN_CAN_RX GPIO_Pin_11
#define PIN_CAN_TX GPIO_Pin_12
#define CanAllID	0xff

CanRxMsg g_RxMessage;

u8  g_SelfCanId = 20;	//����CANID
u8  g_CAN_RX_FLAG = 0; //CAN��������״̬��ʶ
u8  g_CAN_TX_FLAG = 0;  //CAN����״̬��ʶλ
u32 g_CAN_ERR_CNT = 0;  //CAN���ʹ������
u32 g_CAN_ERR_MSG = 0;	//CAN������Ϣ
/*************************************************************
��������:CAN_Config
����ԭ��:void Can_Config(void)
��    ��:
CAN ��ʼ��: ��ͨģʽ��1M�����ʣ�ֻ��������CANID���ݣ�����CAN�ж�
ȫ�ֱ�����g_SelfCanId
��    ��:��
�� �� ֵ:��
*************************************************************/
void CAN_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	CAN_InitTypeDef CAN_InitStructure;
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOx, ENABLE);//ʹ�� PORT ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CANx, ENABLE);//ʹ�� CAN ʱ��
	
	GPIO_InitStructure.GPIO_Pin = PIN_CAN_TX;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //��������
	GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ�� IO
	
	GPIO_InitStructure.GPIO_Pin = PIN_CAN_RX;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ�� IO
	
	
	
	//CAN ��Ԫ����
	CAN_InitStructure.CAN_TTCM=DISABLE; //��ʱ�䴥��ͨ��ģʽ
	CAN_InitStructure.CAN_ABOM=DISABLE; ///����Զ����߹���
	CAN_InitStructure.CAN_AWUM=DISABLE; //˯��ģʽͨ���������
	CAN_InitStructure.CAN_NART=DISABLE; //�򿪱����Զ��ط� //
	CAN_InitStructure.CAN_RFLM=DISABLE; //���Ĳ�����,�µĸ��Ǿɵ� //
	CAN_InitStructure.CAN_TXFP=DISABLE; //���ȼ��ɱ��ı�ʶ������ //
	CAN_InitStructure.CAN_Mode= CAN_Mode_Normal ; //ģʽ���ã� 0,��ͨģʽ;1,�ػ�ģʽ;
	
	/*���ò�����
	tsjw:����ͬ����Ծʱ�䵥Ԫ. CAN_SJW_1tq~CAN_SJW_4tq
	tbs2:ʱ��� 2 ��ʱ�䵥Ԫ.
	tbs1:ʱ��� 1 ��ʱ�䵥Ԫ CAN_BS1_1tq ~CAN_BS1_16tq
	brp :�����ʷ�Ƶ��.��Χ:1~1024;(ʵ��Ҫ�� 1,Ҳ���� 1~1024) tq=(brp)*tpclk1
	ע�����ϲ����κ�һ����������Ϊ 0,�������.
	������=Fpclk1/((tsjw+tbs1+tbs2)*brp);
	Fpclk1 ��ʱ���ڳ�ʼ����ʱ������Ϊ 36M
	������Ϊ:36M/((1+4+1)*6)=1M
	*/
	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq; //����ͬ����Ծ���(Tsjw)
	CAN_InitStructure.CAN_BS1=CAN_BS1_4tq; //ʱ��� 4 ռ��ʱ�䵥λ
	CAN_InitStructure.CAN_BS2=CAN_BS2_1tq;// ʱ��� 1 ռ��ʱ�䵥λ
	CAN_InitStructure.CAN_Prescaler=6; //��Ƶϵ��(Fdiv)Ϊ brp+1 //	
	CAN_Init(CAN1, &CAN_InitStructure); // ��ʼ�� CAN1
	
	
	//�����˲���,ֻ����CAN IDΪslave_id�ı�׼����֡
	CAN_FilterInitStructure.CAN_FilterNumber=0; //������ 0
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32 λ
	CAN_FilterInitStructure.CAN_FilterIdHigh=(((u32)g_SelfCanId<<21)&0xffff0000)>>16;////32 λ ID
	CAN_FilterInitStructure.CAN_FilterIdLow=(((u32)g_SelfCanId<<21)|CAN_ID_STD|CAN_RTR_DATA)&0xffff;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0xFFFF;//32 λ MASK
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0xFFFF;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;// FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //��������� 0
	CAN_FilterInit(&CAN_FilterInitStructure);//�˲�����ʼ��
	
		
	/* CAN ���������� ����ȡ�㲥��*/
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
	//CAN�����ж�����

	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);//FIFO0 ��Ϣ�Һ��ж�����.
	#ifdef  STM32F10X_CL
	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
	#else
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn ;
	#endif /* DATA_IN_ExtSRAM */
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // �����ȼ�Ϊ 1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // �����ȼ�Ϊ 0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	CAN_ITConfig(CAN1,CAN_IT_ERR | CAN_IT_LEC, ENABLE);//CAN��������ж�
	NVIC_InitStructure.NVIC_IRQChannel=CAN1_SCE_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}



/*************************************************************
��������:CAN1_SCE_IRQHandler
����ԭ��:CAN1_SCE_IRQHandler()
��    ��:
CAN1�����жϷ�������ʵ�ַ�������ʱCAN�������
ȫ�ֱ�����g_CAN_TX_FLAG��g_CAN_ERR_CNT
��    ��:��
�� �� ֵ:��
*************************************************************/
void CAN1_SCE_IRQHandler(void)
{
	if(g_CAN_TX_FLAG==1)		//�ж��Ƿ�Ϊ����״̬
	{
		g_CAN_ERR_CNT++;		//CAN�����������
		g_CAN_ERR_MSG=CAN1->ESR;	//CAN1����״̬�Ĵ���ֵ
	}
 	CAN_ClearITPendingBit(CAN1,CAN_IT_LEC);	//���жϱ�־
}




/*************************************************************
��������:CAN1_RX0_IRQHandler �� USB_LP_CAN1_RX0_IRQHandler
����ԭ��:void CAN1_RX0_IRQHandler(void) �� void USB_LP_CAN1_RX0_IRQHandler(void) 
��    ��:
CAN1�жϷ����������յ�CAN1���ݺ󣬽����ݴ�����g_RxMessage��
ȫ�ֱ�����g_RxMessage,g_CAN_RX_FLAG
��    ��:��
�� �� ֵ:��
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
��������:SendCanMessage
����ԭ��:u8 SendCanMessage(u16 stdid ,u8 statusno,u8 orderno,u8 *candata )
��    ��:
ͨ��CAN1��Ŀ��ID����һ֡CAN����
ȫ�ֱ�����g_SelfCanId g_CAN_TX_FLAG
��    ��:
targetID  Ŀ��ID
statusNO  ״̬��
orderNO   �����
canData   4�ֽ�����
�� �� ֵ:
0 ����ʧ��
1 ���ͳɹ�
*************************************************************/
u8 Send_CanMessage(u16 targetID ,u8 statusNO,u8 orderNO,u8 *canData )
{
	u8 mBox = 0;
	u16 i = 0;
	CanTxMsg TxMessage;
	TxMessage.StdId = targetID; // ��׼��ʶ��
	TxMessage.IDE = CAN_Id_Standard; // ʹ�ñ�׼��ʶ��
	TxMessage.RTR = CAN_RTR_Data; // ��Ϣ����Ϊ����֡��
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
	   i++; //�ȴ��������
	}
	//���ͽ���
	g_CAN_TX_FLAG = 0;

	if(i>=0xfff)
	return 1;
	else
	return 0;
}


/****END OF FILE****/
