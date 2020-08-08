/*******************************************************************************
* File Name: can.h
*
* Description:
*  Contains the function prototypes and constants available to the CAN
*  user module.
*
* Note:
*
*******************************************************************************/


#ifndef CAN_H_
#define CAN_H_
#include <stdio.h>
#include <stdlib.h>
#include "stm32f10x.h"

#define CANDATALENGTH 8

void CAN_Config(void);
void CAN1_RX0_IRQHandler(void);
void CAN1_SCE_IRQHandler(void);
u8 Send_CanMessage(u16 targetID ,u8 statusNO,u8 orderNO,u8 *canData );
#endif /* CAN_H_ */


/****END OF FILE****/
