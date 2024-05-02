//*****************************************************************************************
// Author  		: Tessin K Jose
// File Name 	: UART.h 
// Description  : prtototypes for using the UARTStdio functions
// Dependency 	: CCS , Driverlib
// Revision 	: Release date    	Simple Remark    	
//	1.0			: 1-11-2016 		Initial version
//*****************************************************************************************

#ifndef UART_H
#define UART_H

#include "TIResources.h"
#include "utils/uartstdio.h"

#define UART_RX_INT_DISABLE 	0
#define UART_RX_INT_ENABLE 	1

// initialise the uart0 in 115200 8N S mode
void InitConsole(void);

// Separate function to init uart with rx intr enabled
void OpenUART0(uint32_t RxIntEnable);

//write a string to uart
void WriteUART0(char *pString);

// get a number from uart
uint32_t GetNumberFromUART(void);

void CloseUART0(void);

void UART0IntHandler(void);

#endif // UART_H




