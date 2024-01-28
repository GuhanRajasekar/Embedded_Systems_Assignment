#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <./inc/tm4c123gh6pm.h>
#include <inc/hw_memmap.h>
#include <inc/hw_types.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/sysctl.h>
#include <driverlib/uart.h>

// Macros for the various colors
#define COLOR_GREEN_ON     0x08
#define COLOR_BLUE_ON      0x04
#define COLOR_RED_ON       0x02
#define COLOR_CYAN_ON      0x0C  // B + G
#define COLOR_YELLOW_ON    0x0A  // R + G
#define COLOR_MAGENTA_ON   0x06  // B + R
#define COLOR_WHITE_ON     0x0E  // B + R + G
#define NO_COLOR           0x00  // No color

void delayMs(int n);          // Software function to implement delay

char console_cmd_buffer[30];  // global character array to store the echo the contents given by the user
char val;                     // global character variable that is used across functions to read data from the console
int id = 0;                // global variable to take care of indexing of the character buffer array
int Color = -1;               // global variable to denote which color is currently active
int Blink_Delay = 1000;        // global integer to store the blink rate

int findColor()               // Function to determine which color needs to be lit
{
   if(strstr(console_cmd_buffer ,"Color Green"))   return 1; //Green Color
   if(strstr(console_cmd_buffer ,"Color Blue"))    return 2; //Blue Color
   if(strstr(console_cmd_buffer ,"Color Cyan"))    return 3; //Cyan Color
   if(strstr(console_cmd_buffer ,"Color Red"))     return 4; //Red Color
   if(strstr(console_cmd_buffer ,"Color Yellow"))  return 5; //Yellow Color
   if(strstr(console_cmd_buffer ,"Color Magenta")) return 6; //Magenta Color
   if(strstr(console_cmd_buffer ,"Color White"))   return 7; //White Color
   return -1;                            //Invalid Data Entered
}

int findBlinkRate()
{
    if     (strstr(console_cmd_buffer ,"Blink 16"))  return 61.25;
    if(strstr(console_cmd_buffer ,"Blink 2"))        return 500;
    if(strstr(console_cmd_buffer ,"Blink 4"))        return 250;
    if(strstr(console_cmd_buffer ,"Blink 8"))        return 125;
    if(strstr(console_cmd_buffer ,"Blink 1"))        return 1000;// Placing this here otherwise Blink 16 is not being detected as "Blink 1" is a substring of "Blink 16"
    if(strstr(console_cmd_buffer ,"Blink 32"))       return 31.25;
    return 1000;//1000; // Default value
}
void processColors()
{
    switch(Color)
       {
          case 1: GPIO_PORTF_DATA_R = COLOR_GREEN_ON;
                  delayMs(Blink_Delay);
                  GPIO_PORTF_DATA_R = NO_COLOR;
                  delayMs(Blink_Delay);
                  break;

          case 2: GPIO_PORTF_DATA_R = COLOR_BLUE_ON;
                  delayMs(Blink_Delay);
                  GPIO_PORTF_DATA_R = NO_COLOR;
                  delayMs(Blink_Delay);
                  break;

          case 3: GPIO_PORTF_DATA_R = COLOR_CYAN_ON;
                  delayMs(Blink_Delay);
                  GPIO_PORTF_DATA_R = NO_COLOR;
                  delayMs(Blink_Delay);
                  break;

          case 4: GPIO_PORTF_DATA_R = COLOR_RED_ON;
                  delayMs(Blink_Delay);
                  GPIO_PORTF_DATA_R = NO_COLOR;
                  delayMs(Blink_Delay);
                  break;

          case 5: GPIO_PORTF_DATA_R = COLOR_YELLOW_ON;
                  delayMs(Blink_Delay);
                  GPIO_PORTF_DATA_R = NO_COLOR;
                  delayMs(Blink_Delay);
                  break;

          case 6: GPIO_PORTF_DATA_R = COLOR_MAGENTA_ON;
                  delayMs(Blink_Delay);
                  GPIO_PORTF_DATA_R = NO_COLOR;
                  delayMs(Blink_Delay);
                  break;

          case 7: GPIO_PORTF_DATA_R = COLOR_WHITE_ON;
                  delayMs(Blink_Delay);
                  GPIO_PORTF_DATA_R = NO_COLOR;
                  delayMs(Blink_Delay);
                  break;

         default: GPIO_PORTF_DATA_R = NO_COLOR;   // for debugging purposes
                  break;

     }
    return;
}

// Function to handle Enter Key presses
void processEnterKey()
{
    // Enter key has been pressed. Process the contents of the data entered
    if(strstr(console_cmd_buffer , "Color") || strstr(console_cmd_buffer , "Blink"))
    {
         if(strstr(console_cmd_buffer , "Color"))        Color      = findColor();                // Find out which color is requested
         else if(strstr(console_cmd_buffer , "Blink"))   Blink_Delay = findBlinkRate();            // Find out the blink rate
    }
}

// Function to handle BackSpace Key presses
void processBackSpaceKey()
{
    UARTCharPut(UART0_BASE, '\b');   //Move the cursor back by one position to the left
    UARTCharPut(UART0_BASE, ' ');    // Place a ' ' at the place where the character needs to be removed.
                                     // Once ' ' is written, cursor will once again move by one position to the right. We need to move it back again.
    UARTCharPut(UART0_BASE, '\b');   // Move the cursor back again.
    id = id-1;                         // Decrement the index as a character has been removed
    console_cmd_buffer[id] = '\0';    // Terminate the character array at the appropriate position
}

// Function to handle key press that is neither "Enter" nor "Back Space"
void processNormalKey()
{
    //UARTCharPut(UART0_BASE, val);  // Echo from the TIVA board back to the PC
    if(id<=29) console_cmd_buffer[id] = val;
    UARTCharPut(UART0_BASE, console_cmd_buffer[id]);
    id = id +1;
    if(id>=0 && id<=29)  console_cmd_buffer[id] = '\0'; // Terminate the array at a valid position
}
int main()
{
    // Necessary definitions to write data into Port F

    SYSCTL_RCGC2_R |=   0x00000020;   /* enable clock to GPIOF at clock gating control register */
    GPIO_PORTF_DIR_R =  0x0E;        /* enable the GPIO pins for the LED (PF3, 2 1) as output */
    GPIO_PORTF_LOCK_R = 0x4C4F434B;   /* unlock commit register GPIOCR */
    GPIO_PORTF_CR_R =   0x01;         /* make PORTF0 configurable */
    GPIO_PORTF_DEN_R =  0x1F;        // Enabling all the 5 pins of port 5 for digital activity
    GPIO_PORTF_PUR_R =  0x11;        /* enable pull up for pin 4 (SW1) and pin 0 (SW2). If this is not present, I think SW1 and SW2 are always considered to be pressed*///

    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    while(1)
    {
        while(!(UARTCharsAvail(UART0_BASE)))
          {
            processColors();  // Maintain LED Blink state even when there is no new command
          }
        val = UARTCharGet(UART0_BASE);
        if((val == 0x0D))                 processEnterKey();         // If entered character is 0x0D => Enter key has been pressed
        else if(val == 0x08 && id>0)      processBackSpaceKey();     // If entered character is 0x08 => Back Space Key has been pressed
        else                              processNormalKey();        // To process Key press that is Neither "Enter" nor "Back Space"
        processColors();                                             // Maintain LED blink state even in the middle of a new command
    }

  return 0;
}

/* delay n milliseconds (16 MHz CPU clock) */
void delayMs(int n)
{
   int i, j;
   for(i = 0 ; i < n; i++)
     {
        for(j = 0; j < 3180; j++)
            {
                /* do nothing for 1 ms */
                // check for key press periodically
            }
        // After every 1ms check the input entered by the user

       if(UARTCharsAvail(UART0_BASE))
       {
           val = UARTCharGet(UART0_BASE);
           if((val == 0x0D))                 processEnterKey();         // If entered character is 0x0D => Enter key has been pressed
           else if(val == 0x08 && id>0)      processBackSpaceKey();     // If entered character is 0x08 => Back Space Key has been pressed
           else                              processNormalKey();        // To process Key press that is Neither "Enter" nor "Back Space"
       }
      }
   return;  // sw1 not pressed during the entire duration of the delay
}
