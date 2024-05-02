//*****************************************************************************************
// Author  		: Tessin K Jose
// File Name 	: KeyBoard.C
// Description  : functions for using the switches on launchpad and additional switches
// Dependency 	: CCS , Driverlib
// Revision 	: Release date    	Simple Remark
// 1.0			: 1-11-2016 		Initial version
//*****************************************************************************************
#include "MatrixKBD.h"

int KBDReadingFlag,KeyCount,MessageLength;
char * KeyAddress;
const char KeyTable[4][4] = {	{'0','1','2','3'},
								{'4','5','6','7'},
								{'8','9','A','B'},
								{'C','D','E','F'}};

void KBDIntHandler(void);

void OpenKBDPort()
{
	OpenGPIOPortB();
	GPIOPortBSetPin(PIN_4|PIN_5|PIN_6|PIN_7);

	SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOC);
	GPIOPinTypeGPIOInput(GPIO_PORTA_BASE,GPIO_PIN_4 |GPIO_PIN_5|GPIO_PIN_6 |GPIO_PIN_7);
	GPIOPadConfigSet(GPIO_PORTA_BASE,GPIO_PIN_4 |GPIO_PIN_5|GPIO_PIN_6 |GPIO_PIN_7,GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
	GPIOIntTypeSet(GPIO_PORTA_BASE,GPIO_PIN_4 |GPIO_PIN_5|GPIO_PIN_6 |GPIO_PIN_7, GPIO_RISING_EDGE);
	GPIOIntRegister(GPIO_PORTA_BASE, KBDIntHandler);

	KBDReadingFlag = 0;
	KeyCount = 0;
}
int ReadFromKeyBoard(char* MessagePoiter, int Length)
{
	if (KBDReadingFlag == 0)
	{
		KBDReadingFlag = 1;
		KeyAddress = MessagePoiter;
		MessageLength = Length;
		KeyCount = 0;		
		GPIOPortBSetPin(PIN_4|PIN_5|PIN_6|PIN_7);
		GPIOIntClear(GPIO_PORTA_BASE,GPIO_PIN_4 |GPIO_PIN_5|GPIO_PIN_6 |GPIO_PIN_7);
		GPIOIntEnable(GPIO_PORTA_BASE,GPIO_PIN_4 |GPIO_PIN_5|GPIO_PIN_6 |GPIO_PIN_7);
	}
	if (KeyCount == Length ) 
		KBDReadingFlag = 0;
	return KeyCount;
}


void KBDIntHandler(void)
{
	int ColumnNumber =0;
	int RowNumber = 0;
	int Column,Row;
	volatile unsigned int Count;

	GPIOIntDisable(GPIO_PORTA_BASE,GPIO_PIN_4 |GPIO_PIN_5|GPIO_PIN_6 |GPIO_PIN_7);
	GPIOIntClear(GPIO_PORTA_BASE,GPIO_PIN_4 |GPIO_PIN_5|GPIO_PIN_6 |GPIO_PIN_7);

	//Key de-bounce delay//
	Count =100000;
	while(Count--);
	GPIOPortBResetPin(PIN_4|PIN_5|PIN_6|PIN_7);
	Count =10000;
	while(Count--);
	
	for(ColumnNumber=0;ColumnNumber<4;ColumnNumber++)
	{
		GPIOPortBResetPin(PIN_4|PIN_5|PIN_6|PIN_7);
		Count =1000;
		while(Count--);
		Column = (0x10)<<ColumnNumber;
		GPIOPortBSetPin(Column);
		Count =1000;
		while(Count--);
		Row = GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_4 |GPIO_PIN_5|GPIO_PIN_6 |GPIO_PIN_7);
		if(Row)
		{
			for(RowNumber=0;RowNumber<4;RowNumber++)
			{
				if(Row & ((0x10)<<RowNumber))
				{
					*(KeyAddress++) = KeyTable[ColumnNumber][RowNumber];
					KeyCount++;
					break;
				}
			}
		break;
		}
	}
	Count =100000;
	while(Count--);
	GPIOIntClear(GPIO_PORTA_BASE,GPIO_PIN_4 |GPIO_PIN_5|GPIO_PIN_6 |GPIO_PIN_7);
	if(KeyCount < MessageLength )
    {
		GPIOPortBSetPin(PIN_4|PIN_5|PIN_6|PIN_7);
		GPIOIntEnable(GPIO_PORTA_BASE,GPIO_PIN_4 |GPIO_PIN_5|GPIO_PIN_6 |GPIO_PIN_7);            
	}
}
