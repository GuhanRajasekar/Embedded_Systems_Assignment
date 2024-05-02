//*****************************************************************************************
// Author  		: Tessin K Jose
// File Name 	: LED.C 
// Description  : functions for using the LEDs on launchpad and additional LEDs
// Dependency 	: CCS , Driverlib
// Revision 	: Release date    	Simple Remark    	
//	1.0			: 1-11-2016 		Initial version
//*****************************************************************************************

#include "LED.h"

void OpenLEDPort(void)
{
	SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOF );
	GPIOPinTypeGPIOOutput( GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 );
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
}

void OnLED(uint8_t ui8LEDNo)
{
	GPIOPinWrite(GPIO_PORTF_BASE, ui8LEDNo, ui8LEDNo);
}

void OffLED(uint8_t ui8LEDNo)
{
	GPIOPinWrite(GPIO_PORTF_BASE, ui8LEDNo, 0);
}

void ToggleLED(uint8_t ui8LEDNo)
{
	uint8_t ui8LEDCurrentstatus;
	ui8LEDCurrentstatus=GPIOPinRead(GPIO_PORTF_BASE, ui8LEDNo);
	ui8LEDCurrentstatus ^= ui8LEDNo;
	GPIOPinWrite(GPIO_PORTF_BASE, ui8LEDNo,ui8LEDCurrentstatus);
}

void ChangeColourofTriColourLED(void)
{
	uint8_t ui8LEDCurrentstatus;
	ui8LEDCurrentstatus=GPIOPinRead(GPIO_PORTF_BASE, LED_WHITE);
	if((ui8LEDCurrentstatus <= 0)||(ui8LEDCurrentstatus >= 8)) ui8LEDCurrentstatus=2;
	else ui8LEDCurrentstatus *= 2;
	GPIOPinWrite(GPIO_PORTF_BASE, LED_WHITE,ui8LEDCurrentstatus);
}
void CloseLEDPort(void)
{
}
