//*****************************************************************************************
// Author  	: Tessin K Jose
// File Name 	: Clock.h 
// Description  : prtototypes for setting the clock frequency
// Dependency 	: CCS , Driverlib
// Revision 	: Release date    	Simple Remark    	
//	1.0	: 1-11-2016 		Initial version
//*****************************************************************************************

#ifndef CLOCK_H
#define CLOCK_H

#include "TIResources.h"

//The PPL clck frequency after default diividion is 200MHz
//SYSCTL_SYSDIV_3  to SYSCTL_SYSDIV_64  
//SYSCTL_SYSDIV_2_5 to SYSCTL_SYSDIV_63_5
void SetSysClockFrequency(uint32_t ui32ClkDivider);


 
#endif // CLOCK_H




