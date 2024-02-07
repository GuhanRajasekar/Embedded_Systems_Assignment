/*
 * Recoater & Roller BLDC
 * Sarvjit Ajit Patil 20862
 * Date : 10/11/2023
 */



/*
 * main.c
 */
#include "inc/tm4c123gh6pm.h"

#include <stdio.h>

#include <stdint.h>

#include <stdlib.h>

#ifndef UART_H_
#include"UART.h"
#endif

#ifndef GPIO_H_
#include"GPIO.h"
#endif

#ifndef KEYPAD_H_
#include"keypad.h"
#endif

#ifndef SSD_H_
#include"SSD.h"
#endif

char str[20] ;

int data = 0;

void delayMs( int n );


int main(void)
{
    uart0_init();

    uart0_send_str("Welcome to UART0\n");

    keypad_init();

    SSD_init();

    while(1)
    {
        data = 0;

        if(isKeyPressed())
        {
            delayMs(5);

            if(isKeyPressed())
            {
                data = readkey();

                sprintf(str,"Data: %d\n", data);

                uart0_send_str(str);

                SSD_Display(1,data);

    //            while (readKey() == data) ;

                while(isKeyPressed());
            }
        }
    }
}

void delayMs( int n )
{
    int i, j ;

    for(i = 0; i < n ; i ++)
    {
        for(j = 0; j < 3180; j++);
    }
}
