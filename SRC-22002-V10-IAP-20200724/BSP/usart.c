/*******************************************************************************
* File Name: usart.c
*
* Description:
* ±¾ÎÄ¼şÌá¹©USART1Çı¶¯µÄÅäÖÃº¯Êı£¬Çë¸ù¾İĞèÒªĞŞ¸Ä²ÎÊı
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
º¯ÊıÃû³Æ:USART_Config
º¯ÊıÔ­ĞÍ:void USART_Config(void)
¹¦    ÄÜ: ³õÊ¼»¯´®¿Ú2£¬PA9Îªtx,PA10Îªrx, 115200²¨ÌØÂÊ£¬8¸öÊı¾İÎ»£¬1¸öÍ£Ö¹Î»£
¬ÎŞÆæÅ¼Ğ£Ñé,ÎŞÓ²¼şÁ÷¿ØÖÆ¡£
²Î    Êı:ÎŞ
·µ »Ø Öµ:ÎŞ
µ÷ÓÃº¯Êı:ÎŞ
*************************************************************/
void USART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;


	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//Ê¹ÄÜGPIOAµÄÊ±ÖÓ
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//Ê¹ÄÜUSART1µÄÊ±ÖÓ
	USART_DeInit(USART1);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	//³õÊ¼»¯PA2Îª¸´ÓÃÍÆÍìÊä³ö USART TX

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	//³õÊ¼»¯PA3Îª¸¡¿ÕÊäÈë USART RX

	
	USART_InitStructure.USART_BaudRate = BAUDRATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl= USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	

	//¢İ³õÊ¼»¯ NVIC
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//ÇÀÕ¼ÓÅÏÈ¼¶ 0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; //×ÓÓÅÏÈ¼¶ 1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ Í¨µÀÊ¹ÄÜ
	NVIC_Init(&NVIC_InitStructure); //¸ù¾İÖ¸¶¨µÄ²ÎÊı³õÊ¼»¯ VIC ¼Ä´æÆ÷
  USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//¿ªÆô¿ÕÏĞÖĞ¶Ï
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//¿ªÆôÖĞ¶Ï
  USART_ClearITPendingBit(USART1,USART_IT_RXNE); //Çå³ı½ÓÊÕÖĞ¶Ï±êÖ¾
  USART_ClearFlag(USART1, USART_FLAG_TC);//Çå³ı·¢ËÍÖĞ¶Ï±êÖ¾
	USART_Cmd(USART1, ENABLE);
}



/*************************************************************
º¯ÊıÃû³Æ:USART2_IRQHandler
º¯ÊıÔ­ĞÍ:void USART2_IRQHandler(void)
¹¦    ÄÜ:
USART1ÖĞ¶Ï·şÎñº¯Êı£¬½ÓÊÕµ½USART1Êı¾İºó£¬½«Êı¾İ´¢´æÔÚg_RxMessageÖĞ
È«¾Ö±äÁ¿£ºg_RxMessage,g_CAN_RX_FLAG
²Î    Êı:ÎŞ
·µ »Ø Öµ:ÎŞ
*************************************************************/
void USART1_IRQHandler(void)
{ 
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(USART1,USART_IT_RXNE); //Çå³ıÖĞ¶Ï±êÖ¾
		g_usartRxByte[RXCounter++] = USART_ReceiveData(USART1);//(USART1->DR); //¶ÁÈ¡½ÓÊÕµ½µÄÊı¾İ	
	}
		//ÏÂÃæ¸ù¾İ×Ô¼ºĞèÒªÉèÖÃ±êÊ¶Î»£¬´æ´¢Êı¾İ¡£ÇëÎğÔÚÖĞ¶ÏÖĞ´¦ÀíÊı¾İ¡£				
	
    else if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
    {
        USART1->SR;
        USART1->DR;
        g_usartRxFlag = 1;
		
    }     
}




/*************************************************************
º¯ÊıÃû³Æ:USART_SendString
º¯ÊıÔ­ĞÍ:void USART_SendString(u8 *str)
¹¦    ÄÜ: ´ÓUSART1½«str×Ö·û´®Êä³ö
²Î    Êı:str×Ö·û´®
·µ »Ø Öµ:ÎŞ
µ÷ÓÃº¯Êı:ÎŞ
*************************************************************/
void USART_SendString(char *str)
{
  while(*str)
  {
    USART_ClearFlag(USART1, USART_FLAG_TC);  
  	USART_SendData(USART1,*str);
  	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//µÈ´ı
	str++;
  }
}


/*************************************************************
º¯ÊıÃû³Æ:USART2_Send
º¯ÊıÔ­ĞÍ:void USART2_SendS(u8 *data,u8 Lenth)
²Î    Ê££ºĞèÒª·¢ËÍµÄÊı¾İu8 *data£»·¢ËÍÊı¾İµÄ³¤¶Èu8 Lenth
·µ »Ø Öµ:ÎŞ
µ÷ÓÃº¯Êı:ÎŞ
*************************************************************/
void USART1_Send(u8 *data,u8 Lenth)
{
  u8 i;
	for(i=0;i<Lenth;i++)
        {
        USART_ClearFlag(USART1, USART_FLAG_TC);  
        USART_SendData(USART1,*data);
        data++;
        while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//µÈ´ı
        }
}
/*************************************************************
º¯ÊıÃû³Æ:CRC16
º¯ÊıÔ­ĞÍ:unsigned int CRC16(unsigned char *buf, unsigned int length)
²Î    Êı£ºÊı¾İunsigned char *buf£»Êı¾İ³¤¶Èunsigned char length
·µ »Ø Öµ:unsigned int crc
µ÷ÓÃº¯Êı:ÎŞ
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
º¯ÊıÃû³Æ:DataCheck
º¯ÊıÔ­ĞÍ:u8  DataCheck(void)
²Î    Êı£ºÎŞ
·µ »Ø Öµ:0Êı¾İÖ¡Õı³£ £»1Êı¾İÖ¡ÓĞÎó
µ÷ÓÃº¯Êı:unsigned int CRC16(unsigned char *buf, unsigned int length)
*************************************************************/
u16 Crc16=0X00;
u8  CrcL,CrcH;
u8 DataCheck(void)
{
	if((g_usartRxByte[0] ==FH)&&(g_usartRxByte[3] ==(RXCounter-7))&&(g_usartRxByte[RXCounter-1] ==FHE)&&(ID==g_usartRxByte[1]))//ÅĞ¶ÏÊı¾İÖ¡£¬°üÀ¨Ö¡Í·¡¢Ö¡Î²¡¢Êı¾İ³¤¶È
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
º¯ÊıÃû³Æ:Data_Analysis
º¯ÊıÔ­ĞÍ:void Data_Analysis(void)
²Î    Êı£ºÎŞ
ÃüÁî½âÎö:ÉÏÎ»»úµÄÃüÁî¸ñÊ½£º°´ÕÕ×Ö½ÚÀ´Ëµ£ºÖ¡Í·¡¢µØÖ·¡¢ÃüÁîºÅ¡¢Êı¾İ³¤¶È¡¢Êı¾İÓò¡¢CRCĞ£Ñé£¨Á½×Ö½Ú£¬µÍ×Ö½ÚÔÚÇ°£©¡¢Ö¡Î²
     ÏÂÎ»»ú·¢ËÍµÄÃüÁî¸ñÊ½£º°´ÕÕ×Ö½ÚÀ´Ëµ£ºÖ¡Í·¡¢µØÖ·¡¢×´Ì¬ºÅ¡¢Êı¾İ³¤¶È¡¢Êı¾İÓò¡¢CRCĞ£Ñé£¨Á½×Ö½Ú£¬µÍ×Ö½ÚÔÚÇ°£©¡¢Ö¡Î²
ÃüÁîºÅ£º0x01  ¶ÁÈ¡±­×Ó¸÷Í¨µÀµÄÖµ£¬Êı¾İÓòµÚÒ»¸ö×Ö½ÚÊÇ£ºCup_number ¶ÁÈ¡µÚ¼¸¸ö±­×Ó
				0x02	¶ÁÈ¡´«¸ĞÆ÷µÄ×´Ì¬£¬Êı¾İÓòµÄµÚÒ»¸ö×Ö½ÚÊÇ£ºSennor_number ¶ÁÈ¡µÚ¼¸¸ö´«¸ĞÆ÷
×´Ì¬ºÅ£º0x64  ³É¹¦    
				0x32  Ê§°Ü
*************************************************************/
void Data_Analysis(void)
{
    u8 TX_Lenth=0;   //·¢ËÍÊı¾İ³¤¶È
    g_usartTxByte[0]=FH;   //Ö¡Í·
    g_usartTxByte[1]=ID;   //µØÖ·
	  Comman_state=Command_OK;//Ä¬ÈÏÊÇ³É¹¦
	 if(DataCheck()==0x00)
		{			
			if(g_usartRxByte[2]<=2)
			  {
					switch(g_usartRxByte[2])
						{
							case Cup_Value  ://¶ÁÈ¡±­×ÓµÄÖµ£¬¹²¼Æ82¸ö±­×Ó
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
								TX_Lenth=23;//¼ÆËã»Ø¸´Ö¡×Ü³¤£¬
								g_usartTxByte[2]=Comman_state; //»Ø¸´Ö¡ÃüÁîºÅ
								g_usartTxByte[3]=16; //»Ø¸´Êı¾İ³¤¶È
								Crc16=CRC16(g_usartTxByte, TX_Lenth-3);
								g_usartTxByte[TX_Lenth-3]=(u8)Crc16;//Ğ£ÑéµÍ°ËÎ»
								g_usartTxByte[TX_Lenth-2]=(u8)(Crc16>>8);//Ğ£Ñé¸ß°ËÎ»
								g_usartTxByte[TX_Lenth-1]=FHE;//Ö¡Î²								
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
									TX_Lenth=8;//¼ÆËã»Ø¸´Ö¡×Ü³¤£¬
									g_usartTxByte[2]=Comman_state; //»Ø¸´Ö¡ÃüÁîºÅ
									g_usartTxByte[3]=1; //»Ø¸´Êı¾İ³¤¶È
									Crc16=CRC16(g_usartTxByte, TX_Lenth-3);
									g_usartTxByte[TX_Lenth-3]=(u8)Crc16;//Ğ£ÑéµÍ°ËÎ»
									g_usartTxByte[TX_Lenth-2]=(u8)(Crc16>>8);//Ğ£Ñé¸ß°ËÎ»
									g_usartTxByte[TX_Lenth-1]=FHE;//Ö¡Î²									
								
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
