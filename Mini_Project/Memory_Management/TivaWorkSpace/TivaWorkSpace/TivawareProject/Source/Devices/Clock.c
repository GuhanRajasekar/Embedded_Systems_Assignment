//*****************************************************************************************
// Author  	: Tessin K Jose
// File Name 	: Clock.c 
// Description  : function definition for setting the clock frequency
// Dependency 	: CCS , Driverlib
// Revision 	: Release date    	Simple Remark    	
//	1.0	: 1-11-2016 		Initial version
//*****************************************************************************************

#include "Clock.h"

void SetSysClockFrequency(uint32_t ui32ClkDivider)
{
	SysCtlClockSet(ui32ClkDivider| SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
}

