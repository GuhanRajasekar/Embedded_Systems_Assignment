//*****************************************************************************
//
// compdcm_mpu9150.c - Example use of the SensorLib with the MPU9150
//
// Copyright (c) 2013-2017 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
//
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
//
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
//
// This is part of revision 2.1.4.178 of the EK-TM4C123GXL Firmware Package.
//
//*****************************************************************************
#include "main.h"
#include "inc/tm4c123gh6pm.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "sensorlib/hw_mpu9150.h"
#include "sensorlib/hw_ak8975.h"
#include "sensorlib/i2cm_drv.h"
#include "sensorlib/ak8975.h"
#include "sensorlib/mpu9150.h"
#include "sensorlib/comp_dcm.h"
#include "drivers/rgb.h"
#include <math.h>
#include <string.h>
#include "driverlib/pwm.h"

//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>Nine Axis Sensor Fusion with the MPU9150 and Complimentary-Filtered
//! DCM (compdcm_mpu9150)</h1>
//!
//! This example demonstrates the basic use of the Sensor Library, TM4C123G
//! LaunchPad and SensHub BoosterPack to obtain nine axis motion measurements
//! from the MPU9150.  The example fuses the nine axis measurements into a set
//! of Euler angles: roll, pitch and yaw.  It also produces the rotation
//! quaternions.  The fusion mechanism demonstrated is complimentary-filtered
//! direct cosine matrix (DCM) algorithm is provided as part of the Sensor
//! Library.
//!
//! Connect a serial terminal program to the LaunchPad's ICDI virtual serial
//! port at 115,200 baud.  Use eight bits per byte, no parity and one stop bit.
//! The raw sensor measurements, Euler angles and quaternions are printed to
//! the terminal.  The RGB LED begins to blink at 1Hz after initialization is
//! completed and the example application is running.
//
//*****************************************************************************

//*****************************************************************************
//
// Define MPU9150 I2C Address.
//
//*****************************************************************************
#define MPU9150_I2C_ADDRESS     0x68






//*****************************************************************************
//
// Calling functions
//
//*****************************************************************************
 void MPU9150_IN_IT();
 uint32_t Measure_distance(void);
 void Timer0ACapture_init(void);
 void Delay_MicroSecond(int time);
 void UART0_Transmitter(unsigned char data);
 void printstring(char *str);
 void Delay(unsigned long counter);
 void delayMs(int n);
 void send_trigger_pulse(void);
 void Motor_in_it();
 void ConfigureUART(void);
 void ultrasonic();
 void SERVO_PWM_init();
 static inline void enable_irq(void);
 static inline void disable_irq(void);
 void ADC_PWM_init();
 void LCD_RecCommand();               // Function to send necessary control signals to LCD to receive a Command
 void LCD_RecData();                  // Function to send necessary control signals to LCD to receive Data
 void LCD_init();                     // Function to initialize the LCD
 void LCD_PutData(char* data,int size_int);

#define ALPHA 0.1f
#define GREEN_ON GPIO_PORTF_DATA_R=0x08
#define YELLOW_ON GPIO_PORTF_DATA_R=0x0A  // turn on red and green
#define OFF GPIO_PORTF_DATA_R=0x00        // turn of all leds
#define  PWM_FREQUENCY_MOTOR 1000
#define  PWM_FREQUENCY 50

//*****************************************************************************
//
// Global array for holding the color values for the RGB.
//
//*****************************************************************************
 /* global variables  */
uint32_t g_pui32Colors[3];
int_fast32_t i32IPart[16], i32FPart[16],X_I,X_F,Y_I,Y_F,Z_I,Z_F;
uint_fast32_t ui32Idx, ui32CompDCMStarted;
float pfData[16];
float *pfAccel, *pfGyro, *pfMag, *pfEulers, *pfQuaternion;
//float mag;
uint32_t ui32Load_motor, ui32PWMClock,ui8Adjust_motor=0, ui32Load,ui8Adjust=83;
uint32_t time; /*stores pulse on time */
uint32_t distance; /* stores measured distance value */
char mesg[240];  /* string format of distance value */
int frac,i,servo_delay;
int real,status,speed,PWMout_H,PWMout_T;
char version[30];
float pitch, yaw,Filteredpitch =0.0, prevyaw=0.0,Filteredyaw=0.0, prevMag_z=0.0, Distance;
float X,Y,Z, temp_T,temp_H,Mag_z,time1;
float prevFilteredAccelX = 0.0, prevFilteredAccelY = 0.0, prevFilteredAccelZ = 0.0;
    float prevFilteredGyroX = 0.0, prevFilteredGyroY = 0.0, prevFilteredGyroZ = 0.0;
    float prevFilteredMagX = 0.0, prevFilteredMagY = 0.0, prevFilteredMagZ = 0.0;
//*****************************************************************************
//
// Global instance structure for the I2C master driver.
//
//*****************************************************************************
tI2CMInstance g_sI2CInst;

//*****************************************************************************
//
// Global instance structure for the ISL29023 sensor driver.
//
//*****************************************************************************
tMPU9150 g_sMPU9150Inst;

//*****************************************************************************
//
// Global Instance structure to manage the DCM state.
//
//*****************************************************************************
tCompDCM g_sCompDCMInst;

//*****************************************************************************
//
// Global flags to alert main that MPU9150 I2C transaction is complete
//
//*****************************************************************************
volatile uint_fast8_t g_vui8I2CDoneFlag;

//*****************************************************************************
//
// Global flags to alert main that MPU9150 I2C transaction error has occurred.
//
//*****************************************************************************
volatile uint_fast8_t g_vui8ErrorFlag;

//*****************************************************************************
//
// Global flags to alert main that MPU9150 data is ready to be retrieved.
//
//*****************************************************************************
volatile uint_fast8_t g_vui8DataFlag;

//*****************************************************************************
//
// Global counter to control and slow down the rate of data to the terminal.
//
//*****************************************************************************

#define PRINT_SKIP_COUNT        10

uint32_t g_ui32PrintSkipCounter;

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif


// Function to apply a low-pass filter to a single data point
float lowPassFilter(float input, float prevOutput) {
    // Calculate the filtered output using the exponential moving average formula
    return ALPHA * input + (1 - ALPHA) * prevOutput;
}


//*****************************************************************************
//
// MPU9150 Sensor callback function.  Called at the end of MPU9150 sensor
// driver transactions. This is called from I2C interrupt context. Therefore,
// we just set a flag and let main do the bulk of the computations and display.
//
//*****************************************************************************
void
MPU9150AppCallback(void *pvCallbackData, uint_fast8_t ui8Status)
{
    //
    // If the transaction succeeded set the data flag to indicate to
    // application that this transaction is complete and data may be ready.
    //
    if(ui8Status == I2CM_STATUS_SUCCESS)
    {
        g_vui8I2CDoneFlag = 1;
    }

    //
    // Store the most recent status in case it was an error condition
    //
    g_vui8ErrorFlag = ui8Status;
}

//*****************************************************************************
//
// Called by the NVIC as a result of GPIO port B interrupt event. For this
// application GPIO port B pin 2 is the interrupt line for the MPU9150
//
//*****************************************************************************
void
IntGPIOe(void)
{
    unsigned long ulStatus;

    ulStatus = GPIOIntStatus(GPIO_PORTE_BASE, true);

    //
    // Clear all the pin interrupts that are set
    //
    GPIOIntClear(GPIO_PORTE_BASE, ulStatus);

    if(ulStatus & GPIO_PIN_1)
    {
        //
        // MPU9150 Data is ready for retrieval and processing.
        //
        MPU9150DataRead(&g_sMPU9150Inst, MPU9150AppCallback, &g_sMPU9150Inst);
    }
}

//*****************************************************************************
//
// Called by the NVIC as a result of I2C3 Interrupt. I2C3 is the I2C connection
// to the MPU9150.
//
//*****************************************************************************
void
MPU9150I2CIntHandler(void)
{
    //
    // Pass through to the I2CM interrupt handler provided by sensor library.
    // This is required to be at application level so that I2CMIntHandler can
    // receive the instance structure pointer as an argument.
    //
    I2CMIntHandler(&g_sI2CInst);
}

//*****************************************************************************
//
// MPU9150 Application error handler. Show the user if we have encountered an
// I2C error.
//
//*****************************************************************************
void
MPU9150AppErrorHandler(char *pcFilename, uint_fast32_t ui32Line)
{
    //
    // Set terminal color to red and print error status and locations
    //
    UARTprintf("\033[31;1m");
    UARTprintf("Error: %d, File: %s, Line: %d\n"
               "See I2C status definitions in sensorlib\\i2cm_drv.h\n",
               g_vui8ErrorFlag, pcFilename, ui32Line);

    //
    // Return terminal color to normal
    //
    UARTprintf("\033[0m");

    //
    // Set RGB Color to RED
    //
//    g_pui32Colors[0] = 0xFFFF;
//    g_pui32Colors[1] = 0;
//    g_pui32Colors[2] = 0;
//    RGBColorSet(g_pui32Colors);

    //
    // Increase blink rate to get attention
    //
//    RGBBlinkRateSet(10.0f);

    //
    // Go to sleep wait for interventions.  A more robust application could
    // attempt corrective actions here.
    //
    while(1)
    {
        //
        // Do Nothing
        //
    }
}

//*****************************************************************************
//
// Function to wait for the MPU9150 transactions to complete. Use this to spin
// wait on the I2C bus.
//
//*****************************************************************************
void
MPU9150AppI2CWait(char *pcFilename, uint_fast32_t ui32Line)
{
    //
    // Put the processor to sleep while we wait for the I2C driver to
    // indicate that the transaction is complete.
    //
    while((g_vui8I2CDoneFlag == 0) && (g_vui8ErrorFlag == 0))
    {
        //
        // Do Nothing
        //
    }

    //
    // If an error occurred call the error handler immediately.
    //
    if(g_vui8ErrorFlag)
    {
        MPU9150AppErrorHandler(pcFilename, ui32Line);
    }

    //
    // clear the data flag for next use.
    //
    g_vui8I2CDoneFlag = 0;
}
//*****************************************************************************
//
// Configure the UART and its pins.  This must be called before UARTprintf().
//
//*****************************************************************************
void
ConfigureUART(void)
{
    //
    // Enable the GPIO Peripheral used by the UART.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Enable UART0
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Configure GPIO Pins for UART mode.
    //
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, 16000000);
}

void MPU9150_IN_IT(){
    //
    // Setup the system clock to run at 40 Mhz from PLL with crystal reference
    //
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
                       SYSCTL_OSC_MAIN);

    //
    // Enable port B used for motion interrupt.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);


    //
    // Print the welcome message to the terminal.
    //
    UARTprintf("\033[2JMPU9150 Raw Example\n");


    //
    // The I2C3 peripheral must be enabled before use.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C3);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

    //
    // Configure the pin muxing for I2C3 functions on port D0 and D1.
    //
    ROM_GPIOPinConfigure(GPIO_PD0_I2C3SCL);
    ROM_GPIOPinConfigure(GPIO_PD1_I2C3SDA);

    //
    // Select the I2C function for these pins.  This function will also
    // configure the GPIO pins pins for I2C operation, setting them to
    // open-drain operation with weak pull-ups.  Consult the data sheet
    // to see which functions are allocated per pin.
    //
    GPIOPinTypeI2CSCL(GPIO_PORTD_BASE, GPIO_PIN_0);
    ROM_GPIOPinTypeI2C(GPIO_PORTD_BASE, GPIO_PIN_1);

    //
    // Configure and Enable the GPIO interrupt. Used for INT signal from the
    // MPU9150
    //
    ROM_GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_1);
    GPIOIntEnable(GPIO_PORTE_BASE, GPIO_PIN_1);
    ROM_GPIOIntTypeSet(GPIO_PORTE_BASE, GPIO_PIN_1, GPIO_FALLING_EDGE);
    ROM_IntEnable(INT_GPIOE);

    //
    // Keep only some parts of the systems running while in sleep mode.
    // GPIOB is for the MPU9150 interrupt pin.
    // UART0 is the virtual serial port
    // TIMER0, TIMER1 and WTIMER5 are used by the RGB driver
    // I2C3 is the I2C interface to the ISL29023
    //
    ROM_SysCtlPeripheralClockGating(true);
    ROM_SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_GPIOE);
    ROM_SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_UART0);
    ROM_SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_TIMER0);
    ROM_SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_TIMER1);
    ROM_SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_I2C3);
    ROM_SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_WTIMER5);

    //
    // Enable interrupts to the processor.
    //
    ROM_IntMasterEnable();

    //
    // Initialize I2C3 peripheral.
    //
    I2CMInit(&g_sI2CInst, I2C3_BASE, INT_I2C3, 0xff, 0xff,
             ROM_SysCtlClockGet());

    //
    // Initialize the MPU9150 Driver.
    //
    MPU9150Init(&g_sMPU9150Inst, &g_sI2CInst, MPU9150_I2C_ADDRESS,
                MPU9150AppCallback, &g_sMPU9150Inst);

    //
    // Wait for transaction to complete
    //
    MPU9150AppI2CWait(__FILE__, __LINE__);

    //
    // Write application specifice sensor configuration such as filter settings
    // and sensor range settings.
    //
    g_sMPU9150Inst.pui8Data[0] = MPU9150_CONFIG_DLPF_CFG_94_98;
    g_sMPU9150Inst.pui8Data[1] = MPU9150_GYRO_CONFIG_FS_SEL_250;
    g_sMPU9150Inst.pui8Data[2] = (MPU9150_ACCEL_CONFIG_ACCEL_HPF_0_63HZ |
                                  MPU9150_ACCEL_CONFIG_AFS_SEL_2G);
    MPU9150Write(&g_sMPU9150Inst, MPU9150_O_CONFIG, g_sMPU9150Inst.pui8Data, 3,
                 MPU9150AppCallback, &g_sMPU9150Inst);

    //
    // Wait for transaction to complete
    //
    MPU9150AppI2CWait(__FILE__, __LINE__);

    //
    // Configure the data ready interrupt pin output of the MPU9150.
    //
    g_sMPU9150Inst.pui8Data[0] = MPU9150_INT_PIN_CFG_INT_LEVEL |
                                    MPU9150_INT_PIN_CFG_INT_RD_CLEAR |
                                    MPU9150_INT_PIN_CFG_LATCH_INT_EN;
    g_sMPU9150Inst.pui8Data[1] = MPU9150_INT_ENABLE_DATA_RDY_EN;
    MPU9150Write(&g_sMPU9150Inst, MPU9150_O_INT_PIN_CFG,
                 g_sMPU9150Inst.pui8Data, 2, MPU9150AppCallback,
                 &g_sMPU9150Inst);

    //
    // Wait for transaction to complete
    //
    MPU9150AppI2CWait(__FILE__, __LINE__);

    //
    // Initialize the DCM system. 50 hz sample rate.
    // accel weight = .2, gyro weight = .8, mag weight = .2
    //
    CompDCMInit(&g_sCompDCMInst, 1.0f / 50.0f, 0.2f, 0.6f, 0.2f);

    UARTprintf("\033[2J\033[H");
    UARTprintf("MPU9150 9-Axis Simple Data Application Example\n\n");
    UARTprintf("\033[20GX\033[31G|\033[43GY\033[54G|\033[66GZ\n\n");
    UARTprintf("Accel\033[8G|\033[31G|\033[54G|\n\n");
    UARTprintf("Gyro\033[8G|\033[31G|\033[54G|\n\n");
    UARTprintf("Mag\033[8G|\033[31G|\033[54G|\n\n");
    UARTprintf("\n\033[20GPitch\033[31G|\033[43GRoll\033[54G|\033[66GYaw\n\n");
    UARTprintf("Eulers\033[8G|\033[31G|\033[54G|\n\n");

    UARTprintf("\n\033[17GQ1\033[26G|\033[35GQ2\033[44G|\033[53GQ3\033[62G|"
               "\033[71GQ4\n\n");
    UARTprintf("Q\033[8G|\033[26G|\033[44G|\033[62G|\n\n");


    //
    // Enable blinking indicates config finished successfully
    //
    //RGBBlinkRateSet(1.0f);

    ui32CompDCMStarted = 0;

}


void MPU9150()
{
    //
          // Go to sleep mode while waiting for data ready.
          //
          while(!g_vui8I2CDoneFlag)
          {
              ROM_SysCtlSleep();
          }

          //
          // Clear the flag
          //
          g_vui8I2CDoneFlag = 0;

          //
          // Get floating point version of the Accel Data in m/s^2.
          //
          MPU9150DataAccelGetFloat(&g_sMPU9150Inst, pfAccel, pfAccel + 1,
                                   pfAccel + 2);

          //
          // Get floating point version of angular velocities in rad/sec
          //
          MPU9150DataGyroGetFloat(&g_sMPU9150Inst, pfGyro, pfGyro + 1,
                                  pfGyro + 2);

          //
          // Get floating point version of magnetic fields strength in tesla
          //
          MPU9150DataMagnetoGetFloat(&g_sMPU9150Inst, pfMag, pfMag + 1,
                                     pfMag + 2);

          //
          // Check if this is our first data ever.
          //
          if(ui32CompDCMStarted == 0)
          {
              //
              // Set flag indicating that DCM is started.
              // Perform the seeding of the DCM with the first data set.
              //
              ui32CompDCMStarted = 1;
              CompDCMMagnetoUpdate(&g_sCompDCMInst, pfMag[0], pfMag[1],
                                   pfMag[2]);
              CompDCMAccelUpdate(&g_sCompDCMInst, pfAccel[0], pfAccel[1],
                                 pfAccel[2]);
              CompDCMGyroUpdate(&g_sCompDCMInst, pfGyro[0], pfGyro[1],
                                pfGyro[2]);
              CompDCMStart(&g_sCompDCMInst);
          }
          else
          {
              //
              // DCM Is already started.  Perform the incremental update.
              //
              CompDCMMagnetoUpdate(&g_sCompDCMInst, pfMag[0], pfMag[1],
                                   pfMag[2]);
              CompDCMAccelUpdate(&g_sCompDCMInst, pfAccel[0], pfAccel[1],
                                 pfAccel[2]);
              CompDCMGyroUpdate(&g_sCompDCMInst, -pfGyro[0], -pfGyro[1],
                                -pfGyro[2]);
              CompDCMUpdate(&g_sCompDCMInst);
          }

          //
          // Increment the skip counter.  Skip counter is used so we do not
          // overflow the UART with data.
          //
          g_ui32PrintSkipCounter++;
          if(g_ui32PrintSkipCounter >= PRINT_SKIP_COUNT)
          {
              //
              // Reset skip counter.
              //
              g_ui32PrintSkipCounter = 0;

              //
              // Get Euler data. (Roll Pitch Yaw)
              //
              CompDCMComputeEulers(&g_sCompDCMInst, pfEulers, pfEulers + 1,
                                   pfEulers + 2);

              //
              // Get Quaternions.
              //
              CompDCMComputeQuaternion(&g_sCompDCMInst, pfQuaternion);

              //
              // convert mag data to micro-tesla for better human interpretation.
              //
              pfMag[0] *= 1e6;
              pfMag[1] *= 1e6;
              pfMag[2] *= 1e6;

              //
              // Convert Eulers to degrees. 180/PI = 57.29...
              // Convert Yaw to 0 to 360 to approximate compass headings.
              //
              pfEulers[0] *= 57.295779513082320876798154814105f;
              pfEulers[1] *= 57.295779513082320876798154814105f;
              pfEulers[2] *= 57.295779513082320876798154814105f;
              if(pfEulers[2] < 0)
              {
                  pfEulers[2] += 360.0f;
              }

              //
              // Now drop back to using the data as a single array for the
              // purpose of decomposing the float into a integer part and a
              // fraction (decimal) part.
              //
              for(ui32Idx = 0; ui32Idx < 16; ui32Idx++)
              {
                  //
                  // Conver float value to a integer truncating the decimal part.
                  //
                  i32IPart[ui32Idx] = (int32_t) pfData[ui32Idx];

                  //
                  // Multiply by 1000 to preserve first three decimal values.
                  // Truncates at the 3rd decimal place.
                  //
                  i32FPart[ui32Idx] = (int32_t) (pfData[ui32Idx] * 1000.0f);

                  //
                  // Subtract off the integer part from this newly formed decimal
                  // part.
                  //
                  i32FPart[ui32Idx] = i32FPart[ui32Idx] -
                                      (i32IPart[ui32Idx] * 1000);

                  //
                  // make the decimal part a positive number for display.
                  //
                  if(i32FPart[ui32Idx] < 0)
                  {
                      i32FPart[ui32Idx] *= -1;
                  }
              }

              //
              // Print the acceleration numbers in the table.
              //
              UARTprintf("\033[5;17H%3d.%03d", i32IPart[0], i32FPart[0]);
              UARTprintf("\033[5;40H%3d.%03d", i32IPart[1], i32FPart[1]);
              UARTprintf("\033[5;63H%3d.%03d", i32IPart[2], i32FPart[2]);

              //
              // Print the angular velocities in the table.
              //
              UARTprintf("\033[7;17H%3d.%03d", i32IPart[3], i32FPart[3]);
              UARTprintf("\033[7;40H%3d.%03d", i32IPart[4], i32FPart[4]);
              UARTprintf("\033[7;63H%3d.%03d", i32IPart[5], i32FPart[5]);

              //
              // Print the magnetic data in the table.
              //
              UARTprintf("\033[9;17H%3d.%03d", i32IPart[6], i32FPart[6]);
              UARTprintf("\033[9;40H%3d.%03d", i32IPart[7], i32FPart[7]);
              UARTprintf("\033[9;63H%3d.%03d", i32IPart[8], i32FPart[8]);

              //
              // Print the Eulers in a table.
              //
              UARTprintf("\033[14;17H%3d.%03d", i32IPart[9], i32FPart[9]);
              UARTprintf("\033[14;40H%3d.%03d", i32IPart[10], i32FPart[10]);
              UARTprintf("\033[14;63H%3d.%03d", i32IPart[11], i32FPart[11]);
              //UARTprintf("\n Roll:%3d.%03d",i32IPart[9], i32FPart[9]);



              //
              // Print the quaternions in a table format.
              //
              UARTprintf("\033[19;14H%3d.%03d", i32IPart[12], i32FPart[12]);
              UARTprintf("\033[19;32H%3d.%03d", i32IPart[13], i32FPart[13]);
              UARTprintf("\033[19;50H%3d.%03d", i32IPart[14], i32FPart[14]);
              UARTprintf("\033[19;68H%3d.%03d", i32IPart[15], i32FPart[15]);
              pitch = i32IPart[9]+ i32FPart[9]/1000;
              yaw = i32IPart[11]+i32IPart[11]/1000;
              Filteredpitch = lowPassFilter(pitch, Filteredpitch);
              Filteredyaw = lowPassFilter(yaw, Filteredyaw);
              Y_I= Filteredyaw;
              Y_F = Filteredyaw*1000-Filteredyaw;
              Y_F = Y_F%1000;
              X_I= Filteredpitch;
              X_F = abs(Filteredpitch*1000-Filteredpitch);
              X_F = X_F%1000;
//              Z = pfGyro[2];
//              X = (atan2(pfAccel[0], sqrt (pfAccel[1] * pfAccel[1] + pfAccel[2] * pfAccel[2]))*180.0)/3.14;
//              Y = (atan2(pfAccel[1], sqrt (pfAccel[0] * pfAccel[0] + pfAccel[2] * pfAccel[2]))*180.0)/3.14;
//              Z_I= Z;
//              Z_F = Z*1000-Z;
//              Y_I= Y;
//              Y_F = Y*1000-Y;
//              X_I= X;
//              X_F = X*1000-X;
//              X_F = X_F%1000;
              UARTprintf("\nPITCH:%3d.%03d,YAW:%3d.%03d,Z:%3d.%03d \n",X_I,X_F,Y_I,Y_F,Z_I,Z_F);
              Mag_z= i32IPart[8]+i32FPart[8]/1000;
              if (Mag_z-prevMag_z<5.0)
              {

                  status = 1;
                  prevMag_z= Mag_z;
                  Z_I= Mag_z;
                  Z_F = Mag_z*1000-Mag_z;
                  Z_F = Z_F%1000;
                 // mode();

              }
              else if(Mag_z-prevMag_z>5.0){
                  status = 2;
                  prevMag_z= Mag_z;
               //   mode();
              }
              if (abs(Filteredyaw-prevyaw)<=2)
              {
                  prevyaw =Filteredyaw;
                  speed=0;
              }
              else if (abs(Filteredyaw-prevyaw)>2)
              {
                  prevyaw =Filteredyaw;
                  speed=1;
              }
              UARTprintf("\n\033[24;2Hspeed:%d",speed);
          }

 }

//TO CONTROL THE CHANGES OF BLINKING LED, DATA PRINTING ON LCD
void mode(){
    switch(status){
        case 1:
                GREEN_ON;
               // LCD_PutData("Flight",12);
                UARTprintf("\r\033[22;24H                                   ");
                UARTprintf("\r\033[22;24H FLIGHT MODE\n");

                //instruction();
                break;
        case 2:
                GREEN_ON;
                delayMs(100);
                OFF;
                delayMs(100);
                UARTprintf("\r\033[22;24H                                   ");
                UARTprintf("\r\033[22;24H ASCENDING OR DESCENDING MODE\n");
               // LCD_PutData("Flight1",12);

                break;
        case 3:
                UARTprintf("\r\033[22;24H                                  ");
                UARTprintf("\r\033[22;24H OBSTACLE DETECTED\n");

                break;
        case 4:
                UARTprintf("\r                                   ");
    }

}


void configure_systick()
{
    NVIC_ST_CTRL_R      = 0;            // Disable timer during setup
    NVIC_ST_RELOAD_R    = 1600000-1;       // This should expire once every 0.1 s
    NVIC_ST_CTRL_R      = 0x00000007;   // Clock src set to system clock and enable timer and interrupt
}


void tailmotor()
{
    //result = Filteredyaw; // read conversion result
    //ADC0_ISC_R = 8; // clear completion flag
    temp_T=(((Filteredyaw)/360)*4999);
    PWMout_T=(int)temp_T;


    if (speed == 1)
          {
        PWM1_1_CMPA_R= PWMout_T;   // Duty cycle of 0%
            delayMs(60);
          }
    else if(speed ==0 ){
        PWM1_1_CMPA_R  = 2499;
            delayMs(60);
            }
    UARTprintf("\n\033[23;24HPWM:%d",PWMout_T);
}


void PortAB_INIT()
{
    SYSCTL_RCGC2_R |= 0x00000003;       // enable clock to GPIOA, GPIOB at clock gating control register
    // Enable the GPIO pins
    GPIO_PORTA_DIR_R |= 0xC0;       // PA4 to PA7 set to output
    GPIO_PORTB_DIR_R |= 0xFF;       // PB0 to PB7 set to output
    // enable the GPIO pins for digital function
    GPIO_PORTA_DEN_R |= 0xC0;       // enabling PA4 to PA7
    GPIO_PORTB_DEN_R |= 0xFF;       // enabling PB0 to PB8
}
void LCD_init()
{
    /*
       Each time we load portB with necessary command, we need to send appropriate control
       signals to PORTA so that it can latch on to the data.
    */
    GPIO_PORTB_DATA_R = 0x38; LCD_RecCommand();   // 2 Lines and 5×7 character (8-bit data, D0 to D7)
    GPIO_PORTB_DATA_R = 0x01; LCD_RecCommand();   // clear Display Screen
    GPIO_PORTB_DATA_R = 0x0F; LCD_RecCommand();   // Display ON, Cursor Blinking
    GPIO_PORTB_DATA_R = 0x80; LCD_RecCommand();   // Force Cursor to beginning of 1st Line
}


// The function LCD_RecCommand() sets RS(PA6) = 0 and a high to low pulse on E pin(PA7). This will help us send commands to the LCD display
void LCD_RecCommand()
{
    GPIO_PORTA_DATA_R  &=  0xBF;  // Set PA6 to 0, keep other bits unaffected
    GPIO_PORTA_DATA_R  |=  0x80;  // Set PA7 to 1, keep other bits unaffected
    delayMs(10);     // Add some delay
    GPIO_PORTA_DATA_R  &=  0x7F;  // Set PA7 to 0, keep other bits unaffected
}

// The function LCD_RecData() sets RS(PA6) = 1 and a high to low pulse on E pin (PA7). This will help us send data to the LCD Display
void LCD_RecData()
{
    GPIO_PORTA_DATA_R  |=  0x40;  // Set PA6 to 1, keep other bits unaffected
    GPIO_PORTA_DATA_R  |=  0x80;  // Set PA7 to 1, keep other bits unaffected
    delayMs(10);      // Add some delay
    GPIO_PORTA_DATA_R  &=  0x7F;  // Set PA7 to 0, keep other bits unaffected
}

void LCD_PutData(char* data,int size_int)
{
    // Before writing data into LCD, we first need to clear the existing contents that are currently displayed on the LCD
    GPIO_PORTB_DATA_R = 0x01; LCD_RecCommand();   // clear Display Screen
    GPIO_PORTB_DATA_R = 0x0F; LCD_RecCommand();   // Display ON, Cursor Blinking
    GPIO_PORTB_DATA_R = 0x80; LCD_RecCommand();   // Force Cursor to beginning of 1st Line


    //   Each time we load portB with necessary data, we need to send appropriate control signals to PORTA so that it can latch on to the data.

    for(i = 0;i<size_int;i++)
    {
        GPIO_PORTB_DATA_R = data[i];
        LCD_RecData();
        // i = i+1;
        // Move to second line if first line is filled up
        if(i == 16)
        {
            GPIO_PORTB_DATA_R = 0xC0; // Force Cursor To Second Line
            LCD_RecCommand();         // Send Necessary Control Signals
        }
    }
    return;
}



// ultrasonic sensor
void ultrasonic()

{   UARTprintf("2\n");
    if ((GPIO_PORTF_DATA_R & 0x10)==0 ) {
    time = Measure_distance(); /* take pulse duration measurement */
   // distance=time;
    UARTprintf("3\n");
   distance = ((time *10625)/100000); /* convert pulse duration into distance */
   frac=distance%100;
   real=(distance-frac)/100;
    sprintf(mesg, "\r\n distance:%d.%d cm", real,frac); /*convert float type distance data into string */
    UARTprintf("\033[20;30H %s\n ",mesg); /*transmit data to computer */

   Distance = real+frac/100;
   if(Distance<=40.0)
   {
       status = 3;
      // mode();
   }
   else
   {
       status = 4;
   }

}

}
uint32_t Measure_distance(void)
{
      unsigned int lastEdge, thisEdge;
      /* Given 10us trigger pulse. PD3 is trigger pin (output) */
      GPIO_PORTD_DATA_R &= ~(1<<3); /* make trigger  pin low */
      Delay_MicroSecond(10); /*10 seconds delay */
      GPIO_PORTD_DATA_R |= (1<<3); /* make trigger  pin high */
      Delay_MicroSecond(10); /*10 seconds delay */
      GPIO_PORTD_DATA_R &= ~(1<<3); /* make trigger  pin low */
    while(1)
    {
        WTIMER3_ICR_R = 4;                      /* clear Wtimer2A capture flag */
        while(((WTIMER3_RIS_R & 4) == 0)) ;       /* wait till captured */
        if((GPIO_PORTD_DATA_R&(1<<2)))          /*check if rising edge occurs on PD0 echo pin*/
        {
            lastEdge = WTIMER3_TAR_R;           /* save the timestamp */
            WTIMER3_ICR_R = 4;                  /* detect falling edge. Hence clear Wtimer2A capture flag */
            while(((WTIMER3_RIS_R&4) == 0));     /* wait till falling edge is captured */
            thisEdge = WTIMER3_TAR_R;            /* save the timestamp */
            distance = ((thisEdge - lastEdge)* 10625)/10000000;
//            return (thisEdge - lastEdge);
//            if(distance < 30 )
//                return (distance);       /* return the distance */
//            else
                return (distance);
        }
    }
}


void Timer0ACapture_init()                  // PD2, PD3 for echo and trig respectively
{
        SYSCTL_RCGCWTIMER_R  |= (1<<3);     /* enable clock to wide Timer Block 2 */
        SYSCTL_RCGCGPIO_R    |= (1<<3);      /* enable clock to PORTD */
        GPIO_PORTD_DIR_R &= ~0x04;        /* make PD2 an input pin */
        GPIO_PORTD_DEN_R |= 0x04;         /* make PD2 as digital pin */
        GPIO_PORTD_AFSEL_R |= 0x04;       /* use PD2 alternate function */
        GPIO_PORTD_PCTL_R &= ~0x00000F00;  /* configure PD0 for T0CCP0. Even block or WTimer2A */
        GPIO_PORTD_PCTL_R |= 0x00000700;
        /* PD3 as a digital output signal to provide trigger signal */
        GPIO_PORTD_DIR_R |=(1<<3)|(1<<6);         /* set PD3 as a digital output pin */
        GPIO_PORTD_DEN_R |=(1<<3)|(1<<6);         /* make PD3 as digital pin */
        WTIMER3_CTL_R  &= ~1;          /* disable Wtimer2A during setup */
        WTIMER3_CFG_R   = 4;           /* 32-bit Wtimer mode */
        WTIMER3_TAMR_R  = 0x17;        /* up-count, edge-time, capture mode */
        WTIMER3_CTL_R  |= 0x0C;        /* capture the rising edge */
        WTIMER3_CTL_R  |= (1<<0);      /* enable Wtimer2A */
}


void HEADmotor()
{
    //result = Filteredyaw; // read conversion result
    //ADC0_ISC_R = 8; // clear completion flag
    //temp_H=(((Mag_z)/15)*499);
    //PWMout_H=(int)temp_H;
    if(Mag_z>10 && Mag_z<15)
    {
        ui8Adjust_motor= 500;
    }
    else if(Mag_z<10 )
       {
        ui8Adjust_motor= 700;
       }
    else if(Mag_z>15 )
           {
        ui8Adjust_motor= 200;
           }
    //PWM0_2_CMPB_R = PWMout_H;
    UARTprintf("\n\033[25;24Hheadmotor PWM:%d\n",ui8Adjust_motor );
    UARTprintf("\n\033[26;25Hheadmotor PWM:%d",ui32Load_motor );
}

void SERVO_control()
{
    if (Filteredpitch>0.0)
    {
//    time1= (Filteredpitch) * 5.5;
//        for(i=0; i<50; i++)
//          {
//            /* Given 10us trigger pulse */
//            GPIO_PORTA_DATA_R |= (1<<4); /* make control  pin high */
//            Delay_MicroSecond(time1); /*2.0ms seconds delay */
//            GPIO_PORTA_DATA_R &= ~(1<<4); /* make trigger  pin high */
//            Delay_MicroSecond(20000-time1); /*18.0ms seconds delay */
//              }
        ui8Adjust=100;
        if (ui8Adjust>120)
        {
            ui8Adjust=120;
        }
    }
    else if (Filteredpitch<0.0)
    {
        ui8Adjust= 70;
                if (ui8Adjust<60)
                {
                    ui8Adjust=60;
                }
    }
//    else if (Filteredpitch<0.0)
//       {
//       //servo_delay1= (Filteredpitch) * 5.5;
//       //servo_delay= int(servo_delay1);
//
//       Delay_MicroSecond(10000);
//       GPIO_PORTA_DATA_R |= (1<<4);
//       Delay_MicroSecond(20000);
//       GPIO_PORTA_DATA_R &= ~(1<<4);
//       Delay_MicroSecond(17000);
//
//       }
}

int
main(void)
{   disable_irq();

// Necessary definitions to write data into Port F
    SYSCTL_RCGC2_R |=   0x00000020;   /* enable clock to GPIOF at clock gating control register */
    GPIO_PORTF_DIR_R =  0x0E;        /* enable the GPIO pins for the LED (PF3, 2 1) as output */
    GPIO_PORTF_LOCK_R = 0x4C4F434B;   /* unlock commit register GPIOCR */
    GPIO_PORTF_CR_R =   0x01;         /* make PORTF0 configurable */
    GPIO_PORTF_DEN_R =  0x1F;        // Enabling all the 5 pins of port 5 for digital activity
    GPIO_PORTF_PUR_R =  0x11;        /* enable pull up for pin 4 (SW1) and pin 0 (SW2). If this is not present, I think SW1 and SW2 are always considered to be pressed*///

    //
    // Initialize convenience pointers that clean up and clarify the code
    // meaning. We want all the data in a single contiguous array so that
    // we can make our pretty printing easier later.
    //
    pfAccel = pfData;
    pfGyro = pfData + 3;
    pfMag = pfData + 6;
    pfEulers = pfData + 9;
    pfQuaternion = pfData + 12;



    ConfigureUART();
    MPU9150_IN_IT();
    Timer0ACapture_init();  /*initialize Timer0A in edge edge time */
    configure_systick();
    SERVO_PWM_init();

    ADC_PWM_init();
    Motor_in_it();
    enable_irq();
   // PortAB_INIT();                  /* Initialize Port A and Port B */
    strcpy(version,"FLIGHT DESIGN");    /* Initialization of the contents to be displayed on LCD */
   // LCD_init();
   // LCD_PutData(version,32);

    while(1)
    {
        //UARTprintf("1\n");
       // ultrasonic();
        MPU9150();
        mode();
        tailmotor();
        HEADmotor();
        SERVO_control();
    }

}

void Motor_in_it()
{
    SysCtlPWMClockSet(SYSCTL_PWMDIV_64); // CLOCK_CPU/64

    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0); // PWM Module 0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE); // PE5
    delayMs(10);

    GPIOPinTypePWM(GPIO_PORTE_BASE, GPIO_PIN_5);
    GPIOPinConfigure(GPIO_PE5_M0PWM5); //M0 PWM5

    ui32PWMClock = SysCtlClockGet() / 64;
    ui32Load_motor = (ui32PWMClock / PWM_FREQUENCY_MOTOR) - 1;
    PWMGenConfigure(PWM0_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN);
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_2, ui32Load_motor);


    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_5, ui8Adjust_motor * ui32Load_motor / 1000);
    PWMOutputState(PWM0_BASE, PWM_OUT_5_BIT, true);
    PWMGenEnable(PWM0_BASE, PWM_GEN_2);


    //direction of motor 2 - PC6 and PC7 (main motor)
    SYSCTL_RCGCGPIO_R |= 0x04;
    GPIO_PORTC_DIR_R |= (1<<7)|(1<<6);
    GPIO_PORTC_DEN_R |= (1<<7)|(1<<6);
    GPIO_PORTC_DATA_R |= (1<<7);
    GPIO_PORTC_DATA_R &= ~(1<<6);
}

void ADC_PWM_init()
{
    /* Enable Peripheral Clocks */
     SYSCTL_RCGCPWM_R |= 3; /* enable clock to PWM Module 1 and Module 0 */
     SYSCTL_RCGCGPIO_R |= 0x10; /* enable clock to PORTE */
     SYSCTL_RCC_R |= 0x00100000; /* pre-divide by 64 */
 //    SYSCTL_RCC_R &= ~0x00100000; /* pre-divide by 64 */
     SYSCTL_RCC_R |= 0x000E0000;
     /* Enable port PE4 for PWM1 M1PWM2, PE5 for M0PWM5 */
     GPIO_PORTE_AFSEL_R = 0x30; /* PE4 and PE5 uses alternate function */
     GPIO_PORTE_PCTL_R &= ~0x000F0000; /* make PE4 and PE5 PWM output pin */
     GPIO_PORTE_PCTL_R |= 0x00050000;
     GPIO_PORTE_DEN_R |= 0x30; /* pin digital */

     delayMs(10);

     //PWM Module 1 Generator 1
     PWM1_1_CTL_R &= ~(1<<0); /* stop counter */
     PWM1_1_CTL_R &= ~(1<<1);
     PWM1_1_GENA_R = 0x0000008C; /* M1PWM2 output set when reload, */
     /* clear when match PWMCMPA */
     PWM1_1_LOAD_R = 5000; /* set load value for 1kHz (16MHz/16000) */
     PWM1_1_CMPA_R = 4999; /* set duty cycle to min */
     PWM1_1_CTL_R = 1; /* start timer */
     PWM1_ENABLE_R = 0x04; /* start PWM1 ch2 */

     delayMs(10);
//     //PWM Module 0 Generator 2
//     PWM0_2_CTL_R &= ~(1<<0); /* stop counter */
//     PWM0_2_CTL_R &= ~(1<<1);
//     PWM0_2_GENB_R = 0x0000008C; /* M1PWM2 output set when reload, */
//     /* clear when match PWMCMPA */
//     PWM0_2_LOAD_R = 5000; /* set load value for 1kHz (16MHz/16000) */
//     PWM0_2_CMPB_R = 4999; /* set duty cycle to min */
//     PWM0_2_CTL_R = 1; /* start timer */
//     PWM0_ENABLE_R = 0x20; /* start PWM0 ch5 */


 //    //direction of motor 1 - PA2 and PA3
 //    SYSCTL_RCGCGPIO_R |= 0x01;
 //    GPIO_PORTA_DIR_R |= (1<<3)|(1<<2);
 //    GPIO_PORTA_DEN_R |= (1<<3)|(1<<2);
 //    GPIO_PORTA_DATA_R |= (1<<3);
 //    GPIO_PORTA_DATA_R &= ~(1<<2);

     //direction of motor 1 - PE2 and PE3
     SYSCTL_RCGCGPIO_R |= 0x10;
     GPIO_PORTE_DIR_R |= (1<<2)|(1<<3);
     GPIO_PORTE_DEN_R |= (1<<2)|(1<<3);
     GPIO_PORTE_DATA_R |= (1<<2);
     GPIO_PORTE_DATA_R &= ~(1<<3);

//     //direction of motor 2 - PC6 and PC7
//     SYSCTL_RCGCGPIO_R |= 0x04;
//     GPIO_PORTC_DIR_R |= (1<<7)|(1<<6);
//     GPIO_PORTC_DEN_R |= (1<<7)|(1<<6);
//     GPIO_PORTC_DATA_R |= (1<<7);
//     GPIO_PORTC_DATA_R &= ~(1<<6);
}

void SERVO_PWM_init()
{
    ////**************** For Servo Motor*******************************////



//    /* Enable Peripheral Clocks */
//    SYSCTL_RCGCPWM_R |= 2; /* enable clock to PWM1 */
//    SYSCTL_RCGCGPIO_R |= 0x20; /* enable clock to PORTF */
//    //SYSCTL_RCC_R &= ~0x00100000; /* no pre-divide for PWM clock */
//    //SYSCTL_RCC_R |= 0x000E0000;
//    /* Enable port PF2 for PWM1 M1PWM7 */
//    GPIO_PORTF_AFSEL_R = 6; /* PF2 uses alternate function */
//    GPIO_PORTF_PCTL_R &= ~0x00000F00; /* make PF2 PWM output pin */
//    GPIO_PORTF_PCTL_R |= 0x00000500;
//    GPIO_PORTF_DEN_R |= 6; /* pin digital */
//    // Set the pin as an output
//    GPIO_PORTA_DIR_R |= 0x10;  // Set PB0 as an output
//
//        // Enable the pin
//    GPIO_PORTA_DEN_R |= 0x10;  // Enable digital function for PB0
//
//    PWM1_3_CTL_R &= ~(1<<0); /* stop counter */
//    PWM1_3_CTL_R &= ~(1<<1);
//    PWM1_3_GENA_R = 0x0000008C; /* M1PWM7 output set when reload, */
//    /* clear when match PWMCMPA */
//    GPIO_PORTA_DR2R_R |= 0x10;  // Set PA4 to 2mA drive strength
//    PWM1_3_LOAD_R |= 5000; /* set load value for 1kHz (16MHz/16000) */
//    PWM1_3_CMPA_R |= 4999; /* set duty cycle to min */
//    PWM1_3_CTL_R |= 1; /* start timer */
//    PWM1_ENABLE_R |= 0x40; /* start PWM1 ch7 */
//    //
//    ////////////////////////////////////////////////////////////////////

    SysCtlPWMClockSet(SYSCTL_PWMDIV_64); // CLOCK_CPU/64

        SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0); // PWM Module 0
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC); // PC4
        delayMs(10);

        GPIOPinTypePWM(GPIO_PORTC_BASE, GPIO_PIN_4);
        GPIOPinConfigure(GPIO_PC4_M0PWM6); //M0 PWM6

        ui32PWMClock = SysCtlClockGet() / 64;
        ui32Load = (ui32PWMClock / PWM_FREQUENCY) - 1;
        PWMGenConfigure(PWM0_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN);
        PWMGenPeriodSet(PWM0_BASE, PWM_GEN_3, ui32Load);

        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_6, ui8Adjust * ui32Load / 1000);
        PWMOutputState(PWM0_BASE, PWM_OUT_6_BIT, true);
        PWMGenEnable(PWM0_BASE, PWM_GEN_3);
}




static inline void enable_irq()
{
    __asm ("    CPSIE  I\n");
}

static inline void disable_irq()
{
    __asm ("    CPSID  I\n");
}

void delayMs(int n)

{

int i, j;

for(i = 0 ; i < n; i++)

for(j = 0; j < 1000; j++) {}    /* do nothing for 1 ms */

}
void SysTick_Handler(void)  //100 ms delay
{
    //if required
}

void Delay_MicroSecond(int time)
{
    int i;
    SYSCTL_RCGCTIMER_R  |= 0x00000002;     /* enable clock to Timer Block 1 */
    TIMER1_CTL_R = 0;            /* disable Timer before initialization */
    TIMER1_CFG_R = 0x04;         /* 16-bit option */
    TIMER1_TAMR_R = 0x02;        /* periodic mode and down-counter */
    TIMER1_TAILR_R = 16 - 1;  /* TimerA interval load value reg */
    TIMER1_ICR_R = 0x1;          /* clear the TimerA timeout flag */
    TIMER1_CTL_R |= 0x01;        /* enable Timer A after initialization */
    for(i = 0; i < time; i++)
    {
        while ((TIMER1_RIS_R & 0x1) == 0); /*wait for TimerA timeout flag */

    //    GPIO_PORTF_DATA_R |= 0x0E;

        TIMER1_ICR_R = 0x1;   /* clear the TimerA timeout flag */
    }
}


