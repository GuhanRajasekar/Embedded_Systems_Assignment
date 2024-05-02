//*****************************************************************************************
// Author  	: Tessin K Jose
// File Name 	: AppMain.c
// Description  : Frequency Counter
// Dependency 	: CCS , Driverlib
// Revision 	: Release date    	Simple Remark    	
//	1.0	: 1-11-2016 		Initial version
//*****************************************************************************************

#include "App.h"

extern uint32_t ui32Timer0IntFlag;
extern uint32_t ui32PinIntCount[4];

uint32_t ui32InputPulseCount=0;
uint32_t ui32InputFrequency=0;

int main(void)
{

	SetSysClockFrequency(SYSCTL_SYSDIV_5); //200/5 = 40MHz
	IntMasterDisable();
	InitConsole();
	OpenGPIOCInputPort();
	EnableGPIOCPinInt(IN_4);

	UARTprintf("Connect Your Input signal to PortC pin4 \n");

	OpenTimer0(1000000);
	IntMasterEnable();
	StartTimer0();

	while( 1 )
	{

		if(ui32Timer0IntFlag == 1)
		{
			ui32Timer0IntFlag =0;
			if(ui32InputPulseCount !=0)
			{
				ui32InputFrequency = ui32PinIntCount[0] - ui32InputPulseCount;
			}
			ui32InputPulseCount = ui32PinIntCount[0];
			UARTprintf("Input frequency is %d Hz \n",ui32InputFrequency);
		}
	}

	return 0;
}
