//*****************************************************************************************
// Author  		: Tessin K Jose
// File Name 	: SysTimer.c 
// Description  : functions for using the on chip system Timer 
// Dependency 	: CCS , Driverlib
// Revision 	: Release date    	Simple Remark    	
//	1.0			: 1-11-2016 		Initial version
//*****************************************************************************************

#include "SysTimer.h"

uint32_t ui32SysTickCount =0;
uint32_t ui32SysTickIntStat =0;

int32_t OpenSysTimer(uint32_t TimePeriodInUs)
{
	uint32_t ui32Period;
	int32_t i32ReturnValue;
	

	ui32Period = (SysCtlClockGet()/1000000)*(TimePeriodInUs);
	if(ui32Period < 16777215)
	{
		SysTickPeriodSet(ui32Period);
		// Enable the SysTick Interrupt.
		SysTickIntEnable();
		i32ReturnValue = SYSTIMER_SUCCESS;
	}
	else i32ReturnValue = SYSTIMER_ERR;

	return i32ReturnValue;
}

void StopSysTimer(void)
{
	SysTickDisable();
}

void StartSysTimer(void)
{
	SysTickEnable();
}

void CloseSysTimer(void)
{
	SysTickDisable();
	SysTickIntDisable();
}

void SysTickIntHandler(void)
{	
	ui32SysTickCount++;
	ui32SysTickIntStat =1;
}


