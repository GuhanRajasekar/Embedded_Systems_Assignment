//*****************************************************************************************
// Author  	: Tessin K Jose
// File Name 	: DigitalInputPort.c
// Description  : functions for using the GPIO PORT C pins(4,5,6,7)as input port
// Dependency 	: CCS , Driverlib
// Revision 	: Release date    	Simple Remark    	
//	1.0	: 1-11-2016 		Initial version
//*****************************************************************************************

#include "DigitalInputPort.h"

uint32_t ui32PinIntStatus[4];
uint32_t ui32PinIntCount[4];

void OpenGPIOCInputPort(void)
{
	SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOC);
	GPIOPinTypeGPIOInput(GPIO_PORTC_BASE,GPIO_PIN_4 |GPIO_PIN_5|GPIO_PIN_6 |GPIO_PIN_7);
	GPIOPadConfigSet(GPIO_PORTC_BASE,GPIO_PIN_4 |GPIO_PIN_5|GPIO_PIN_6 |GPIO_PIN_7, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
}

void EnableGPIOCPinInt(uint8_t ui8PinNo)
{
	GPIOIntTypeSet(GPIO_PORTC_BASE,ui8PinNo, GPIO_RISING_EDGE);
	GPIOIntClear(GPIO_PORTC_BASE,ui8PinNo);
	GPIOIntRegister(GPIO_PORTC_BASE, GPIOCPinIntHandler);
	GPIOIntEnable(GPIO_PORTC_BASE,ui8PinNo);
//	IntEnable( INT_GPIOC_TM4C123 );
}

uint32_t ReadPinValue(uint8_t ui8PinNo)
{
	
	int32_t PortInReg;
	uint32_t ui32PinValue;

	PortInReg= GPIOPinRead(GPIO_PORTC_BASE, ui8PinNo);
	if((PortInReg & ui8PinNo)==1)ui32PinValue =PIN_HIGH;
	else ui32PinValue =PIN_LOW;

	return ui32PinValue;
}

void WaitPinInt(uint8_t ui8PinNo)
{
	uint32_t ui32KeyIndex;

	if(ui8PinNo == GPIO_PIN_4)ui32KeyIndex =0;
	else if(ui8PinNo == GPIO_PIN_5)ui32KeyIndex =1;
	else if(ui8PinNo == GPIO_PIN_6)ui32KeyIndex =2;
	else if(ui8PinNo == GPIO_PIN_7)ui32KeyIndex =3;

	ui32PinIntStatus[ui32KeyIndex]=PIN_LOW;	
	while(ui32PinIntStatus[ui32KeyIndex] ==PIN_LOW);
}

uint32_t CheckPinInt(uint8_t ui8PinNo)
{
	uint32_t ui32KeyIndex;

	if(ui8PinNo == GPIO_PIN_4)ui32KeyIndex =0;
	else if(ui8PinNo == GPIO_PIN_5)ui32KeyIndex =1;
	else if(ui8PinNo == GPIO_PIN_6)ui32KeyIndex =2;
	else if(ui8PinNo == GPIO_PIN_7)ui32KeyIndex =3;

	return ui32PinIntStatus[ui32KeyIndex];
}

void ClearPinInt(uint8_t ui8PinNo)
{
	uint32_t ui32KeyIndex;

	if(ui8PinNo == GPIO_PIN_4)ui32KeyIndex =0;
	else if(ui8PinNo == GPIO_PIN_5)ui32KeyIndex =1;
	else if(ui8PinNo == GPIO_PIN_6)ui32KeyIndex =2;
	else if(ui8PinNo == GPIO_PIN_7)ui32KeyIndex =3;

	ui32PinIntStatus[ui32KeyIndex]=PIN_LOW;	
}

void CloseGPIOCInputPort(void)
{
	GPIOIntClear(GPIO_PORTC_BASE, GPIO_PIN_4 |GPIO_PIN_5|GPIO_PIN_6 |GPIO_PIN_7);
	GPIOIntDisable(GPIO_PORTC_BASE, GPIO_PIN_4 |GPIO_PIN_5|GPIO_PIN_6 |GPIO_PIN_7);
//	IntDisable( INT_GPIOC_TM4C123 );
}

void GPIOCPinIntHandler(void)
{
	if (GPIOIntStatus(GPIO_PORTC_BASE, false) & GPIO_PIN_4)
	{	GPIOIntClear(GPIO_PORTC_BASE, GPIO_PIN_4);  // Clear interrupt flag
		ui32PinIntStatus[0] = PIN_HIGH;
		ui32PinIntCount[0]++;
	}
	if (GPIOIntStatus(GPIO_PORTC_BASE, false) & GPIO_PIN_5)
	{	GPIOIntClear(GPIO_PORTC_BASE, GPIO_PIN_5);  // Clear interrupt flag
		ui32PinIntStatus[1] = PIN_HIGH;
		ui32PinIntCount[1]++;
	}
	if (GPIOIntStatus(GPIO_PORTC_BASE, false) & GPIO_PIN_6)
	{	GPIOIntClear(GPIO_PORTC_BASE, GPIO_PIN_6);  // Clear interrupt flag
		ui32PinIntStatus[2] = PIN_HIGH;
		ui32PinIntCount[2]++;
	}
	if (GPIOIntStatus(GPIO_PORTC_BASE, false) & GPIO_PIN_7)
	{	GPIOIntClear(GPIO_PORTC_BASE, GPIO_PIN_7);  // Clear interrupt flag
		ui32PinIntStatus[3] = PIN_HIGH;
		ui32PinIntCount[3]++;
	}
}

