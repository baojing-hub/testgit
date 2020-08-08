/*******************************************************************************
* File Name: rcc.c
*
* Description:
* ���ļ��ṩMCUʱ�ӵ����ú������������Ҫ�޸Ĳ���
* Note:
*
********************************************************************************/
#include"rcc.h"
static u8  fac_us=0;//us��ʱ������
static u16 fac_ms=0;//ms��ʱ������
RCC_ClocksTypeDef RCC_ClockFreq;
/*************************************************************
��������:RCC_Config
����ԭ��:void RCC_Config()
��    ��:
ʱ�ӳ�ʼ��
ʹ��8M�ⲿʱ��
ϵͳʱ��Ƶ�ʣ�72M
PCLK1:36M

ȫ�ֱ�������
��    ��:��
�� �� ֵ:��
*************************************************************/
void RCC_Config(void)
{

	ErrorStatus HSEStartUpStatus;

    SystemInit();     //Դ��system_stm32f10x.c�ļ�,ֻ��Ҫ���ô˺���,������RCC������.�����뿴2_RCC

	RCC_DeInit();	  /* RCC������������RCC�Ĵ�������ΪĬ��ֵ */

    RCC_HSEConfig(RCC_HSE_ON);	  /* ʹ�� HSE�����ⲿ����ʱ�Ӿ���HSE */

    HSEStartUpStatus = RCC_WaitForHSEStartUp();	   /* �ȴ� HSE �������ȴ��ⲿ����ʱ�Ӿ���HSE������ */

    if(HSEStartUpStatus == SUCCESS)			       /*  HSE �����ȶ����Ѿ��� */
    {
        RCC_HCLKConfig(RCC_SYSCLK_Div1); 	       /* ����AHBʱ�ӣ�HCLK����HCLK = SYSCLK */
        RCC_PCLK2Config(RCC_HCLK_Div1); 	       /* ���ø���AHBʱ�ӣ�PCLK2����PCLK2 = HCLK */
        RCC_PCLK1Config(RCC_HCLK_Div2);		       /* ���õ���AHBʱ�ӣ�PCLK1����PCLK1 = HCLK/2 */
				#ifdef  STM32F10X_CL
        RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_9);	 /* ����PLLʱ��Դ����Ƶϵ����PLLCLK = HSEʱ��Ƶ�� 8MHz * 9 = 72 MHz */
        #else
				RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
			  #endif
			  RCC_PLLCmd(ENABLE);										 /* ʹ��PLL */

        while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)		 /* ���ָ����RCC��־λ��PLL׼���ñ�־��������񣺵ȴ� PLL���� */
        {
        }

        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);				 /* ѡ��PLL��Ϊϵͳʱ��Դ */

        while(RCC_GetSYSCLKSource() != 0x08)					 /* �ȴ�PLL��Ϊϵͳʱ��Դ���� */
        {
        }
    }

	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//ѡ���ⲿʱ��  HCLK/8
	fac_us=SystemCoreClock/8000000;	//Ϊϵͳʱ�ӵ�1/8
	fac_ms=(u16)fac_us*1000;//����ÿ��ms��Ҫ��systickʱ����
	/**************************************************
	��ȡRCC����Ϣ,������
	��ο�RCC_ClocksTypeDef�ṹ�������,��ʱ��������ɺ�,
	���������ֵ��ֱ�ӷ�ӳ�������������ֵ�����Ƶ��
	***************************************************/
	RCC_GetClocksFreq(&RCC_ClockFreq);							 /* ��оƬ�ڸ�ģ���ʱ��Ƶ�ʱ����ڲ�RCC_ClockFreq */

}



/*************************************************************
��������:Delay_Us
����ԭ��:void Delay_Us(u32 nus)
��    ��:΢�뼶��ȷ��ʱ
ȫ�ֱ�����TimingDelay
��    ��:��
�� �� ֵ:��
*************************************************************/
void Delay_Us(u32 nus)
{
	u32 temp;
	SysTick->LOAD=nus*fac_us; //ʱ�����
	SysTick->VAL=0x00;        //��ռ�����
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //��ʼ����
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //�رռ�����
	SysTick->VAL =0X00;       //��ռ�����
}



/*************************************************************
��������:Delay_ms
����ԭ��:void Delay_ms(__IO uint32_t nTime)
��    ��:���뼶��ȷ��ʱ
ȫ�ֱ�����TimingDelay
��    ��:��
�� �� ֵ:��
*************************************************************/
void Delay_Ms(u16 nms)
{
	u32 temp;
	SysTick->LOAD=(u32)nms*fac_ms;//ʱ�����(SysTick->LOADΪ24bit)
	SysTick->VAL =0x00;           //��ռ�����
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //��ʼ����
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //�رռ�����
	SysTick->VAL =0X00;       //��ռ�����
}






/****END OF FILE****/
