//*****************************************************************************************
// Author  		: Tessin K Jose
// File Name 	: DigitalOutputPort.h 
// Description  : prtototypes for using the GPIO PORT B as output port
// Dependency 	: CCS , Driverlib
// Revision 	: Release date    	Simple Remark    	
//	1.0			: 1-11-2016 		Initial version
//*****************************************************************************************

#ifndef DIGITAL_OUT_PORT_H
#define DIGITAL_OUT_PORT_H

#include "TIResources.h"


#define PIN_0 		GPIO_PIN_0
#define PIN_1 		GPIO_PIN_1
#define PIN_2 		GPIO_PIN_2
#define PIN_3 		GPIO_PIN_3
#define PIN_4 		GPIO_PIN_4
#define PIN_5 		GPIO_PIN_5
#define PIN_6 		GPIO_PIN_6
#define PIN_7 		GPIO_PIN_7

#define GPIO_PIN_ALL 	0xFF

// port B will be configured as out put port
void OpenGPIOPortB(void);

// write 8 bit data to 8 pins
void GPIOPortBWrite(uint8_t ui8Val);

// set selected pin
void GPIOPortBSetPin(uint8_t ui8PinNo);

// reset selected pin
void GPIOPortBResetPin(uint8_t ui8PinNo);

// toggle selected pin
void GPIOPortBTogglePin(uint8_t ui8PinNo);

void CloseGPIOPortB(void);

#endif // DIGITAL_OUT_PORT_H







