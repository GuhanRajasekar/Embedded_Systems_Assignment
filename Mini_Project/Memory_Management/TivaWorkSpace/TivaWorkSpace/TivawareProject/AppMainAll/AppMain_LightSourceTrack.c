//*****************************************************************************************
// Author  		: Tessin K Jose
// File Name 	: AppMain.c
// Description  : Light Source tracking
// Dependency 	: CCS , Driverlib
// Revision 	: Release date    	Simple Remark    	
//	1.0			: 1-11-2016 		Initial version
//*****************************************************************************************

#include "App.h"

extern uint32_t ui32Timer0IntFlag;
void RunStepperMotor(void);

uint8_t ui8_Stepval[8]= {12,4,6,2,3,1,9,8};
int32_t i32StepIndex=0;
uint32_t ui32SteppingMode=0;
uint32_t ui32StepTimemS = 1000;
int32_t i32CurrentPosition=0;

uint32_t ui32ADCInputInmV,ui32ADC1InputInmV;
uint32_t ui32ADCInputMaxInmV =0;
uint32_t ui32ADC1InputMaxInmV =0;

int main(void)
{

	SetSysClockFrequency(SYSCTL_SYSDIV_5); //200/5 = 40MHz
	IntMasterDisable();
	InitConsole();
	OpenADCPort(ADC_CHANNEL_EXTERNAL,ADC_PROC_TRIGGER,0);
	OpenADC1Port(ADC_CHANNEL_EXTERNAL,ADC_PROC_TRIGGER,0);
	OpenGPIOPortB();
	GPIOPortBWrite(0);
	OpenLEDPort();
	OffLED(LED_WHITE);
	OpenTimer0(1000*ui32StepTimemS);
	IntMasterEnable();
	StartTimer0();


	while( 1 )
	{
		if(ui32Timer0IntFlag == 1)
		{
			ui32Timer0IntFlag =0;

			ui32ADCInputInmV = ReadADCSampleInmV();
			ui32ADC1InputInmV = ReadADC1SampleInmV();

			if(ui32ADC1InputInmV > ui32ADCInputInmV)
			{
				if(i32CurrentPosition < 24)
				{
					i32CurrentPosition++;
					i32StepIndex++;
					if(i32StepIndex >7) i32StepIndex = 0;
					GPIOPortBWrite(ui8_Stepval[i32StepIndex]);
				}
			}
			else if(ui32ADCInputInmV > ui32ADC1InputInmV)
			{
				if(i32CurrentPosition > -24)
				{
					i32CurrentPosition--;
					i32StepIndex--;
					if(i32StepIndex < 0)i32StepIndex = 7;
					GPIOPortBWrite(ui8_Stepval[i32StepIndex]);
				}
			}

		}

	}

	return 0;
}

