/*******************************************************************************
* File Name: gpio.h
*
* Description:
*  Contains the function prototypes and constants available to the GPIO
*  user module.
*
* Note:
*
*******************************************************************************/
#ifndef GPIO_H
#define GPIO_H
#include "stm32f10x.h"

#define LED1_ON     GPIO_ResetBits(GPIOB,GPIO_Pin_0)
#define LED1_OFF    GPIO_SetBits(GPIOB,GPIO_Pin_0)
#define LED2_ON     GPIO_ResetBits(GPIOB,GPIO_Pin_1)
#define LED2_OFF    GPIO_SetBits(GPIOB,GPIO_Pin_1)
#define Raed_Sens1  GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_8)
#define Raed_Sens2  GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_9)
void GPIO_Config(void);
void Led_Ctrl(void);
void Read_Sensorstatus(void);

#endif




/****END OF FILE****/
