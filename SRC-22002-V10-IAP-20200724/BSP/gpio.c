/*******************************************************************************
* File Name: gpio.c
*
* Description:
* ���ļ��ṩͨ��GPIO���������ú������������Ҫ�޸Ĳ���
* Note:
*
********************************************************************************/

#include "gpio.h"
#include "rcc.h"

u8 Sensor1_flag=0;
u8 Sensor2_flag=0;

/*************************************************************
��������:GPIO_Config
����ԭ��:void GPIO_Config()
��    ��: ����ͨ��GPIO�ڡ�
��    ��:��
�� �� ֵ:��
���ú���:��
*************************************************************/
void GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  /****��ʼ��LED1��LED2****/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//ʹ��GPIOB��ʱ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_0 | GPIO_Pin_1 );
	
	/****��ʼ��SENS1��SENS2****/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);//ʹ��GPIOD��ʱ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
}


/*************************************************************
��������:Led_Ctrl
����ԭ��:void Led_Ctrl(void)
��    ��: ��������ָʾ���ϵ����˸����
��    ��:��
�� �� ֵ:��
���ú���:��
*************************************************************/
void Led_Ctrl(void)
{
//	��������ָʾ��	
	LED1_ON;
	Delay_Ms(200);
	LED1_OFF;
	Delay_Ms(200);
	LED1_ON;
	Delay_Ms(200);
	LED1_OFF;
}
/*************************************************************
��������:Read_Sensorstatus
����ԭ��:void Read_Sensorstatus(void)
��    ��:��ȡ������������״̬
��    ��:��
�� �� ֵ:��
���ú���:��
*************************************************************/
void Read_Sensorstatus(void)
{
	if(Raed_Sens1==1)Sensor1_flag=1;
	else Sensor1_flag=0;
	if(Raed_Sens2==1)Sensor2_flag=1;
	else Sensor2_flag=0;
}


/****END OF FILE****/
