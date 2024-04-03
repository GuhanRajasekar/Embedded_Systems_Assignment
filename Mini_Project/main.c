#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>  //as we need to use atoi() function
#include <./inc/tm4c123gh6pm.h>
#include <inc/hw_memmap.h>
#include <inc/hw_types.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/sysctl.h>
#include <driverlib/uart.h>
#include <ctype.h>
#include <main.h>

//// Macros for the various colors
//#define COLOR_GREEN_ON     0x08
//#define COLOR_BLUE_ON      0x04
//#define COLOR_RED_ON       0x02
//#define COLOR_CYAN_ON      0x0C  // B + G
//#define COLOR_YELLOW_ON    0x0A  // R + G
//#define COLOR_MAGENTA_ON   0x06  // B + R
//#define COLOR_WHITE_ON     0x0E  // B + R + G
//#define NO_COLOR           0x00  // No color
//
//#define STACK_SIZE 100 // size of the dummy array that will be used to save the context of a particular task before task switching
//#define THREAD_NUM 4   // defining the max  number of tasks
//struct tcb
//{
//    long* sp;           // pointer that will point to the Stack of that particular task
//    struct tcb* next;   // pointer pointing to the TCB of the next task
//    int priority;       // lower the number, higher is the priority of that task (useful only for event threads)
//};
//
//typedef struct tcb tcbtype;
//
//tcbtype tcbs[THREAD_NUM];  // Array of tcbs. Each task will have its own tcb
//long stacks[THREAD_NUM][STACK_SIZE]; // Dummy array that helps us save the context of a particular task before switching on to the next task
//
//char NUMto7SEG[10] = {    0x3f,  // 0
//                          0x06,  // 1
//                          0x5B,  // 2
//                          0x4F,  // 3
//                          0x66,  // 4
//                          0x6D,  // 5
//                          0x7D,  // 6
//                          0x07,  // 7
//                          0x7F,  // 8
//                          0x6F   // 9
//                      };


int main()
{
    DisableInterrupts();
    Init_PortF();
    Init_Systick();
    init_portab();
    OS_AddThreads(&task1, &task2, &task3, &task4);
    start_os();
}

void task1(void)
{
    int i = 0;
    while(1)
    {
        GPIO_PORTF_DATA_R = 0x02;
        if(i > 9)
            i = 0;
        else i++;

        for(int j1 = 0; j1<1200000; j1++) // slower task
            asm("NOP");
        (GPIO_PORTA_DATA_R = 0x10);
        GPIO_PORTB_DATA_R = NUMto7SEG[i];
    }
}

void task2(void)
{
    int j = 0;
    while(1)
    {
        GPIO_PORTF_DATA_R = 0x04;

             if(j>9)
                j=0;

             else j++;

             for(int j1 = 0; j1<1000000; j1++)
                     asm("NOP");

             (GPIO_PORTA_DATA_R = 0x20);
             GPIO_PORTB_DATA_R = NUMto7SEG[j];
    }
}

void task3(void)
{
    int k = 0;
    while(1)
    {
        GPIO_PORTF_DATA_R = 0x08;
              if(k>9)
                  k = 0;
              else k++;

              for(int j1=0;j1<1000000;j1++)
                   asm("NOP");
              (GPIO_PORTA_DATA_R = 0x40);
              GPIO_PORTB_DATA_R = NUMto7SEG[k];
    }
}

void task4(void)
{
    int l = 0;
    while(1)
    {
        GPIO_PORTF_DATA_R = 0x0e;
              if(l>9)
                  l = 0;

              else l++;

              for(int j1=0;j1<800000; j1++)
                  asm("NOP");
              (GPIO_PORTA_DATA_R = 0x80);
              GPIO_PORTB_DATA_R = NUMto7SEG[l];
    }
}

void Init_PortF(void)
{
    volatile unsigned long delay;
    SYSCTL_RCGC2_R |= 0x00000020; /* Activate Clock for PORTF */
    delay = SYSCTL_RCGC2_R; /* Allow time for clock to start*/

    GPIO_PORTF_DIR_R =  0x0E;  /* enable the GPIO pins for the LED (PF3, 2 1) as output */
    GPIO_PORTF_LOCK_R = 0x4C4F434B;   /* unlock commit register GPIOCR */
    GPIO_PORTF_CR_R = 0x0E;
    GPIO_PORTF_AMSEL_R = 0x00; /* disable analog on PF*/
    GPIO_PORTF_PCTL_R = 0x00000000; /* PCTL GPIO on PF4 - PF0*/
    GPIO_PORTF_DIR_R = 0x0E; /* PF4, PF0 as inputs and (PF3, PF2 ,PF1) as input pins*/
    GPIO_PORTF_AFSEL_R = 0x00; /* disable alternate function on PF7 - PF0*/
    GPIO_PORTF_PUR_R = 0x00; /* enable pull up on PF0 and PF4*/
    GPIO_PORTF_DEN_R = 0x0E;
}

void Init_Systick(void)
{
    NVIC_ST_RELOAD_R = 15999999; // reload value corresponding to 1 second
    NVIC_ST_CTRL_R = 7;
    NVIC_SYS_PRI3_R= (NVIC_SYS_PRI3_R & 0x1fffffff) | 0x60000000;
}

void EnableInterrupts(void)
{
    __asm ("     CPSIE I\n");
}

void DisableInterrupts(void)
{
    __asm("    CPSID  I\n");
}

void Set_initial_stack(int i)
{
    tcbs[i].sp = &stacks[i][STACK_SIZE-16]; // Initializing Stack Pointer
    stacks[i][STACK_SIZE - 1] = 0x01000000; // Thumb mode
    stacks[i][STACK_SIZE-3] = 1;
    stacks[i][STACK_SIZE-4] = 2;
    stacks[i][STACK_SIZE-5] = 3;
    stacks[i][STACK_SIZE-6] = 4;
    stacks[i][STACK_SIZE-7] = 5;
    stacks[i][STACK_SIZE-8] = 6;
    stacks[i][STACK_SIZE-9] = 7;
    stacks[i][STACK_SIZE-10] = 8;
    stacks[i][STACK_SIZE-11] = 9;
    stacks[i][STACK_SIZE-12] = 10;
    stacks[i][STACK_SIZE-13] = 11;
    stacks[i][STACK_SIZE-14] = 12;
    stacks[i][STACK_SIZE-15] = 13;
    stacks[i][STACK_SIZE-16] = 14;

}

void OS_AddThreads(void (*task0)(void), void(*task1)(void), void(*task2)(void) , void(*task3)(void) )
{
    tcbs[0].next = &tcbs[1];
    tcbs[1].next = &tcbs[2];
    tcbs[2].next = &tcbs[3];
    tcbs[3].next = &tcbs[0];

    Set_initial_stack(0);
    stacks[0][STACK_SIZE-2] = (long)(task0);

    Set_initial_stack(1);
    stacks[1][STACK_SIZE-2] = (long)(task1);

    Set_initial_stack(2);
    stacks[2][STACK_SIZE-2] = (long)(task2);

    Set_initial_stack(3);
    stacks[3][STACK_SIZE-2] = (long)(task3);

    runpt = &tcbs[0];
}

void start_os(void)
{
    asm("LDR R0, =runpt");
    asm("LDR R2,[R0]");
    asm("LDR SP,[R2]");
    asm("POP {R4-R11}");
    asm("POP {R0 - R3}");
    asm("POP {R12}");
    asm("POP {LR}");
    asm("POP {LR}");  // to get to the position where function pointer of that particular task is stored
    asm("POP {R1}");
    asm("CPSIE I");
    asm("BX LR");  // BX is branch instruction
}

__attribute__((naked))
void SysTick_Handler(void)
{
    asm("CPSID I");
    asm("PUSH {R4-R11}");
    asm("LDR R0, =runpt");
    asm("LDR R1, [R0]");
    asm("STR SP,[R1]");
    asm("LDR R1, [R1,#4]"); // getting next thread  tcb
    asm("STR R1,[R0]");
    asm("LDR SP,[R1]");
    asm("POP {R4-R11}");
    asm("CPSIE I");
    asm("BX LR");
}

void init_portab(void)
{
    SYSCTL_RCGC2_R |= 0x00000023;
    GPIO_PORTA_CR_R = 0xF0;
    GPIO_PORTB_CR_R = 0xFF;

    GPIO_PORTA_AMSEL_R = 0x00;
    GPIO_PORTB_AMSEL_R = 0x00;

    GPIO_PORTB_DIR_R = 0xFF;
    GPIO_PORTA_DIR_R = 0xF0;

    GPIO_PORTB_AFSEL_R = 0x00;
    GPIO_PORTA_AFSEL_R = 0x00;

    GPIO_PORTB_DEN_R = 0xFF;
    GPIO_PORTA_DEN_R = 0xF0;
}
