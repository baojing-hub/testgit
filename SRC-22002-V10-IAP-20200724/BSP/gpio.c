/*******************************************************************************
* File Name: gpio.c
*
* Description:
* 本文件提供通用GPIO驱动的配置函数，请根据需要修改参数
* Note:
*
********************************************************************************/

#include "gpio.h"
#include "rcc.h"

u8 Sensor1_flag=0;
u8 Sensor2_flag=0;

/*************************************************************
函数名称:GPIO_Config
函数原型:void GPIO_Config()
功    能: 配置通用GPIO口。
参    数:无
返 回 值:无
调用函数:无
*************************************************************/
void GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  /****初始化LED1和LED2****/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//使能GPIOB的时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_0 | GPIO_Pin_1 );
	
	/****初始化SENS1和SENS2****/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);//使能GPIOD的时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
}


/*************************************************************
函数名称:Led_Ctrl
函数原型:void Led_Ctrl(void)
功    能: 程序运行指示灯上电后闪烁两次
参    数:无
返 回 值:无
调用函数:无
*************************************************************/
void Led_Ctrl(void)
{
//	程序运行指示灯	
	LED1_ON;
	Delay_Ms(200);
	LED1_OFF;
	Delay_Ms(200);
	LED1_ON;
	Delay_Ms(200);
	LED1_OFF;
}
/*************************************************************
函数名称:Read_Sensorstatus
函数原型:void Read_Sensorstatus(void)
功    能:读取两个传感器的状态
参    数:无
返 回 值:无
调用函数:无
*************************************************************/
void Read_Sensorstatus(void)
{
	if(Raed_Sens1==1)Sensor1_flag=1;
	else Sensor1_flag=0;
	if(Raed_Sens2==1)Sensor2_flag=1;
	else Sensor2_flag=0;
}


/****END OF FILE****/
