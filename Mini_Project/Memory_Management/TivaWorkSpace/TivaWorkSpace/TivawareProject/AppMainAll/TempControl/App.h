//*****************************************************************************************
// Author  	: Tessin K Jose
// File Name 	: App.h 
// Description  : All header files & MACROS the application requires  
// Dependency 	: None
// Revision 	: Release date    	Simple Remark    	
//	1.0	: 1-11-2016 		Initial version
//*****************************************************************************************

#ifndef APP_H
#define APP_H

#include "Devices.h"
#include "GenLib.h"

// In mV from data sheet//
#define TEMP_SENS_SENSITIVITY 10
#define TEMP_SENS_VALUE_AT_0_C 2730
// Set Application limits//
#define ROOM_TEMP 	27
#define MIN_TEMP 	30
#define MAX_TEMP 	50
#define HYSTERISIS 	(2*TEMP_SENS_SENSITIVITY)

void Task1_GenerateSquareWave(void);
void Task2_TempControl(void);

#endif // APP_H








