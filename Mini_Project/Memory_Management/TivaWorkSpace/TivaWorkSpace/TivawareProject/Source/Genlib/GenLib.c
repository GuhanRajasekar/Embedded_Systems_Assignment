//*****************************************************************************************
// Author  	: Tessin K Jose
// File Name 	: GenLib.c
// Description  : Hardware independent utility lib
// Dependency 	: None
// Revision 	: Release date    	Simple Remark    	
//	1.0	: 1-11-2016 		Initial version
//*****************************************************************************************

#include "GenLib.h"

int IsValidHex(unsigned int hex);
int Hex2Asc(int hexin);
int Asc2Hex(int cin);

uint32_t ui32ASCItoInteger( char *pucText)
{
	uint32_t ui32Accum;
	unsigned char ucCurrentChar;
	uint32_t  ui32Digit;
	uint8_t ui8Index;

	ui8Index = 0;
	ui32Accum = 0;
	ucCurrentChar = pucText[ui8Index];
	ui8Index++;

	while((ucCurrentChar!=0)&&(ucCurrentChar>='0')&&(ucCurrentChar<='9'))
	{
		ui32Digit = ucCurrentChar - '0';
		ui32Accum = ui32Accum * 10;
		ui32Accum = ui32Accum + ui32Digit;
		ucCurrentChar = pucText[ui8Index];
		ui8Index++;
	}
	return(ui32Accum);
}

unsigned int AsciitoHex(char *arg)
{	unsigned int value, temp, status;

	value = 0;
	temp = 0;

	 while(*arg)
	 {
		temp = Asc2Hex(*arg);

		status = IsValidHex(*arg);
		if(status == 0)
		break;

		value = value << 4;
		value = value | temp;
		arg++;
	 }
    return value;
}

int IsValidHex(unsigned int hex)
{
	if( ((hex >= 0x30) && (hex <= 0x39)) ||
		((hex >= 0x41) && (hex <= 0x46)) )
		return 1;

	else
	    return 0;
}


int Hex2Asc(int hexin)
{
	hexin += 0x30;

	if(hexin > 0x39)
 		hexin += 7;

	return hexin;
}


int Asc2Hex(int cin)
{
 	cin		=	cin & 0x7f;
 	cin 	= 	cin - 0x30;

 	if(cin > 9)
 		cin = cin - 7;

 	return cin;
}

