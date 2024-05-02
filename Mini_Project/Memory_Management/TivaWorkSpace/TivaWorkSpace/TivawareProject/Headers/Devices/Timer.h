//*****************************************************************************************
// Author  		: Tessin K Jose
// File Name 	: SysTimer.h 
// Description  : prtototypes for using the on chip Timer Modules 
// Dependency 	: CCS , Driverlib
// Revision 	: Release date    	Simple Remark    	
//	1.0			: 1-11-2016 		Initial version
//*****************************************************************************************

#ifndef TIMER_H
#define TIMER_H

#include "TIResources.h"

// initialize the timer for selected period mode
void OpenTimer0(uint32_t TimePeriodInUs);
void StartTimer0(void);
void StopTimer0(void);
void RestartTimer0(void);
void CloseTimer0(void);
void Timer0IntHandler(void);

void OpenTimer1(uint32_t TimePeriodInUs);
void StartTimer1(void);
void StopTimer1(void);
void RestartTimer1(void);
void CloseTimer1(void);
void Timer1IntHandler(void);

#endif // TIMER_H



