//#define COLOR_GREEN_ON     0x08
//#define COLOR_BLUE_ON      0x04
//#define COLOR_RED_ON       0x02
//#define COLOR_CYAN_ON      0x0C  // B + G
//#define COLOR_YELLOW_ON    0x0A  // R + G
//#define COLOR_MAGENTA_ON   0x06  // B + R
//#define COLOR_WHITE_ON     0x0E  // B + R + G
//#define NO_COLOR           0x00  // No color

#define STACK_SIZE 100 // size of the dummy array that will be used to save the context of a particular task before task switching
#define THREAD_NUM 7   // defining the max  number of threads(tasks)
struct tcb
{
    long* sp;           // pointer that will point to the Stack of that particular task
    struct tcb* next;   // pointer pointing to the TCB of the next task
    int priority;       // lower the number, higher is the priority of that task (useful only for event threads)
};

typedef struct tcb tcbtype;

tcbtype tcbs[THREAD_NUM];  // Array of tcbs. Each task will have its own tcb
tcbtype* runpt;
long stacks[THREAD_NUM][STACK_SIZE]; // Dummy array that helps us save the context of a particular task before switching on to the next task

void Init_Systick(void);      // Function to initialize Systick Handler
void Init_UART(void);         // Function to initialize UART
//extern void Systick_INT_Handler(); // Function that will be called every time the Systick Handler fires
void EnableInterrupts(void);  // Function to enable interrupts
void DisableInterrupts(void); // Function to disable interrupts

// Declaration of all the task functions
void task0(void);   /* task 0 makes the RED LED ON continuously and also displays the count on the right most SSD*/
void task1(void);   /* task 1 makes the BLUE LED ON continuously and displays its count on the second SSD from the right*/
void task2(void);   /* task 2 makes the GREEN LED ON continuously and displays its count on the third SSD from the right*/
void task3(void);   /* task 3 makes the WHITE LED ON continuously and displays its count on the fourth SSD from the right*/
void task4(void);   /* task 4 is to send sin wave values from the look up table to LTC 1661 DAC*/
void task5(void);   /* task5 makes use of dynamically allocated memory to do its function */
void task6(void);   /* task6 also makes use of dynamically allocated memory to do its functions */

void start_os(void);
void Set_initial_stack(int i);  // Function to set up the initial stack

// Function to add threads using function pointers
void OS_AddThreads(void);
//void OS_AddThreads(void(*task0)(void), void(*task1)(void),void(*task2)(void),void(*task3)(void),void(*task4)(void));

void delay_1ms(void);       // Function to add a delay of 1ms
int  detectKeyPress(void);  // Function to detect key press
int  processKeyPress(void); // Function for key press identification
void print(char[]);         // Function to print the given character array
void HeapMemDump(void);     // Function to provide output snapshot if memory usage

char NUMto7SEG[10] = {    0x3f,  // 0
                          0x06,  // 1
                          0x5B,  // 2
                          0x4F,  // 3
                          0x66,  // 4
                          0x6D,  // 5
                          0x7D,  // 6
                          0x07,  // 7
                          0x7F,  // 8
                          0x6F   // 9
                      };

int count = 10;        // Number of times the task must be run
int sin_index = 0;     // index to send the sin values in the lookup table
int num = 0;           // To indicate the key that was pressed in the 4x4 keypad display
unsigned int data = 0; // variable to hold the sin value that is sent to the LTC 1661 DAC
int mem = -1;          // variable to store the return value of HeapMemInit() function

// For debugging purposes
int debug_var[50];
int debug_var_task4, debug_var_task5, debug_var_task6;

extern unsigned int __heap_start__;  // Indicates start of the heap section in memory
unsigned int heap_start;             // To store the start of heap section of memory

/*  The following character arrays are used to print information regarding the memory that is
    allocated and de-allocated to tasks 5 and 6
 */
char task5_alloc[50];
char task6_alloc[50];

char task5_dealloc[50];
char task6_dealloc[50];

char task5_location[50];
char task6_location[50];


