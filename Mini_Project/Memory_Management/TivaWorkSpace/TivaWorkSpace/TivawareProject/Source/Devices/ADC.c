//*****************************************************************************************
// Author  		: Tessin K Jose
// File Name 	: ADC.c 
// Description  : functions for using the on chip 12bit ADC 
// Dependency 	: CCS , Driverlib
// Revision 	: Release date    	Simple Remark    	
//	1.0			: 1-11-2016 		Initial version
//*****************************************************************************************

#include "ADC.h"

int32_t i32ADCTriggerMode=ADC_PROC_TRIGGER;
int32_t i32ADCChannel =ADC_CHANNEL_EXTERNAL;

void OpenADCPort(int32_t Channel, int32_t TriggerMode,int32_t SampleRate)
{
	// The ADC0 peripheral must be enabled for use.
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	
	if(Channel == ADC_CHANNEL_EXTERNAL)
	{
		i32ADCChannel =ADC_CHANNEL_EXTERNAL;

	   	//  ADC0 is used with AIN0 on port E7.
	   	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

	   	// Select the analog ADC function for these pins.
	   	GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);

		if(TriggerMode == ADC_PROC_TRIGGER)
		{
			i32ADCTriggerMode=ADC_PROC_TRIGGER;

	   		// Enable sample sequence 3 with a processor signal trigger.  Sequence 3
	   		// will do a single sample when the processor sends a signal to start the
	   		// conversion.  Each ADC module has 4 programmable sequences, sequence 0
	   		// to sequence 3.  This example is arbitrarily using sequence 3.
	  	   	ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);

		   	// Configure step 0 on sequence 3.  Sample channel 0 (ADC_CTL_CH0) in
	   		// single-ended mode (default) and configure the interrupt flag
	   		// (ADC_CTL_IE) to be set when the sample is done.  Tell the ADC logic
	   		// that this is the last conversion on sequence 3 (ADC_CTL_END).
	   		ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);

	  	   	ADCSequenceEnable(ADC0_BASE, 3);

	   		// Clear the interrupt status flag. 
	   		ADCIntClear(ADC0_BASE, 3);
		}			
	}
	else if(Channel == ADC_CHANNEL_INTERNAL_TEMP_SENS)
	{
		i32ADCChannel = ADC_CHANNEL_INTERNAL_TEMP_SENS; 
		i32ADCTriggerMode=ADC_PROC_TRIGGER;

 	    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
		ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_TS | ADC_CTL_IE | ADC_CTL_END);
		ADCSequenceEnable(ADC0_BASE, 3);
		ADCIntClear(ADC0_BASE, 3);
	}
}

uint32_t ReadADCSample(void)
{
	uint32_t pui32ADC0Value[1];

	if(i32ADCTriggerMode == ADC_PROC_TRIGGER)
	{
		ADCIntClear(ADC0_BASE, 3);
		
		// Trigger the ADC conversion.
		ADCProcessorTrigger(ADC0_BASE, 3);

		 // Wait for conversion to be completed.
		 while(!ADCIntStatus(ADC0_BASE, 3, false));			 
	}
 	// Read ADC Value.
 	ADCSequenceDataGet(ADC0_BASE, 3, pui32ADC0Value);

	return pui32ADC0Value[0];
}

uint32_t ReadADCSampleInmV(void)
{
	uint32_t pui32ADC0Value[1];
	uint32_t ui32TempSenInmV;

	if(i32ADCTriggerMode == ADC_PROC_TRIGGER)
	{
		ADCIntClear(ADC0_BASE, 3);
		
		// Trigger the ADC conversion.
		ADCProcessorTrigger(ADC0_BASE, 3);

		 // Wait for conversion to be completed.
		 while(!ADCIntStatus(ADC0_BASE, 3, false));			 
	}
 	// Read ADC Value.
 	ADCSequenceDataGet(ADC0_BASE, 3, pui32ADC0Value);
	ui32TempSenInmV = (pui32ADC0Value[0] * 3300)/4096;
	return ui32TempSenInmV;
}

uint32_t ReadADCInternalTempSens(void)
{
	uint32_t pui32ADC0Value[1];
	uint32_t ui32TempValueC;

	if((i32ADCChannel == ADC_CHANNEL_INTERNAL_TEMP_SENS) &&(i32ADCTriggerMode == ADC_PROC_TRIGGER))
	{
		ADCIntClear(ADC0_BASE, 3);
		
		// Trigger the ADC conversion.
		ADCProcessorTrigger(ADC0_BASE, 3);

		// Wait for conversion to be completed.
		while(!ADCIntStatus(ADC0_BASE, 3, false));

		ADCSequenceDataGet(ADC0_BASE, 3, pui32ADC0Value);
		ui32TempValueC = (1475 - ((2475 * pui32ADC0Value[0])) / 4096)/10;			 
	}
 	else ui32TempValueC = ADC_ERR;

	return ui32TempValueC;
}

void CloseADCPort(void)
{
	ADCIntClear(ADC0_BASE, 3);
	SysCtlPeripheralDisable(SYSCTL_PERIPH_ADC0);
}

