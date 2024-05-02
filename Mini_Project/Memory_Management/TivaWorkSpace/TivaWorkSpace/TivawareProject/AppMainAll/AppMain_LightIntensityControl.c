//*****************************************************************************************
// Author  	: Tessin K Jose
// File Name 	: AppMain.c
// Description  : Light Intensity control
// Dependency 	: CCS , Driverlib
// Revision 	: Release date    	Simple Remark    	
//	1.0	: 1-11-2016 		Initial version
//*****************************************************************************************
#include "App.h"

uint32_t ui32PWMDutyRatioPercentage=50;

int main(void)
{
	SetSysClockFrequency(SYSCTL_SYSDIV_5); //200/5 = 40MHz
	IntMasterDisable();
	InitConsole();
	OpenKeyBoardPort(INT_ENABLE);
	OpenLEDPort();
	OffLED(LED_WHITE);

	if (PWM_SUCCESS == OpenPWMModule(1000)) //Setup PWM module with SF = 1000Hz
	{
		ConfigurePWMDutyCycle(50);
		UARTprintf("Light Intensity is set to 50 percentage \n");
		EnablePWMOutput();
	}

	UARTprintf("Press Switch 1 to increase Light Intensity \n");
	UARTprintf("Press Switch 2 to decrease Light Intensity \n");

	IntMasterEnable();

	while( 1 )
	{
//		ProcessUARTCommand();
		if(KEY_ON == CheckKeyPress(SW1))
		{
			ClearKeyPress(SW1);
			ui32PWMDutyRatioPercentage +=5;
			if(ui32PWMDutyRatioPercentage > 99)
			{
				ui32PWMDutyRatioPercentage = 99;
				UARTprintf("Intensity set to maximum \n");
			}
			ConfigurePWMDutyCycle(ui32PWMDutyRatioPercentage);
			OffLED(LED_WHITE);
			OnLED(LED_RED);
		}

		if(KEY_ON == CheckKeyPress(SW2))
		{
			ClearKeyPress(SW2);
			ui32PWMDutyRatioPercentage -=5;
			if(ui32PWMDutyRatioPercentage < 1)
			{
				ui32PWMDutyRatioPercentage = 1;
				UARTprintf("Intensity set to minimum \n");
			}
			ConfigurePWMDutyCycle(ui32PWMDutyRatioPercentage);
			OffLED(LED_WHITE);
			OnLED(LED_GREEN);
		}
	}

	return 0;
}
