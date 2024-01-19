#include <stdint.h>
#include "inc/tm4c123gh6pm.h"
// Green, Blue, Cyan, Red, Yellow, Magenta, White
#define COLOR_GREEN_ON     0x08
#define COLOR_BLUE_ON      0x04
#define COLOR_RED_ON       0x02
#define COLOR_CYAN_ON      0x0C  // B + G
#define COLOR_YELLOW_ON    0x0A  // R + G
#define COLOR_MAGENTA_ON   0x06  // B + R
#define COLOR_WHITE_ON     0x0E  // B + R + G
#define NO_COLOR           0x00  // No color

void delayMs(int n);
int sw1_pressed = 0;   // flag to indicate if sw1 has been pressed
int sw2_pressed = 0;   // flag to indicate if sw2 has been pressed
int count_sw1 = 1;     // to keep track of the number of times SW1 was pressed
int count_sw2 = 1;     // to keep track of the number of times SW2 was pressed
float on_off_delay = 1000;
void read_sw1()
{
    int current_state_sw1 = GPIO_PORTF_DATA_R & 0x10;
    sw1_pressed = 0;
    if((current_state_sw1) == 0)
    {
        for(int i = 0 ; i<100; i++)
        {
            for(int j = 0; j < 1200; j++)
            {
                // check for key press periodically
            }
        }

        current_state_sw1 = GPIO_PORTF_DATA_R & 0x10;
        if(current_state_sw1 == 0)
        {
            sw1_pressed = 1;
            count_sw1 = count_sw1 + 1;
            if(count_sw1 == 8) count_sw1 = 1;
        }
    }
    return;
}
int main(void)
{
SYSCTL_RCGC2_R |= 0x00000020;   /* enable clock to GPIOF at clock gating control register */
GPIO_PORTF_DIR_R = 0x0E;    /* enable the GPIO pins for the LED (PF3, 2 1) as output */
GPIO_PORTF_DEN_R = 0x1F;    // Enabling all the 5 pins of port 5 for digital activity
GPIO_PORTF_PUR_R = 0x11; /* enable pull up for pin 4 (SW1) and pin 0 (SW2). If this is not present, I think SW1 and SW2 are always considered to be pressed*///
int flag = 0;           // to indicate if reset was pressed
while(1)
{
    int x = GPIO_PORTF_DATA_R & 0x10 ; // keep reading data from PF4 ( SW1 )
    if(x==0 && flag!=0)
    {
       for(;;)
       {
           switch(count_sw1)
           {
              case 1: GPIO_PORTF_DATA_R = COLOR_GREEN_ON;
                      delayMs(on_off_delay);
                      if(on_off_delay != 31.25) GPIO_PORTF_DATA_R = NO_COLOR;   // to make sure that LED is completely ON when MAX speed is reached (16 times in 1s)
                      delayMs(on_off_delay);
                      break;

              case 2: GPIO_PORTF_DATA_R = COLOR_BLUE_ON;
                      delayMs(on_off_delay);
                      if(on_off_delay != 31.25) GPIO_PORTF_DATA_R = NO_COLOR;   // to make sure that LED is completely ON when MAX speed is reached (16 times in 1s)
                      delayMs(on_off_delay);
                      break;

              case 3: GPIO_PORTF_DATA_R = COLOR_CYAN_ON;
                      delayMs(on_off_delay);
                      if(on_off_delay != 31.25) GPIO_PORTF_DATA_R = NO_COLOR;   // to make sure that LED is completely ON when MAX speed is reached (16 times in 1s)
                      delayMs(on_off_delay);
                      break;

              case 4: GPIO_PORTF_DATA_R = COLOR_RED_ON;
                      delayMs(on_off_delay);
                      if(on_off_delay != 31.25) GPIO_PORTF_DATA_R = NO_COLOR;   // to make sure that LED is completely ON when MAX speed is reached (16 times in 1s)
                      delayMs(on_off_delay);
                      break;

              case 5: GPIO_PORTF_DATA_R = COLOR_YELLOW_ON;
                      delayMs(on_off_delay);
                      if(on_off_delay != 31.25) GPIO_PORTF_DATA_R = NO_COLOR;   // to make sure that LED is completely ON when MAX speed is reached (16 times in 1s)
                      delayMs(on_off_delay);
                      break;

              case 6: GPIO_PORTF_DATA_R = COLOR_MAGENTA_ON;
                      delayMs(on_off_delay);
                      if(on_off_delay != 31.25) GPIO_PORTF_DATA_R = NO_COLOR;   // to make sure that LED is completely ON when MAX speed is reached (16 times in 1s)
                      delayMs(on_off_delay);
                      break;

              case 7: GPIO_PORTF_DATA_R = COLOR_WHITE_ON;
                      delayMs(on_off_delay);
                      if(on_off_delay != 31.25) GPIO_PORTF_DATA_R = NO_COLOR;   // to make sure that LED is completely ON when MAX speed is reached (16 times in 1s)
                      delayMs(on_off_delay);
                      break;

              default: GPIO_PORTF_DATA_R = NO_COLOR;   // for debug purposes
                       break;
           }
       }
    }
    flag = 1; // to make sure that the lights are not activated the first time when the reset button is pressed
}
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
        // for every 1ms keep checking if key1 has been pressed

        read_sw1();
        if(sw1_pressed == 1) return;

    }
    return;  // sw1 not pressed during the entire duration of the delay
}
