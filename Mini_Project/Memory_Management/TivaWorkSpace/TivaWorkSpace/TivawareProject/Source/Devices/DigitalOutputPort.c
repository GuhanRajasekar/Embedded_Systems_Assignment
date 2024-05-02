//*****************************************************************************************
// Author  	: Tessin K Jose
// File Name 	: DigitalOutputPort.h 
// Description  : function definitions for using the GPIO PORT B as output port
// Dependency 	: CCS , Driverlib
// Revision 	: Release date    	Simple Remark    	
//	1.0	: 1-11-2016 		Initial version
//*****************************************************************************************

#include "DigitalOutputPort.h"

void OpenGPIOPortB(void)
{
	SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOB );
	GPIOPinTypeGPIOOutput( GPIO_PORTB_BASE, GPIO_PIN_ALL);
}

void GPIOPortBWrite(uint8_t ui8Val)
{
	GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_ALL, ui8Val);
}

void GPIOPortBSetPin(uint8_t ui8PinNo)
{
	GPIOPinWrite(GPIO_PORTB_BASE, ui8PinNo, ui8PinNo);
}

void GPIOPortBResetPin(uint8_t ui8PinNo)
{
	GPIOPinWrite(GPIO_PORTB_BASE, ui8PinNo, 0);
}

void GPIOPortBTogglePin(uint8_t ui8PinNo)
{
	uint8_t ui8PinCurrentstatus;
	
	ui8PinCurrentstatus = GPIOPinRead(GPIO_PORTB_BASE, ui8PinNo);	
	ui8PinCurrentstatus ^= ui8PinNo;
	GPIOPinWrite(GPIO_PORTB_BASE, ui8PinNo, ui8PinCurrentstatus);
}
 
void CloseGPIOPortB(void)
{
	SysCtlPeripheralDisable( SYSCTL_PERIPH_GPIOB );
}









