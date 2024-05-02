//*****************************************************************************************
// Author  	: Tessin K Jose
// File Name 	: AppMain.c
// Description  : Motor speed control with PWM module
// Dependency 	: CCS , Driverlib
// Revision 	: Release date    	Simple Remark    	
//	1.0	: 1-11-2016 		Initial version
//*****************************************************************************************

#include "App.h"

uint32_t ui32PWMDutyRatioPercentage=50;
char cUserInputstring[16];

int main(void)
{

	SetSysClockFrequency(SYSCTL_SYSDIV_5); //200/5 = 40MHz
	IntMasterDisable();
	InitConsole();
	if (PWM_SUCCESS == OpenPWMModule(100)) //Setup PWM module with SF = 100Hz
	{
		ConfigurePWMDutyCycle(50);
		UARTprintf("PWM Module is started with D = 50 percentage \n");
		EnablePWMOutput();
	}

	UARTprintf("Choose the Duty Cycle You would like to set 1 to 99 \n");

	while( 1 )
	{
		UARTgets(cUserInputstring,16);
		ui32PWMDutyRatioPercentage = ui32ASCItoInteger(cUserInputstring);

		if((ui32PWMDutyRatioPercentage <100)&&(ui32PWMDutyRatioPercentage >0))
		{
			ConfigurePWMDutyCycle(ui32PWMDutyRatioPercentage);
			UARTprintf("Duty Cycle set to  %d percentage \n",ui32PWMDutyRatioPercentage);
		}
		else UARTprintf("Invalid Duty Cycle \n");
	}

	return 0;
}

