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
int count_key1 = 0;     // to keep track of the number of times SW1 was pressed
int read_sw1()
{
    int x ;
    // read x pin for sufficient time to record the change
    for(int i=0;i<20;i++) x = GPIO_PORTF_DATA_R & 0x10 ;
    if(x==0) count_key1 += 1;
    if(count_key1 == 8) count_key1 = 1;  // once all the 7 colors are done, start from green
    return count_key1;
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
           // increasing the read time so that the press gets recorded
           int n = read_sw1();
           switch(n)
           {
              case 1: GPIO_PORTF_DATA_R = COLOR_GREEN_ON;
                      delayMs(1000);
                      GPIO_PORTF_DATA_R = NO_COLOR;
                      delayMs(1000);
                      break;

              case 2: GPIO_PORTF_DATA_R = COLOR_BLUE_ON;
                      delayMs(1000);
                      GPIO_PORTF_DATA_R = NO_COLOR;
                      delayMs(1000);
                      break;

              case 3: GPIO_PORTF_DATA_R = COLOR_CYAN_ON;
                      delayMs(1000);
                      GPIO_PORTF_DATA_R = NO_COLOR;
                      delayMs(1000);
                      break;

              case 4: GPIO_PORTF_DATA_R = COLOR_RED_ON;
                      delayMs(1000);
                      GPIO_PORTF_DATA_R = NO_COLOR;
                      delayMs(1000);
                      break;

              case 5: GPIO_PORTF_DATA_R = COLOR_YELLOW_ON;
                      delayMs(1000);
                      GPIO_PORTF_DATA_R = NO_COLOR;
                      delayMs(1000);
                      break;

              case 6: GPIO_PORTF_DATA_R = COLOR_MAGENTA_ON;
                      delayMs(1000);
                      GPIO_PORTF_DATA_R = NO_COLOR;
                      delayMs(1000);
                      break;

              case 7: GPIO_PORTF_DATA_R = COLOR_WHITE_ON;
                      delayMs(1000);
                      GPIO_PORTF_DATA_R = NO_COLOR;
                      delayMs(1000);
                      break;

              default: GPIO_PORTF_DATA_R = COLOR_WHITE_ON;
                      delayMs(1000);
                      GPIO_PORTF_DATA_R = NO_COLOR;
                      delayMs(1000);
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
    for(j = 0; j < 3180; j++) {}    /* do nothing for 1 ms */
}
