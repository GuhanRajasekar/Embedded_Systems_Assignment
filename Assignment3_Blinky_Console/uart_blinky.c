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
#include <ctype.h>

// Macros for the various colors
#define COLOR_GREEN_ON     0x08
#define COLOR_BLUE_ON      0x04
#define COLOR_RED_ON       0x02
#define COLOR_CYAN_ON      0x0C  // B + G
#define COLOR_YELLOW_ON    0x0A  // R + G
#define COLOR_MAGENTA_ON   0x06  // B + R
#define COLOR_WHITE_ON     0x0E  // B + R + G
#define NO_COLOR           0x00  // No color

void delayMs(int n);           // Software function to implement delay
void processInvalidCommand();  // Forward Declaration of the function that gives direction to the suer when an invalid command has been entered
void read_sw1();               // Function to handle sw1 press

char console_cmd_buffer[30];   // global character array to store the echo the contents given by the user
char val;                      // global character variable that is used across functions to read data from the console
int id = 0;                    // global variable to take care of indexing of the character buffer array
int Color = 0;                 // global variable to denote which color is currently active ( 0 indicates no color )
int Blink_Delay = 1000;        // global integer to store the blink rate
int sw1_pressed = 0;           // flag to indicate if sw1 has been pressed

int findColor()               // Function to determine which color needs to be lit
{
   if(strcmp(console_cmd_buffer ,"color green")== 0)    return 1; //Green Color
   if(strcmp(console_cmd_buffer ,"color blue") == 0)    return 2; //Blue Color
   if(strcmp(console_cmd_buffer ,"color cyan") == 0)    return 3; //Cyan Color
   if(strcmp(console_cmd_buffer ,"color red") == 0)     return 4; //Red Color
   if(strcmp(console_cmd_buffer ,"color yellow") == 0)  return 5; //Yellow Color
   if(strcmp(console_cmd_buffer ,"color magenta") == 0) return 6; //Magenta Color
   if(strcmp(console_cmd_buffer ,"color white") == 0)   return 7; //White Color
   processInvalidCommand();                                       // If none of the entered data is valid, give appropriate prompt to the user
   return Color;                                                  //Invalid Data Entered (Simply return the current state of Color => Do not modify it)
}

int findBlinkRate()
{
    if(strcmp(console_cmd_buffer ,"blink 1") == 0)   return 1000;
    if(strcmp(console_cmd_buffer ,"blink 2")  == 0)  return 500;
    if(strcmp(console_cmd_buffer ,"blink 4")  == 0)  return 250;
    if(strcmp(console_cmd_buffer ,"blink 8")  == 0)  return 125;
    if(strcmp(console_cmd_buffer ,"blink 16")  == 0) return 61.25;
    if(strcmp(console_cmd_buffer ,"blink 32") == 0)  return 31.25;
    processInvalidCommand();  // If none of the entered data is valid, give appropriate prompt to the user
    return Blink_Delay;       // Invalid Blink Command entered by the user => Retain the previous value of the blink rate
}

// Function to take care of sw1 press
void read_sw1()
{
    sw1_pressed = 0;
    int current_state_sw1 = GPIO_PORTF_DATA_R & 0x10;      // read the status of sw1
    if((current_state_sw1) == 0x00)
    {
        while(current_state_sw1 == 0x00)
        {
            current_state_sw1 = GPIO_PORTF_DATA_R & 0x10;  // read the data from sw1 again
            if(current_state_sw1 == 0x10)
            {
                sw1_pressed = 1;                           // consider sw1 to be pressed
                Color = Color + 1;
                if(Color == 8) Color = 1;
                break;
            }
        }
    }
    return;
}

// Function to handle which Color must be blinking and at what rate the blink must be happening
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

// Function to empty the Character Buffer Array
void emptyBuffer()
{
    console_cmd_buffer[0] = '\0';  // Setting the first character as a null character => Emptying the contents of the character buffer array
    id = 0;                        // Setting the index of the character array as 0 as we need to start from the beginning
}
// Function to handle Enter Key presses
void processEnterKey()
{
    // Enter key has been pressed. Process the contents of the data entered
    if(strstr(console_cmd_buffer , "color") || strstr(console_cmd_buffer , "blink"))
    {
        // strstr() searches if the specified string is a subset of the contents of the character buffer array console_cmd_buffer
         if(strstr(console_cmd_buffer , "color"))        Color       = findColor();                // Find out which color is requested
         else if(strstr(console_cmd_buffer , "blink"))   Blink_Delay = findBlinkRate();            // Find out the blink rate
    }
    else processInvalidCommand(); // Command with neither Color nor Blink has been entered => Give appropriate prompt to user to enter valid command

    /* Once Enter Key is pressed and Color and Blink Rate have been processed, empty the contents of the character array
       to receive further instructions from the user */
    emptyBuffer();
    UARTCharPut(UART0_BASE, '\n'); // Once Enter key is pressed and the character buffer has been emptied, move on to the next line
    UARTCharPut(UART0_BASE, '\r'); // After moving to the new line, also move to the extreme left end
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
    /*
       We DO NOT perform the lower case conversion here as the characters entered by
       the users need to be printed without any modification
     */
    if(id<=29) console_cmd_buffer[id] = (val);
    UARTCharPut(UART0_BASE, console_cmd_buffer[id]); // This makes the characters appear on the console (This is the Echo Operation)
    console_cmd_buffer[id] = tolower(val);           // Convert everything to lower cases characters and store in the buffer array
    id = id +1;
    if(id>=0 && id<=29)  console_cmd_buffer[id] = '\0'; // Terminate the array at a valid position
}


// Functions to give directions to the user when an Invalid Command has been entered
void processInvalidCommand()
{
    char* s  = "\n\n\rPlease enter any of the following commands:\n\r1).Color <Color_Name>\n\r2).Blink <Blink_Rate>\n\r";
    int i = 0; // for indexing purposes
    while(s[i] != '\0')
    {
        UARTCharPut(UART0_BASE, s[i]);
        i = i+1;
    }
    return;
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
            read_sw1();       // Check for sw1 press even when the user has not given any new command
            processColors();  // Maintain LED Blink state even when there is no new command
          }
        val = UARTCharGet(UART0_BASE);
        if((val == 0x0D))                 processEnterKey();         // If entered character is 0x0D => Enter key has been pressed
        else if(val == 0x08 && id>0)      processBackSpaceKey();     // If entered character is 0x08 => Back Space Key has been pressed
        else                              processNormalKey();        // To process Key press that is Neither "Enter" nor "Back Space"
        read_sw1();                                                  // Keep checking for sw1 press
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
       read_sw1();                                                      // Check the status of sw1 periodically

      }
   return;  // sw1 not pressed during the entire duration of the delay
}
