//*****************************************************************************************
// Author  	: Tessin K Jose
// File Name 	: AppMain.c
// Description  : On off Temp Control & Square wave generation for Int Pre-emption check
// Dependency 	: CCS , Driverlib
// Revision 	: Release date    	Simple Remark    	
//	1.0	: 1-11-2016 		Initial version
//*****************************************************************************************

#include "App.h"

uint32_t ui32SetTemperature;
uint32_t ui32TemperatureReferenceInmV;
uint32_t ui32ADCInputInmV;
uint32_t ui32HeaterOnOff;

uint32_t ui32Task1PeriodInUs;
uint32_t ui32Task2PeriodInUs;
uint32_t ui32HighPriorityTask;
uint32_t ui32Task2Count;


int main(void)
{

	SetSysClockFrequency(SYSCTL_SYSDIV_5); //200/5 = 40MHz
	IntMasterDisable();
	InitConsole();
	OpenADCPort(ADC_CHANNEL_EXTERNAL,ADC_PROC_TRIGGER,0);
	OpenGPIOPortB();
	GPIOPortBWrite(0);
	OpenLEDPort();
	OffLED(LED_WHITE);

	ui32HeaterOnOff =0;
	UARTprintf("Enter the temperature to be set in Temperature Controller %d C to %d C \n",MIN_TEMP,MAX_TEMP);
	ui32SetTemperature =0;
	while ((ui32SetTemperature<MIN_TEMP)||(ui32SetTemperature>MAX_TEMP))
	{
		ui32SetTemperature = GetNumberFromUART();
		if((ui32SetTemperature<MIN_TEMP)||(ui32SetTemperature>MAX_TEMP))
			UARTprintf("Invalid Temp enter again \n");
	}
	UARTprintf("The set temperature is = %d C \n",ui32SetTemperature);
	ui32TemperatureReferenceInmV = TEMP_SENS_VALUE_AT_0_C +( ui32SetTemperature * TEMP_SENS_SENSITIVITY);
	UARTprintf("The temperature reference in mV = %d mV \n",ui32TemperatureReferenceInmV);

	UARTprintf("Enter the temperature control sampling period in US \n");
	ui32Task2PeriodInUs = GetNumberFromUART();
	OpenTimer1(ui32Task2PeriodInUs );

	UARTprintf("Enter the period of square wave in US \n");
	ui32Task1PeriodInUs = GetNumberFromUART()/2;
	OpenTimer0(ui32Task1PeriodInUs );

	IntPriorityGroupingSet(5);// bits 7:6 preempt priority, bit 5 sub priority

	UARTprintf("Which Task should have high priority \n 1= Square Wave \n 2= Temp Control \n");
	ui32HighPriorityTask = GetNumberFromUART();
	if(ui32HighPriorityTask == 2)
	{
		IntPrioritySet(INT_TIMER1A, 0x20);
		IntPrioritySet(INT_TIMER0A, 0x60);
		UARTprintf("Temp Control is set as high priority task\n");
	}
	else if(ui32HighPriorityTask == 1)
	{
		IntPrioritySet(INT_TIMER1A, 0x60);
		IntPrioritySet(INT_TIMER0A, 0x20);
		UARTprintf("Square Wave generation is set as high priority task\n");
	}
	else
	{
		ui32HighPriorityTask = 1;
		IntPrioritySet(INT_TIMER1A, 0x60);
		IntPrioritySet(INT_TIMER0A, 0x20);
		UARTprintf("Invalid option, default Square Wave generation is set as high priority task\n");
	}

	IntMasterEnable();

	StartTimer0();
	StartTimer1();

	while( 1 )
	{
	}

	return 0;
}


void Task1_GenerateSquareWave(void)
{
	GPIOPortBTogglePin(PIN_0);
}

void Task2_TempControl(void)
{
	uint32_t ui32TimemS;

	ui32Task2Count++;
	ui32ADCInputInmV = ReadADCSampleInmV();
	ui32TimemS = (ui32Task2PeriodInUs/1000)*ui32Task2Count;

	if((ui32HeaterOnOff == 0)&&(ui32ADCInputInmV < (ui32TemperatureReferenceInmV - (HYSTERISIS/2))))
	{
		UARTprintf("Time %d ms : Temp sens value = %d mV\n",ui32TimemS, ui32ADCInputInmV);
		UARTprintf("Time %d ms : Turned on Heater \n",ui32TimemS);
		GPIOPortBSetPin(PIN_1);
		OnLED(LED_RED);
		ui32HeaterOnOff =1;
	}
	if((ui32HeaterOnOff == 1)&&(ui32ADCInputInmV > (ui32TemperatureReferenceInmV +(HYSTERISIS/2))))
	{
		UARTprintf("Time %d ms : Temp sens value = %d mV\n",ui32TimemS, ui32ADCInputInmV);
		UARTprintf("Time %d ms : Turned off Heater \n",ui32TimemS);
		GPIOPortBResetPin(PIN_1);
		OffLED(LED_RED);
		ui32HeaterOnOff =0;
	}
}


