

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
#include "sine_table.h"
#include "periph_ctl.h"
#include "HeapMem.h"
#include "GPIO.h"

// The following variables have been defined in HeapMem.c file
extern unsigned int uiBlockID;
extern int HeapInitFlag;
extern int HeapSize;
extern int HeapUnitSize;
extern void *pHeapStartAddr;
extern int BlockHeaderSize;
extern BLOCK_HEADER *psFirstUsedBlock,*psFirstFreeBlock;

// Function to initialize Systick Timer
void Init_Systick(void)
{
    NVIC_ST_RELOAD_R = 15999999; // reload value corresponding to 1 second
    NVIC_ST_CTRL_R = 7;
    NVIC_SYS_PRI3_R= (NVIC_SYS_PRI3_R & 0x1fffffff) | 0x60000000;
}

// Function to initialize the dummy stack for the all the background tasks
void Set_initial_stack(int i)
{
    tcbs[i].sp = &stacks[i][STACK_SIZE-16]; // Initializing Stack Pointer
    stacks[i][STACK_SIZE - 1] = 0x01000000; // Thumb mode
    stacks[i][STACK_SIZE-3] = 1;   // R14
    stacks[i][STACK_SIZE-4] = 2;   // R12
    stacks[i][STACK_SIZE-5] = 3;   // R3
    stacks[i][STACK_SIZE-6] = 4;   // R2
    stacks[i][STACK_SIZE-7] = 5;   // R1
    stacks[i][STACK_SIZE-8] = 6;   // R0
    stacks[i][STACK_SIZE-9] = 7;   // R11
    stacks[i][STACK_SIZE-10] = 8;  // R10
    stacks[i][STACK_SIZE-11] = 9;  // R9
    stacks[i][STACK_SIZE-12] = 10; // R8
    stacks[i][STACK_SIZE-13] = 11; // R7
    stacks[i][STACK_SIZE-14] = 12; // R6
    stacks[i][STACK_SIZE-15] = 13; // R5
    stacks[i][STACK_SIZE-16] = 14; // R4.  Stack Pointer will be pointing to this location.

}


// Function to set up a doubly linked list structure that connects TCBs of all the individual tasks
//void OS_AddThreads(void (*task0)(void), void(*task1)(void), void(*task2)(void) , void(*task3)(void), void(*task4)(void) )
void OS_AddThreads()
{
    // Here we form a circular linked list structure chaining all the main threads
    tcbs[0].next = &tcbs[1];
    tcbs[1].next = &tcbs[2];
    tcbs[2].next = &tcbs[3];
    tcbs[3].next = &tcbs[4];
    tcbs[4].next = &tcbs[5];
    tcbs[5].next = &tcbs[6];
    tcbs[6].next = &tcbs[0];

    // Assigning priorities to each task (lower the number, higher the priority)
    for(int pri = 0; pri < THREAD_NUM ; pri++)
    {
        tcbs[pri].priority = pri;
    }


    Set_initial_stack(0);  // Set up dummy stack for task0 that will be used to save the context for task 0
    stacks[0][STACK_SIZE-2] = (long)(task0); // store the function pointer of task0 in the last but first location of the dummy array of task0. This is for the Program Counter

    Set_initial_stack(1); // Set up dummy stack for task1 that will be used to save the context for task 1
    stacks[1][STACK_SIZE-2] = (long)(task1);  // store the function pointer of task1 in the last but first location of the dummy array of task1. This is for the Program Counter

    Set_initial_stack(2); // Set up dummy stack for task2 that will be used to save the context for task 2
    stacks[2][STACK_SIZE-2] = (long)(task2);  // store the function pointer of task2 in the last but first location of the dummy array of task2. This is for the Program Counter

    Set_initial_stack(3); // Set up dummy stack for task3 that will be used to save the context for task 3
    stacks[3][STACK_SIZE-2] = (long)(task3);  // store the function pointer of task3 in the last but first location of the dummy array of task3. This is for the Program Counter

    Set_initial_stack(4);  // Set up dummy stack for task4 that will be used to save the context for task 4
    stacks[4][STACK_SIZE-2] = (long)(task4); // store the function pointer of task4 in the last but first location of the dummy array of task4. This is for the Program Counter

    Set_initial_stack(5);  // Set up dummy stack for task5 that will be used to save the context for task 5
    stacks[5][STACK_SIZE-2] = (long)(task5); // store the function pointer of task5 in the last but first location of the dummy array of task5. This is for the Program Counter

    Set_initial_stack(6);  // Set up dummy stack for task5 that will be used to save the context for task 6
    stacks[6][STACK_SIZE-2] = (long)(task6); // store the function pointer of task6 in the last but first location of the dummy array of task6. This is for the Program Counter

    runpt = &tcbs[0]; // Make runpt point to tcb of the first task
}


// Function to make it look like the system was already in steady state
void start_os(void)
{
    asm("LDR R0, =runpt");  // Store the address of "runpt" in register R0
    asm("LDR R2,[R0]");     // Dereference R0 to get the "runpt" value. Store runpt in register R2
    asm("LDR SP,[R2]");     // Dereference R2 => Dereference runpt => This gives sp value. Store sp of that particular task in the actual SP register.
    asm("POP {R4-R11}");    // Get the contents in the next 8 locations of the stack memory and store it in registers R4 to R11 => Saving the context
    asm("POP {R0 - R3}");   // Get the contents in the next 4 locations of the stack memory and store it in registers R0 to R3  => Saving the context
    asm("POP {R12}");       // Get the content pointed to by the stack pointer and store it in register R12 => Saving the context
    asm("POP {LR}");
    asm("POP {LR}");  // to get to the position where function pointer of that particular task is stored
    asm("POP {R1}");
    asm("CPSIE I");   // Enable the interrupts
    asm("BX LR");  // BX is branch instruction
}

int main()
{
    DisableInterrupts();                                 // Disable the interrupts
    heap_start = (unsigned int)&__heap_start__;          // storing the start  address of heap memory section
    mem = HeapMemInit((void*)heap_start, 512 , 16);      // Heap memory initialization (512 bytes of memory in chunks of 16 bytes)
    Init_PortAB();                                       // Initialize ports A and B
    Init_PortC();                                        // Initialize Port C
    Init_PortE();                                        // Intialize Port E
    Init_PortF();                                        // Initialize Port F
    Init_Systick();                                      // Initialize Systick Timer

    //UART Initialization
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    configure_spi();                                     // Configure the spi to send values to DAC
    OS_AddThreads();                                     // Form a linked list, chaining the TCBs of the tasks together
    start_os();
    return 0; // Program Control never reaches this point
}

/* task 0 makes the RED LED ON continuously and also displays the count on the right most SSD*/
void task0(void)
{
    int i = 0;
    while(1)
    {
        GPIO_PORTF_DATA_R = 0x02;  // red color LED
        if(i > 9)
            i = 0;
        else i++;

        for(int j1 = 0; j1<1000000; j1++) // slower task (1200000)
            asm("NOP");
        (GPIO_PORTA_DATA_R = 0x10); //PA4 is made high => Right most SSD is chosen
        GPIO_PORTB_DATA_R = NUMto7SEG[i];
    }
}

/* task 1 makes the BLUE LED ON continuously and displays its count on the second SSD from the right*/
void task1(void)
{
    int j = 0;
    while(1)
    {
        GPIO_PORTF_DATA_R = 0x04;  // Blue LED

             if(j>9)
                j=0;

             else j++;

             for(int j1 = 0; j1<1000000; j1++) //(1000000)
                     asm("NOP");

             (GPIO_PORTA_DATA_R = 0x20); // PA5 = 1 => Second LED from the right is chosen
             GPIO_PORTB_DATA_R = NUMto7SEG[j]; // Display the count value indicated by j
    }
}

/* task 2 makes the GREEN LED ON continuously and displays its count on the third SSD from the right*/
void task2(void)
{
    int k = 0;
    while(1)
    {
        GPIO_PORTF_DATA_R = 0x08;  // Green color LED
              if(k>9)
                  k = 0;
              else k++;

              for(int j1=0;j1<1000000;j1++) // 1000000
                   asm("NOP");
              (GPIO_PORTA_DATA_R = 0x40);  // Third SSD from the right is chosen
              GPIO_PORTB_DATA_R = NUMto7SEG[k]; // Display the value indicated by k on the SSD
    }
}

/* task 3 makes the WHITE LED ON continuously and displays its count on the second SSD from the right*/
void task3(void)
{
        int l = 0;
        while(1)
        {
            GPIO_PORTF_DATA_R = 0x0e;  // white LED
                  if(l>9)
                      l = 0;

                  else l++;

                  for(int j1=0;j1<1000000; j1++) //400000
                      asm("NOP");
                  (GPIO_PORTA_DATA_R = 0x80);  // PA7 = 1 => Left most SSD is chosen
                  GPIO_PORTB_DATA_R = NUMto7SEG[l]; // Count value indicated by l is displayed on the left most SSD
        }
}

/*task 4 keeps Magenta LED ON sends value to LTC1661 DAC to generate sinusoidal waveform and view it on the oscilloscope*/
void task4(void)
{
  while(1)
  {
       GPIO_PORTF_DATA_R = 0x06;             // Magenta LED
       sin_index = (sin_index+1)%127;        // Increment the index to send the next value
       data      =  sine[sin_index];         // Pick a value to be sent from the look up table
       unsigned int command   =  0x9000;     // To send value on Channel A

       debug_var_task4  = data;                  // For viewing purposes

       int temp =  GPIO_PORTF_DATA_R ;
       unsigned temp_data = ((data<<2)|(command));
       GPIO_PORTF_DATA_R &= ~0x04; /* assert SS low */
       while((SSI3_SR_R & 2) == 0);/* wait until FIFO not full */
       SSI3_DR_R = (temp_data>>8);
       while(SSI3_SR_R & 0x10);    /* wait until transmit complete */
       SSI3_DR_R = temp_data;      /* transmit high byte */
       while(SSI3_SR_R & 0x10);    /* wait until transmit complete */
       GPIO_PORTF_DATA_R |= 0x04;
       GPIO_PORTF_DATA_R = temp;   /* keep SS idle high */
  }
}


void task5(void)
{

   void* ptr;
   int flag = 0;
   int* ptr_task5;
   while(1)
   {
       GPIO_PORTF_DATA_R = 0x0A;             // Yellove LED
       GPIO_PORTE_DATA_R = 0x00;             // To make sure PortC values change for interrupt handling
       debug_var_task5 = *ptr_task5;         // For viewing and de-bugging purposes
       if(mem == 1)  // Heap Memory Initialization was successful
       {
          if(flag == 0)
          {
              ptr = MemAlloc(16);      // Request 16 bytes of memory from Heap Section
              strcpy(task5_alloc, "Memory allocated to task5 at location: ");
              itoa((int)ptr,task5_location,10); // third argument of 10 denotes decimal base
              strcat(task5_alloc, task5_location);

              print_newline();
              print(task5_alloc);
              print_newline();

              ptr_task5 = (int *)ptr;  // Store the starting address of allocated memory in ptr_task5 pointer
              *ptr_task5 = 0;          // Initialize 0 in the location pointed to by ptr_task5
              flag = 1;
          }
          else
          {
              *ptr_task5 = *ptr_task5 + 1;
              for(int i=1;i<=500;i++) delay_1ms();  // wait for 0.5s
              if(*ptr_task5 == 11)
              {
                  MemFree(ptr);  // To Free up the memory
                  strcpy(task5_dealloc , "Memory de-allocated to task5 at location: ");
                  strcat(task5_dealloc, task5_location);

                  print_newline();
                  print(task5_dealloc);
                  print_newline();
                  flag = 0;      // Start the process all over again
              }

          }

       }
   }

}

// Similar to task5, task6 also makes use of dynamically allocated memory section
void task6(void)
{
    void* ptr2;
    int flag2 = 0;
    int* ptr_task6;
    while(1)
    {
        GPIO_PORTE_DATA_R = 0x00;             // To make sure PortC values change for interrupt handling
        GPIO_PORTF_DATA_R = 0x0C;             // Cyan LED
        debug_var_task6 = *ptr_task6;         // For viewing and de-bugging purposes
        if(mem == 1)  // Heap Memory Initialization was successful
        {
           if(flag2 == 0)
           {
               ptr2 = MemAlloc(16);            // Request 16 bytes of memory from Heap Section
               ptr_task6  = (int *)ptr2;       // Store the starting address of allocated memory in ptr_task5 pointer
               *ptr_task6 = 0;                 // Initialize 0 in the location pointed to by ptr_task5
               strcpy(task6_alloc,"Memory allocated to task6 at location: ");
               itoa((int)ptr2,task6_location,10); // third argument of 10 denotes decimal base
               strcat(task6_alloc, task6_location);

               print_newline();
               print(task6_alloc);
               print_newline();
               flag2 = 1;

           }
           else
           {
               *ptr_task6 = *ptr_task6 + 1;          // Increment the value in the location pointed to by the pointer ptr_task6
               for(int i=1;i<=500;i++) delay_1ms();  // wait for 0.5s
               if(*ptr_task6 == 11)
               {
                   MemFree(ptr2);        // To Free up the memory
                   strcpy(task6_dealloc,"Memory deallocated to task6 at location: ");
                   strcat(task6_dealloc, task6_location);

                   print_newline();
                   print(task6_dealloc);
                   print_newline();
                   flag2 = 0;          // Start the process all over again
               }
           }

        }
    }
}

// Function to enable interrupts
void EnableInterrupts(void)
{
    __asm ("     CPSIE I\n");
}

// Function to disable interrupts
void DisableInterrupts(void)
{
    __asm("    CPSID  I\n");
}


// Systick_Handler() function is the preemptive scheduler that will determine which background task needs to be run next

/*
  When we add __attribute__((naked)) command, it means we are responsible for managing the function's stack frame.
  This involves saving and restoring the context of the function. This is done using assembly code as shown in the function.
 */
__attribute__((naked))
void SysTick_Handler(void)
{
    // The following 5 lines of assembly code will help us save the context of the background task that is currently running
    asm("CPSID I");           // Disable the interrupts. This is done to prevent interrupts when we switch from one task to another
    asm("PUSH {R4-R11}");     // Pushing the contents of R4-R11 on to the dummy array that acts as stack for each task
    asm("LDR R0, =runpt");    // Store the address of runpt in R0
    asm("LDR R1, [R0]");      // Dereference R0 to get "runpt". Store "runpt" in R1 register
    asm("STR SP,[R1]");       // Store the value of actual SP in the "sp" pointer of the tcb structure of that particular task

    // If count value is greater than 0 perform the same task again
    if(count > 0) count -= 1;

    // If the count for the current task has been exhausted, then move on to the next task (Compute the count value for the new task to be executed)
    else
    {
        runpt = runpt->next;             //make runpt point to the tcb of the next task to be performed
        switch(runpt->priority)
        {
           case 0:  count = 18;  break;
           case 1:  count = 10;  break;
           case 2:  count =  8;  break;
           case 4:  count =  6;  break;
           case 5:  count =  4;  break;
           case 6:  count =  2;  break;
           default: count =  1;  break;
        }
    }

    // The next 5 steps will help us switch from one task to another
    asm("LDR R0, =runpt");
    asm("LDR R1, [R0]");
    asm("LDR SP,[R1]");       // Dereferencing R1 gives "sp" of the new task. This value is loaded to SP so that SP now points to the dummy array of the task that needs to be executed
    asm("POP {R4-R11}");      // Transfer the contents of the new task on to the registers R4 - R11 and start executing the new task
    asm("CPSIE I");           // Enable interrupts

    // Branch to the address contained in the Link Register (LR).
    //Used to return from the interrupt handler, effectively restoring the context of the interrupted code and continuing its execution.
    asm("BX LR");

}



// Function to handle key press as interrupts
void GPIO_PORTC_Handler(void)
{
    DisableInterrupts();    // Disable the interrupts
    if((GPIO_PORTC_DATA_R & 0xF0) != 0xF0)  // Key Press Detected
    {
        GPIO_PORTE_DATA_R = 0x07; // Driving PE3 low   (Last row driven low)
        if((GPIO_PORTC_DATA_R & 0xF0) == 0xE0)              // left most key in the last row
        {
            // Dynamically change the priorities of the tasks here
            for(int wait = 0; wait<80; wait++) delay_1ms();
            for(int i=0;i<THREAD_NUM;i++)      tcbs[i].priority = (tcbs[i].priority + 1) % THREAD_NUM;
        }

        else if((GPIO_PORTC_DATA_R & 0xF0) == 0xD0)         // Second key from the left in the last row
        {
            for(int wait = 0; wait<80; wait++) delay_1ms(); // To avoid debouncing issues
            print_newline();
            print("Inhale.... Exhale.....and Repeat");
            print_newline();
        }

        else if((GPIO_PORTC_DATA_R & 0xF0) == 0xB0)        // Third key from the left in the last row
        {
            for(int wait = 0; wait<80; wait++) delay_1ms(); // To avoid debouncing issues
            print_newline();
            print("This mini project is a Pre-Emptive Task Scheduler done as part of Embedded Systems Course");
            print_newline();
        }

        else if((GPIO_PORTC_DATA_R & 0xF0) == 0x70)         // Right most key in the last row
        {
            for(int wait = 0; wait<80; wait++) delay_1ms(); // To avoid debouncing issues
            HeapMemDump();   // output snapshot of memory usage
        }
    }
    GPIO_PORTC_ICR_R = 0xF0; // Clearing the interrupts
    EnableInterrupts();      // Enable the interrupts
}

// Function to add a delay of 1ms
void delay_1ms(void)
{

    for(int j = 0; j<3180; j++) //3180
       {
            // Do nothing for 1 ms
       }
}

//Function to print contents of the given array
void print(char text[])
{
    int index = 0;
    while(text[index] != '\0')
    {
        UARTCharPut(UART0_BASE, text[index]);
        index = index+1;
    }
    text[0] = '\0'; // Empty the contents of the array before the next print process
    return;
}


// Function to print gap of one line
void print_newline()
{
    UARTCharPut(UART0_BASE, '\n');
    UARTCharPut(UART0_BASE, '\r');
    return;
}

//To provide output snapshot of memory usage
void HeapMemDump()
{
    BLOCK_HEADER *UsedBlock,*FreeBlock;
    if(HeapInitFlag ==0) print("No Heap Memory Exists");
    else
    {
        UsedBlock = psFirstUsedBlock;
        print("\n\rUsed Blocks:\n\r");
        do
        {
            itoa(UsedBlock->uiBlockID , block_id , 10);  // The third argument 10 deontes the base
            itoa((int)UsedBlock ,start_addr,10);
            itoa(UsedBlock->uiSize ,size,10);

            //UARTprintf("Block ID = %d, Start Addr = %08x , Size = %d\n",UsedBlock->uiBlockID,UsedBlock,UsedBlock->uiSize);
            print("Block ID = ");
            print(block_id);
            print(", Start Address = ");
            print(start_addr);
            print(", Size = ");
            print(size);
            print_newline();

            UsedBlock = (BLOCK_HEADER *)(UsedBlock->pChildBlock);
        }
        while (UsedBlock != NULL);

        FreeBlock = psFirstFreeBlock;
        print("\n\rFree Blocks:\n\r");
        do
        {
//            UARTprintf("Block ID = %d, Start Addr = %08x , Size = %d\n",FreeBlock->uiBlockID,FreeBlock,FreeBlock->uiSize);
            itoa(FreeBlock->uiBlockID , block_id , 10);
            itoa((int)FreeBlock,start_addr,10);
            itoa(FreeBlock->uiSize,size,10);
            print("Block ID = ");
            print(block_id);
            print(", Start Address = ");
            print(start_addr);
            print(", Size = ");
            print(size);
            print_newline();
            FreeBlock = (BLOCK_HEADER *)(FreeBlock->pChildBlock);
        }
        while (FreeBlock != NULL);
    }
    return;
}
