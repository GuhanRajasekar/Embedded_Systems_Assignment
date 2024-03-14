/**
 * Lab 05 UART Controlled Stopwatch
 * Date : 24 Feb 2023
*/
#include <stdint.h>
#include "inc/tm4c123gh6pm.h"

// initialization functions
void EnableInterrupts(void);
void UART0_setup(void);
void GPIO_setup(void);
void systick_setup(void);
// user defined functions
void UART0_Transmit(char data);
void UART0_command_check(void);
void set_7_seg(int,int);
void disp_stopwatch_time();
void led_action();

// hex values of digits for 7 segment display
int disp_digits[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};
// variable for keeping track of time in RUN state / stopwatch time
unsigned int disp_timer = 0;
unsigned int led_timer = 0;
// For switch 1 state
// 0xF0 means Run state
// 0x0F means Stop State
int state_sw1 = 0x0F;
// For switch 2 state
// 0xF0 means Run state
// 0x0F means Pause State
int state_sw2 = 0xF0;
// Flag variable to detect if interrupt has occurred, used for 1ms time delay using SysTick_Handler
// It is inverted every time interrupt handler is called. (0x55 <---> 0xAA)
int flag_1ms = 0x55;
// sw_interrupt_flag is set when switch interrupts are detected first, then there is cool off time of 200ms
// during which if an interrupt is detected, it is ignored (assuming humans don't press switches that fast so it is wrong value)
// cool_off_time is updated in SysTick_Handler() and reset along with sw_interrupt_flag after 200ms
int sw1_interrupt_flag = 0x00;
int sw2_interrupt_flag = 0x00;
int sw1_cool_off_time = 0;
int sw2_cool_off_time = 0;

// array to store command characters, top specifying total characters received. (stack implementation - only push allowed)
char command_buffer[6];
int top = 0;
// status flags used in uart
// uart_echo_flag is set each time a new character is received, and reset after echo
int uart_echo_flag = 0x00;
// uart_cmd_select value indicates the command to be executed
// The lower 4 bits correspond to 4 commands
// bits: 3 2 1 0 <=> command : 4 3 2 1, whichever bit is 1 that command will be executed.
// Each time new character is received, it is compared with command arrays
// Only those whose command[0:top-1] == received_char will be 1
// Initially all commands are equally likely so value (0000 1111)
int uart_cmd_select = 0x0F;
// uart_backspace_flag is set when 'backspace' is detected, top is decremented and uart_cmd_select is computed again
int uart_backspace_flag = 0x00;
// flag to indicate buffer overflow so that user can be informed
int uart_buffer_overflow = 0x00;
// flag to indicate status whether command was valid or not.
// Set each time 'enter key' is pressed
// 0x0F : invalid command
// 0xFF : command executed
int uart_state_update = 0x00;

// array storing valid commands
char command1[6] = {'s','t','a','r','t','0'};
char command2[6] = {'s','t','o','p','0','0'};
char command3[6] = {'h','o','l','d','0','0'};
char command4[6] = {'r','e','s','u','m','e'};
// array storing standard dispaly messages
char msg1[15] = {'b','u','f','f','e','r',' ','o','v','e','r','f','l','o','w'};
char msg2[16] = {'c','o','m','m','a','n','d',' ','e','x','e','c','u','t','e','d'};
char msg3[15] = {'i','n','v','a','l','i','d',' ','c','o','m','m','a','n','d'};
char msg4[9] = {'b','a','c','k','s','p','a','c','e'};


int main(void)
{
    EnableInterrupts();
    systick_setup();
    GPIO_setup();
    UART0_setup();
    while(1)
    {
        disp_stopwatch_time();
        led_action();
        UART0_command_check();
    }
    return 0;
}

void inline EnableInterrupts(void)
{
    // Global interrupts enable
    __asm  ("CPSIE  I\n");      // Change processor state-enable interrupts
}

void UART0_setup()
{
    SYSCTL_RCGCUART_R |= 0x01; // Enable UART0
    SYSCTL_RCGCGPIO_R |= 0x01; // Enable corresponding GPIO port (PORTA)
    // Set GPIO AFSEL bits
    // xxx... PA7 PA6 PA5 PA4 PA3 PA2 PA1 PA0
    //                                Tx  Rx
    GPIO_PORTA_AFSEL_R = 0x03;
    // PCTL is 32 bit register, 4 bits are used to select alternate functions of each GPIO pin
    GPIO_PORTA_PCTL_R = 0x00000011;
    GPIO_PORTA_DEN_R |= 0x03; // GPIO digital enable for PA0, PA1
    // clk = 16 MHz
    // Desired baud rate = clk / (16 * clk_div)
    // UARTIBRD and UARTFBRD
    // If Desired baud rate = XXX.YYY
    // then UARTIBRD = XXX and UARTFBRD = (0.YYY * 64) + 0.5
    // baud_rate = 115200
    UART0_IBRD_R = 8;
    UART0_FBRD_R = 44;
    // 0x60 for 8 data bits, 1 stop, 1 start
    UART0_LCRH_R = 0x60;
    UART0_CC_R = 0x0;
    UART0_CTL_R = 0x0301;
    GPIO_PORTA_AMSEL_R = 0;

    // Interrupt setup for UART0
    UART0_IM_R |= 0x10;       // enable interrupt mask for receive
    UART0_ICR_R = (0x010);    // clear interrupt if any
    NVIC_PRI1_R |= 0x00006000;// priority = 3
    NVIC_EN0_R |= 0x00000020; // enable interrupt for UART0 (IRQ number = 5)
}

void GPIO_setup()
{
    SYSCTL_RCGC2_R |= 0x00000023;     // enable clock to GPIOA, GPIOB and GPIOF at clock gating control register
    GPIO_PORTF_LOCK_R = 0x4C4F434B;   // unlock commit register GPIOCR
    GPIO_PORTF_CR_R = 0x01;           // make PORTF0 configurable
    // Enable the GPIO pins
    // For PORTF the LED (PF3, 2 1) as output
    // For PORTB, all pins are used to set 7 segment display
    // For PORTA, pins 7 to 4 are used for selecting one of the four 7 segment display
    GPIO_PORTA_DIR_R |= 0xF0;
    GPIO_PORTB_DIR_R |= 0xFF;
    // X X X SW1 G B R SW2 -> bits in PORTF
    GPIO_PORTF_DIR_R |= 0x0E;
    // enable the GPIO pins for digital function
    GPIO_PORTA_DEN_R |= 0xF0;
    GPIO_PORTB_DEN_R |= 0xFF;
    GPIO_PORTF_DEN_R |= 0x1F;
    GPIO_PORTF_PUR_R = 0x11;           // enable pull up for SW1 and SW2

    // interrupt setup for sw1 and sw2
    // Interrupt sense register sets value for bits 4 and 0. (1 - level triggered, 0 - edge triggered)
    GPIO_PORTF_IS_R &= ~(0x11);
    // Interrupt Both Edges is not set
    GPIO_PORTF_IBE_R &= ~(0x11);
    // If both edges not set then
    // Trigger controlled by IEV (Interrupt Even Register) for negative/positive or rising/falling
    GPIO_PORTF_IEV_R &= ~(0x11);
    // Clearing previous interrupts if any
    GPIO_PORTF_ICR_R |= (0x11);
    // Unmask interrupt for sw1 and sw2 (bit 4 and 0)
    GPIO_PORTF_IM_R |= 0x11;
    // set interrupt priority = 5 for PORTF
    //NVIC_PRI7_R = (NVIC_PRI7_R & 0xFF00FFFF) | 0x00A00000;
    NVIC_PRI7_R |= 0x00A00000;       // priority = 5
    NVIC_EN0_R |= 0x40000000;        // NVIC enabled for PORTF (IRQ number = 30)
}

void systick_setup()
{
    // clk_freq = 16 MHz
    // 1 tick = 1 / (16000000) = 62.5 ns
    // for time_delay = 1 ms
    // Reload value = (clk_freq * time_delay) - 1
    //              = 16000 - 1
    // reload with number of clocks per second
    NVIC_ST_RELOAD_R = 16000-1;
    // enable SysTick interrupt, use system clock
    NVIC_ST_CTRL_R = 7;
}

void UART0_Transmit(char data)
{
    while((UART0_FR_R & (1<<5)) != 0)
    {
        // wait until Tx buffer full
    }
    UART0_DR_R = data; // send the byte
}

void UART0_command_check()
{
    // if new character received then update uart_cmd_select and echo back the character
    if(uart_echo_flag == 0xFF)
    {
        // command[top-1] : new received character in command_buffer
        // command_buffer[top-1] : corresponding character of standard commands
        // they are compared for each command, the result is shifted to respective bit position
        // and ANDED with previous value
        uart_cmd_select &= ( ((command1[top-1]==command_buffer[top-1])<<0) | ((command2[top-1]==command_buffer[top-1])<<1) | ((command3[top-1]==command_buffer[top-1])<<2) | ((command4[top-1]==command_buffer[top-1])<<3) );
        uart_echo_flag = 0x00;                      // reset uart_echo_flag
        UART0_Transmit(command_buffer[top-1]);      // echo the received character
    }
    // if buffer_overflow set transmit msg1 to user
    if(uart_buffer_overflow == 0xFF)
    {
        UART0_Transmit('\n');
        for(int i = 0; i < 6; i++)
        {
            UART0_Transmit('\b');
        }
        for(int i = 0; i < 15; i++)
        {
            UART0_Transmit(msg1[i]);
        }
        for(int i = 0; i < 15; i++)
        {
            UART0_Transmit('\b');
        }
        UART0_Transmit('\n');
        uart_buffer_overflow = 0x00;                // reset buffer_overflow
    }
    // if uart_state_update = 'valid', transmit msg2 to user
    if(uart_state_update == 0xFF)
    {
        UART0_Transmit('\n');
        for(int i = 0; i < 6; i++)
        {
            UART0_Transmit('\b');
        }
        for(int i = 0; i < 16; i++)
        {
            UART0_Transmit(msg2[i]);
        }
        for(int i = 0; i < 16; i++)
        {
            UART0_Transmit('\b');
        }
        UART0_Transmit('\n');
        uart_state_update = 0x00;                   // reset uart_state_update
    }
    // if uart_state_update is invalid, transmit msg3 to user
    else if(uart_state_update == 0x0F)
    {
        UART0_Transmit('\n');
        for(int i = 0; i < 6; i++)
        {
            UART0_Transmit('\b');
        }
        for(int i = 0; i < 15; i++)
        {
            UART0_Transmit(msg3[i]);
        }
        for(int i = 0; i < 15; i++)
        {
            UART0_Transmit('\b');
        }
        UART0_Transmit('\n');
        uart_state_update = 0x00;                   // reset uart_state_update
    }
    if(uart_backspace_flag == 0xFF)
    {
        UART0_Transmit('\n');
        for(int i = 0; i < top+1; i++)
        {
            UART0_Transmit('\b');
        }
        for(int i = 0; i < 9; i++)
        {
            UART0_Transmit(msg4[i]);
        }
        UART0_Transmit('\n');
        for(int i = 0; i < 9; i++)
        {
            UART0_Transmit('\b');
        }
        for(int i = 0; i < top; i++)
        {
            uart_cmd_select &= ( ((command1[i]==command_buffer[i])<<0) | ((command2[i]==command_buffer[i])<<1) | ((command3[i]==command_buffer[i])<<2) | ((command4[i]==command_buffer[i])<<3) );
            UART0_Transmit(command_buffer[i]);
        }
        uart_backspace_flag = 0x00;
    }
}

void led_action()
{
    // if less than 200 ms, LED is ON else it is OFF when in blink state
    if(led_timer < 200)
    {
        int color;
        if((state_sw1 == 0xF0) && (state_sw2 == 0xF0))
        {
            color = 0x08;   // green
        }
        else if((state_sw1 == 0xF0) && (state_sw2 == 0x0F))
        {
            color = 0x04;   // blue
        }
        else
        {
            color = 0x02;   // red
        }
        GPIO_PORTF_DATA_R = color;
    }
    else
    {   // LEDs are OFF if not Stop state
        if(state_sw1 != 0x0F)
        {
            GPIO_PORTF_DATA_R = 0x00;
        }
    }
}

void disp_stopwatch_time()
{
    int time_in_sec = disp_timer / 1000;
    int t_sec = time_in_sec % 60;
    int t_min = time_in_sec / 60;
    set_7_seg(0x10,(t_sec % 10));
    set_7_seg(0x20,(t_sec / 10));
    set_7_seg(0x40,(t_min % 10));
    set_7_seg(0x80,(t_min / 10));
}

void set_7_seg(int position,int index)
{
    int prev_value = flag_1ms;
    while(prev_value == flag_1ms)
    {
        GPIO_PORTA_DATA_R = position;
        if(position == 0x40)// to display dot in 7 segment
        {
            // Display data plus the dot
            GPIO_PORTB_DATA_R = disp_digits[index] + 0x80;
        }
        else
        {
            GPIO_PORTB_DATA_R = disp_digits[index];
        }
    }
}

void UART0_Handler(void)
{
    UART0_ICR_R = (0x010);                  // clear the interrupt
    char c = UART0_DR_R;                    // read the data from UART0_DR register
    // if received character is 'enter key'
    if(c == '\r')
    {
        uart_state_update = 0xFF;           // assume command is valid and check if it matches standard commands
        // if command is 'start' and state_sw1 = 'stop'
        if((uart_cmd_select == 0x01) && (state_sw1 == 0x0F))
        {
            state_sw1 = 0xF0;               // set state_sw1 = 'run'
        }
        // if command is 'stop', reset all states and timer
        else if(uart_cmd_select == 0x02)
        {
            state_sw1 = 0x0F;
            state_sw2 = 0xF0;
            disp_timer = 0;
        }
        // if command is 'hold' and state_sw1 = 'run'
        else if((uart_cmd_select == 0x04) && (state_sw1 == 0xF0))
        {
            state_sw2 = 0x0F;               // set state_sw2 = 'pause'
        }
        // if command is 'resume'
        else if(uart_cmd_select == 0x08)
        {
            state_sw2 = 0xF0;
        }
        else
        {
            uart_state_update = 0x0F;       // set uart_state_update = 'invalid'
        }
        uart_cmd_select = 0x0F;             // reset uart_cmd_select
        top = 0;                            // reset received char count
    }
    // if received character is 'backspace key'
    else if(c == '\b')
    {
        uart_backspace_flag = 0xFF;         // set uart_backspace_flag
        uart_cmd_select = 0x0F;             // reset uart_cmd_select
        top--;                              // decrement top of the command_buffer
    }
    // new character received, add to command_buffer and set corresponding flag
    else if(top < 6)
    {
        command_buffer[top++] = c;
        uart_echo_flag = 0xFF;
    }
    // command_buffer overflow, top is reset again to 0, set uart_buffer_overflow flag
    // inform the user through uart
    else
    {
        top = 0;
	//uart_cmd_select = 0x1F;             // reset uart_cmd_select
        uart_buffer_overflow = 0xFF;
    }
}

void SysTick_Handler(void)
{
    // if it is in RUN mode in SW1 and SW2 both, update disp_timer
    if(state_sw1 == 0xF0 && state_sw2 == 0xF0)
    {
        disp_timer = disp_timer + 1;
    }
    led_timer = led_timer + 1;
    if(led_timer >= 400)
    {
        led_timer = led_timer - 400;
    }
    // sw_interrupt_flag will reset only after 200ns
    if(sw1_interrupt_flag == 0xFF)
    {
        sw1_cool_off_time++;
        if(sw1_cool_off_time == 200)
        {
            sw1_interrupt_flag = 0x00;
        }
    }
    if(sw2_interrupt_flag == 0xFF)
    {
        sw2_cool_off_time++;
        if(sw2_cool_off_time == 200)
        {
            sw2_interrupt_flag = 0x00;
        }
    }
    // invert the flag
    flag_1ms ^= 0xFF;
}

void GPIO_PORTF_Handler(void)
{
    // interrupt detected in switch 1 and interrupt flag is reset then update state_sw1
    // this is to avoid double clicks induced by mistake that erroneously changes the state
    if(GPIO_PORTF_MIS_R == 0x10 && sw1_interrupt_flag == 0x00)
    {
        state_sw1 ^= 0xFF;
        sw1_interrupt_flag = 0xFF;
        sw1_cool_off_time = 0;
        if(state_sw1 == 0x0F)
        {
            state_sw2 = 0xF0;
            disp_timer = 0;
        }
    }
    // interrupt detected in switch 2 and interrupt flag is reset then update state_sw2 only if state_sw1 == RUN
    if(GPIO_PORTF_MIS_R == 0x01 && sw2_interrupt_flag == 0x00)
    {
        if(state_sw1 == 0xF0)
        {
            state_sw2 ^= 0xFF;
            sw2_interrupt_flag = 0xFF;
            sw2_cool_off_time = 0;
        }
    }
    // clear the interrupt flags
    GPIO_PORTF_ICR_R = 0x11;
}
