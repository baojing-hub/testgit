/*******************************************************************************
* File Name: can.h
*
* Description:
*  Contains the function prototypes and constants available to the USART
*  user module.
*
* Note:
*
*******************************************************************************/

#ifndef USART_H_
#define USART_H_

#include <stdio.h>
#include <stdlib.h>
#include "stm32f10x.h"

#define FH 0X55
#define ID 0X00
#define FHE 0XAA
#define Cup_Value      0x01
#define Sensor_Status  0X02
#define Command_OK     0x64
#define Command_ERROR  0x32
#define BAUDRATE 115200
extern u8 g_usartRxFlag;
void USART_Config(void);
void USART1_IRQHandler(void);
void USART_SendString(char *str);
#endif /* USART_H_ */



/****END OF FILE****/
