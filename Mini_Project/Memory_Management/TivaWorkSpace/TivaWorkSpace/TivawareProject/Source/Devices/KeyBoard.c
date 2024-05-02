//*****************************************************************************************
// Author  		: Tessin K Jose
// File Name 	: KeyBoard.C 
// Description  : functions for using the switches on launchpad and additional switches
// Dependency 	: CCS , Driverlib
// Revision 	: Release date    	Simple Remark    	
// 1.0			: 1-11-2016 		Initial version
//*****************************************************************************************

#include "KeyBoard.h"

uint32_t ui32KeyValue[4];
uint32_t ui32KeyPressstatus[4];

// currently two keys are conneted key1 to portF0 and key2 to portF4

void OpenKeyBoardPort(uint8_t ui8IntEnable)
{
	SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOF);
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));
	/*Unlock it: Otherwise, PF0 is not visible */
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
	HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;

	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE,GPIO_PIN_0 |GPIO_PIN_4);
	GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_0 |GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
	if(ui8IntEnable ==INT_ENABLE)
	{
		GPIOIntTypeSet(GPIO_PORTF_BASE,GPIO_PIN_0 |GPIO_PIN_4, GPIO_FALLING_EDGE);
		GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_0 |GPIO_INT_PIN_4);
		GPIOIntRegister(GPIO_PORTF_BASE, KeyPressIntHandler);
		GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_0 |GPIO_INT_PIN_4);
//		IntEnable( INT_GPIOF_TM4C123 );
	}

	/*Lock it once you have done */
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
	HWREG(GPIO_PORTF_BASE + GPIO_O_CR) = 0x00;
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
}
uint32_t ReadKeyValue(uint8_t ui8KeyNo)
{
	
	int32_t PortInReg;
	PortInReg= GPIOPinRead(GPIO_PORTF_BASE, GPIO_INT_PIN_0 |GPIO_INT_PIN_4);
	if((PortInReg & 0x1)==0)ui32KeyValue[0] =KEY_ON;
	else ui32KeyValue[0] =KEY_OFF; 
	if((PortInReg & 0x8)==0)ui32KeyValue[1] =KEY_ON;
	else ui32KeyValue[1] =KEY_OFF; 
	return ui32KeyValue[ui8KeyNo];
}

void WaitKeyPress(uint8_t ui8KeyNo)
{
	ui32KeyPressstatus[ui8KeyNo]=KEY_OFF;	
	while(ui32KeyPressstatus[ui8KeyNo] ==0);
}

uint32_t CheckKeyPress(uint8_t ui8KeyNo)
{
	return ui32KeyPressstatus[ui8KeyNo];	
}

void ClearKeyPress(uint8_t ui8KeyNo)
{
	ui32KeyPressstatus[ui8KeyNo]=KEY_OFF;	
}

void CloseKeyBoardPort(void)
{
	/*Unlock it: Otherwise, PF0 is not visible */
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
	HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;

	GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_0 |GPIO_INT_PIN_4);
	GPIOIntDisable(GPIO_PORTF_BASE, GPIO_INT_PIN_0 |GPIO_INT_PIN_4);
//	IntDisable( INT_GPIOF_TM4C123 );

	/*Lock it once you have done */
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
	HWREG(GPIO_PORTF_BASE + GPIO_O_CR) = 0x00;
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;

}

void KeyPressIntHandler(void)
{
	volatile uint32_t Count;

	if (GPIOIntStatus(GPIO_PORTF_BASE, false) & GPIO_PIN_0)
	{	// PF0 was interrupt cause
		GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_0);  // Clear interrupt flag
		ui32KeyPressstatus[0] = KEY_ON;
	}
	if(GPIOIntStatus(GPIO_PORTF_BASE, false) & GPIO_PIN_4)
	{
		GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4);  // Clear interrupt flag
		ui32KeyPressstatus[1] = KEY_ON;
	}
	//Key de-bounce delay//
	Count =10000;
	while(Count--);

}



