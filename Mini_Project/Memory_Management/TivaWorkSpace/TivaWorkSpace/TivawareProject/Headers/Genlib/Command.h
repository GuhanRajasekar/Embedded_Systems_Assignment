//*****************************************************************************************
// Author  	: Tessin K Jose
// File Name 	: Command.h
// Description  : prtototypes for command processing lib
// Dependency 	: None
// Revision 	: Release date    	Simple Remark    	
//	1.0	: 1-11-2016 		Initial version
//*****************************************************************************************

#ifndef COMMAND_H
#define COMMAND_H

#include "GenLib.h"
#include "MyString.h"
#include "Devices.h"
#include "MemOps.h"
#include "HeapMem.h"

void GetCommand(char *pcBuf, uint32_t ui32Len);
void ParseCommand(char *pcCommand);
int SearchCommand(char *pcCommand);
int ExecuteCommand(int iCommandNo);
void ProcessUARTCommand(void);

#endif // COMMAND_H








