//*****************************************************************************************
// Author  		: Tessin K Jose
// File Name 	: DigitalInputPort.h 
// Description  : prtototypes for using the GPIO PORT C pins(4,5,6,7)as input port
// Dependency 	: CCS , Driverlib
// Revision 	: Release date    	Simple Remark    	
//	1.0			: 1-11-2016 		Initial version
//*****************************************************************************************

#ifndef DIGITAL_IN_PORT_H
#define DIGITAL_IN_PORT_H

#include "TIResources.h"

#define IN_4 		GPIO_PIN_4
#define IN_5 		GPIO_PIN_5
#define IN_6 		GPIO_PIN_6
#define IN_7 		GPIO_PIN_7


#define PIN_LOW 	0
#define PIN_HIGH	1


// configure portc pins 4-7 as input ports
void OpenGPIOCInputPort(void);

//Enable interrupt on selected pin of portc
void EnableGPIOCPinInt(uint8_t ui8PinNo);

uint32_t ReadPinValue(uint8_t ui8PinNo);

//wait till the pin interrupt is registered automatically clears
void WaitPinInt(uint8_t ui8PinNo);

// check whether the pin interrupt got registered in software
uint32_t CheckPinInt(uint8_t ui8PinNo);

//clear pin interrupt flag in software
void ClearPinInt(uint8_t ui8PinNo);

void CloseGPIOCInputPort(void);

void GPIOCPinIntHandler(void);

#endif // DIGITAL_IN_PORT_H







