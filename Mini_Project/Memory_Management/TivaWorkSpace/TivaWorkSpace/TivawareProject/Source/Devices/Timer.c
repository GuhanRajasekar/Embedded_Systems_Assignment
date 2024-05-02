//*****************************************************************************************
// Author  		: Tessin K Jose
// File Name 	: Timer.c 
// Description  : functions for using the on chip Timer modules
// Dependency 	: CCS , Driverlib
// Revision 	: Release date    	Simple Remark    	
//	1.0			: 1-11-2016 		Initial version
//*****************************************************************************************

#include "Timer.h"

uint32_t ui32Timer0Count =0;
uint32_t ui32Timer0IntFlag =0;

uint32_t ui32Timer1Count =0;
uint32_t ui32Timer1IntFlag =0;

uint32_t ui32Timer0Period=0;
uint32_t ui32Timer1Period=0;

void OpenTimer0(uint32_t TimePeriodInUs)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

	ui32Timer0Period = (SysCtlClockGet()/1000000)*(TimePeriodInUs);
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Timer0Period -1);
	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
//	IntPrioritySet(INT_TIMER0A, 0xE0);
//	TimerEnable(TIMER0_BASE, TIMER_A);
}

void StartTimer0(void)
{
	TimerEnable(TIMER0_BASE, TIMER_A);
}

void StopTimer0(void)
{
	TimerDisable(TIMER0_BASE, TIMER_A);
}

void RestartTimer0(void)
{
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Timer0Period -1);	
	TimerEnable(TIMER0_BASE, TIMER_A);
}

void CloseTimer0(void)
{
	TimerDisable(TIMER0_BASE, TIMER_A);
	TimerIntDisable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntDisable(INT_TIMER0A);
	SysCtlPeripheralDisable(SYSCTL_PERIPH_TIMER0);
	
}

void Timer0IntHandler(void)
{	
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	ui32Timer0Count++;
	ui32Timer0IntFlag =1;
}



void OpenTimer1(uint32_t TimePeriodInUs)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
	TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);

	ui32Timer1Period = (SysCtlClockGet()/1000000)*(TimePeriodInUs);
	TimerLoadSet(TIMER1_BASE, TIMER_A, ui32Timer1Period -1);
	IntEnable(INT_TIMER1A);
	TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
//	IntPrioritySet(INT_TIMER1A, 0x00);
//	TimerEnable(TIMER1_BASE, TIMER_A);
}

void StartTimer1(void)
{
	TimerEnable(TIMER1_BASE, TIMER_A);
}

void StopTimer1(void)
{
	TimerDisable(TIMER1_BASE, TIMER_A);
}

void RestartTimer1(void)
{
	TimerLoadSet(TIMER1_BASE, TIMER_A, ui32Timer1Period -1);	
	TimerEnable(TIMER1_BASE, TIMER_A);
}

void CloseTimer1(void)
{
	TimerDisable(TIMER1_BASE, TIMER_A);
	TimerIntDisable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
	IntDisable(INT_TIMER1A);
	SysCtlPeripheralDisable(SYSCTL_PERIPH_TIMER1);
	
}

void Timer1IntHandler(void)
{	
	TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
	ui32Timer1Count++;
	ui32Timer1IntFlag =1;
}


