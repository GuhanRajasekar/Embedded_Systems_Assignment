/*
 * periph_ctl.h
 *
 *  Created on: 10-Feb-2023
 *      Author: jainv
 */

#ifndef PERIPH_CTL_H_
#define PERIPH_CTL_H_

#include "inc/tm4c123gh6pm.h"
#include <stdint.h>
#include <stdbool.h>
#include <./inc/tm4c123gh6pm.h>
#include <inc/hw_memmap.h>
#include <inc/hw_types.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/sysctl.h>
#include <driverlib/uart.h>

void delayMs(int n)
{
    int i, j;
    for(i =0; i < n; i++)
        for(j =0; j <600; j++){}/* do nothing for 1 ms */
}


/*
   The Systick Timer will keep counting every clock cycle.
   It's count value is decremented by one for every clock cycle.
   Once the count value reaches 0, it will generate an interrupt, reload its value and continue counting again.
   Or it can also stop, depending on how the counter is configured.
 */
//void configure_systick()
//{
//    NVIC_ST_CTRL_R      = 0;            // Initially, Disable timer during setup (Timer stops counting and does not generate any interrupts)
//    NVIC_ST_RELOAD_R    = 1250-1;
////    NVIC_ST_RELOAD_R    = 2500-1;       // This should expire once every 0.15625 ms (T = 1/16MHz = 62.5 ns => 2500 * 62.5n = 0.15625ms)
//    /* The above command makes the timer count from 2499 to 0. Once it reaches 0 it generates an interrupt, reloads the value of
//       2499 and starts counting down again. Effectively  */
//
//
//    /*
//     NVIC_ST_CTRL_R      = 0x00000007  => This code does the following.
//     Bit 0 (ENABLE)    is set to 1, enabling the SysTick timer.
//     Bit 1 (TICKINT)   is set to 1, enabling the SysTick interrupt.
//     Bit 2 (CLKSOURCE) is set to 1, selecting the processor clock as the clock source for the SysTick timer.
//     */
//    NVIC_ST_CTRL_R      = 0x00000007;   // Clock src set to system clock and enable timer and interrupt
//}

void configure_spi()
{
    SYSCTL_RCGCSSI_R |= (1<<3); /* enable clock to SSI3 */
    SYSCTL_RCGCGPIO_R |= 8;     /* enable clock to GPIOD for SSI3 */
    SYSCTL_RCGCGPIO_R |= 0x20;  /* enable clock to GPIOF for slave select */
                                /* configure PORTD 3, 1 for SSI3 clock and Tx */
    GPIO_PORTD_AMSEL_R &= ~0x09;/* disable analog for these pins */
    GPIO_PORTD_DEN_R |= 0x09;   /* and make them digital */
    GPIO_PORTD_AFSEL_R |= 0x09; /* enable alternate function */
    GPIO_PORTD_PCTL_R &= ~0xF00F;/* assign pins to SSI3 */
    GPIO_PORTD_PCTL_R |= 0x1001;/* assign pins to SSI3 */
                                /* configure PORTF 2 for slave select */
    GPIO_PORTF_DEN_R |= 0x04;   /* make the pin digital */
    GPIO_PORTF_DIR_R |= 0x04;   /* make the pin output */
    GPIO_PORTF_DATA_R |= 0x04;  /* keep SS idle high */
                                /* SPI Master, POL = 0, PHA = 0, clock = 4 MHz, 16 bit data */
    SSI3_CR1_R = 0;             /* disable SSI and make it master */
    SSI3_CC_R = 0;              /* use system clock */
    SSI3_CPSR_R = 2;            /* prescaler divided by 2 */
    SSI3_CR0_R = 0x0007;        /* 8 MHz SSI clock, SPI mode, 8 bit data */
    SSI3_CR1_R |= 2;            /* enable SSI3 */
}

//void send_dac(unsigned int data, unsigned int command)
//{
//    int temp =  GPIO_PORTF_DATA_R ;
//    unsigned temp_data = ((data<<2)|(command));
//    GPIO_PORTF_DATA_R &= ~0x04; /* assert SS low */
//    while((SSI3_SR_R & 2) == 0);/* wait until FIFO not full */
//    SSI3_DR_R = (temp_data>>8);
//    while(SSI3_SR_R & 0x10);    /* wait until transmit complete */
//    SSI3_DR_R = temp_data;      /* transmit high byte */
//    while(SSI3_SR_R & 0x10);    /* wait until transmit complete */
//    GPIO_PORTF_DATA_R |= 0x04;
//    GPIO_PORTF_DATA_R = temp;   /* keep SS idle high */
//}

#endif /* PERIPH_CTL_H_ */
