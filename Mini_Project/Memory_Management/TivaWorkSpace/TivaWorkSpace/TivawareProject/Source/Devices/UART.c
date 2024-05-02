//*****************************************************************************************
// Author  		: Tessin K Jose
// File Name 	: UART.c 
// Description  : functions for using the UARTStdio functions
// Dependency 	: CCS , Driverlib
// Revision 	: Release date    	Simple Remark    	
//	1.0			: 1-11-2016 		Initial version
//*****************************************************************************************

#include "UART.h"
#include "GenLib.h"


//*****************************************************************************
//
// This function sets up UART0 to be used for a console to display information
//
//*****************************************************************************
void InitConsole(void)
{
    // Enable GPIO port A which is used for UART0 pins.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    // Configure the pin muxing for UART0 functions on port A0 and A1.
    // This step is not necessary if your part does not support pin muxing.
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    // Enable UART0 so that we can configure the clock.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    // Use the internal 16MHz oscillator as the UART clock source.
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    // Select the alternate (UART) function for these pins.
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Initialize the UART for console I/O.
    UARTStdioConfig(0, 115200, 16000000);
}

void OpenUART0(uint32_t RxIntEnable)
{

	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
		(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
	if(RxIntEnable == UART_RX_INT_ENABLE)
	{
		UARTIntEnable(UART0_BASE, UART_INT_RX ); //only enable RX 
		IntEnable(INT_UART0); //enable the UART interrupt
	}
}

void WriteUART0(char *pString)
{
	uint8_t uiIndex = 0; 
	
	while((pString[uiIndex]!=0))
	{
		UARTCharPut(UART0_BASE,pString[uiIndex]);
		uiIndex++;
	}
}

uint32_t GetNumberFromUART(void)
{
	char cUserInputstring[16];
	uint32_t ui32NumFromUART;

	UARTgets(cUserInputstring,16);
	ui32NumFromUART = ui32ASCItoInteger(cUserInputstring);
	return ui32NumFromUART;
}

void CloseUART0(void)
{	
	UARTIntDisable(UART0_BASE, UART_INT_RX ); 
	IntDisable(INT_UART0); 
	SysCtlPeripheralDisable(SYSCTL_PERIPH_UART0);
}

void UART0IntHandler(void)
{
	uint32_t ui32Status;
	uint32_t ui32CharRx[12];
	uint32_t ui32RxCount;

	ui32Status = UARTIntStatus(UART0_BASE, true); //get interrupt status
	UARTIntClear(UART0_BASE, ui32Status); //clear the asserted interrupts
	ui32RxCount =0;
	while(UARTCharsAvail(UART0_BASE)) //loop while there are chars
	{
		ui32CharRx[ui32RxCount] = UARTCharGetNonBlocking(UART0_BASE);
		UARTCharPutNonBlocking(UART0_BASE,ui32CharRx[ui32RxCount] ); //echo character
		ui32RxCount++;
	}	
}

