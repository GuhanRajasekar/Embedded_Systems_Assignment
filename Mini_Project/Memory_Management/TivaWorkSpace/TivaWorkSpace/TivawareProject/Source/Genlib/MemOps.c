//*****************************************************************************************
// Author  	: Tessin K Jose
// File Name 	: MemOps.c
// Description  : memory manipulation lib
// Dependency 	: None
// Revision 	: Release date    	Simple Remark    	
//	1.0	: 1-11-2016 		Initial version
//*****************************************************************************************

#include "MemOps.h"

int GetHexData(char *pcInput);
int ValidateMemory(int Addr ,int NumBytes);

extern char _stack;
char *pUsedMem;

unsigned int Indata[16];

int MemRead(uint32_t MemoryAddress, uint32_t NumBytes , uint32_t Format)
{
	unsigned int  i, j, *pUInt;
	int *pInt;
	unsigned short *puShort;
	unsigned char *pChar;

	if(Format == ARG_WORD)
	{
		UARTprintf("\nAddress       0        4        8        C \n");

		for(i=0; i<= NumBytes; i +=16)
		{
			UARTprintf("%08x:  ", MemoryAddress + i);

			for(j = 0; j < 16; j +=4)
			{
				pUInt = (unsigned int *) (MemoryAddress +i +j);
		 		UARTprintf("%08x ",*pUInt);
			}

			UARTprintf("\n");
		}
	}
	else if(Format == ARG_CHAR)
	{
		UARTprintf("\nAddress  \n");

		for(i=0; i<= NumBytes; i +=16)
		{
			UARTprintf("%08x:  ", MemoryAddress + i);

			for(j = 0; j < 16; j++)
			{
				pChar = (unsigned char *) (MemoryAddress +i +j);
				UARTprintf("%c ",*pChar);
			}

			UARTprintf("\n");
		}
	}
	else if(Format == ARG_INT)
	{
	UARTprintf("\nAddress        0          4          8          C \n");

		for(i=0; i<= NumBytes; i +=16)
		{
			UARTprintf("%08x:  ", MemoryAddress + i);

			for(j = 0; j < 16; j+= 4)
			{
				pInt = (int *) (MemoryAddress +i +j);
				UARTprintf("%010d ",*pInt);
			}

			UARTprintf("\n");
		}
	}
	else if(Format == ARG_BYTE)
	{
	UARTprintf("\nAddress \n");

		for(i=0; i<= NumBytes; i +=16)
		{
			UARTprintf("%08x:  ", MemoryAddress + i);

			for(j = 0; j < 16; j++)
			{
				pChar = (unsigned char *) (MemoryAddress +i +j);
				UARTprintf("%02x ",*pChar);
			}

			UARTprintf("\n");
		}
	}
	else if(Format == ARG_HWORD)
	{
	UARTprintf("\nAddress \n");

		for(i=0; i<= NumBytes; i +=16)
		{
			UARTprintf("%08x:  ", MemoryAddress + i);

			for(j = 0; j < 16; j+= 2)
			{
				puShort = (unsigned short *) (MemoryAddress +i +j);
				UARTprintf("%04x ",*puShort);
			}

			UARTprintf("\n");
		}
	}
 return M_VALID;
}


int MemWrite(uint32_t DestMemoryAddress, uint32_t SrcMemoryAddress, uint32_t NumBytes ,uint32_t Mode)
{
	int  i,Count,ReturnVal;
	unsigned int *pUIntDest;
	unsigned char *pCharSrc,*pCharDest;

	if(Mode == ARG_ADDR)
	{
		pCharDest = (unsigned char *)DestMemoryAddress;
		pCharSrc = (unsigned char *)SrcMemoryAddress;
		if (M_VALID == ValidateMemory(DestMemoryAddress ,NumBytes))
		{
			for(i=0; i < NumBytes; i++) *(pCharDest+i) = *(pCharSrc+i);
			ReturnVal = M_VALID;
		}
		else ReturnVal = M_ERROR;
	}
	else if(Mode == ARG_WORD)
	{
		pUIntDest = (unsigned int *)DestMemoryAddress;
		Count= GetHexData((char *)SrcMemoryAddress);
		if(Count > NumBytes)Count = NumBytes;
		if (M_VALID == ValidateMemory(DestMemoryAddress ,Count))
		{
			for(i=0; i < Count; i++) *(pUIntDest+i) = Indata[i];
			ReturnVal = M_VALID;
		}
		else ReturnVal = M_ERROR;
	}
	else if(Mode == ARG_STRING)
	{
		pCharDest = (unsigned char *)DestMemoryAddress;
		pCharSrc = (unsigned char *)SrcMemoryAddress;
		Count = mystrlen((unsigned char *)SrcMemoryAddress);
		if(Count > NumBytes)Count = NumBytes;
		if (M_VALID == ValidateMemory(DestMemoryAddress ,Count))
		{
			for(i=0; i < Count; i++) *(pCharDest+i) = *(pCharSrc+i);
			ReturnVal = M_VALID;
		}
		else ReturnVal = M_ERROR;
	}
	return ReturnVal;
}


int GetHexData(char *pcInput)
{
	int l,n,i;
	char Data[16];

	mystrdelws (pcInput,LEADING_WS);
	l=mystrlen(pcInput);
	i=0;
	while(l)
	{
		n=0;
		while((pcInput[n] != 0x20)&&(pcInput[n] != 0x9)&&(pcInput[n] != '\0'))n++;
		mystrncpy(Data,pcInput,n);
		Indata[i++] = AsciitoHex(Data);
		mystrcpy(pcInput,&pcInput[n+1]);
		mystrdelws (pcInput,LEADING_WS);
		l=mystrlen(pcInput);
	}
	return i;
}

int ValidateMemory(int Addr ,int NumBytes)
{
	char UserOp[6];

	pUsedMem = &_stack;
	if((Addr >= 0x20000000)&& (Addr <= (int)pUsedMem))
	{
		UARTprintf("This is RAM area used by program do you want to continue y/n \n");
		UARTgets(UserOp,6);
		if((UserOp[0] == 'Y')||(UserOp[0] == 'y'))
			return M_VALID;
		else
			return M_ERROR;
	}
	if( ((Addr >= 0x00000000)&& (Addr <= 0x00040000))||
		((Addr >= 0xE0000000)&& (Addr <= 0xFFFFFFFF))     )
		return M_ERROR;
	else
		return M_VALID;
}
