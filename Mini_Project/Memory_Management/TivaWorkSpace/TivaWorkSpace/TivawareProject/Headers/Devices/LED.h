//*****************************************************************************************
// Author  		: Tessin K Jose
// File Name 	: LED.h 
// Description  : prtototypes for using the LED on launchpad and additional LED if required
// Dependency 	: CCS , Driverlib
// Revision 	: Release date    	Simple Remark    	
//	1.0			: 1-11-2016 		Initial version
//*****************************************************************************************

#ifndef LED_H
#define LED_H

#include "TIResources.h"

#define LED1		(GPIO_PIN_1)
#define LED2		(GPIO_PIN_2)
#define LED3		(GPIO_PIN_3)

#define LED_RED		(GPIO_PIN_1)
#define LED_BLUE	(GPIO_PIN_2)
#define LED_GREEN	(GPIO_PIN_3)

#define LED_MAGENTA	(GPIO_PIN_1|GPIO_PIN_2)
#define LED_ORANGE	(GPIO_PIN_1|GPIO_PIN_3)
#define LED_CYAN	(GPIO_PIN_2|GPIO_PIN_3)
#define LED_WHITE	(GPIO_PIN_1 |GPIO_PIN_2|GPIO_PIN_3)

// configure gpio pins connected to LED
void OpenLEDPort(void);

void OnLED(uint8_t ui8LEDNo);

void OffLED(uint8_t ui8LEDNo);

void ToggleLED(uint8_t ui8LEDNo);

// changes the colour of LED in cyclic order
void ChangeColourofTriColourLED(void);

void CloseLEDPort(void);

#endif // LED_H




