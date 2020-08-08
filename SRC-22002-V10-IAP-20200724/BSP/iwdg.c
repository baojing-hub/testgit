/*******************************************************************************
* File Name: ciwdg.h
*
* Description:
* 本文件提供看门狗驱动的配置函数，请根据需要修改参数
* Note:
*
********************************************************************************/

#include "iwdg.h"



/*************************************************************
函数名称:IWDG_Config
函数原型:void IWDG_Config(void)
功    能:
	初始化独立看门狗
全局变量：
参    数:无
返 回 值:无
*************************************************************/
void IWDG_Config(void)
{
    /* 写入0x5555,用于允许狗狗寄存器写入功能 */
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

    /* 狗狗时钟分频,40K/256=156HZ(6.4ms)*/
    IWDG_SetPrescaler(IWDG_Prescaler_256);

    /* 喂狗时间 5s/6.4MS=781 .注意不能大于0xfff*/
    IWDG_SetReload(781);

    /* 喂狗*/
    IWDG_ReloadCounter();

    /* 使能狗狗*/
    IWDG_Enable();
}



/*************************************************************
函数名称:IWDG_Feed
函数原型:void IWDG_Feed(void)
功    能:
	喂独立看门狗
全局变量：
参    数:无
返 回 值:无
*************************************************************/
void IWDG_Feed(void)
{
    IWDG_ReloadCounter(); //reload
}




/****END OF FILE****/
