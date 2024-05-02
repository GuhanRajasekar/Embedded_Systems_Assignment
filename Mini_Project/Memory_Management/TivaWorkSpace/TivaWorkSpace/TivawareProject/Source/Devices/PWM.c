//*****************************************************************************************
// Author  		: Tessin K Jose
// File Name 	: PWM.c 
// Description  : functions for using the on chip PWM Module
// Dependency 	: CCS , Driverlib
// Revision 	: Release date    	Simple Remark    	
//	1.0			: 1-11-2016 		Initial version
//*****************************************************************************************

#include "PWM.h"

#define PWM_SUCCESS 		0
#define PWM_ERR			1
#define PWM_ERR_SF_HIGH		2
#define PWM_ERR_SF_LOW		3
 
int32_t i32PWMFrequency;
int32_t i32PWMTimerPeriodInPWMClks;

int32_t OpenPWMModule(int32_t SwitchingFrequencyHz)
{	
	int32_t i32PWMReturn;

	// choose PWM clk = sys clk /32
	SysCtlPWMClockSet( SYSCTL_PWMDIV_32 );

    SysCtlPeripheralEnable( SYSCTL_PERIPH_PWM1 );

    // GPIO port D pin 0 is PWM output
	SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOD );
    GPIOPinTypePWM( GPIO_PORTD_BASE, GPIO_PIN_0 );
    GPIOPinConfigure( GPIO_PD0_M1PWM0 );

	// Configure the PWM module in down counting mode
 	PWMGenConfigure( PWM1_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN );
	
	i32PWMFrequency = SysCtlClockGet() / 32;
	i32PWMTimerPeriodInPWMClks = (i32PWMFrequency / SwitchingFrequencyHz) -1;
	
	if(i32PWMTimerPeriodInPWMClks > 65535) i32PWMReturn = PWM_ERR_SF_LOW;
	else if(i32PWMTimerPeriodInPWMClks < 100)i32PWMReturn = PWM_ERR_SF_HIGH;
	else
	{
		PWMGenPeriodSet( PWM1_BASE, PWM_GEN_0, i32PWMTimerPeriodInPWMClks );     //Total Pulse Period
		PWMPulseWidthSet( PWM1_BASE, PWM_OUT_0, i32PWMTimerPeriodInPWMClks / 2 ); //Positive pulse period
		PWMOutputState( PWM1_BASE, PWM_OUT_0_BIT, false );
        	PWMGenDisable( PWM1_BASE, PWM_GEN_0 );
		i32PWMReturn = PWM_SUCCESS;
	}
	return i32PWMReturn;
}


uint32_t ConfigurePWMDutyCycle(uint32_t DutyCyclePercentage)
{	
	int32_t i32PWMReturn;

	if(DutyCyclePercentage < 100)
	{	
		PWMPulseWidthSet( PWM1_BASE, PWM_OUT_0, (DutyCyclePercentage * i32PWMTimerPeriodInPWMClks) / 100 ); //Positive pulse period
		i32PWMReturn =	PWM_SUCCESS;
	}
	else i32PWMReturn = PWM_ERR;
	
	return i32PWMReturn;
}

void EnablePWMOutput(void)
{
	PWMOutputState( PWM1_BASE, PWM_OUT_0_BIT, true );
        PWMGenEnable( PWM1_BASE, PWM_GEN_0 );
}

void DisablePWMOutput(void)
{
	PWMOutputState( PWM1_BASE, PWM_OUT_0_BIT, false );
        PWMGenDisable( PWM1_BASE, PWM_GEN_0 );
}

void ClosePWMModule(void)
{
	PWMOutputState( PWM1_BASE, PWM_OUT_0_BIT, false );
        PWMGenDisable( PWM1_BASE, PWM_GEN_0 );	
	SysCtlPeripheralDisable( SYSCTL_PERIPH_PWM1 );
}

