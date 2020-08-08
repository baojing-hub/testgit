/*******************************************************************************
* File Name: usart.c
*
* Description:
* ���ļ��ṩUSART1���������ú������������Ҫ�޸Ĳ���
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
��������:USART_Config
����ԭ��:void USART_Config(void)
��    ��: ��ʼ������2��PA9Ϊtx,PA10Ϊrx, 115200�����ʣ�8������λ��1��ֹͣλ�
�����żУ��,��Ӳ�������ơ�
��    ��:��
�� �� ֵ:��
���ú���:��
*************************************************************/
void USART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;


	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//ʹ��GPIOA��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//ʹ��USART1��ʱ��
	USART_DeInit(USART1);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	//��ʼ��PA2Ϊ����������� USART TX

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	//��ʼ��PA3Ϊ�������� USART RX

	
	USART_InitStructure.USART_BaudRate = BAUDRATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl= USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	

	//�ݳ�ʼ�� NVIC
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ� 0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; //�����ȼ� 1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ ͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure); //����ָ���Ĳ�����ʼ�� VIC �Ĵ���
  USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//���������ж�
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�
  USART_ClearITPendingBit(USART1,USART_IT_RXNE); //��������жϱ�־
  USART_ClearFlag(USART1, USART_FLAG_TC);//��������жϱ�־
	USART_Cmd(USART1, ENABLE);
}



/*************************************************************
��������:USART2_IRQHandler
����ԭ��:void USART2_IRQHandler(void)
��    ��:
USART1�жϷ����������յ�USART1���ݺ󣬽����ݴ�����g_RxMessage��
ȫ�ֱ�����g_RxMessage,g_CAN_RX_FLAG
��    ��:��
�� �� ֵ:��
*************************************************************/
void USART1_IRQHandler(void)
{ 
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(USART1,USART_IT_RXNE); //����жϱ�־
		g_usartRxByte[RXCounter++] = USART_ReceiveData(USART1);//(USART1->DR); //��ȡ���յ�������	
	}
		//��������Լ���Ҫ���ñ�ʶλ���洢���ݡ��������ж��д������ݡ�				
	
    else if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
    {
        USART1->SR;
        USART1->DR;
        g_usartRxFlag = 1;
		
    }     
}




/*************************************************************
��������:USART_SendString
����ԭ��:void USART_SendString(u8 *str)
��    ��: ��USART1��str�ַ������
��    ��:str�ַ���
�� �� ֵ:��
���ú���:��
*************************************************************/
void USART_SendString(char *str)
{
  while(*str)
  {
    USART_ClearFlag(USART1, USART_FLAG_TC);  
  	USART_SendData(USART1,*str);
  	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ�
	str++;
  }
}


/*************************************************************
��������:USART2_Send
����ԭ��:void USART2_SendS(u8 *data,u8 Lenth)
��    ʣ����Ҫ���͵�����u8 *data���������ݵĳ���u8 Lenth
�� �� ֵ:��
���ú���:��
*************************************************************/
void USART1_Send(u8 *data,u8 Lenth)
{
  u8 i;
	for(i=0;i<Lenth;i++)
        {
        USART_ClearFlag(USART1, USART_FLAG_TC);  
        USART_SendData(USART1,*data);
        data++;
        while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ�
        }
}
/*************************************************************
��������:CRC16
����ԭ��:unsigned int CRC16(unsigned char *buf, unsigned int length)
��    ��������unsigned char *buf�����ݳ���unsigned char length
�� �� ֵ:unsigned int crc
���ú���:��
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
��������:DataCheck
����ԭ��:u8  DataCheck(void)
��    ������
�� �� ֵ:0����֡���� ��1����֡����
���ú���:unsigned int CRC16(unsigned char *buf, unsigned int length)
*************************************************************/
u16 Crc16=0X00;
u8  CrcL,CrcH;
u8 DataCheck(void)
{
	if((g_usartRxByte[0] ==FH)&&(g_usartRxByte[3] ==(RXCounter-7))&&(g_usartRxByte[RXCounter-1] ==FHE)&&(ID==g_usartRxByte[1]))//�ж�����֡������֡ͷ��֡β�����ݳ���
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
��������:Data_Analysis
����ԭ��:void Data_Analysis(void)
��    ������
�������:��λ���������ʽ�������ֽ���˵��֡ͷ����ַ������š����ݳ��ȡ�������CRCУ�飨���ֽڣ����ֽ���ǰ����֡β
     ��λ�����͵������ʽ�������ֽ���˵��֡ͷ����ַ��״̬�š����ݳ��ȡ�������CRCУ�飨���ֽڣ����ֽ���ǰ����֡β
����ţ�0x01  ��ȡ���Ӹ�ͨ����ֵ���������һ���ֽ��ǣ�Cup_number ��ȡ�ڼ�������
				0x02	��ȡ��������״̬��������ĵ�һ���ֽ��ǣ�Sennor_number ��ȡ�ڼ���������
״̬�ţ�0x64  �ɹ�    
				0x32  ʧ��
*************************************************************/
void Data_Analysis(void)
{
    u8 TX_Lenth=0;   //�������ݳ���
    g_usartTxByte[0]=FH;   //֡ͷ
    g_usartTxByte[1]=ID;   //��ַ
	  Comman_state=Command_OK;//Ĭ���ǳɹ�
	 if(DataCheck()==0x00)
		{			
			if(g_usartRxByte[2]<=2)
			  {
					switch(g_usartRxByte[2])
						{
							case Cup_Value  ://��ȡ���ӵ�ֵ������82������
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
								TX_Lenth=23;//����ظ�֡�ܳ���
								g_usartTxByte[2]=Comman_state; //�ظ�֡�����
								g_usartTxByte[3]=16; //�ظ����ݳ���
								Crc16=CRC16(g_usartTxByte, TX_Lenth-3);
								g_usartTxByte[TX_Lenth-3]=(u8)Crc16;//У��Ͱ�λ
								g_usartTxByte[TX_Lenth-2]=(u8)(Crc16>>8);//У��߰�λ
								g_usartTxByte[TX_Lenth-1]=FHE;//֡β								
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
									TX_Lenth=8;//����ظ�֡�ܳ���
									g_usartTxByte[2]=Comman_state; //�ظ�֡�����
									g_usartTxByte[3]=1; //�ظ����ݳ���
									Crc16=CRC16(g_usartTxByte, TX_Lenth-3);
									g_usartTxByte[TX_Lenth-3]=(u8)Crc16;//У��Ͱ�λ
									g_usartTxByte[TX_Lenth-2]=(u8)(Crc16>>8);//У��߰�λ
									g_usartTxByte[TX_Lenth-1]=FHE;//֡β									
								
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
