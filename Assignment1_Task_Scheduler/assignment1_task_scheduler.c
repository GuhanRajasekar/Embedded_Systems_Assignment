#include<stdlib.h>
#include<stdio.h>
#include<string.h>
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


// function to check if the task is present in the ready_queue
// If present, this function will return the position of the task in the ready_queue
int isTask_In_Ready_Queue(int id)
{
  int pos  = 1; // variable to indicate the position of the task in the ready_queue
  int flag = 0; // variable to indicate whether the task is present in the ready_queue or not
  struct TCB* temp = ready_head;
  if(temp == NULL) return -1; // ready_queue is empty
  while(temp!= NULL)
  {
    if(temp -> task_id == id)
    {
        flag = 1;
        return pos;
    }
    pos += 1;                      // Increment the position variable
    temp = temp->ptr_next_task;    // Move the pointer to point to the next task
  }
  if(flag == 0) return -1; //task not present in the ready_queue
}


// function to check if the task is present in the waiting_queue
// If present, this function will return the position of the task in the waiting_queue
int isTask_In_Waiting_Queue(int id)
{
  int pos  = 1; // variable to indicate the position of the task in the waiting_queue
  int flag = 0; // variable to indicate whether the task is present in the waiting_queue or not
  struct TCB* temp = waiting_head;
  if(temp == NULL) return -1; // waiting_queue is empty
  while(temp!= NULL)
  {
    if(temp -> task_id == id)
    {
        flag = 1;
        return pos;
    }
    pos  += 1;                     // Increment the position variable
    temp = temp->ptr_next_task;    // Move the pointer to point to the next task
  }
  if(flag == 0) return -1;  // task not present in the waiting_queue
}


// function to update the status of the tasks in the ready_queue
// Except the first task in the ready queue, all the other tasks have a task state of 2, to indicate ready
// The first task in the ready queue will have a status of 3, to indicate that it is the running task
void update_running_task()
{
   if(ready_head != NULL) ready_head -> task_state = 3; // first task of the ready_queue is the running task
   struct TCB* temp = ready_head->ptr_next_task;
   while(temp != NULL)
   {
       temp->task_state = 2;
       temp = temp->ptr_next_task;
   }
   return;
}

// function to return the position in which a task needs to be inserted in the ready_queue based on its priority
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

    update_running_task(); // update the running task        
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

// Function to suspend the running task with the given event ID. 
void sus_run_task(int event_id)
{
    struct TCB* temp = ready_head;          // temp holds the address of the running task
    ready_head = ready_head->ptr_next_task; // update the ready_queue
    update_running_task();                  // update the running task

    temp->task_event_id = event_id;         // update the event ID value
    temp->task_state    = 1;                // 1 indicates the task is in waiting_queue
    temp->ptr_next_task = waiting_head;     // move the running task into the waiting queue
    waiting_head = temp;                    // make the task the first task of the waiting queue
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
            printf("Task State    : Waiting\n");
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
            if(temp1->task_state == 3)          printf("Task State    : Running\n");
            else if(temp1->task_state == 2)     printf("Task State    : Ready\n");
            printf("Task Event ID : %d\n",temp1->task_event_id);
            printf("\n");  // for readability
            temp1 = temp1->ptr_next_task;
        }
    }

    return;
}

void delete_Tasks(int pos, int flag)
{
    struct TCB* temp1;
    struct TCB* temp2;
    if(flag == 1)  // delete task from the waiting_queue
     {
         temp1 = waiting_head;
         if(pos == 1)     // delete first task from the waiting_queue
         {
            waiting_head = waiting_head->ptr_next_task;
            free(temp1);   // free up the dynamically created memory (delete the task)
         }
         else            // delete an intermediate task from the waiting_queue
         {
            for(int i = 1;i<(pos-1);i++)  // go to one position before the task that needs to be deleted
            {
                temp1 = temp1->ptr_next_task;
            }
            temp2 = temp1->ptr_next_task;
            temp1->ptr_next_task = temp2->ptr_next_task;
            free(temp2);
         }
     }
    else if(flag == 2)     // delete task from the ready_queue
     {
         temp1 = ready_head;
         if(pos == 1)     // delete first task from the ready_queue
         {
            ready_head = ready_head->ptr_next_task;
            free(temp1);   // free up the dynamically created memory (delete the task)
         }
         else            // delete an intermediate task from the ready_queue
         {
            for(int i = 1;i<(pos-1);i++)  // go to one position before the task that needs to be deleted
            {
                temp1 = temp1->ptr_next_task;
            }
            temp2 = temp1->ptr_next_task;
            temp1->ptr_next_task = temp2->ptr_next_task;
            free(temp2);
         }
     }
    return;
}

// function to move tasks from ready_queue to waiting_queue
void move_task_ready_to_wait(int event_id,int pos)
{
    struct TCB* temp = ready_head;
    for(int i=1;i<pos;i++) temp = temp->ptr_next_task;  // make temp point to the necessary pointer in the ready_queue
    Insert_waiting_queue(temp->task_id,temp->task_priority,event_id); // insert the task in the waiting_queue
    delete_Tasks(pos,2); // delete the concerned task from the ready_queue
    update_running_task(); // update the status of the tasks in the running queue
    return;
}

// function to trigger the event with the mentioned Event ID and the move the corresponding tasks to the Ready Queue
void move_task_wait_to_ready(int event_id)
{
    struct TCB* temp = waiting_head;
    int pos = 1; // function of the task to be removed from the waiting_queue
    while(temp != NULL)
    {
        if(temp->task_event_id == event_id)
        {
            Insert_ready_queue(temp->task_id,temp->task_priority);
            update_running_task();
            delete_Tasks(pos,1);  // delete the correspoding task from the waiting_queue
        }
        temp = temp->ptr_next_task;  // iterate over the waiting_queue to remove other tasks with the same event ID
        pos += 1;
    }
}
int main()
{
    ready_head   = NULL;    // ready_queue is empty
    waiting_head = NULL;    // waiting_queue is empty
    int x;                  // to take input from the user regarding what task has to be performed
    int task_id,event_id,task_priority; // variables to store information regarding the task
    
    FILE *fp;
    char line[100];
    char *token;
    int data[100][4];  // Adjust dimensions as needed
    int row = 0;

    // Open the file
    fp = fopen("init_tasks.txt", "r");
    if (fp == NULL) {
        printf("Error opening file!\n");
        exit(1);
    }

    // Skip the header line
    fgets(line, sizeof(line), fp);

    // Read each line and store values in the array
    while (fgets(line, sizeof(line), fp) != NULL) {
        token = strtok(line, ",");
        int col = 0;
        while (token != NULL) {
            data[row][col] = atoi(token);  // Convert string to integer
            token = strtok(NULL, ",");
            col++;
        }
        row++;
    }

    // Close the file
    fclose(fp);

    // Now you have the CSV values in the 2D array `data`
    Insert_waiting_queue(data[0][0],data[0][1],data[0][3]);
    Insert_waiting_queue(data[1][0],data[1][1],data[1][3]);
    Insert_waiting_queue(data[2][0],data[2][1],data[2][3]);
    Insert_waiting_queue(data[3][0],data[3][1],data[3][3]);

    Insert_ready_queue(data[4][0] , data[4][1]);
    Insert_ready_queue(data[5][0] , data[5][1]);
    Insert_ready_queue(data[6][0] , data[6][1]);
    Insert_ready_queue(data[7][0] , data[7][1]);


    
    print_tasks();

    while(1)
    {
        printf("What task would you like to perform?\n");
        printf("1: Add a new task\n");
        printf("2: Delete a task from the Ready or Waiting queue\n");
        printf("3: Move the task from Ready to Waiting queue\n");
        printf("4: Trigger the event of Event ID\n");
        printf("5: Suspend running task with the given Event ID\n");
        scanf("%d",&x);
        
        switch (x)
        {
        // Entering a new task in the ready / waiting queue
        case 1:
            printf("Enter the task id: ");
            scanf("%d",&task_id);
            printf("Enter Event ID of the task: ");
            scanf("%d",&event_id);
            printf("Enter the priority of the task: ");
            scanf("%d",&task_priority);
            
            //If event id is 0, insert the task in appropriate position in the ready_queue
            //If event id is not 0, insert the new task at the beginning of the waiting_queue
            if(event_id == 0) Insert_ready_queue(task_id,task_priority);
            else              Insert_waiting_queue(task_id,task_priority,event_id);
            printf("\n");
            print_tasks();
            break;

        // Delete a task from ready or waiting queue
        case 2:
            int pos; // position of the task that needs to be deleted in the ready_queue or the waiting_queue
            printf("Enter a task ID that needs to be deleted: ");
            scanf("%d",&task_id);
            
            if(isTask_In_Waiting_Queue(task_id) != -1)    // delete task from the waiting queue
            {
                pos = isTask_In_Waiting_Queue(task_id);
                delete_Tasks(pos,1);  // flag 1 indicates that element to be deleted is present in the waiting_queue
                print_tasks();
            }
            
            else if(isTask_In_Ready_Queue(task_id) != -1) // delete task from the waiting queue
            {
                pos = isTask_In_Ready_Queue(task_id);
                delete_Tasks(pos,2);                            // flag 2 indicates that element to be deleted is present in the waiting_queue
                if(ready_head != NULL) update_running_task();   // Update the read_queue in case the running task has been deleted
                print_tasks();
            }
            else printf("Task is neither present in the ready_queue nor in the waiting_queue\n");
            break;
        
        // Move task from ready_queue to waiting_queue with the mentioned event_id
        case 3:
            printf("Enter the task ID that needs to be moved from Ready to Waiting Queue: ");
            scanf("%d",&task_id);
            pos = isTask_In_Ready_Queue(task_id);
            if(pos == -1)  // mentioned task is not present in the ready_queue
            {
                printf("Mentioned task is not present in Ready Queue\n\n");
                break;
            }
            printf("Enter Event ID: ");
            scanf("%d",&event_id);
            move_task_ready_to_wait(event_id,pos);  // move the task from ready_queue to waiting_queue
            print_tasks();
           break;
        
        // Trigger the event with event ID  => Move tasks from the mentioned event ID from the waiting queue to the ready queue
        case 4:
           printf("Enter the Event ID: ");
           scanf("%d",&event_id);
           move_task_wait_to_ready(event_id);
           print_tasks();
           break;

        // Suspend a running task with the given event ID
        case 5:
            printf("Enter the Event ID: ");
            scanf("%d",&event_id);
            sus_run_task(event_id);
            print_tasks();
        default:
            break;
        }
    }
    return 0;
}