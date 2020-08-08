/*******************************************************************************
* File Name: rcc.c
*
* Description:
* 本文件提供MCU时钟的配置函数，请根据需要修改参数
* Note:
*
********************************************************************************/
#include"rcc.h"
static u8  fac_us=0;//us延时倍乘数
static u16 fac_ms=0;//ms延时倍乘数
RCC_ClocksTypeDef RCC_ClockFreq;
/*************************************************************
函数名称:RCC_Config
函数原型:void RCC_Config()
功    能:
时钟初始化
使用8M外部时钟
系统时钟频率：72M
PCLK1:36M

全局变量：无
参    数:无
返 回 值:无
*************************************************************/
void RCC_Config(void)
{

	ErrorStatus HSEStartUpStatus;

    SystemInit();     //源自system_stm32f10x.c文件,只需要调用此函数,则可完成RCC的配置.具体请看2_RCC

	RCC_DeInit();	  /* RCC重启：将外设RCC寄存器重设为默认值 */

    RCC_HSEConfig(RCC_HSE_ON);	  /* 使能 HSE：打开外部高速时钟晶振HSE */

    HSEStartUpStatus = RCC_WaitForHSEStartUp();	   /* 等待 HSE 就绪：等待外部高速时钟晶振HSE起振工作 */

    if(HSEStartUpStatus == SUCCESS)			       /*  HSE 晶振稳定且已就绪 */
    {
        RCC_HCLKConfig(RCC_SYSCLK_Div1); 	       /* 设置AHB时钟（HCLK）：HCLK = SYSCLK */
        RCC_PCLK2Config(RCC_HCLK_Div1); 	       /* 设置高速AHB时钟（PCLK2）：PCLK2 = HCLK */
        RCC_PCLK1Config(RCC_HCLK_Div2);		       /* 设置低速AHB时钟（PCLK1）：PCLK1 = HCLK/2 */
				#ifdef  STM32F10X_CL
        RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_9);	 /* 设置PLL时钟源及倍频系数：PLLCLK = HSE时钟频率 8MHz * 9 = 72 MHz */
        #else
				RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
			  #endif
			  RCC_PLLCmd(ENABLE);										 /* 使能PLL */

        while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)		 /* 检查指定的RCC标志位（PLL准备好标志）设置与否：等待 PLL就绪 */
        {
        }

        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);				 /* 选择PLL作为系统时钟源 */

        while(RCC_GetSYSCLKSource() != 0x08)					 /* 等待PLL作为系统时钟源就绪 */
        {
        }
    }

	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//选择外部时钟  HCLK/8
	fac_us=SystemCoreClock/8000000;	//为系统时钟的1/8
	fac_ms=(u16)fac_us*1000;//代表每个ms需要的systick时钟数
	/**************************************************
	获取RCC的信息,调试用
	请参考RCC_ClocksTypeDef结构体的内容,当时钟配置完成后,
	里面变量的值就直接反映了器件各个部分的运行频率
	***************************************************/
	RCC_GetClocksFreq(&RCC_ClockFreq);							 /* 将芯片内各模块的时钟频率保存在参RCC_ClockFreq */

}



/*************************************************************
函数名称:Delay_Us
函数原型:void Delay_Us(u32 nus)
功    能:微秒级精确延时
全局变量：TimingDelay
参    数:无
返 回 值:无
*************************************************************/
void Delay_Us(u32 nus)
{
	u32 temp;
	SysTick->LOAD=nus*fac_us; //时间加载
	SysTick->VAL=0x00;        //清空计数器
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //开始倒数
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//等待时间到达
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //关闭计数器
	SysTick->VAL =0X00;       //清空计数器
}



/*************************************************************
函数名称:Delay_ms
函数原型:void Delay_ms(__IO uint32_t nTime)
功    能:毫秒级精确延时
全局变量：TimingDelay
参    数:无
返 回 值:无
*************************************************************/
void Delay_Ms(u16 nms)
{
	u32 temp;
	SysTick->LOAD=(u32)nms*fac_ms;//时间加载(SysTick->LOAD为24bit)
	SysTick->VAL =0x00;           //清空计数器
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //开始倒数
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//等待时间到达
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //关闭计数器
	SysTick->VAL =0X00;       //清空计数器
}






/****END OF FILE****/
