/*
 * keypad.c
 *
 *  Created on: 02-Feb-2024
 *      Author: Sarvjit
 */

#include <stdlib.h>

#include <stdio.h>

//#include "inc/tm4c123gh6pm.h"

#ifndef KEYPAD_H_
#include"keypad.h"
#endif

#ifndef GPIO_H_
#include"GPIO.h"
#endif

#ifndef UART_H_
#include"UART.h"
#endif

int num = 0;

char str[20];

void keypad_init(void)
{
    GPIOC_INIT();

    GPIOE_INIT();

    GPIO_PORTE_DATA_R = 0x00 ;
}

int isKeyPressed(void)
{
    GPIO_PORTE_DATA_R = 0x00 ;

    if( (GPIO_PORTC_DATA_R & 0xF0) != 0xF0 )
        return 1;
    else
        return 0;
}

int readkey(void)
{
    int row = 0, col = 0;

    for(row = 0; row < 4; row ++)
    {
        GPIO_PORTE_DATA_R = ( 0x0F & ~(1 << row) );

        num = GPIO_PORTC_DATA_R & 0xF0 ;

        if( num != 0x0F)
        {
            if( num == 0xE0)
            {
                col = 1;
                return ( (row * 4) + col );
            }
            else if( num == 0xD0)
            {
                col = 2;
                return ( (row * 4) + col );
            }
            else if( num == 0xB0)
            {
                col = 3;
                return ( (row * 4) + col );
            }
            else if( num == 0x70)
            {
                col = 4;
                return ( (row * 4) + col );
            }

        }

        delay_1_Ms();
    }

    return 0;

}

void delay_1_Ms(void)
{
    int i, j ;

    for(i = 0; i < 1 ; i ++)
    {
        for(j = 0; j < 3180; j++);
    }
}
