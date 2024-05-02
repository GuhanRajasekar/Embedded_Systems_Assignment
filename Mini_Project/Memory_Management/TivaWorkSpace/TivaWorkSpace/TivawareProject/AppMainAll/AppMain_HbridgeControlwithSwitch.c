//*****************************************************************************************
// Author  	: Tessin K Jose
// File Name 	: AppMain.c
// Description  : H-Bridge Control with switches
// Dependency 	: CCS , Driverlib
// Revision 	: Release date    	Simple Remark    	
//	1.0	: 1-11-2016 		Initial version
//*****************************************************************************************

#include "App.h"

uint32_t ui32MotorOnOff=0;
uint32_t ui32MotorForwardReverse=0;


int main(void)
{

	SetSysClockFrequency(SYSCTL_SYSDIV_5); //200/5 = 40MHz
	IntMasterDisable();
	InitConsole();
	OpenKeyBoardPort(INT_ENABLE);
	OpenLEDPort();
	OffLED(LED_WHITE);
	OnLED(LED_RED);
	OpenGPIOPortB();
	GPIOPortBResetPin(PIN_1 |PIN_0);

	UARTprintf("Press Switch 1 to start or stop the Motor \n");
	UARTprintf("Press Switch 2 to change the direction of Motor \n");
	UARTprintf("LED = RED , Motor Stopped \n");
	UARTprintf("LED = GREEN , Motor Running in Forward Direction \n");
	UARTprintf("LED = BLUE , Motor Running in Reverse Direction \n");

	IntMasterEnable();

	while( 1 )
	{
		if(KEY_ON == CheckKeyPress(SW1))
		{
			ClearKeyPress(SW1);
			GPIOPortBTogglePin(PIN_0); //Pin0 controls on off
			if(ui32MotorOnOff == 0)
			{
				ui32MotorOnOff =1;
				OffLED(LED_RED);
			}
			else
			{
				ui32MotorOnOff =0;
				OffLED(LED_WHITE);
				OnLED(LED_RED);
			}
		}
		if(KEY_ON == CheckKeyPress(SW2))
		{
			ClearKeyPress(SW2);
			if(ui32MotorOnOff ==1)
			{
				GPIOPortBTogglePin(PIN_1);// Pin1 controls direction
				if(ui32MotorForwardReverse == 0)
				{
					ui32MotorForwardReverse =1;
					OffLED(LED_WHITE);
					OnLED(LED_GREEN);
				}
				else
				{
					ui32MotorForwardReverse =0;
					OffLED(LED_WHITE);
					OnLED(LED_BLUE);
				}
			}
		}
	}

	return 0;
}

