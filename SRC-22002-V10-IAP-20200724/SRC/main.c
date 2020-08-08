/**
*****************************************************************************
**
**  File        : main.c
**
**  Abstract    : main function.
**
**  Functions   : main
**
**
*****************************************************************************
*/

/* Includes */
#include "main.h"
extern u32 *pBuffer;
/* Private macro */
/* Private variables */


/* Private function prototypes */
/* Private functions */

/**
**===========================================================================
**
**  Abstract: main program
**
**===========================================================================
*/
int main(void)
{
  /* TODO - Add your application code here */
	Hal_Init();
  /* Infinite loop */
  while (1)
  {	
		ADS8698_RadeData(); 
    Data_MaxmumValue(pBuffer,5);		
	  Read_Sensorstatus();
  }
}

