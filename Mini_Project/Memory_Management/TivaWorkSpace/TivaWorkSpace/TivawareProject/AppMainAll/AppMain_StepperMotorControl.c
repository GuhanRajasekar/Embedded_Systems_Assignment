//*****************************************************************************************
// Author  		: Tessin K Jose
// File Name 	: AppMain.c
// Description  : Stepper motor control
// Dependency 	: CCS , Driverlib
// Revision 	: Release date    	Simple Remark    	
//	1.0			: 1-11-2016 		Initial version
//*****************************************************************************************

#include "App.h"

#define MOTOR_ON 	1
#define MOTOR_OFF 	0
#define MOTOR_FORWARD 	1
#define MOTOR_REVERSE 	0

extern uint32_t ui32Timer0IntFlag;

void RunStepperMotor(void);

uint8_t ui8_Stepval[3][8]= {{8,4,2,1,8,4,2,1},{12,6,3,9,12,6,3,9},{12,4,6,2,3,1,9,8}};
int32_t i32StepIndex=0;
uint32_t ui32SteppingMode=0;
uint32_t ui32StepTimemS;

uint32_t ui32MotorOnOff=MOTOR_OFF;
uint32_t ui32MotorForwardReverse=MOTOR_FORWARD;

int main(void)
{

	SetSysClockFrequency(SYSCTL_SYSDIV_5); //200/5 = 40MHz
	IntMasterDisable();
	InitConsole();
	OpenKeyBoardPort(INT_ENABLE);
	OpenLEDPort();
	OffLED(LED_WHITE);
	OnLED(LED_RED);
	OpenGPIOPortB();
	GPIOPortBWrite(0);

	UARTprintf("Select The Stepping mode \n 1= Full step 1 phase \n 2= Full step 2 phase \n 3= Half step   \n");
	ui32SteppingMode = GetNumberFromUART();
	ui32SteppingMode -=1;

	switch (ui32SteppingMode)
	{
	case 0:
		UARTprintf("Selected  Full step 1 phase \n");
		break;
	case 1:
		UARTprintf("Selected  Full step 2 phase \n");
		break;
	case 2:
		UARTprintf("Selected Half stepping \n");
		break;
	default :
		UARTprintf("Invalid option, Selected  Full step 1 phase by default\n");
		ui32SteppingMode =0;
		break;
	}

	UARTprintf("Select The Step time in mS \n");
	ui32StepTimemS = GetNumberFromUART();
	OpenTimer0(ui32StepTimemS *1000);

	UARTprintf("Press Switch 1 to start or stop the Motor \n");
	UARTprintf("Press Switch 2 to change the direction of Motor \n");
	UARTprintf("LED = RED , Motor Stopped \n");
	UARTprintf("LED = GREEN , Motor Running in Forward Direction \n");
	UARTprintf("LED = BLUE , Motor Running in Reverse Direction \n");

	IntMasterEnable();
	StartTimer0();

	while( 1 )
	{
		if(KEY_ON == CheckKeyPress(SW1))
		{
			ClearKeyPress(SW1);
			if(ui32MotorOnOff == MOTOR_OFF)
			{
				ui32MotorOnOff = MOTOR_ON;
				OffLED(LED_RED);
				if(ui32MotorForwardReverse == MOTOR_FORWARD)
					OnLED(LED_GREEN);
				else
					OnLED(LED_BLUE);
			}
			else
			{
				ui32MotorOnOff = MOTOR_OFF;
				OffLED(LED_WHITE);
				OnLED(LED_RED);
			}
		}

		if(KEY_ON == CheckKeyPress(SW2))
		{
			ClearKeyPress(SW2);
			if(ui32MotorOnOff == MOTOR_ON)
			{
				if(ui32MotorForwardReverse == MOTOR_REVERSE)
				{
					ui32MotorForwardReverse = MOTOR_FORWARD;
					OffLED(LED_WHITE);
					OnLED(LED_GREEN);
				}
				else
				{
					ui32MotorForwardReverse = MOTOR_REVERSE;
					OffLED(LED_WHITE);
					OnLED(LED_BLUE);
				}
			}
		}

		if(ui32Timer0IntFlag == 1)
		{
			ui32Timer0IntFlag =0;
			RunStepperMotor();
		}
	}

	return 0;
}


void RunStepperMotor(void)
{
	if(ui32MotorOnOff == MOTOR_ON)
	{
		if(ui32MotorForwardReverse == MOTOR_FORWARD)
		{
			i32StepIndex++;
			if(i32StepIndex >7) i32StepIndex = 0;
			GPIOPortBWrite(ui8_Stepval[ui32SteppingMode][i32StepIndex]);
		}
		else if(ui32MotorForwardReverse == MOTOR_REVERSE)
		{
			i32StepIndex--;
			if(i32StepIndex < 0)i32StepIndex = 7;
			GPIOPortBWrite(ui8_Stepval[ui32SteppingMode][i32StepIndex]);
		}
	}
}
