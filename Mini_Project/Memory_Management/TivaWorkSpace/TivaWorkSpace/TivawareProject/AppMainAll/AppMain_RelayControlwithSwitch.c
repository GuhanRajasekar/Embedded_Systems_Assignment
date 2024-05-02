//*****************************************************************************************
// Author  	: Tessin K Jose
// File Name 	: AppMain.c
// Description  : High Voltage Relay control with switch
// Dependency 	: CCS , Driverlib
// Revision 	: Release date    	Simple Remark    	
//	1.0	: 1-11-2016 		Initial version
//*****************************************************************************************

#include "App.h"

int main(void)
{

	SetSysClockFrequency(SYSCTL_SYSDIV_5); //200/5 = 40MHz
	IntMasterDisable();
	InitConsole();
	OpenKeyBoardPort(INT_ENABLE);
	OpenLEDPort();
	OffLED(LED_WHITE);
	OpenGPIOPortB();
	GPIOPortBResetPin(PIN_0);

	UARTprintf("Press Switch 2 to on or off the relay \n");
	UARTprintf("LED = RED , Relay On \n");
	UARTprintf("LED = OFF , Relay off \n");

	IntMasterEnable();

	while( 1 )
	{
		if(KEY_ON == CheckKeyPress(SW2))
		{
			ClearKeyPress(SW2);
			GPIOPortBTogglePin(PIN_0); //Pin0 controls on off
			ToggleLED(LED_RED);
		}
	}
	return 0;
}

