/*******************************************************************************
* File Name: tim.c
*
* Description:
* ���ļ��ṩTIM1���������ú������������Ҫ�޸Ĳ���
* Note:
*
********************************************************************************/

#include "tim.h"
u8 timeout_flag =0;
/*************************************************************
��������:TIM2_Config
����ԭ��:void TIM2_Config(void)
��    ��:
TIM1��ʼ������ʱ
ȫ�ֱ�����
��    ��:��
�� �� ֵ:��
*************************************************************/
void TIM2_Config(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	  NVIC_InitTypeDef NVIC_InitStructure;
	
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //ʹ�ܶ�ʱ��4ʱ��

    /* Time Base configuration */
    TIM_TimeBaseStructure.TIM_Prescaler = 71; //Ԥ��ƵΪ1M,��ʱT1��ʱ��Ϊ1M
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = 999; //����ֵΪ1000��1msһ���ж�
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
		TIM_ITConfig(TIM2,TIM_IT_Update|TIM_IT_Trigger,ENABLE);
	
		//�ݳ�ʼ�� NVIC
		NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//��ռ���ȼ� 3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //�����ȼ� 3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ ͨ��ʹ��
		NVIC_Init(&NVIC_InitStructure); //����ָ���Ĳ�����ʼ�� VIC �Ĵ���
   
	 /* TIM1 counter enable */
    TIM_Cmd(TIM2, ENABLE);
}

/**********************************************************/
//TIM2�жϺ���
//����:������ˮ��
void TIM2_IRQHandler(void)   //TIM2�ж�
{ 
	u8 count;

	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
	{
		
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //���TIMx�����жϱ�־ 

    count++;
		if(count==15)	
		{
      timeout_flag=1;
			count=0;
		}	
	}	
}
/****END OF FILE****/
