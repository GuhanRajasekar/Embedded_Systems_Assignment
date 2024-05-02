//*****************************************************************************************
// Author  		: Tessin K Jose
// File Name 	: SysTimer.h 
// Description  : prtototypes for using the on chip system Timer 
// Dependency 	: CCS , Driverlib
// Revision 	: Release date    	Simple Remark    	
//	1.0			: 1-11-2016 		Initial version
//*****************************************************************************************

#ifndef SYSTIMER_H
#define SYSTIMER_H

#include "TIResources.h"


#define SYSTIMER_SUCCESS 		0
#define SYSTIMER_ERR			1

// initialise the sys timer
int32_t OpenSysTimer(uint32_t TimePeriodInUs);

void StopSysTimer(void);

void StartSysTimer(void);

void CloseSysTimer(void);

void SysTickIntHandler(void);

#endif // SYSTIMER_H




