//*****************************************************************************************
// Author  		: Tessin K Jose
// File Name 	: KeyBoard.h 
// Description  : prtototypes for using the switches on launchpad and additional switches
// Dependency 	: CCS , Driverlib
// Revision 	: Release date    	Simple Remark    	
//	1.0			: 1-11-2016 		Initial version
//*****************************************************************************************

#ifndef KEY_BOARD_H
#define KEY_BOARD_H

#include "TIResources.h"

#define KEY1 	0
#define KEY2 	1
#define ANY_KEY 0xFF

#define SW1	KEY2   // On the board it is written like this
#define SW2 KEY1

#define KEY_OFF 0
#define KEY_ON	1

#define INT_ENABLE 	1
#define INT_DISABLE 	0

// enable the two switches preset on the board
// choose to enable or disable interrupt
void OpenKeyBoardPort(uint8_t ui8IntEnable);

//check whether the switch is currently on /off polling mode
uint32_t ReadKeyValue(uint8_t ui8KeyNo);

// wait till the key is pressed
void WaitKeyPress(uint8_t ui8KeyNo);

//check whether the key press interrupt is registered in software
uint32_t CheckKeyPress(uint8_t ui8KeyNo);

// clear the interrupt registered in software
void ClearKeyPress(uint8_t ui8KeyNo);

void CloseKeyBoardPort(void);

void KeyPressIntHandler(void);

#endif // KEY_BOARD_H





