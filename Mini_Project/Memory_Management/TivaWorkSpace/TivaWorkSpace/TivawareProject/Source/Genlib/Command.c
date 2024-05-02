//*****************************************************************************************
// Author  	: Tessin K Jose
// File Name 	: Command.c
// Description  : Command processing lib
// Dependency 	: None
// Revision 	: Release date    	Simple Remark    	
//	1.0	: 1-11-2016 		Initial version
//*****************************************************************************************

#include "Command.h"

#define NUM_COMMANDS 16
#define NUM_ARGS 4
#define LEN_ARGS 64
#define LEN_COMMANDS 16


#define NUM_SYMBOLS 4
#define LEN_SYMBOLS 16

extern char _str1, _str2, _str3, _str4;

extern BLOCK_HEADER *psFirstUsedBlock,*psFirstFreeBlock;
extern int BlockHeaderSize;

char cCommandString[128];
char cCommandStucture[NUM_ARGS+1][LEN_ARGS];
int iNumArguments;
char CommandTable[NUM_COMMANDS][LEN_COMMANDS]={
		"mystrlen",
		"mystrcpy",
		"mystrncpy",
		"mystrcmp",
		"mystrncmp",
		"mystrcat",
		"mystrcase",
		"mystrdelws",
		"peekmem",
		"pokemem",
		"setpwmdc",
		"meminit",
		"memalloc",
		"memfree",
		"memdump",
		"memexit",
};

char SymbolTable[NUM_SYMBOLS][LEN_SYMBOLS]={
		"_str1",
		"_str2",
		"_str3",
		"_str4",
};

char* AddrTable[NUM_SYMBOLS]={
		&_str1,
		&_str2,
		&_str3,
		&_str4,
};


int iCommandNo;

void Runmystrlen(void);
void Runmystrcpy(void);
void Runmystrncpy(void);
void Runmystrcmp(void);
void Runmystrncmp(void);
void Runmystrcat(void);
void Runmystrcase(void);
void Runmystrdelws(void);
void Runpeekmem(void);
void Runpokemem(void);
void Runsetpwmdc(void);
void Runmeminit(void);
void Runmemalloc(void);
void Runmemfree(void);
void Runmemdump(void);
void Runmemexit(void);

unsigned int ResolveAddr(char *pArg);
int GetArgType(char Arg);

void ProcessUARTCommand(void)
{
	if(UARTPeek('\r')!= -1)
	{
		GetCommand(cCommandString, 128);
		ParseCommand(cCommandString);
		iCommandNo = SearchCommand(&cCommandStucture[0][0]);
		ExecuteCommand(iCommandNo);
		UARTprintf("\nTIVATERM>");
	}
}

void GetCommand(char *pcBuf, uint32_t ui32Len)
{
	UARTgets(pcBuf,ui32Len);
}

void ParseCommand(char *pcCommand)
{
	int l,n,i;

	mystrdelws (pcCommand,LEADING_WS);
	l=mystrlen(pcCommand);
	i=0;
	while((l>0) && (i<(NUM_ARGS+1)))
	{
		n=0;
		if(pcCommand[0] == '"')
		{
			n=1;
			while((pcCommand[n] != '"')&&(pcCommand[n] != '\0'))n++;
			mystrncpy(&cCommandStucture[i++][0],&pcCommand[1],(n-1));
			mystrcpy(pcCommand,&pcCommand[n+1]);
		}
		else
		{
			while((pcCommand[n] != 0x20)&&(pcCommand[n] != 0x9)&&(pcCommand[n] != '\0'))n++;
			mystrncpy(&cCommandStucture[i++][0],pcCommand,n);
			mystrcpy(pcCommand,&pcCommand[n]);
		}
		mystrdelws (pcCommand,LEADING_WS);
		l=mystrlen(pcCommand);
	}
	iNumArguments =i-1;
	for(;i<(NUM_ARGS+1);i++)cCommandStucture[i][0]= '\0';
}

int SearchCommand(char *pcCommand)
{
	int i,ReturnValue;

	ReturnValue =-1;

	for(i=0;i<NUM_COMMANDS;)
	{
		if(STRCMP_SUCCESS == mystrcmp(pcCommand,&CommandTable[i][0], CASE_INSENSITIVE))
		{
			ReturnValue =i;
			break;
		}
		i++;
	}

	return ReturnValue;
}

int ExecuteCommand(int iCommandNo)
{
	switch (iCommandNo)
	{
		case 0:
			Runmystrlen();
			break;
		case 1:
			Runmystrcpy();
			break;
		case 2:
			Runmystrncpy();
			break;
		case 3:
			Runmystrcmp();
			break;
		case 4:
			Runmystrncmp();
			break;
		case 5:
			Runmystrcat();
			break;
		case 6:
			Runmystrcase();
			break;
		case 7:
			Runmystrdelws();
			break;
		case 8:
			Runpeekmem();
			break;
		case 9:
			Runpokemem();
			break;
		case 10:
			Runsetpwmdc();
			break;
		case 11:
			Runmeminit();
			break;
		case 12:
			Runmemalloc();
			break;
		case 13:
			Runmemfree();
			break;
		case 14:
			Runmemdump();
			break;
		case 15:
			Runmemexit();
			break;

		default :
			UARTprintf("Unknown Command");
			break;
	}
	return 0;
}

void Runmystrlen(void)
{
	int iCount;
	iCount = mystrlen(&cCommandStucture[1][0]);
	UARTprintf("Length of the string = %d ",iCount);
}

void Runmystrcpy(void)
{
	char* pS;
	pS = mystrcpy(&cCommandStucture[1][0],&cCommandStucture[2][0]);
	UARTprintf("Copied string is '%s'",pS);
}

void Runmystrncpy(void)
{
	char* pS;
	pS = mystrncpy(&cCommandStucture[1][0],&cCommandStucture[2][0],ui32ASCItoInteger(&cCommandStucture[3][0]));
	UARTprintf("Copied string is '%s'",pS);
}

void Runmystrcmp(void)
{
	int iFlag;
	iFlag = mystrcmp(&cCommandStucture[1][0],&cCommandStucture[2][0],ui32ASCItoInteger(&cCommandStucture[3][0]));
	if(iFlag ==STRCMP_SUCCESS) UARTprintf("Strings are Equal");
	else UARTprintf("Strings are Not equal");
}

void Runmystrncmp(void)
{
	int iFlag;
	iFlag = mystrncmp(&cCommandStucture[1][0],&cCommandStucture[2][0],ui32ASCItoInteger(&cCommandStucture[3][0]));
	if(iFlag ==STRCMP_SUCCESS) UARTprintf("Strings to length n are Equal");
	else UARTprintf("Strings to length n are Not equal");
}

void Runmystrcat(void)
{
	char* pS;
	pS = mystrcat(&cCommandStucture[1][0],&cCommandStucture[2][0]);
	UARTprintf("Resultant string is '%s'",pS);
}

void Runmystrcase(void)
{
	mystrcase(&cCommandStucture[1][0],ui32ASCItoInteger(&cCommandStucture[2][0]));
	UARTprintf("Resultant string is '%s'",&cCommandStucture[1][0]);
}

void Runmystrdelws(void)
{
	mystrdelws(&cCommandStucture[1][0],ui32ASCItoInteger(&cCommandStucture[2][0]));
	UARTprintf("Resultant string is '%s'",&cCommandStucture[1][0]);
}

unsigned int ResolveAddr(char *pArg)
{
	int i,ReturnValue;

	ReturnValue = AsciitoHex(pArg);

	for(i=0;i<NUM_SYMBOLS;)
	{
		if(STRCMP_SUCCESS == mystrcmp(pArg,&SymbolTable[i][0], CASE_INSENSITIVE))
		{
			ReturnValue = (unsigned int)AddrTable[i];
			break;
		}
		i++;
	}

	return ReturnValue;
}

int GetArgType(char Arg)
{
	int ReturnVal;

	switch (Arg)
	{
		case 'a':
			ReturnVal = ARG_ADDR;
			break;
		case 'w':
			ReturnVal = ARG_WORD;
			break;
		case 'h':
			ReturnVal = ARG_HWORD;
			break;
		case 'b':
			ReturnVal = ARG_BYTE;
			break;
		case 'i':
			ReturnVal = ARG_INT;
			break;
		case 'u':
			ReturnVal = ARG_UINT;
			break;
		case 'c':
			ReturnVal = ARG_CHAR;
			break;
		case 's':
			ReturnVal = ARG_STRING;
			break;
		case 'f':
			ReturnVal = ARG_FILE;
			break;
		case 'p':
			ReturnVal = ARG_PATERN;
			break;
		case 'r':
			ReturnVal = ARG_RAND;
			break;
		default :
			ReturnVal = 0;
			break;
	}
	return ReturnVal;
}

// Arg Type  Addr-a, Word - w, HalfWord -h, byte -b, int -i, unit -u, char -c,string -s, file-f, pattern -p, random -r
void Runpeekmem(void)
{
	unsigned int ReadAddr,Format;

	ReadAddr = ResolveAddr(&cCommandStucture[1][0]);
	Format = GetArgType(cCommandStucture[3][0]);

	MemRead( ReadAddr, ui32ASCItoInteger(&cCommandStucture[2][0]) , Format);
}


void Runpokemem(void)
{
	int Mode,Returnval;
	unsigned int DestAddr,SrcAddr;

	DestAddr = ResolveAddr(&cCommandStucture[1][0]);
	Mode = GetArgType(cCommandStucture[2][0]);
	SrcAddr = ResolveAddr(&cCommandStucture[3][0]);

	if(Mode == ARG_ADDR)
	{
		Returnval = MemWrite(DestAddr,SrcAddr,ui32ASCItoInteger(&cCommandStucture[4][0]) ,Mode);
	}
	else
	{
		Returnval = MemWrite(DestAddr,(unsigned int)&cCommandStucture[3][0],ui32ASCItoInteger(&cCommandStucture[4][0]) ,Mode);
	}

	if(Returnval == M_ERROR)UARTprintf("Memory Access Denied");

}

void Runsetpwmdc(void)
{
	unsigned int ui32PWMDutyRatioPercentage;

	ui32PWMDutyRatioPercentage = ui32ASCItoInteger(&cCommandStucture[1][0]);

	if((ui32PWMDutyRatioPercentage <100)&&(ui32PWMDutyRatioPercentage >0))
	{
		ConfigurePWMDutyCycle(ui32PWMDutyRatioPercentage);
		UARTprintf("Duty Cycle set to  %d percentage \n",ui32PWMDutyRatioPercentage);
	}
	else UARTprintf("Invalid Duty Cycle \n");
}

void Runmeminit(void)
{
	int Returnval;
	Returnval = HeapMemInit((void*)0x20005000,ui32ASCItoInteger(&cCommandStucture[1][0]),ui32ASCItoInteger(&cCommandStucture[2][0]));
	if(Returnval == HEAP_ERROR)UARTprintf(" meminit returned error ");
}

void Runmemalloc(void)
{
	BLOCK_HEADER *UsedBlock;
	void *pData;
	pData = MemAlloc(ui32ASCItoInteger(&cCommandStucture[1][0]));
	if(pData == NULL)UARTprintf(" memalloc returned error ");
	else
	{
		UsedBlock = (BLOCK_HEADER *)((unsigned int)pData -BlockHeaderSize);
		UARTprintf("Block ID = %d, Start Addr = %08x , Size = %d\n",UsedBlock->uiBlockID,UsedBlock,UsedBlock->uiSize);
	}
}

void Runmemfree(void)
{
	int Returnval;
	unsigned int BlockId;
	BLOCK_HEADER *CurrentBlock;
	void *pData;

	BlockId = ui32ASCItoInteger(&cCommandStucture[1][0]);
	CurrentBlock = psFirstUsedBlock;
	while (CurrentBlock->uiBlockID != BlockId)
	{
		if(CurrentBlock->pChildBlock == NULL)
		{
			UARTprintf(" Invalid block ID ");
			break;
		}
		else
		{
			CurrentBlock = (BLOCK_HEADER *)(CurrentBlock->pChildBlock);
		}
	}

	if(CurrentBlock->uiBlockID == BlockId)
	{
		pData = (void*)((unsigned int)CurrentBlock + BlockHeaderSize);
		Returnval = MemFree(pData);
		if(Returnval == HEAP_ERROR)UARTprintf(" memfree returned error ");
	}
}

void Runmemdump(void)
{
	HeapMemDump();
}

void Runmemexit(void)
{
	int Returnval;
	Returnval = HeapMemExit();
	if(Returnval == HEAP_ERROR)UARTprintf(" memexit returned error ");
}
