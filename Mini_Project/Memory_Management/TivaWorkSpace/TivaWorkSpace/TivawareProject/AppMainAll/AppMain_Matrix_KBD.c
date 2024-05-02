//*****************************************************************************************
// Author  	: Tessin K Jose
// File Name 	: AppMain.c
// Description  : Matrix Keyboard
// Dependency 	: CCS , Driverlib
// Revision 	: Release date    	Simple Remark    	
//	1.0	: 1-11-2016 		Initial version
//*****************************************************************************************
#include "App.h"

char KBDInData[16];
volatile int Count;

int main(void)
{
	int i;

	SetSysClockFrequency(SYSCTL_SYSDIV_5); //200/5 = 40MHz
	IntMasterDisable();
	InitConsole();
	OpenKBDPort();
	IntMasterEnable();

	UARTprintf(" Enter the Keys on Matrix keyboard \n");



	while( 1 )
	{
			Count =100000;
			while(Count--);
		if( ReadFromKeyBoard(KBDInData,1) ==1)
			UARTprintf("\n %c",KBDInData[0]);
	}

	return 0;
}
