/*******************************************************************************
* File Name: rtc.c
*
* Description:
* ���ļ��ṩrtc�����ú������������Ҫ�޸Ĳ���
* Note:
*
********************************************************************************/
#include "rtc.h"

/*************************************************************
��������:RTC_NVIC_Config
����ԭ��:static void RTC_NVIC_Config(void)
��    ��:
RTC�ж����ȼ�����
ȫ�ֱ�������
��    ��:��
�� �� ֵ:��
*************************************************************/
static void RTC_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn; //RTCȫ���ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //��ռ���ȼ�1λ,�����ȼ�3λ
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //��ռ���ȼ�0λ,�����ȼ�4λ
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //ʹ�ܸ�ͨ���ж�
    NVIC_Init(&NVIC_InitStructure); //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
}

/*************************************************************
��������:RTC_Config
����ԭ��:u8 RTC_Config(void)
��    ��:
RTC��ʼ��
ȫ�ֱ�������
��    ��:��
�� �� ֵ:0����ʼ���ɹ���1����ʼ��ʧ��
*************************************************************/
u8 RTC_Config(void)
{

    u8 temp = 0;

    if (BKP_ReadBackupRegister(BKP_DR1) != 0x5050) //��ָ���ĺ󱸼Ĵ����ж�������:��������д���ָ�����ݲ����
    {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE); //ʹ��PWR��BKP����ʱ��
        PWR_BackupAccessCmd(ENABLE); //ʹ�ܺ󱸼Ĵ�������
        BKP_DeInit(); //��λ��������
        RCC_LSEConfig(RCC_LSE_ON); //�����ⲿ���پ���(LSE),ʹ��������پ���
        while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) //���ָ����RCC��־λ�������,�ȴ����پ������
        {
            temp++;
            Delay_Ms(10);
        }
        if (temp >= 250)
            return 1; //��ʼ��ʱ��ʧ��,����������
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE); //����RTCʱ��(RTCCLK),ѡ��LSE��ΪRTCʱ��
        RCC_RTCCLKCmd(ENABLE); //ʹ��RTCʱ��
        RTC_WaitForLastTask(); //�ȴ����һ�ζ�RTC�Ĵ�����д�������
        RTC_WaitForSynchro(); //�ȴ�RTC�Ĵ���ͬ��
        RTC_ITConfig(RTC_IT_SEC, ENABLE); //ʹ��RTC���ж�
        RTC_WaitForLastTask(); //�ȴ����һ�ζ�RTC�Ĵ�����д�������
        RTC_EnterConfigMode(); // ��������
        RTC_SetPrescaler(32767); //����RTCԤ��Ƶ��ֵ
        RTC_WaitForLastTask(); //�ȴ����һ�ζ�RTC�Ĵ�����д�������
        RTC_ExitConfigMode(); //�˳�����ģʽ
        BKP_WriteBackupRegister(BKP_DR1, 0X5050); //��ָ���ĺ󱸼Ĵ�����д���û���������
    } else //ϵͳ������ʱ
    {

        RTC_WaitForSynchro(); //�ȴ����һ�ζ�RTC�Ĵ�����д�������
        RTC_ITConfig(RTC_IT_SEC, ENABLE); //ʹ��RTC���ж�
        RTC_WaitForLastTask(); //�ȴ����һ�ζ�RTC�Ĵ�����д�������
    }
    RTC_NVIC_Config(); //RCT�жϷ�������
    return 0; //ok
}



/*************************************************************
��������:RTC_IRQHandler
����ԭ��:void RTC_IRQHandler(void)
��    ��:
RTC�жϷ�����
ȫ�ֱ�������
��    ��:��
�� �� ֵ:��
*************************************************************/
void RTC_IRQHandler(void)
{
    if (RTC_GetITStatus(RTC_IT_SEC) != RESET) //�����ж�
    {
    }
    if (RTC_GetITStatus(RTC_IT_ALR) != RESET) //�����ж�
    {
        RTC_ClearITPendingBit(RTC_IT_ALR); //�������ж�
    }
    RTC_ClearITPendingBit(RTC_IT_SEC | RTC_IT_OW); //�������ж�
    RTC_WaitForLastTask();
}

/****END OF FILE****/
