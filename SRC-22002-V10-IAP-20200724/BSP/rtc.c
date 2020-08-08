/*******************************************************************************
* File Name: rtc.c
*
* Description:
* 本文件提供rtc的配置函数，请根据需要修改参数
* Note:
*
********************************************************************************/
#include "rtc.h"

/*************************************************************
函数名称:RTC_NVIC_Config
函数原型:static void RTC_NVIC_Config(void)
功    能:
RTC中断优先级配置
全局变量：无
参    数:无
返 回 值:无
*************************************************************/
static void RTC_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn; //RTC全局中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //先占优先级1位,从优先级3位
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //先占优先级0位,从优先级4位
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //使能该通道中断
    NVIC_Init(&NVIC_InitStructure); //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
}

/*************************************************************
函数名称:RTC_Config
函数原型:u8 RTC_Config(void)
功    能:
RTC初始化
全局变量：无
参    数:无
返 回 值:0：初始化成功；1：初始化失败
*************************************************************/
u8 RTC_Config(void)
{

    u8 temp = 0;

    if (BKP_ReadBackupRegister(BKP_DR1) != 0x5050) //从指定的后备寄存器中读出数据:读出了与写入的指定数据不相乎
    {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE); //使能PWR和BKP外设时钟
        PWR_BackupAccessCmd(ENABLE); //使能后备寄存器访问
        BKP_DeInit(); //复位备份区域
        RCC_LSEConfig(RCC_LSE_ON); //设置外部低速晶振(LSE),使用外设低速晶振
        while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) //检查指定的RCC标志位设置与否,等待低速晶振就绪
        {
            temp++;
            Delay_Ms(10);
        }
        if (temp >= 250)
            return 1; //初始化时钟失败,晶振有问题
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE); //设置RTC时钟(RTCCLK),选择LSE作为RTC时钟
        RCC_RTCCLKCmd(ENABLE); //使能RTC时钟
        RTC_WaitForLastTask(); //等待最近一次对RTC寄存器的写操作完成
        RTC_WaitForSynchro(); //等待RTC寄存器同步
        RTC_ITConfig(RTC_IT_SEC, ENABLE); //使能RTC秒中断
        RTC_WaitForLastTask(); //等待最近一次对RTC寄存器的写操作完成
        RTC_EnterConfigMode(); // 允许配置
        RTC_SetPrescaler(32767); //设置RTC预分频的值
        RTC_WaitForLastTask(); //等待最近一次对RTC寄存器的写操作完成
        RTC_ExitConfigMode(); //退出配置模式
        BKP_WriteBackupRegister(BKP_DR1, 0X5050); //向指定的后备寄存器中写入用户程序数据
    } else //系统继续计时
    {

        RTC_WaitForSynchro(); //等待最近一次对RTC寄存器的写操作完成
        RTC_ITConfig(RTC_IT_SEC, ENABLE); //使能RTC秒中断
        RTC_WaitForLastTask(); //等待最近一次对RTC寄存器的写操作完成
    }
    RTC_NVIC_Config(); //RCT中断分组设置
    return 0; //ok
}



/*************************************************************
函数名称:RTC_IRQHandler
函数原型:void RTC_IRQHandler(void)
功    能:
RTC中断服务函数
全局变量：无
参    数:无
返 回 值:无
*************************************************************/
void RTC_IRQHandler(void)
{
    if (RTC_GetITStatus(RTC_IT_SEC) != RESET) //秒钟中断
    {
    }
    if (RTC_GetITStatus(RTC_IT_ALR) != RESET) //闹钟中断
    {
        RTC_ClearITPendingBit(RTC_IT_ALR); //清闹钟中断
    }
    RTC_ClearITPendingBit(RTC_IT_SEC | RTC_IT_OW); //清闹钟中断
    RTC_WaitForLastTask();
}

/****END OF FILE****/
