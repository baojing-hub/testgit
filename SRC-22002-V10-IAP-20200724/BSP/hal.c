#include "hal.h"


void Hal_Init(void)
{
	RCC_Config();//ʱ�ӳ�ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	//�����ʼ��
	CAN_Config();
	USART_Config();
	TIM2_Config();
	GPIO_Config();
	ADS8698_Init();
	//....
}

