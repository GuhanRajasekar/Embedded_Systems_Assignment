#include <stdint.h>
#include <math.h>
#include <./inc/tm4c123gh6pm.h>
void delayMs(int n);
int main(void)
{
        //int i;
        int x = 4999;
        float temp;
        int temp2;
        /* Enable Peripheral Clocks */
        SYSCTL_RCGCPWM_R |= 2; /* enable clock to PWM1 */
        SYSCTL_RCGCGPIO_R |= 0x20; /* enable clock to PORTF */
        SYSCTL_RCC_R &= ~0x00100000; /* no pre-divide for PWM clock */
        SYSCTL_RCC_R |= 0x000E0000;
        /* Enable port PF2 for PWM1 M1PWM7 */
        GPIO_PORTF_AFSEL_R = 6; /* PF2 uses alternate function */
        GPIO_PORTF_PCTL_R &= ~0x00000F00; /* make PF2 PWM output pin */
        GPIO_PORTF_PCTL_R |= 0x00000500;
        GPIO_PORTF_DEN_R |= 6; /* pin digital */


        PWM1_3_CTL_R &= ~(1<<0); /* stop counter */
        PWM1_3_CTL_R &= ~(1<<1);
        PWM1_3_GENA_R = 0x0000008C; /* M1PWM7 output set when reload, */
        /* clear when match PWMCMPA */
        PWM1_3_LOAD_R = 5000; /* set load value for 1kHz (16MHz/16000) */
        PWM1_3_CMPA_R = 4999; /* set duty cycle to min */
        PWM1_3_CTL_R = 1; /* start timer */
        PWM1_ENABLE_R = 0x40; /* start PWM1 ch7 */



        SYSCTL_RCGCGPIO_R |= 0x01;
        GPIO_PORTA_DIR_R |= (1<<3)|(1<<2);
        GPIO_PORTA_DEN_R |= (1<<3)|(1<<2);
        GPIO_PORTA_DATA_R |= (1<<3);
        GPIO_PORTA_DATA_R &= ~(1<<2);
        //GPIO_PORTA_DATA_R &= ~(1<<3);
        //GPIO_PORTA_DATA_R |= (1<<2);


        volatile int result;



        SYSCTL_RCGCADC_R |= 0x00000001; /* enable clock to ADC0 */
        SYSCTL_RCGCGPIO_R |= 0x10; /* enable clock to PE (AIN0 is on PE3) */



        ADC0_ACTSS_R |= 0x00000008; /* enable ADC0 sequencer 3 */
        ADC0_EMUX_R &= ~0xF000; /* software trigger conversion */
        ADC0_SSMUX3_R = 0; /* get input from channel 0 */
        /* initialize PE3 for AIN0 input */
        GPIO_PORTE_AFSEL_R |= 8; /* enable alternate function */
        GPIO_PORTE_DEN_R &= ~8; /* disable digital function */
        GPIO_PORTE_AMSEL_R |= 8; /* enable analog function */
        ADC0_SSCTL3_R |= 6; /* take one sample at a time, set flag at 1st sample */
        ADC0_ACTSS_R |= 8; /* enable ADC0 sequencer 3 */
        while(1) {
                ADC0_PSSI_R |= 8; /* start a conversion sequence 3 */
                while((ADC0_RIS_R & 8) == 0)
                ; /* wait for conversion complete */
                result = ADC0_SSFIFO3_R; /* read conversion result */
                ADC0_ISC_R = 8; /* clear completion flag */
                temp=(((result)/4095.0)*4999.0);
                temp2=(int)temp;
                x=4999-temp2;
                PWM1_3_CMPA_R=x;
                delayMs(1000);
        }


}



void delayMs(int n)
{
    int i, j;
    for(i = 0 ; i < n; i++)
    for(j = 0; j < 3180; j++) {}
}
