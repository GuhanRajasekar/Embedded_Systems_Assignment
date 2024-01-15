#include<stdlib.h>
#include<stdio.h>
/*
   TCB ( Task control block) contains all the necessary information related to the task.
*/
struct TCB
{
   int task_id;
   int task_priority;                  // lower the number, higher the priority
   int task_state;                     // 1 : Waiting, 2: Ready , 3: Running
   int task_event_id;                  // denotes an event the task is waiting for to get completed
   int* ptr_context;                   // set to NULL by default
   struct TCB* ptr_next_task;          // points to the next task
};

struct TCB* ready_head;                // pointer pointing to first task of ready_queue
struct TCB* waiting_head;              // pointer pointing to first task of waiting_queue

// function to return the position in which a task needs to be inserted in the ready_queue
int getPos(int priority)
{
    int pos  = 1;   // position in which the task needs to be placed in the ready_queue
    int flag = 0;   // 0 indicates that the task has the least priority and needs to be placed at the end of the ready queue
    struct TCB* temp = ready_head;
    while(temp!= NULL)
    {
        if(priority < temp->task_priority)
        {
            flag = 1;  // 1 indicates that the task need not be placed at the end of the ready_queue
            return pos;
        }
        pos += 1;
        temp = temp->ptr_next_task;   // move on to the next task
    } 
    if(flag == 0) return -1; // -1 indicates that the task needs to be placed at the end of the ready_queue
    return pos;
}
void Insert_ready_queue(int id,int priority)
{
    struct TCB* temp = (struct TCB*)malloc(sizeof(struct TCB)); // dynamically allocating a block of memeory to store the contents of TCB structure
    struct TCB* temp1;          // pointer to iterate over the ready_queue
    temp->task_id = id;
    temp->task_priority = priority;
    temp->task_state = 2;       // 2 indicates running state
    temp->task_event_id = 0;    // 0 indicates that the current task is not waiting for the execution of any event/data/resource
    temp->ptr_context = NULL;   // ptr_context is set to NULL by default
    temp->ptr_next_task = NULL;
    int pos;                    // position where the task needs to be placed in the ready_queue

    // ready_queue is empty and the current task needs to be placed as the first one
    if(ready_head == NULL)      
    {
        temp->ptr_next_task = ready_head;
        ready_head = temp;
    }
    // ready_queue is not empty and the new task needs to be placed in the queue according to its priority
    else
    {
       pos = getPos(priority);
       printf("Position is :%d \n",pos);
       
       // task to be placed at the end of the queue as it has the least priority 
       if(pos == -1)    
       {
           temp1 =  ready_head;
           while(temp1->ptr_next_task != NULL)
           {
               temp1 = temp1->ptr_next_task;
           }
           temp1->ptr_next_task = temp;  // attach the node at the end
       }
       
       // task to be placed at the beginning of the ready_queue
       else if(pos == 1)
       {
           temp->ptr_next_task = ready_head;
           ready_head = temp;
       }

       // task needs to be placed at a particular position within the ready queue
       else
       {
           temp1 = ready_head;
           for(int i = 1;i<pos-1;i++)
           {
              temp1 = temp1->ptr_next_task;
           }
           temp->ptr_next_task  = temp1->ptr_next_task;
           temp1->ptr_next_task = temp; 
       }
    }

    if(ready_head != NULL) ready_head->task_state = 3; // first task of the ready_queue is "Running"
        
}

// function to insert tasks in the waiting queue
void Insert_waiting_queue(int id, int priority, int event_id)
{
    struct TCB* temp = (struct TCB*)malloc(sizeof(struct TCB)); // dynamically allocating a block of memeory to store the contents of TCB structure
    temp->task_id = id;
    temp->task_priority = priority;
    temp->task_state = 1;           // 1 indicates waiting state
    temp->task_event_id = event_id; // This number indicates the event that the current task is waiting for to get executed
    temp->ptr_context = NULL;       // ptr_context is set to NULL by default
    temp->ptr_next_task = NULL;

    // waiting_queue is currently empty
    if(waiting_head == NULL)
    {
        waiting_head = temp;
    }
    // If waiting_queue is not empty, add the task to the beginning of the waiting_queue
    // No need to check priority in the waiting_queue
    else
    {
        temp->ptr_next_task = waiting_head;
        waiting_head = temp;
    }
}

// Function to print the tasks in the wait and the ready queue
void print_tasks()
{
    struct TCB* temp1 = ready_head;    // pointer to traverse the ready_queue and print the elements
    struct TCB* temp2 = waiting_head;  // pointer to traverse the waiting_queue and print the elements
    
    if(waiting_head == NULL) printf("Waiting Queue is empty\n");  
    else
    {
        printf("Contents of the waiting queue: \n");
        while(temp2 != NULL)
        {
            printf("Task ID       : %d\n",temp2->task_id);
            printf("Task Priority : %d\n",temp2->task_priority);
            printf("Task State    : %d\n",temp2->task_state);
            printf("Task Event ID : %d\n",temp2->task_event_id);
            printf("\n");  // for readability
            temp2 = temp2->ptr_next_task;
        }
    }

    if(ready_head == NULL) printf("Ready Queue is empty\n");
    else
    {
        printf("Contents of the ready_queue are: \n");
        while(temp1 != NULL)
        {
            printf("Task ID       : %d\n",temp1->task_id);
            printf("Task Priority : %d\n",temp1->task_priority);
            printf("Task State    : %d\n",temp1->task_state);
            printf("Task Event ID : %d\n",temp1->task_event_id);
            printf("\n");  // for readability
            temp1 = temp1->ptr_next_task;
        }
    }

    return;
}
int main()
{
    ready_head   = NULL;    // ready_queue is empty
    waiting_head = NULL;    // waiting_queue is empty
    

    //Inserting dummy entries in the waiting_queue
    Insert_waiting_queue(54,2,21);
    Insert_waiting_queue(25,6,29);
    Insert_waiting_queue(27,7,127);
    Insert_waiting_queue(213,16,7);

    //Inserting dummy entries in the ready_queue
    Insert_ready_queue(27,7);
    Insert_ready_queue(77,64);
    Insert_ready_queue(108,16);
    Insert_ready_queue(57,1);

    print_tasks();
    return 0;
}