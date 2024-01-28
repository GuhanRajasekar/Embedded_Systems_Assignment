#include <stdint.h>
#include <stdbool.h>
#include <./inc/tm4c123gh6pm.h>
#include <inc/hw_memmap.h>
#include <inc/hw_types.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/sysctl.h>
#include <driverlib/uart.h>

char console_cmd_buffer[30];  // global character array to store the echo the contents given by the user
char* Color;                  // global string to store the Color
int Blink_Rate;               // global integer to store the blink rate

int main()
{
    char val;
    int i = 0; // variable used for indexing purposes
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));


    while(1)
    {
        while(!(UARTCharsAvail(UART0_BASE)));
            val = UARTCharGet(UART0_BASE);

            // Check for Enter Character
            // If the received character is 0x0D => Enter key has been pressed
           if( (val == 0x0D))
           {
                 // Enter key has been pressed
                // process the data here to take necessary action based on the contents of the console_cmd_buffer array
                // Based on the contents of the array, either change the color or change the blink speed
           }


            // Support Back Space functionality
            else if(val == 0x08 && i>0)
              {
                  UARTCharPut(UART0_BASE, '\b');   //Move the cursor back by one position to the left
                  UARTCharPut(UART0_BASE, ' ');    // Place a ' ' at the place where the character needs to be removed.
                                                   // Once ' ' is written, cursor will once again move by one position to the right. We need to move it back again.
                  UARTCharPut(UART0_BASE, '\b');   // Move the cursor back again.
                  i = i-1;                         // Decrement the index as a character has been removed
                  console_cmd_buffer[i] = '\0';    // Terminate the character array at the appropriate position
              }

            // Process normal characters other than 'Enter' and 'BackSpace'
            else
            {
                //UARTCharPut(UART0_BASE, val);  // Echo from the TIVA board back to the PC
                if(i<=29) console_cmd_buffer[i] = val;
                UARTCharPut(UART0_BASE, console_cmd_buffer[i]);
                i = i+1;
                if(i>=0 && i<=29)  console_cmd_buffer[i] = '\0'; // Terminate the buffer (character array) at a valid position
            }


    }

    return 0;
}
