
	BLINKY WITH 7 SEGMENT DISPLAY AND 4x4 KEYPAD		

______________________________________________________________

Description:
______________________________________________________________

This program implements blinking LED using both switch buttons, UART console interface, keypad and 7-segment display.

a) Button switch( SW1 and SW2):
Implementing a colour wheel using SW1 switch that is present on the TIVA board.
With each key (SW1) press the LED cycles through different colours starting from Green in the following order
Order: Green, Blue, Cyan, Red, Yellow, Magenta, White

SW2 is used to increase the blink speed starting from once in two seconds to 16 times in a second. Doubling the rate with every key press (SW2). Once the max blink speed is reached and the next key (SW2) press occurs, the LED will switch to "continuously ON" state and wait for  the next key press (SW2)
The blink cycle will repeat again from the beginning on the next key (SW2) press 

b) UART console:
The following commands can be given from the UART console to change the color and blink rate:
1. > color  [color_name]
2. > blink [ blink_rate] (number of times led blinks in a minute, zero means glow continuously)
3. > pause 
4. > resume
5. > stop
6. > start 

c) Keypad:
The first two keys of the 4x4 keypad are used to pause, resume, stop, and restart functionality. The first key is used to toggle between pause and resume state. In the pause state the system holds the present state and continues to blink as soon as same key is pressed to resume the activity. The second key is used to toggle between stop and restart state. In stop state the system is turned off and starts from initial state when the same key is pressed. The same functionalities can be implemented through the UART console.

d) 7-segment display:
The rightmost 7-segment display is used to show the present colour code (Green #1 .. White #7) of the LED.
The two left 7-segment displays are used to count the number of colour changes.

_____________________________________

SETUP:
_____________________________________

The UART baud rate is set as 115200 bps.
Use a terminal with baud rate set to 115200 bps.

_____________________________________

Function descriptions:
_____________________________________

1) switch_console_read(): It polls the GPIO_PORTF for switch press, and polls the UART interface for input from console, and updates the colour or delay values. 
2) read_string_from_console(): Reads a string from the console. 'Enter' is recognized as end of string.
3) preprocess(): Pre-processes the string to remove extra spaces/tabs, support backspace functionality and convert to lowercase.
4) str_compare(): Compares the pre-processed string with the supported commands.
5) switch_debounce(int switch_addr, int mask):  It checks if the switch is pressed and returns 1 if ON or 0 if OFF. It also solves the debouncing problem. If the switch is kept pressed, the led stops blinking and retains it's previous colour.
6) keypad_press(): Checks if a key is pressed and finds the key number. Implements pause/stop if relevant key is pressed, and waits for resume/restart.
7) check_resume(): Once the system is paused, it checks for resume via UART/keypad press.
8) check_start(): Once the system is stopped, it checks for restart via UART/keypad press.
9) display_all_SSD(): Displays colour code + number of cycle changes on the 7-segment display.
10) UART_Print_str(): Prints a string on the UART interface.
11) delayMs(): For introducing a delay of 1ms.

_____________________________________

Macros
_____________________________________

1) COLOR_X_ON: It is used to define the port address for blinking led of colour 'X'. For example, COLOR_RED_ON specifies the address for 'RED' colour
2) BLINK_DELAY_T: It is used for introducing different delays for different clock frequencies. For example, BLINK_DELAY_1000 keeps the led ON for 1000ms and OFF for 1000ms.
