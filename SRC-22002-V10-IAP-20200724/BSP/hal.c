#include "hal.h"


void Hal_Init(void)
{
	RCC_Config();//时钟初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	//外设初始化
	CAN_Config();
	USART_Config();
	TIM2_Config();
	GPIO_Config();
	ADS8698_Init();
	//....
}

