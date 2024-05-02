//*****************************************************************************************
// Author  	: Tessin K Jose
// File Name 	: MemOps.h
// Description  : prtototypes for memory manipulation functions
// Dependency 	: None
// Revision 	: Release date    	Simple Remark    	
//	1.0	: 1-11-2016 		Initial version
//*****************************************************************************************

#ifndef MEMOPS_H
#define MEMOPS_H

#include "GenLib.h"
#include "MyString.h"
#include "Devices.h"

#define M_VALID 	0
#define M_ERROR 	1

#define ARG_ADDR 	1
#define ARG_WORD 	2
#define ARG_HWORD 	3
#define ARG_BYTE 	4
#define ARG_INT 	5
#define ARG_UINT 	6
#define ARG_CHAR 	7
#define ARG_STRING 	8
#define ARG_FILE 	9
#define ARG_PATERN 	10
#define ARG_RAND 	11


int MemRead(uint32_t MemoryAddress, uint32_t NumBytes , uint32_t Format);
int MemWrite(uint32_t DestMemoryAddress, uint32_t SrcMemoryAddress, uint32_t NumBytes ,uint32_t Mode);


#endif // MEMOPS_H








