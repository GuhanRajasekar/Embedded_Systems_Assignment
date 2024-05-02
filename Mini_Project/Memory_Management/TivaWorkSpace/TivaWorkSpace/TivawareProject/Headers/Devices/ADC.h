//*****************************************************************************************
// Author  		: Tessin K Jose
// File Name 	: ADC.h 
// Description  : prtototypes for using the on chip 12bit ADC 
// Dependency 	: CCS , Driverlib
// Revision 	: Release date    	Simple Remark    	
//	1.0			: 1-11-2016 		Initial version
//*****************************************************************************************

#ifndef ADC_H
#define ADC_H

#include "TIResources.h"

#define ADC_PROC_TRIGGER		0
#define ADC_TIMER_TRIGGER		1
#define ADC_CHANNEL_EXTERNAL		0
#define ADC_CHANNEL_INTERNAL_TEMP_SENS	1

#define ADC_ERR	0xFFFF

// Initialise the ADC //
// channel = internal temp / external
// trigger = processor / timer (timer not implemented in this version)
// sample rate = not relevant now give 0
void OpenADCPort(int32_t Channel, int32_t TriggerMode,int32_t SampleRate);

//reads adc sample in binary form
uint32_t ReadADCSample(void);

//reads and converts to mv
uint32_t ReadADCSampleInmV(void);

//reads temp in degree celcius if internal temp selected
uint32_t ReadADCInternalTempSens(void);

//close adc port
void CloseADCPort(void);

// Initialise the ADC //
// channel = internal temp / external
// trigger = processor / timer (timer not implemented in this version)
// sample rate = not relevant now give 0
void OpenADC1Port(int32_t Channel, int32_t TriggerMode,int32_t SampleRate);

//reads adc sample in binary form
uint32_t ReadADC1Sample(void);

//reads and converts to mv
uint32_t ReadADC1SampleInmV(void);

//reads temp in degree celcius if internal temp selected
uint32_t ReadADC1InternalTempSens(void);

//close adc port
void CloseADC1Port(void);

#endif // ADC_H








