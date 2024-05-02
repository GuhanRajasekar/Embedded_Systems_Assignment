//*****************************************************************************************
// Author  	: Tessin K Jose
// File Name 	: AppMain.c
// Description  : Motor speed  measurement and control
// Dependency 	: CCS , Driverlib
// Revision 	: Release date    	Simple Remark    	
//	1.0	: 1-11-2016 		Initial version
//*****************************************************************************************
#include "App.h"

extern uint32_t ui32Timer0IntFlag;
extern uint32_t ui32PinIntCount[4];

uint32_t ui32InputPulseCount=0;
uint32_t ui32InputFrequency=0;

uint32_t ui32PWMDutyRatioPercentage=50;
int sampleCount =0;
int RPMSample[10];


int main(void)
{
	int i;

	SetSysClockFrequency(SYSCTL_SYSDIV_5); //200/5 = 40MHz
	IntMasterDisable();
	InitConsole();
	OpenGPIOCInputPort();
	EnableGPIOCPinInt(IN_4);
	OpenTimer0(6000000);

	UARTprintf("Connect Your Input signal to PortC pin4 \n");

	if (PWM_SUCCESS == OpenPWMModule(500)) //Setup PWM module with SF = 100Hz
	{
		ConfigurePWMDutyCycle(50);
		UARTprintf("PWM Module is started with D = 50 percentage \n");
		EnablePWMOutput();
	}

	UARTprintf("Use Command 'setpwmdc 1-99' to change the duty cycle \n");

	IntMasterEnable();
	StartTimer0();

	while( 1 )
	{
		ProcessUARTCommand();
		if(ui32Timer0IntFlag == 1)
		{
			ui32Timer0IntFlag =0;
			if(ui32InputPulseCount !=0)
			{
				RPMSample [sampleCount++] = ui32PinIntCount[0] - ui32InputPulseCount;
			}
			ui32InputPulseCount = ui32PinIntCount[0];
			if(sampleCount==10)
			{
				ui32InputFrequency =0;
				for(i=0;i<10;i++)ui32InputFrequency += RPMSample[i];
				sampleCount =0;
				ui32InputFrequency = ui32InputFrequency/10;
				UARTprintf("Motor RPM is %d  \n",ui32InputFrequency*10);
			}
		}
	}

	return 0;
}

