//*****************************************************************************************
// Author  		: Tessin K Jose
// File Name 	: PWM.h 
// Description  : prtototypes for using the on chip PWM Module 
// Dependency 	: CCS , Driverlib
// Revision 	: Release date    	Simple Remark    	
//	1.0			: 1-11-2016 		Initial version
//*****************************************************************************************

#ifndef PWM_H
#define PWM_H

#include "TIResources.h"


#define PWM_SUCCESS 		0
#define PWM_ERR			1
#define PWM_ERR_SF_HIGH		2
#define PWM_ERR_SF_LOW		3
 
// initialise the pwm module to work with selected switching frequency
int32_t OpenPWMModule(int32_t SwitchingFrequencyHz);

// configure / change duty cycle
uint32_t ConfigurePWMDutyCycle(uint32_t DutyCyclePercentage);

void EnablePWMOutput(void);

void DisablePWMOutput(void);

void ClosePWMModule(void);

#endif // PWM_H

