#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <./inc/tm4c123gh6pm.h>
#include <inc/hw_memmap.h>
#include <inc/hw_types.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/sysctl.h>
#include <driverlib/uart.h>
#include <ctype.h>

// Macros for the various colors
#define COLOR_GREEN_ON     0x08
#define COLOR_BLUE_ON      0x04
#define COLOR_RED_ON       0x02
#define COLOR_CYAN_ON      0x0C  // B + G
#define COLOR_YELLOW_ON    0x0A  // R + G
#define COLOR_MAGENTA_ON   0x06  // B + R
#define COLOR_WHITE_ON     0x0E  // B + R + G
#define NO_COLOR           0x00  // No color

void delayMs(int n);           // Software function to implement delay
void processInvalidCommand();  // Forward Declaration of the function that gives direction to the suer when an invalid command has been entered
void read_sw1();               // Function to handle sw1 press
void read_sw2();               // Function to handle sw2 press
void removeWhiteSpaces();      // Function to remove all the white spaces (space and tabs) entered in the command by the user
void pwm_setup();              // Function to establish PWM signals
void adc_conv(void);
void getTimerValue();          // Function to get the timer count value given by the user (in seconds)
void SSD_Display_Handler();    // Function to display the values in the seven segment display
void processSSD(int);          // Function to handle display of numbers in SSD (Seven Segment Display)
void delaySSD(void);           // Function to insert some delay between the glowing of various LEDs
void GPIO_PORT_AB_Init(void);  // Function to initialize PORTA and PORTB to get the display the count value in SSD
void delay_n_ms(int n);        // Function to give delay of "n" ms
void EnableInterrupts(void);   // Function to enable interrupts
void DisableInterrupts(void);  // Function to disable interrupts
void systick_setup();          // Function to setup the systick timer with the necessary conditions

char console_cmd_buffer[30];   // global character array to store the contents given by the user
char console_cmd_buffer2[30];  // global character array to store the contents given by the user without the spaces and the tabs
char val;                      // global character variable that is used across functions to read data from the console
int id = 0;                    // global variable to take care of indexing of the character buffer array
int Color = 0;                 // global variable to denote which color is currently active ( 0 indicates no color )
int Blink_Delay = 1000;        // global integer to store the blink rate
int count_sw2 = 0;             // to keep track of the number of times SW2 was pressed
int seconds = 0;               // Timer value that will be displayed in the Seven Segment Displays
int timer_start_value = 0 ;    // Temporary variable that holds the count value given by the user
int stop_flag =  1;            // 0/1 => Stop Command is  inactive / active
int pause_flag = 0;            // 0/1 => Pause Command is inactive / active (Upon resetm we assume the timer is paused)
int set_flag   = 0;            // 1 => New value has been given by the user

void inline EnableInterrupts(void)
{
    // Global interrupts enable
    __asm  ("CPSIE  I\n");      // Change processor state-enable interrupts
}


void inline DisableInterrupts(void)
{
    // Global interrupts disable
    __asm  ("CPSID  I\n");      // Change processor state-disable interrupts
}


// Function to initialize the systick timer to enable interrupts at pre-defined intervals
void systick_setup()
{

    // 1600000 is the count value that indicates that 1s has elapsed
    NVIC_ST_RELOAD_R = 16000000-1;

    // Systick Control Register
    /* Bit 0 = 1 => Counter loads the "RELOAD" value and begins counting down. On reaching 0, bit 16("COUNT") is set.
                    Along with setting of bit 16, an interrupt is generated if bit 1 (INTEN) is set. INTEN stands for "Interrupt Enable I think.
                    Then the counter reloads the "RELOAD" value again and starts counting down.
    */

    /* Bit 1 = 1 => An interrupt is generated if the systick timer counts down to 0*/
    /* Bit 2 = 1 => Systick Timer uses the System clock */


    NVIC_ST_CTRL_R = 7;
}

void SysTick_Handler(void)
{
  if(stop_flag == 1)               // stop state
  {
     if(set_flag == 0)              // Reset the counter when a new value has not been given by the user
      {
         seconds = 0;
         timer_start_value = 0;
      }

  }

  else
  {
      if(pause_flag == 1) seconds = seconds ; // Freeze the timer;
      else
      {
          seconds = seconds - 1;
          if(seconds < 0) seconds = timer_start_value;
      }
  }
}

// Function to initialize ports A and B
void GPIO_PORT_AB_Init()
{
    // The following is not required as the clock for ports A and B has been enabled in the main() funciton
     SYSCTL_RCGC2_R |= 0x00000003;       // enable clock to GPIOA, GPIOB at clock gating control register

     // Enable the GPIO pins
     GPIO_PORTA_DIR_R |= 0xF0;       // PA4 to PA7 set to output
     GPIO_PORTB_DIR_R |= 0xFF;       // PB0 to PB7 set to output

     // enable the GPIO pins for digital function
     GPIO_PORTA_DEN_R |= 0xF0;       // enabling PA4 to PA7
     GPIO_PORTB_DEN_R |= 0xFF;       // enabling PB0 to PB8
}
// Function to remove all the white spaces (space and tabs) entered in the command by the user
void removeWhiteSpaces()
{
    int i = 0,k=0;
    while(console_cmd_buffer[i] != '\0')
    {
        if(!isspace(console_cmd_buffer[i]))
            {
               console_cmd_buffer2[k] = console_cmd_buffer[i];
               k = k + 1;
            }
        i = i+1;
    }
    console_cmd_buffer2[k] = '\0'; // Null terminate the new array
}

int findColor()               // Function to determine which color needs to be lit
{
   if(strcmp(console_cmd_buffer2 ,"colorgreen")== 0)    return 1; //Green Color
   if(strcmp(console_cmd_buffer2 ,"colorblue") == 0)    return 2; //Blue Color
   if(strcmp(console_cmd_buffer2 ,"colorcyan") == 0)    return 3; //Cyan Color
   if(strcmp(console_cmd_buffer2 ,"colorred") == 0)     return 4; //Red Color
   if(strcmp(console_cmd_buffer2 ,"coloryellow") == 0)  return 5; //Yellow Color
   if(strcmp(console_cmd_buffer2 ,"colormagenta") == 0) return 6; //Magenta Color
   if(strcmp(console_cmd_buffer2 ,"colorwhite") == 0)   return 7; //White Color
   processInvalidCommand();                                       // If none of the entered data is valid, give appropriate prompt to the user
   return Color;                                                  //Invalid Data Entered (Simply return the current state of Color => Do not modify it)
}

int findBlinkRate()
{
    /*
       Although this function is not called for sw2 press, we still modify count_sw2 value here.
       This ensures continuity in the various blink rates as mentioned by the user in the console and through sw2 press
       For example if user gives the command Blink 8 , the next time switch 2 is pressed, it must be go to Blink 16.
       For that to happen, we need to modify count_sw2 variable which will be used in the switch case statements in the read_sw2() function.
     */
    if(strcmp(console_cmd_buffer2 ,"blink1") == 0)   { count_sw2 = 6; return 1000;}
    if(strcmp(console_cmd_buffer2 ,"blink2")  == 0)  { count_sw2 = 1; return 250;}
    if(strcmp(console_cmd_buffer2 ,"blink4")  == 0)  { count_sw2 = 2; return 125;}
    if(strcmp(console_cmd_buffer2 ,"blink8")  == 0)  { count_sw2 = 3; return 61.25;}
    if(strcmp(console_cmd_buffer2 ,"blink16")  == 0) { count_sw2 = 4; return 31.25;}
    if(strcmp(console_cmd_buffer2 ,"blink32") == 0)  { count_sw2 = 5; return 15.625;}
    processInvalidCommand();  // If none of the entered data is valid, give appropriate prompt to the user
    return Blink_Delay;       // Invalid Blink Command entered by the user => Retain the previous value of the blink rate
}

// Function to take care of sw1 press
void read_sw1()
{
    DisableInterrupts();
    int current_state_sw1 = GPIO_PORTF_DATA_R & 0x10;      // read the status of sw1
    if((current_state_sw1) == 0x00)
    {
        while(current_state_sw1 == 0x00)
        {
            current_state_sw1 = GPIO_PORTF_DATA_R & 0x10;  // read the data from sw1 again
            if(current_state_sw1 == 0x10)                  // switch 1 released
            {
                delay_n_ms(25);                            // Wait for 20ms before taking action
                set_flag   = 0;                            // Reset set flag while taking start and stop action
                if(stop_flag == 1)
                {

                    stop_flag  = 0; // go to start state
                    pause_flag = 0; // In start state, override pause state as the start state has higher precedence
                }
                else stop_flag = 1; // go to stop state
            }
        }
    }
    EnableInterrupts();
    return;
}

void read_sw2()
{
    DisableInterrupts();
    int current_state_sw2 = GPIO_PORTF_DATA_R & 0x01;   // read the status of sw2
    if(current_state_sw2 == 0x00)
    {
        while(current_state_sw2 == 0x00)
        {
            current_state_sw2 = GPIO_PORTF_DATA_R & 0x01;  // read the data from sw2 again
            if(current_state_sw2 == 0x01)                  // switch 2 released
            {
                delay_n_ms(25);                            // Wait for 20ms before taking action (to overcome de-bouncing)

                // Toggle between pause and resume only when stop flag is not active
                if(stop_flag == 0)
                {
                    if(pause_flag == 0) pause_flag = 1;   // Pause the timer
                    else                pause_flag = 0;   // Resume the timer
                }
            }
        }
    }
    EnableInterrupts();
    return;
}

// Functions to give directions to the user when an Invalid Command has been entered
void processInvalidCommand()
{

    UARTCharPut(UART0_BASE, '\n');
    UARTCharPut(UART0_BASE, '\r');

    int print_index = 0;
    while(console_cmd_buffer2[print_index] != '\0')
    {
        UARTCharPut(UART0_BASE, console_cmd_buffer2[print_index]);
        print_index += 1;
    }

    UARTCharPut(UART0_BASE, '\n');
    UARTCharPut(UART0_BASE, '\r');
//    if(pause_flag == 0 && stop_flag == 0)
//    {
        /* Here the if condition makes sure that that the prompt is not displayed when stop state is active */
        char* s  = "\n\n\rPlease enter any one of the following commands:\n\r"
                         "1). Timer Set\n\r"
                         "2). Timer Start\n\r"
                         "3). Timer Stop\n\r"
                         "4). Timer Pause\n\r"
                         "5). Timer Resume\n\r";
        int i = 0; // for indexing purposes
        while(s[i] != '\0')
          {
             UARTCharPut(UART0_BASE, s[i]);
             i = i+1;
          }
//    }
    return;
}

// Function to handle which Color must be blinking and at what rate the blink must be happening
void processColors()
{
    switch(Color)
       {
          case 1: GPIO_PORTF_DATA_R = COLOR_GREEN_ON;
                  delayMs(Blink_Delay);
                  GPIO_PORTF_DATA_R = NO_COLOR;
                  delayMs(Blink_Delay);
                  break;

          case 2: GPIO_PORTF_DATA_R = COLOR_BLUE_ON;
                  delayMs(Blink_Delay);
                  GPIO_PORTF_DATA_R = NO_COLOR;
                  delayMs(Blink_Delay);
                  break;

          case 3: GPIO_PORTF_DATA_R = COLOR_CYAN_ON;
                  delayMs(Blink_Delay);
                  GPIO_PORTF_DATA_R = NO_COLOR;
                  delayMs(Blink_Delay);
                  break;

          case 4: GPIO_PORTF_DATA_R = COLOR_RED_ON;
                  delayMs(Blink_Delay);
                  GPIO_PORTF_DATA_R = NO_COLOR;
                  delayMs(Blink_Delay);
                  break;

          case 5: GPIO_PORTF_DATA_R = COLOR_YELLOW_ON;
                  delayMs(Blink_Delay);
                  GPIO_PORTF_DATA_R = NO_COLOR;
                  delayMs(Blink_Delay);
                  break;

          case 6: GPIO_PORTF_DATA_R = COLOR_MAGENTA_ON;
                  delayMs(Blink_Delay);
                  GPIO_PORTF_DATA_R = NO_COLOR;
                  delayMs(Blink_Delay);
                  break;

          case 7: GPIO_PORTF_DATA_R = COLOR_WHITE_ON;
                  delayMs(Blink_Delay);
                  GPIO_PORTF_DATA_R = NO_COLOR;
                  delayMs(Blink_Delay);
                  break;

         default: GPIO_PORTF_DATA_R = NO_COLOR;   // for debugging purposes
                  break;

     }
    return;
}

// Function to empty the Character Buffer Array
void emptyBuffer()
{
    console_cmd_buffer[0]  = '\0';  // Setting the first character as a null character => Emptying the contents of the character buffer array
    console_cmd_buffer2[0] = '\0';  // Setting the first character as a null character => Emptying the contents of the character buffer array
    id = 0;                         // Setting the index of the character array as 0 as we need to start from the beginning
}

// Function to get the timer value given by the user (in seconds)
void getTimerValue()
{

    char timer_value[3];
    for(int i=0;i<3;i++)  timer_value[i] = console_cmd_buffer2[i+8];
    seconds = atoi(timer_value);
    timer_start_value = seconds;
    return;
}

// Function to handle Enter Key presses
void processEnterKey()
{


    removeWhiteSpaces(); // Copy all the contents from console_cmd_buffer to console_cmd_buffer2 except the whitespaces ( spaces and tabs)
//    // Enter key has been pressed. Process the contents of the data entered
//    if(strstr(console_cmd_buffer2 , "color") || strstr(console_cmd_buffer2 , "blink"))
//    {
//        // strstr() searches if the specified string is a subset of the contents of the character buffer array console_cmd_buffer
//         if(strstr(console_cmd_buffer2 , "color"))        Color       = findColor();                // Find out which color is requested
//         else if(strstr(console_cmd_buffer2 , "blink"))   Blink_Delay = findBlinkRate();            // Find out the blink rate
//    }

    DisableInterrupts();
    if (strstr(console_cmd_buffer2,"timerset"))              {set_flag = 1; stop_flag = 1  ; getTimerValue();}
    else if(strcmp(console_cmd_buffer2,"timerstart")  == 0)  {set_flag = 0; stop_flag  = 0 ; pause_flag = 0;}  // Start the timer and Override the pause state
    else if(strcmp(console_cmd_buffer2,"timerpause")  == 0)  {pause_flag = 1 ;}  // Pause the timer
    else if(strcmp(console_cmd_buffer2,"timerresume") == 0)  {pause_flag = 0 ;}  // Timer must continue from where it left off
    else if(strcmp(console_cmd_buffer2,"timerstop")   == 0)  {set_flag = 0; stop_flag  = 1 ;}  // Stop the timer
    else  processInvalidCommand(); // Command with neither Color nor Blink has been entered => Give appropriate prompt to user to enter valid command

    /* Once Enter Key is pressed and Color and Blink Rate have been processed, empty the contents of the character array
       to receive further instructions from the user */
    emptyBuffer();
    UARTCharPut(UART0_BASE, '\n'); // Once Enter key is pressed and the character buffer has been emptied, move on to the next line
    UARTCharPut(UART0_BASE, '\r'); // After moving to the new line, also move to the extreme left end
    EnableInterrupts();
}

// Function to handle BackSpace Key presses
void processBackSpaceKey()
{
    UARTCharPut(UART0_BASE, '\b');   //Move the cursor back by one position to the left
    UARTCharPut(UART0_BASE, ' ');    // Place a ' ' at the place where the character needs to be removed.
                                     // Once ' ' is written, cursor will once again move by one position to the right. We need to move it back again.
    UARTCharPut(UART0_BASE, '\b');   // Move the cursor back again.
    id = id-1;                         // Decrement the index as a character has been removed
    console_cmd_buffer[id] = '\0';    // Terminate the character array at the appropriate position
}

// Function to handle key press that is neither "Enter" nor "Back Space"
void processNormalKey()
{
    //UARTCharPut(UART0_BASE, val);  // Echo from the TIVA board back to the PC
    /*
       We DO NOT perform the lower case conversion here as the characters entered by
       the users need to be printed without any modification
     */
    if(id<=29) console_cmd_buffer[id] = (val);
    UARTCharPut(UART0_BASE, console_cmd_buffer[id]); // This makes the characters appear on the console (This is the Echo Operation)
    console_cmd_buffer[id] = tolower(val);           // Convert everything to lower cases characters and store in the buffer array
    id = id +1;
    if(id>=0 && id<=29)  console_cmd_buffer[id] = '\0'; // Terminate the array at a valid position
}


// Function to handle Seven Segment displays
void processSSD(int ssd)
{

   int disp_num;  // Number to be displayed on the SSD
   // Extracting digits
   int units = seconds % 10;         // digit in units place of time elapsed
   int tens = (seconds / 10) % 10;   // digit in tens place of the time elapsed
   int hundreds = seconds / 100;     // digit in hundredth place of the time elapsed

   if(ssd == 2)
   {
       // PA6 = 1 => Second SSD from the left is chosen to display the hundreds digit of the count value
//       GPIO_PORTA_DATA_R     = (0x40);
       GPIO_PORTA_DATA_R     = (0x40) | (GPIO_PORTA_DATA_R & 0x0F);
       disp_num = hundreds;
   }

   else if(ssd == 3)
   {
       // PA5 = 1 => Third SSD from the left is chosen to display the tens digit of the count value
//       GPIO_PORTA_DATA_R     = 0x20;
       GPIO_PORTA_DATA_R     = (0x20) | (GPIO_PORTA_DATA_R & 0x0F);
       disp_num = tens;
   }

   else
   {
       // PA4 = 1 => Right most SSD is chosen to display the units place of the count value
//       GPIO_PORTA_DATA_R = 0x10;
       GPIO_PORTA_DATA_R     = (0x10) | (GPIO_PORTA_DATA_R & 0x0F);
       disp_num = units;
   }


   switch(disp_num)
   {
     case 1  : GPIO_PORTB_DATA_R = 0x06; break;
     case 2  : GPIO_PORTB_DATA_R = 0x5B; break;
     case 3  : GPIO_PORTB_DATA_R = 0x4F; break;
     case 4  : GPIO_PORTB_DATA_R = 0x66; break;
     case 5  : GPIO_PORTB_DATA_R = 0x6D; break;
     case 6  : GPIO_PORTB_DATA_R = 0x7D; break;
     case 7  : GPIO_PORTB_DATA_R = 0x07; break;
     case 8  : GPIO_PORTB_DATA_R = 0x7F; break;
     case 9  : GPIO_PORTB_DATA_R = 0x6F; break;
     default : GPIO_PORTB_DATA_R = 0x3F; break;
   }

   return;
}


void SSD_Display_Handler()
{
    processSSD(2);   // Second SSD from the left
    delaySSD();

    processSSD(3);   // Third SSD from the left
    delaySSD();

    processSSD(4);   // Fourth SSD from the left
    delaySSD();
}

void pwm_setup()
{
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
    GPIO_PORTA_DATA_R |= (1<<3);         // Setting PA3 high without affecting other bits of Port A
    GPIO_PORTA_DATA_R &= ~(1<<2);        // Resetting PA2 without affecting other bits of Port A
    //GPIO_PORTA_DATA_R &= ~(1<<3);
    //GPIO_PORTA_DATA_R |= (1<<2);



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
}

void adc_conv()
{
    volatile int result;
    int x = 4999;
    float temp;
    int temp2;
    ADC0_PSSI_R |= 8; /* start a conversion sequence 3 */
    while((ADC0_RIS_R & 8) == 0)
    ; /* wait for conversion complete */
    result = ADC0_SSFIFO3_R; /* read conversion result */
    ADC0_ISC_R = 8; /* clear completion flag */
    temp=(((result)/4095.0)*4999.0);
    temp2=(int)temp;
    x=4999-temp2;
    PWM1_3_CMPA_R=x;
//    delay_n_ms(1000);
}


int main()
{

    EnableInterrupts();  // Enable the interrupts
    systick_setup();     // Setup the systick timer

    // Necessary definitions to write data into Port F
    SYSCTL_RCGC2_R |=   0x00000020;   /* enable clock to GPIOF at clock gating control register */
    GPIO_PORTF_DIR_R =  0x0E;        /* enable the GPIO pins for the LED (PF3, 2 1) as output */
    GPIO_PORTF_LOCK_R = 0x4C4F434B;   /* unlock commit register GPIOCR */
    GPIO_PORTF_CR_R =   0x01;         /* make PORTF0 configurable */
    GPIO_PORTF_DEN_R =  0x1F;        // Enabling all the 5 pins of port 5 for digital activity
    GPIO_PORTF_PUR_R =  0x11;        /* enable pull up for pin 4 (SW1) and pin 0 (SW2). If this is not present, I think SW1 and SW2 are always considered to be pressed*///

    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    pwm_setup();  // Function to set up PWM signals
    GPIO_PORT_AB_Init(); // Initialize PORT B and PORT A for SSD display purposes
    while(1)
    {
        while(!(UARTCharsAvail(UART0_BASE)))
          {
            read_sw1();       // Check for sw1 press even when the user has not given any new command
            read_sw2();       // Check for sw2 press even when the user has not given any new command
//            processColors();  // Maintain LED Blink state even when there is no new command
            adc_conv();
            SSD_Display_Handler();
          }
        val = UARTCharGet(UART0_BASE);
        if((val == 0x0D))                 processEnterKey();         // If entered character is 0x0D => Enter key has been pressed
        else if(val == 0x08 && id>0)      processBackSpaceKey();     // If entered character is 0x08 => Back Space Key has been pressed
        else                              processNormalKey();        // To process Key press that is Neither "Enter" nor "Back Space"
        read_sw1();                                                  // Keep checking for sw1 press
        read_sw2();                                                  // Keep checking for sw2 press
        processColors();                                             // Maintain LED blink state even in the middle of a new command
        adc_conv();
        SSD_Display_Handler();
    }

  return 0;
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
        // After every 1ms check the input entered by the user

       if(UARTCharsAvail(UART0_BASE))
       {
           val = UARTCharGet(UART0_BASE);
           if((val == 0x0D))                 processEnterKey();         // If entered character is 0x0D => Enter key has been pressed
           else if(val == 0x08 && id>0)      processBackSpaceKey();     // If entered character is 0x08 => Back Space Key has been pressed
           else                              processNormalKey();        // To process Key press that is Neither "Enter" nor "Back Space"
       }
       read_sw1();                                                      // Check the status of sw1 periodically
       read_sw2();                                                      // Check the status of sw2 periodically
       SSD_Display_Handler();
      }
   return;  // sw1 not pressed during the entire duration of the delay
}

void delaySSD()
{

    for(int j = 0; j<3180; j++)
       {
            // Do nothing for 1 ms
       }
}


void delay_n_ms(int n)
{
     int i, j;
    for(i = 0 ; i < n; i++)
    for(j = 0; j < 3180; j++) {}
}
