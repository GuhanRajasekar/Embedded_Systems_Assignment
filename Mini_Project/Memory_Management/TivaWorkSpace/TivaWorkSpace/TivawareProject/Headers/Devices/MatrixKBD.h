//*****************************************************************************************
// Author  		: Tessin K Jose
// File Name 	: KeyBoard.h
// Description  : prtototypes for using the matrix keyboard
// Dependency 	: CCS , Driverlib
// Revision 	: Release date    	Simple Remark
//	1.0			: 1-11-2016 		Initial version
//*****************************************************************************************

#ifndef MATRIX_KBD_H
#define MATRIX_KBD_H

#include "TIResources.h"
#include "Devices.h"

//column are connected to port B4-7 Rows are connected to Port A 4-7
void OpenKBDPort(void);
int ReadFromKeyBoard(char* MessagePoiter, int MessageLength);

#endif // MATRIX_KBD_H
