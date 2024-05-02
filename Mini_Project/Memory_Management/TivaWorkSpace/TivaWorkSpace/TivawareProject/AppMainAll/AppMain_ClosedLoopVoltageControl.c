//*****************************************************************************************
// Author  		: Tessin K Jose
// File Name 	: AppMain.c
// Description  : Closed Loop Voltage Control
// Dependency 	: CCS , Driverlib
// Revision 	: Release date    	Simple Remark    	
//	1.0			: 1-11-2016 		Initial version
//*****************************************************************************************

#include "App.h"


char cUserInputstring[16];
uint32_t ui32ReferenceVoltageInmV;
uint32_t ui32ADCInputInmV;

int main(void)
{

	SetSysClockFrequency(SYSCTL_SYSDIV_5); //200/5 = 40MHz
	IntMasterDisable();
	InitConsole();
	OpenADCPort(ADC_CHANNEL_EXTERNAL,ADC_PROC_TRIGGER,0);
	OpenGPIOPortB();
	GPIOPortBWrite(0);

	UARTprintf("Enter the reference voltage to be set in mV \n");
	UARTgets(cUserInputstring,16);
	ui32ReferenceVoltageInmV = ui32ASCItoInteger(cUserInputstring);

	while( 1 )
	{
		ui32ADCInputInmV = ReadADCSampleInmV();
		if(ui32ADCInputInmV > ui32ReferenceVoltageInmV)
			GPIOPortBResetPin(PIN_0);
		else if(ui32ADCInputInmV < ui32ReferenceVoltageInmV)
			GPIOPortBSetPin(PIN_0);
	}

	return 0;
}
