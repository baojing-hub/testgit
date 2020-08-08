/*******************************************************************************
* File Name: tim.c
*
* Description:
* 本文件提供TIM1驱动的配置函数，请根据需要修改参数
* Note:
*
********************************************************************************/

#include "tim.h"
u8 timeout_flag =0;
/*************************************************************
函数名称:TIM2_Config
函数原型:void TIM2_Config(void)
功    能:
TIM1初始化：定时
全局变量：
参    数:无
返 回 值:无
*************************************************************/
void TIM2_Config(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	  NVIC_InitTypeDef NVIC_InitStructure;
	
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //使能定时器4时钟

    /* Time Base configuration */
    TIM_TimeBaseStructure.TIM_Prescaler = 71; //预分频为1M,此时T1的时钟为1M
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = 999; //计数值为1000，1ms一个中断
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
		TIM_ITConfig(TIM2,TIM_IT_Update|TIM_IT_Trigger,ENABLE);
	
		//⑤初始化 NVIC
		NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//抢占优先级 3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //子优先级 3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ 通道使能
		NVIC_Init(&NVIC_InitStructure); //根据指定的参数初始化 VIC 寄存器
   
	 /* TIM1 counter enable */
    TIM_Cmd(TIM2, ENABLE);
}

/**********************************************************/
//TIM2中断函数
//功能:控制流水灯
void TIM2_IRQHandler(void)   //TIM2中断
{ 
	u8 count;

	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
	{
		
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //清除TIMx更新中断标志 

    count++;
		if(count==15)	
		{
      timeout_flag=1;
			count=0;
		}	
	}	
}
/****END OF FILE****/
