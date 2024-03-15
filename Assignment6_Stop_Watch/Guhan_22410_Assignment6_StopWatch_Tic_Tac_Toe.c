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

// Macros for the various colors
#define COLOR_GREEN_ON     0x08
#define COLOR_BLUE_ON      0x04
#define COLOR_RED_ON       0x02
#define COLOR_CYAN_ON      0x0C  // B + G
#define COLOR_YELLOW_ON    0x0A  // R + G
#define COLOR_MAGENTA_ON   0x06  // B + R
#define COLOR_WHITE_ON     0x0E  // B + R + G
#define NO_COLOR           0x00  // No color

void GPIO_setup();                   // Function to initialize the necessary ports required for Systick Timer Assignment
void GPIOE_INIT();                   // Function to Initialize Port E of TIVA
void GPIOC_INIT();                   // Function to Initialize Port C of TIVA
void GPIOF_INIT();                   // Function to Initialize Port F of TIVA
void GPIOAB_INIT();                  // Function to Initialize Ports A and B of TIVA
void delayMs(int n);                 // Software function to implement delay
void processInvalidCommand();        // Forward Declaration of the function that gives direction to the suer when an invalid command has been entered
void read_sw1();                     // Function to handle sw1 press
void read_sw2();                     // Function to handle sw2 press
void removeWhiteSpaces();            // Function to remove all the white spaces (space and tabs) entered in the command by the user
void StartStopCommand_Console();     // Function to handle Start and Stop Commands given through console
void StartStopCommand_Keypad();      // Function to handle Start and Stop Commands given through keypad
void ResumePauseCommand_Console();   // Function to handle Resume and Pause Commands given through console
void ResumePauseCommand_Keypad();    // Function to handle Resume and Pause Commands given through keypad
int  detectKeyPress();               // Function to detect if any one of the keys in the 4x4 keypad was pressed
int  processKeyPress();              // Function to check which one of the keys in the 4x4 keypad was pressed
void processSSD(int);                // Function to handle display of numbers in SSD (Seven Segment Display)
void delaySSD(void);                 // Function to insert some delay between the glowing of various LEDs
void SSD_Display_Handler();          // Function to handle display of numbers in SSD with delay in between various SSDs
void LCD_RecCommand();               // Function to send necessary control signals to LCD to receive a Command
void LCD_RecData();                  // Function to send necessary control signals to LCD to receive Data
void LCD_init();                     // Function to initialize the LCD
void delay_1Ms_LCD_Starter();        // Delay used to send high to low pulse on PA7 (Enable Line of LCD)
void processPeek();                  // Function to process Peek Command
void processPoke();                  // Function to process Poke Command
void EnableInterrupts(void);         // Function to enable interrupts
void systick_setup(void);            // Function to initialize the Systick Timer
void gameArray_init(void);           // Function to initialize the Character array that will be used to play tic tac toe
void print_game_status(void);        // Function to print the 3x3 matrix of the tic tac toe game
void check_game_status(void);        // Function to check the status of the tic tac toe game and to give the necessary verdict

char console_cmd_buffer[50];       // global character array to store the contents given by the user
char console_cmd_buffer2[50];      // global character array to store the contents given by the user without the spaces and the tabs
char val;                          // global character variable that is used across functions to read data from the console
int id = 0;                        // global variable to take care of indexing of the character buffer array
int Color = 0;                     // global variable to denote which color is currently active ( 0 indicates no color )
int Color_Pause = 0;               // global variable to save the Color state when pause command is active
int Blink_Delay = 250;             // global variable to store the blink delay
int Blink_Pause = 0;               // global variable to save the blink delay value before entering into pause state
int Blink_Delay_Pause = 250;       // global variable to save the blink delay before going into pause state
int sw1_pressed = 0;               // flag to indicate if sw1 has been pressed
int sw1_delay = 0;                 // flag to keep counting to avoid debouncing issues in sw1 press (Systick Handling)
int sw2_pressed = 0;               // flag to indicate if sw2 has been pressed
int sw2_delay  = 0;                // flag to keep counting to avoid debouncing issues in sw1 press (Systick Handling)
int count_sw2 = 0;                 // to keep track of the number of times SW2 was pressed
int stop_flag =  1;                // 0/1 => Stop Command is  inactive / active [For the systick assignment, we assume that the stop state is active upon reset]
int pause_flag = 0;                // 0/1 => Pause Command is inactive / active
int num = 0;                       // This variable is used in ProcessKeyPress() function
int key1_pressed_first_time = 0;   // 0/1 => Key 1 has not been pressed at all / Key 1 has been pressed
int color_change = 0;              // Number of color changes that will be displayed on the first two seven segment displays, starting from the right
char poke_content[100];            // To store the content to be poked and displayed on LCD
extern char version[30];           // Declaring here. For memory allocation, refer .lds file
int time_elapsed = 0;              // Number to be displayed on the SSD (Updated using Systick_Handler)
int seconds = 0;                   // Time elapsed in seconds (To be displayed on the first 3 SSDs)
int curr_state = 0;                // 0/1/2 -> Ready / Running / Pause
int prev_state = 0;                // 0/1/2 -> Ready / Running / Pause
char game[4][4];                   // Array to hold the contents of the tic tac toe game
int moves = 0;                     // Global variable to keep track of the number of moves played in the tic tac toe game
int tic_tac_toe_row[] = {3,0,0,0, 3,1,1,1, 3,2,2,2, 3,3,3,3};  // Used for indexing purposes in tic tac toe game
int tic_tac_toe_col[] = {3,0,1,2, 3,0,1,2, 3,0,1,2, 3,3,3,3};  // Used for indexing purposes in tic tac toe game


void GPIO_setup()
{
    SYSCTL_RCGC2_R |= 0x00000023;     /* enable clock to GPIOA, GPIOB and GPIOF at clock gating control register */
    GPIO_PORTF_LOCK_R = 0x4C4F434B;   /* unlock commit register GPIOCR */
    GPIO_PORTF_CR_R = 0x01;           /* make PORTF0 configurable */
    // Enable the GPIO pins
    // For PORTF the LED (PF3, 2 1) as output
    // For PORTB, all pins are used to set 7 segment display
    // For PORTA, pins 7 to 4 are used for selecting one of the four 7 segment display
    GPIO_PORTA_DIR_R |= 0xF0;
    GPIO_PORTB_DIR_R |= 0xFF;
    // X X X SW1 G B R SW2 -> bits in PORTF
    GPIO_PORTF_DIR_R |= 0x0E;
    // enable the GPIO pins for digital function
    GPIO_PORTA_DEN_R = 0xF0;
    GPIO_PORTB_DEN_R = 0xFF;
    GPIO_PORTF_DEN_R = 0x1F;
    GPIO_PORTF_PUR_R = 0x11;          // enable pull up for SW1 and SW2

    // Interrupt setup for sw1 and sw2
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
    NVIC_PRI7_R = (NVIC_PRI7_R & 0xFF00FFFF) | 0x00A00000;
    NVIC_EN0_R = 0x40000000;        // NVIC enabled for PORTF (IRQ number = 30)


    // Initializing PORT E  for 4x4 Keypad Interfacing
    SYSCTL_RCGC2_R    |= 0x00000010 ;       /* Enable clock to GPIO_E_ at clock gating control register */
    SYSCTL_RCGCGPIO_R |= 0x00000010 ;       /* Enable and provide a clock to GPIO Port_E_ in Run mode */
    GPIO_PORTE_DIR_R  |= 0x0F ;             /*  GPIO Direction | 0 -> INPUT | 1 -> OUTPUT  (We are configuring PORTE as output and connecting it to rows of 4x4 Keypad)*/
    GPIO_PORTE_DEN_R  |= 0x0F;              /* enable the GPIO pins for digital function */
    GPIO_PORTE_DATA_R  = 0x00 ;             /* Initializing PORTE Data to all 0s*/

    // Initializing PORT C for 4x4  KeyPad Interfacing
    SYSCTL_RCGC2_R    |= 0x00000004 ;       /* Enable clock to GPIO_C_ at clock gating control register */
    SYSCTL_RCGCGPIO_R |= 0x00000004 ;       /* Enable and provide a clock to GPIO Port_C_ in Run mode */
    GPIO_PORTC_DIR_R  &= ~ 0xF0 ;           /*  GPIO Direction | 0 -> INPUT | 1 -> OUTPUT (We are configuring PORTC as input and connecting it to columns of 4x4 Keypad)*/
    GPIO_PORTC_ODR_R |= 0xF0 ;              /* 1 -> The corresponding pin is configured as open drain */
    GPIO_PORTC_DEN_R |= 0xF0;               /* enable the GPIO pins for digital function */
    GPIO_PORTC_PUR_R |= 0xF0 ;              /* 1 -> The corresponding pin's weak pull-up resistor is enabled */
}

void GPIOAB_INIT()
{
   // The following is not required as the clock for ports A and B has been enabled in the main() funciton
   // SYSCTL_RCGC2_R |= 0x00000003;       // enable clock to GPIOA, GPIOB at clock gating control register

    // Enable the GPIO pins
    GPIO_PORTA_DIR_R |= 0xF0;       // PA4 to PA7 set to output
    GPIO_PORTB_DIR_R |= 0xFF;       // PB0 to PB7 set to output

    // enable the GPIO pins for digital function
    GPIO_PORTA_DEN_R |= 0xF0;       // enabling PA4 to PA7
    GPIO_PORTB_DEN_R |= 0xFF;       // enabling PB0 to PB8
}


//Function to Initialize PortC of TIVA
void GPIOC_INIT()
{
    SYSCTL_RCGC2_R    |= 0x00000004 ;       /* Enable clock to GPIO_C_ at clock gating control register */
    SYSCTL_RCGCGPIO_R |= 0x00000004 ;       /* Enable and provide a clock to GPIO Port_C_ in Run mode */
    GPIO_PORTC_DIR_R  &= ~ 0xF0 ;           /*  GPIO Direction | 0 -> INPUT | 1 -> OUTPUT (We are configuring PORTC as input and connecting it to columns of 4x4 Keypad)*/
    GPIO_PORTC_ODR_R |= 0xF0 ;              /* 1 -> The corresponding pin is configured as open drain */
    GPIO_PORTC_DEN_R |= 0xF0;               /* enable the GPIO pins for digital function */
    GPIO_PORTC_PUR_R |= 0xF0 ;              /* 1 -> The corresponding pin's weak pull-up resistor is enabled */
}

// Function to Initialize PortE of TIVA
void GPIOE_INIT()
{
    SYSCTL_RCGC2_R    |= 0x00000010 ;       /* Enable clock to GPIO_E_ at clock gating control register */
    SYSCTL_RCGCGPIO_R |= 0x00000010 ;       /* Enable and provide a clock to GPIO Port_E_ in Run mode */
    GPIO_PORTE_DIR_R  |= 0x0F ;             /*  GPIO Direction | 0 -> INPUT | 1 -> OUTPUT  (We are configuring PORTE as output and connecting it to rows of 4x4 Keypad)*/
    GPIO_PORTE_DEN_R  |= 0x0F;              /* enable the GPIO pins for digital function */
    GPIO_PORTE_DATA_R  = 0x00 ;             /* Initializing PORTE Data to all 0s*/
    return;
}

// Function to initialize PortF of TIVA
void GPIOF_INIT()
{
    GPIO_PORTF_LOCK_R = 0x4C4F434B;   /* unlock commit register GPIOCR */
    GPIO_PORTF_CR_R = 0x01;           /* make PORTF0 configurable */
    GPIO_PORTF_DIR_R =  0x0E;         /* enable the GPIO pins for the LED (PF3, 2 1) as output */
    GPIO_PORTF_DEN_R =  0x1F;        // Enabling all the 5 pins of port 5 for digital activity
    GPIO_PORTF_PUR_R =  0x11;        /* enable pull up for pin 4 (SW1) and pin 0 (SW2). If this is not present, I think SW1 and SW2 are always considered to be pressed*///

    // Interrupt setup for sw1 and sw2
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
    NVIC_PRI7_R = (NVIC_PRI7_R & 0xFF00FFFF) | 0x00A00000;
    NVIC_EN0_R = 0x40000000;        // NVIC enabled for PORTF (IRQ number = 30)
}

// Function to initialize the systick timer to enable interrupts at pre-defined intervals
void systick_setup()
{
    // reload with number of clocks per second
    // 1600000 is the count value that indicates that 100ms (0.1s) has elapsed
    NVIC_ST_RELOAD_R = 1600000-1;

    // Systick Control Register
    /* Bit 0 = 1 => Counter loads the "RELOAD" value and begins counting down. On reaching 0, bit 16("COUNT") is set.
                    Along with setting of bit 16, an interrupt is generated if bit 1 (INTEN) is set. INTEN stands for "Interrupt Enable I think.
                    Then the counter reloads the "RELOAD" value again and starts counting down.
    */

    /* Bit 1 = 1 => An interrupt is generated if the systick timer counts down to 0*/
    /* Bit 2 = 1 => Systick Timer uses the System clock */


    NVIC_ST_CTRL_R = 7;
}

// Function to initialize the character array that will be used for playing tic tac toe
void gameArray_init()
{
   for(int i=0;i<4;i++)
   {
       for(int j=0;j<4;j++)
       {
           game[i][j] = '-';
       }
   }
}

void print_game_status()
{
    UARTCharPut(UART0_BASE,'\n'); // Start from the next line
    UARTCharPut(UART0_BASE,'\r'); // Start from the extreme left
    for(int i=0;i<3;i++)
    {
        for(int j=0;j<3;j++)
        {
            UARTCharPut(UART0_BASE,game[i][j]);
            UARTCharPut(UART0_BASE,' ');
        }
        UARTCharPut(UART0_BASE,'\n'); // Start from the next line
        UARTCharPut(UART0_BASE,'\r'); // Start from the extreme left
    }
    return;
}

void print_invalid_move()
{
    char* s  = "\n\nInvalid Move\n\r";
    int i = 0; // for indexing purposes
    while(s[i] != '\0')
      {
         UARTCharPut(UART0_BASE, s[i]);
         i = i+1;
      }
    return;
}

void check_game_status()
{
   int row = 0,col=0;
   num = processKeyPress();            // Identify which key was pressed in the 4x4 keypad
   if(num != -1) row = tic_tac_toe_row[num];     // Get the row    index corresponding to the key press of the 4x4 keypad
   if(num != -1) col = tic_tac_toe_col[num];     // Get the column index corresponding to the key press of the 4x4 keypad

   while(detectKeyPress() != -1)
   {
       // Stay in this loop to handle the case where the switch is pressed for a long time before releasing
   }
   // Some delay to overcome debouncing issue
   for(int i=0;i<80;i++) delaySSD();

   if((game[row][col] == 'X') || (game[row][col] == 'O')) print_invalid_move();  // Place already used. Hence it is an invalid move
   else
    {
       game[row][col] = (moves%2 == 0) ? 'X' : 'O'; // To alternate between X and O
       moves += 1; // Increment the number of moves so that the other guy can enter his/her move the next time
    }

       print_game_status(); // After every entry , print the status of the game

   return;
}
// The function LCD_RecCommand() sets RS(PA6) = 0 and a high to low pulse on E pin(PA7). This will help us send commands to the LCD display
void LCD_RecCommand()
{
    GPIO_PORTA_DATA_R  &=  0xBF;  // Set PA6 to 0, keep other bits unaffected
    GPIO_PORTA_DATA_R  |=  0x80;  // Set PA7 to 1, keep other bits unaffected
    delay_1Ms_LCD_Starter();      // Add some delay
    GPIO_PORTA_DATA_R  &=  0x7F;  // Set PA7 to 0, keep other bits unaffected
}

// The function LCD_RecData() sets RS(PA6) = 1 and a high to low pulse on E pin (PA7). This will help us send data to the LCD Display
void LCD_RecData()
{
    GPIO_PORTA_DATA_R  |=  0x40;  // Set PA6 to 1, keep other bits unaffected
    GPIO_PORTA_DATA_R  |=  0x80;  // Set PA7 to 1, keep other bits unaffected
    delay_1Ms_LCD_Starter();      // Add some delay
    GPIO_PORTA_DATA_R  &=  0x7F;  // Set PA7 to 0, keep other bits unaffected
}

void LCD_init()
{
    /*
       Each time we load portB with necessary command, we need to send appropriate control
       signals to PORTA so that it can latch on to the data.
    */
    GPIO_PORTB_DATA_R = 0x38; LCD_RecCommand();   // 2 Lines and 5×7 character (8-bit data, D0 to D7)
    GPIO_PORTB_DATA_R = 0x01; LCD_RecCommand();   // clear Display Screen
    GPIO_PORTB_DATA_R = 0x0F; LCD_RecCommand();   // Display ON, Cursor Blinking
    GPIO_PORTB_DATA_R = 0x80; LCD_RecCommand();   // Force Cursor to beginning of 1st Line
}

void LCD_PutData(char* data,int size_int)
{
    /* Before writing data into LCD, we first need to clear the existing contents that are currently displayed on the LCD */
    GPIO_PORTB_DATA_R = 0x01; LCD_RecCommand();   // clear Display Screen
    GPIO_PORTB_DATA_R = 0x0F; LCD_RecCommand();   // Display ON, Cursor Blinking
    GPIO_PORTB_DATA_R = 0x80; LCD_RecCommand();   // Force Cursor to beginning of 1st Line

    /*
       Each time we load portB with necessary data, we need to send appropriate control
       signals to PORTA so that it can latch on to the data.
    */

//    int i = 0;
    for(int i = 0;i<size_int;i++)
    {
        GPIO_PORTB_DATA_R = data[i]; LCD_RecData();
        // i = i+1;
        // Move to second line if first line is filled up
        if(i == 16)
        {
            GPIO_PORTB_DATA_R = 0xC0; // Force Cursor To Second Line
            LCD_RecCommand();         // Send Necessary Control Signals
        }
    }
    return;
}

// Function to remove all the white spaces (space and tabs) entered in the command by the user
void removeWhiteSpaces()
{
    int i = 0,k=0;
    while(console_cmd_buffer[i] != '\0')
    {

        if(!isspace(console_cmd_buffer[i]))
            {
               console_cmd_buffer2[k] = tolower(console_cmd_buffer[i]);
               k = k + 1;
            }
        i = i+1;
    }
    console_cmd_buffer2[k] = '\0'; // Null terminate the new array
}

int findColor()               // Function to determine which color needs to be lit
{
   if(strcmp(console_cmd_buffer2 ,"colorgreen")== 0)    {color_change += 1; return 1;} //Green Color
   if(strcmp(console_cmd_buffer2 ,"colorblue") == 0)    {color_change += 1; return 2;} //Blue Color
   if(strcmp(console_cmd_buffer2 ,"colorcyan") == 0)    {color_change += 1; return 3;} //Cyan Color
   if(strcmp(console_cmd_buffer2 ,"colorred") == 0)     {color_change += 1; return 4;} //Red Color
   if(strcmp(console_cmd_buffer2 ,"coloryellow") == 0)  {color_change += 1; return 5;} //Yellow Color
   if(strcmp(console_cmd_buffer2 ,"colormagenta") == 0) {color_change += 1; return 6;} //Magenta Color
   if(strcmp(console_cmd_buffer2 ,"colorwhite") == 0)   {color_change += 1; return 7;} //White Color
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
    if(strcmp(console_cmd_buffer2 ,"blink1") == 0)   { count_sw2 = 6; return 250;}
    if(strcmp(console_cmd_buffer2 ,"blink2")  == 0)  { count_sw2 = 1; return 125;}
    if(strcmp(console_cmd_buffer2 ,"blink4")  == 0)  { count_sw2 = 2; return 62.5;}
    if(strcmp(console_cmd_buffer2 ,"blink8")  == 0)  { count_sw2 = 3; return 30.625;}
    if(strcmp(console_cmd_buffer2 ,"blink16")  == 0) { count_sw2 = 4; return 15.75;}
    if(strcmp(console_cmd_buffer2 ,"blink32") == 0)  { count_sw2 = 5; return 7.8125;}
    processInvalidCommand();  // If none of the entered data is valid, give appropriate prompt to the user
    return Blink_Delay;       // Invalid Blink Command entered by the user => Retain the previous value of the blink rate
}

// Function to take care of sw1 press
void read_sw1()
{
    if(pause_flag == 0 && stop_flag == 0)
    {
        sw1_pressed = 0;
        int current_state_sw1 = GPIO_PORTF_DATA_R & 0x10;      // read the status of sw1
        if((current_state_sw1) == 0x00)
        {
            while(current_state_sw1 == 0x00)
            {
                current_state_sw1 = GPIO_PORTF_DATA_R & 0x10;  // read the data from sw1 again
                if(current_state_sw1 == 0x10)
                {
                    sw1_pressed = 1;                           // consider sw1 to be pressed
                    color_change += 1;                         // number of color changes will be displayed on the first two SSDs from the left
                    Color = Color + 1;
                    if(Color == 8) Color = 1;
                    break;
                }
            }
        }
     }
    return;
}

void read_sw2()
{
    if(pause_flag == 0 && stop_flag == 0)
     {
        int current_state_sw2 = GPIO_PORTF_DATA_R & 0x01;   // read the status of sw2
        sw2_pressed = 0;
        if(current_state_sw2 == 0x00)
        {
            while(current_state_sw2 == 0x00)
            {
                current_state_sw2 = GPIO_PORTF_DATA_R & 0x01;  // read the data from sw2 again
                if(current_state_sw2 == 0x01)                     // If value is still 0, consider sw2 to be pressed.
                {
                    sw2_pressed = 1;                           // consider sw2 to be pressed
                    count_sw2 = count_sw2 + 1;                 // increment the count value of sw2 press to increase the blink speed of the LED
                    if(count_sw2 == 7) count_sw2 = 1;
                    switch(count_sw2)
                    {
                       case 1: Blink_Delay = 125;   // blink 2 times in two seconds  => blink once in 1 second
                               break;

                       case 2: Blink_Delay = 62.5;    // blink 4 times in two seconds  => blink 2 times in 1 second
                               break;

                       case 3: Blink_Delay = 31.25;  // blink 8 times in two seconds  => blink 4 times in 1 second
                               break;

                       case 4: Blink_Delay = 15.625;  // blink 16 times in two seconds => blink 8 times in 1 second
                               break;

                       case 5: Blink_Delay = 7.8125;  // blink 32 times in two seconds => blink 16 times in one second => MAX SPEED (LED Constantly On)
                               break;

                       case 6: Blink_Delay = 250;   // After max speed switch back to blinking once in 2 seconds  (This is the lowest speed)
                               break;
                    }
                }
            }
        }
     }
   return;
}

// Function for key Press Detection
int detectKeyPress()
{
    GPIO_PORTE_DATA_R = 0x00 ;   // Drive all the rows of the 4x4 keypad low

    if( (GPIO_PORTC_DATA_R & 0xF0) != 0xF0 )
        return 1;    // Data in (PC7 - PC4) != 1111  => Some key was pressed
    else
        return -1;    // Data in (PC7 - PC4)  = 1111 => No key was pressed
}

// Function for Key Press Identification
int processKeyPress()
{
        int row = 0;
        for(row = 0; row < 3; row ++)
        {
            if(row == 0)         GPIO_PORTE_DATA_R = 0x0E;      // Driving PE0 low
            else if(row == 1)    GPIO_PORTE_DATA_R = 0x0D;      // Driving PE1 low
            else                 GPIO_PORTE_DATA_R = 0x0B;      // Driving PE2 low
            num = GPIO_PORTC_DATA_R & 0xF0 ;             // num stores the value of data in PC4 - PC7
            if(num != 0xF0)                             // Some key was pressed
            {
                 switch(num)
                 {
                   case 0xE0: return (row*4) + 1;  // 1st column from the left
                   case 0xD0: return (row*4) + 2;  // 2nd column from the left
                   case 0xB0: return (row*4) + 3;  // 3rd column from the left
                   case 0x70: return (row*4) + 4;  // 4th column from the left
                 }
            }
        }

    return -1; //invalid position
}

// Function to handle Seven Segment displays
void processSSD(int ssd)
{
   int disp_num;  // Number to be displayed on the SSD
   // Extracting digits
   int units = seconds % 10;         // digit in units place of time elapsed
   int tens = (seconds / 10) % 10;   // digit in tens place of the time elapsed
   int hundreds = seconds / 100;     // digit in hundredth place of the time elapsed

   if(ssd == 1)
   {
       // PA7 = 1 => Left most SSD is chosen to display the tens digit of the color change
       GPIO_PORTA_DATA_R     = 0x80;
       disp_num = hundreds;
   }

   else if(ssd == 2)
   {
       // PA6 = 1 => Second SSD from the left is chosen to display the tens digit of the color change
       GPIO_PORTA_DATA_R     = 0x40;
       disp_num = tens;
   }

   else if(ssd == 3)
   {
       // PA5 = 1 => Third SSD from the left is chosen to display the tens digit of the color change
       GPIO_PORTA_DATA_R     = 0x20;
       disp_num = units;
   }

   else
   {
       // PA4 = 1 => Right most SSD is chosen to display the color code (Set PA4 to 1 without affecting PA3 - PA0)
       GPIO_PORTA_DATA_R = 0x10;
       disp_num = time_elapsed;
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

   // Display the DOT as well
   if(ssd == 3)
   {
      GPIO_PORTA_DATA_R     = 0x20; // Select the third SSD from the left
      GPIO_PORTB_DATA_R    |= 0x80; // Display the numbers as well as the DOT
   }

   return;
}

void SSD_Display_Handler()
{
    // time_elapsed is incremented in SysTick_Handler() function which is once in every 100ms (0.1s)
    // Another way of saying it is time_elapsed is incremented once for every 100ms (0.1s)

    // Do NOT display 0 in the left most SSD.
    // Because this leads to issues when we are displaying things in the LCD
    // Hence display valuen in the left most SSD only when seconds >= 100
    if(seconds >= 100)
    {
        processSSD(1);
        delaySSD();
    }


    processSSD(2);
    delaySSD();

    processSSD(3);
    delaySSD();

    processSSD(4);
    delaySSD();


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

// Function to  the Character Buffer Array
void emptyBuffer()
{
    console_cmd_buffer[0]  = '\0';  // Setting the first character as a null character => Emptying the contents of the character buffer array
    console_cmd_buffer2[0] = '\0';  // Setting the first character as a null character => Emptying the contents of the character buffer array
    id = 0;                         // Setting the index of the character array as 0 as we need to start from the beginning
}

// Function to handle Start and Stop Commands that is given through console
void StartStopCommand_Console()
{

    pause_flag = 0;
    if(strcmp(console_cmd_buffer2,"timerstart") == 0)
    {
        Color = 1;              // Set color to green
        Blink_Delay = 50;       // Set delay to the least value
        stop_flag = 0;          // System in START state
        curr_state = 1;
        return;
    }
    else if(strcmp(console_cmd_buffer2,"timerstop") == 0)
    {
        Color = 1;
        Blink_Delay = 1;  // Good enough to keep the LED continuosly ON
        time_elapsed = 0; // Reset the count value
        seconds      = 0; // Reset the count value
        stop_flag = 1;    // System in STOP state
        curr_state = 0;
        return;
    }
    else return;
}

// Function to handle Start and Stop Commands that is given through key1 of 4x4 keypad
void StartStopCommand_Keypad()
{
    pause_flag = 0; // reset the pause state as the start/stop command will have higher precedence
    int key1 = processKeyPress();
    if(key1== 1)
    {
      while(key1 == 1)
      {
         key1 = processKeyPress();
         if(key1 != 1)
         {
             /* Start the color if one of the following cases is true:
                1). Stop feature is currently active OR
                2). Key 1 is pressed for the very first time and stop feature is not active
              */
             if (    (stop_flag == 1) // Stop feature is active
                  || (key1_pressed_first_time == 0 && stop_flag == 0)
                )
              {
                 key1_pressed_first_time = 1;
                 color_change = 1;                // Start of system indicates first color (green)
                 Color = 1;                       // Set color to green
                 Blink_Delay = 250;              // Set speed to the lowest speed
                 stop_flag = 0;                   // Set stop feature to inactive
                 return;
              }

             if(stop_flag == 0) // Stop feature is currently not active
              {
                Color        = 0;  // Set color to no color
                color_change = 0;  // Stop of system indicates no color is active
                stop_flag    = 1;  // Stop the blinking
                return;
              }
         }
      }
    }
    return;
}
// Function to handle Resume and Pause Commands
void ResumePauseCommand_Console()
{

    /*
     Handle Pause and Resume Commands only when the system is NOT in the STOP state
     */
    if(stop_flag == 0)
    {
        /* Here the pause_flag == 0 check will make sure that in the case of user entering multiple
           pause commands continuously, only the first pause command is considered
        */
        if(    (strcmp(console_cmd_buffer2, "timerpause") == 0)
            && (pause_flag == 0)
          )
        {
           // Save the Color and Blink Delay before going into pause state
           // Once state of the color is saved , set color to No color
           pause_flag  = 1;   // System in PAUSE state
           Color       = 2;   // Blue in pause state
           Blink_Delay = 50;  // Just a random number chosen as delay for the Systick Assignment
           curr_state = 2;    // Flag to indicate PAUSE state for setting appropriate text on LCD
           return;
        }
        if((strcmp(console_cmd_buffer2 , "timerresume") == 0))
        {
            pause_flag = 0;    // System in RESUME state
            Color = 1;         // Green color in resume state
            Blink_Delay = 50;  // Just a random number chosen as delay for the Systick Assignment
            curr_state = 1;  // Flag to indicate RESUME state for setting appropriate text on LCD
            return;
        }
    }
    return;
}

void ResumePauseCommand_Keypad()
{
    int key2 = processKeyPress();      // Check which key is pressed
    if(key2 == 2)                      // If key 2 is pressed enter the "if condition"
    {
        while(key2 == 2)               // As long as key 2 is pressed stay in the while loop
        {
            key2 = processKeyPress();  // Keep checking the status of key 2
            if(key2 != 2)              // If key 2 has been released then consider that as a valid key 2 press and take necessary action
            {
                // If pause state is inactive => Make it active
                if(pause_flag == 0)
                {
                    pause_flag  = 1;    // to indicate that pause state is active
                    Color_Pause = Color;
                    Blink_Delay_Pause = Blink_Delay;
                    Color = 0;
                    return;
                }

                // if pause is active => disable it
                if(pause_flag == 1)
                {
                    pause_flag = 0;
                    Color = Color_Pause;
                    Blink_Delay = Blink_Delay_Pause;
                    return;
                }
            }
        }
    }
    return;
}

// Function to handle Enter Key presses
void processEnterKey()
{
    removeWhiteSpaces(); // Copy all the contents from console_cmd_buffer to console_cmd_buffer2 except the whitespaces ( spaces and tabs)

    // Enter key has been pressed. Process the contents of the data entered
    // strstr() searches if the specified string is a subset of the contents of the character buffer array console_cmd_buffer
//    if     ( (strstr(console_cmd_buffer2, "color" )) && (stop_flag == 0) && (pause_flag == 0))  Color       = findColor();      // Find out which color is requested
//    else if( (strstr(console_cmd_buffer2, "blink" )) && (stop_flag == 0) && (pause_flag == 0))  Blink_Delay = findBlinkRate();  // Find out the blink rate
//    else if( (strstr(console_cmd_buffer2, "pause" )) && (stop_flag == 0))  ResumePauseCommand_Console();  // Process Pause Command
//    else if( (strstr(console_cmd_buffer2, "resume")) && (stop_flag == 0))  ResumePauseCommand_Console(); // Process Resume Command
//    else if(strstr(console_cmd_buffer2, "stop"))     StartStopCommand_Console();   // Process Stop Command
//    else if(strstr(console_cmd_buffer2, "start"))    StartStopCommand_Console();   // Process Start Command
//    else if(strstr(console_cmd_buffer2, "peek"))     processPeek();                // Process Peek Command
//    else if(strstr(console_cmd_buffer2, "poke"))     processPoke();                // Process Poke Command

    if       (strcmp(console_cmd_buffer2 , "timerstop" )  == 0) StartStopCommand_Console();    // Process Timer Stop Command
    else if  (strcmp(console_cmd_buffer2 , "timerstart")  == 0) StartStopCommand_Console();    // Process Timer Start Command
    else if  (strcmp(console_cmd_buffer2 , "timerpause")  == 0) ResumePauseCommand_Console();  // Process Timer Pause Command
    else if  (strcmp(console_cmd_buffer2 , "timerresume") == 0) ResumePauseCommand_Console();  // Process Timer Resume Command
    else processInvalidCommand(); // Command with neither Color nor Blink has been entered => Give appropriate prompt to user to enter valid command

    /* Once Enter Key is pressed and Color and Blink Rate have been processed, empty the contents of the character array
       to receive further instructions from the user */
    emptyBuffer();
    UARTCharPut(UART0_BASE, '\n'); // Once Enter key is pressed and the character buffer has been emptied, move on to the next line
    UARTCharPut(UART0_BASE, '\r'); // After moving to the new line, also move to the extreme left end
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
    if(id<=39) console_cmd_buffer[id] = (val);
    UARTCharPut(UART0_BASE, console_cmd_buffer[id]);    // This makes the characters appear on the console (This is the Echo Operation)
    // console_cmd_buffer[id] = tolower(val);           // Convert everything to lower cases characters and store in the buffer array
    id = id +1;

    // Buffer size has been increased to accomodate 40 characters given by the user
    // This is done to display more content in LCD of EDU Arm Board
    if(id>=0 && id<=39)  console_cmd_buffer[id] = '\0'; // Terminate the array at a valid position
}


// Functions to give directions to the user when an Invalid Command has been entered
void processInvalidCommand()
{
    if(pause_flag == 0 && stop_flag == 0)
    {
        /* Here the if condition makes sure that that the prompt is not displayed when stop state is active */
        char* s  = "\n\n\rPlease enter any one of the following commands:\n\r"
                         "1). Timer Start\n\r"
                         "2). Timer Stop\n\r"
                         "3). Timer Pause\n\r"
                         "4). Timer Resume\n\r";
        int i = 0; // for indexing purposes
        while(s[i] != '\0')
          {
             UARTCharPut(UART0_BASE, s[i]);
             i = i+1;
          }
        print_game_status();
    }
    return;
}

void processPoke()
{
    // we need spaces in poke command to differentiate between number of bytes to write and the content to write
    // Hence for processing poke command, the original buffer console_cmd_buffer[] is used and not console_cmd_buffer2[]
    // For now the assumption is that a valid poke command is given by the user with only one space
    // Later on space handling must be done

    int i = 0,j=0,k=0,l=0;  // for indexing purposes
    char addr_char[8];      // to store the starting address of the Poke content
    char* endptr;           // will be used in strtol function
    int read_flag = 1;      // 1 / 0 => read / don't read from the console
    char size_char[100]; int size_int;

    for(int index = 0; index < sizeof(poke_content) ; index++)
    {
        poke_content [index] = '\0';   // Empty out the contents of poke_content array before filling it with data (To get rid of stale data)
    }

    while(console_cmd_buffer[i] != '\0')
    {

//       if(i>4 && i<15)
        if(i>6 && i<15)
       {
           addr_char[j] = console_cmd_buffer[i];  // storing the location to be poked in add_char[] array
           j = j+1;
       }

       if(read_flag == 0)
       {
           poke_content[l] = console_cmd_buffer[i];  // Read the contents to be poked
           l = l + 1;
       }

       if( (i >= 16) && (read_flag == 1))
           {
               while(!isspace(console_cmd_buffer[i]))
               {
                   size_char[k] = console_cmd_buffer[i];
                   k += 1;  // Increment the index of size_char
                   i += 1;  // read the next character from  console_cmd_buffer
                   read_flag = 0;
               }
           }
       i = i+1;
    }

    size_int = atoi(size_char);                        // storing the number of variables to be poked in an integer
    unsigned int addr = strtol(addr_char,&endptr,16);  // starting address of the poke address in long (integer of 8 bytes)format
    char* ptr1 = (char*)addr;                           // starting address of the poke content in char* format
    char* ptr2 = ptr1;                                  // we use ptr2 to display the poke content on LCD because ptr1 will be incremented while populating poke_content array

    UARTCharPut(UART0_BASE, '\n');
    UARTCharPut(UART0_BASE, '\r');

    for(int x=0;x<size_int;x++)
    {
        *ptr1 = poke_content[x];    // writing the given contents in the specified memory location
         ptr1 = ptr1+1;
    }

    /*
       If the poke address is the same as the address of "Version 0.01" then we display that content on LCD
       Right now , the  address of "Version 0.01" is 0x20004000. Hence we check if ptr is the same as 0x20004000. If so we will display the contents on LCD
    */
    if(ptr2 == version) LCD_PutData(poke_content,size_int);
     return;
}

// Function to process Peek Command (Assume for now that the Peek Command is a valid Peek Command)
void processPeek()
{
//    write_some_data();  // put some data starting from 0x20000000 for debugging purposes
    int i = 0,j=0,k=0;  // for indexing purposes
    char addr_char[8]  ;
    char* endptr;
    char size_char[100]; int size_int;
    while(console_cmd_buffer[i] != '\0')
    {
//        if(i>3 && i<14)
        if(i>6 && i<15)
        {
            addr_char[j] = console_cmd_buffer[i];
            j = j + 1;
        }
        if(i>15)
        {
            size_char[k] = console_cmd_buffer[i];
            k = k+1;
        }
        i = i+1;
    }

    unsigned int addr = strtol(addr_char,&endptr,16);
    char* ptr = (char*)addr;

    UARTCharPut(UART0_BASE, '\n');
    UARTCharPut(UART0_BASE, '\r');
    size_int = atoi(size_char);
    for(int x = 0;x<size_int;x++)
    {
        UARTCharPut(UART0_BASE, *(ptr));
        ptr = ptr+1;
    }
    UARTCharPut(UART0_BASE, '\n');
    UARTCharPut(UART0_BASE, '\r');
}

void inline EnableInterrupts(void)
{
    // Global interrupts enable
    __asm  ("CPSIE  I\n");      // Change processor state-enable interrupts
}

void SysTick_Handler(void)
{
   // Handle Start and Stop States
   if(sw1_pressed == 1)
   {
       // STOP state is active
       if(stop_flag == 1)
       {
           Color = 1;
           Blink_Delay = 1;  // Good enough to keep the LED continuosly ON
           time_elapsed = 0; // Reset the count value
           seconds      = 0; // Reset the count value
       }

       // START state is active
       else
       {
           Color = 1;
           Blink_Delay = 50;  // No reason behind 50. Just a number I have chosen for the Systick Assignment
       }

       // SysTick_Handler() is called for every 100ms.
       // So when we set sw1_delay to 5 => We are allowing a time of 500ms between accepting sw1 presses
       sw1_delay += 1;
       if(sw1_delay == 5) sw1_pressed = 0; // Start accepting further interrupts

   }

   // Handle Pause and Resume states
   if((sw2_pressed == 1) && (stop_flag == 0))
   {
       // PAUSE state is active
       if(pause_flag == 1) Color = 2;  // Blink Blue in PAUSE state
       // RESUME state is active
       else Color = 1;                 // Blink Green in RESUME state
       Blink_Delay = 50;               // No reason behind 50. Just a number I have chosen for the Systick Assignment
       sw2_delay += 1;

       // SysTick_Handler() is called for every 100ms.
       // So when we set sw2_delay to 5 => We are allowing a time of 500ms between accepting sw2 presses
       if(sw2_delay == 5) sw2_pressed = 0; // Start accepting further interrupts
   }

   // Increment the counter when system is in running state (Both stop and pause are disabled)
   if( (stop_flag == 0) && (pause_flag == 0))
   {
       time_elapsed += 1;            // Update the time that has elapsed

       // time_elapsed = 10 => 1 second has elapsed
       if(time_elapsed == 10)
       {
           time_elapsed = 0;
           seconds += 1;
           if(seconds == 1000) seconds = 0; // Reset seconds count to 0 to accommodate it in Seven Segment Display
       }
   }

   // Update LCD only when state changes
   if(curr_state != prev_state)
   {
        prev_state = curr_state;
        if(curr_state == 0)       LCD_PutData("Timer Ready",11);
        else if(curr_state == 1)  LCD_PutData("Timer Running",13);
        else                      LCD_PutData("Timer Paused",12);
   }
}

void GPIO_PORTF_Handler()
{
    // Handle sw1 press
    if(GPIO_PORTF_MIS_R == 0x10 && sw1_pressed == 0)
    {
         sw1_pressed = 1;   // Here we say that sw1 has been pressed
         sw1_delay   = 0;   // Start the delay for to avoid de-bouncing issues in sw1. This variable will be incremented in the SysTick_Handler() function.

         // Toggle between start and stop state
         if(stop_flag == 0)
             {
                     stop_flag  = 1; // STOP  state is active
                     curr_state = 0; // READY (STOP)
             }
         else
             {
                 stop_flag  = 0; // START state is active
                 pause_flag = 0; // De-activate PAUSE flag if it was active before
                 curr_state = 1; // Running (START , RESUME)
             }
    }

    // Handle sw2 press
    if(GPIO_PORTF_MIS_R == 0x01 && sw2_pressed == 0)
    {
        // Only handle pause / resume activity when system is not in stop state
        if(stop_flag == 0)
        {
            sw2_pressed = 1; // Here we say that sw2 has been pressed
            sw2_delay   = 0; // Start the delay for to avoid de-bouncing issues in sw2

            // Toggle between pause and resume state
            if(pause_flag == 0)
                {
                   pause_flag = 1; // PAUSE  state is active
                   curr_state = 2; // PAUSE
                }
            else
                {
                  pause_flag = 0; // RESUME state is active
                  curr_state = 1; // RUNNING (RESUME)
                }
        }
    }


    // clear the interrupt flags (Necessary for the interrupts to work properly)
    GPIO_PORTF_ICR_R = 0x11;
    return;
}
int main()
{
//    keypad_init(); // To initialize keypad

    // Necessary definitions to write data into Port F
    EnableInterrupts();  // Enable the interrupts
    GPIO_setup();        // Initialize the necessary GPIO ports
    systick_setup();                /* Initialize the Systick Timer*/
//    SYSCTL_RCGC2_R |=   0x00000023;   /* enable clock to GPIOA, GPIOB and GPIOF at clock gating control register */
//    GPIO_PORTF_DIR_R =  0x0E;        /* enable the GPIO pins for the LED (PF3, 2 1) as output */
//    GPIO_PORTF_LOCK_R = 0x4C4F434B;   /* unlock commit register GPIOCR */
//    GPIO_PORTF_CR_R =   0x01;         /* make PORTF0 configurable */
//    GPIO_PORTF_DEN_R =  0x1F;        // Enabling all the 5 pins of port 5 for digital activity
//    GPIO_PORTF_PUR_R =  0x11;        /* enable pull up for pin 4 (SW1) and pin 0 (SW2). If this is not present, I think SW1 and SW2 are always considered to be pressed*///




//    strcpy(version,"Version 0.01");    /* Initialization of the contents to be displayed on LCD */
    LCD_init();
    LCD_PutData("Timer Ready",11);

    gameArray_init();  // Initialize the character array that will be used for playing tic tac toe


    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    while(1)
    {
        while(!(UARTCharsAvail(UART0_BASE)))
          {
//            read_sw1();       // Check for sw1 press even when the user has not given any new command
//            read_sw2();       // Check for sw2 press even when the user has not given any new command

            processColors();  // Maintain LED Blink state even when there is no new command
            if(detectKeyPress() == 1) check_game_status();
            SSD_Display_Handler();
          }

        val = UARTCharGet(UART0_BASE);
        if((val == 0x0D))                 processEnterKey();        // If entered character is 0x0D => Enter key has been pressed
        else if(val == 0x08 && id>0)      processBackSpaceKey();    // If entered character is 0x08 => Back Space Key has been pressed
        else                              processNormalKey();       // To process Key press that is Neither "Enter" nor "Back Space"
//        read_sw1();                                                 // Keep checking for sw1 press
//        read_sw2();                                                 // Keep checking for sw2 press
         processColors();                                            // Maintain LED blink state even in the middle of a new command
//        if(processKeyPress() == 1) StartStopCommand_Keypad();       // Handle toggling between start  and stop commands given through keypad
//        if(processKeyPress() == 2) ResumePauseCommand_Keypad();     // Handle toggling between resume and pause commands given through keypad
        SSD_Display_Handler();
        if(detectKeyPress() == 1) check_game_status();
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
           if((val == 0x0D))                 processEnterKey();      // If entered character is 0x0D => Enter key has been pressed
           else if(val == 0x08 && id>0)      processBackSpaceKey();  // If entered character is 0x08 => Back Space Key has been pressed
           else                              processNormalKey();     // To process Key press that is Neither "Enter" nor "Back Space"
       }
//       read_sw1();                                                   // Check the status of sw1 periodically
//       read_sw2();                                                   // Check the status of sw2 periodically
//       if(processKeyPress() == 1) StartStopCommand_Keypad();         // Handle toggling between start  and stop commands given through keypad
//       if(processKeyPress() == 2) ResumePauseCommand_Keypad();       // Handle toggling between resume and pause commands given through keypad
         SSD_Display_Handler();
         if(detectKeyPress() == 1) check_game_status();
     }
   return;  // sw1 not pressed during the entire duration of the delay
}

void delay_1Ms_LCD_Starter()
{
    for(int j = 0; j < 3180; j++)
      {
          // 1ms delay
      }
   return;
}
void delaySSD()
{

    for(int j = 0; j<3180; j++)
       {
            // Do nothing for 1 ms
       }
}

