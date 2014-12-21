/* Tests cetegorical mutual exclusion with different numbers of threads.
 * Automatic checks only catch severe problems like crashes.
 */
#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/malloc.h"
#include "threads/synch.h"
#include "threads/thread.h"
#include "lib/random.h" //generate random numbers

#define BUS_CAPACITY 3
#define SENDER 0
#define RECEIVER 1
#define NORMAL 0
#define HIGH 1
#define EMPTY 0
#define INUSE 1

/*
 *	initialize task with direction and priority
 *	call o
 * */
typedef struct {
	int direction;
	int priority;
} task_t;

/* static struct semaphore bus; */
static struct lock monitor_lock;
static struct condition bus;
static int num_of_priorities;
static int num_of_bus_user;
static int bus_direction; 

void batchScheduler(unsigned int num_tasks_send, unsigned int num_task_receive,
        unsigned int num_priority_send, unsigned int num_priority_receive);

void senderTask(void *);
void receiverTask(void *);
void senderPriorityTask(void *);
void receiverPriorityTask(void *);


void oneTask(task_t task);/*Task requires to use the bus and executes methods below*/
void getSlot(task_t task); /* task tries to use slot on the bus */
void transferData(task_t task); /* task processes data on the bus either sending or receiving based on the direction*/
void leaveSlot(task_t task); /* task release the slot */

    

/* initializes semaphores */ 
void init_bus(void){ 
 
    random_init((unsigned int)123456789); 
    
    /* msg("NOT IMPLEMENTED");*/
    /* FIXME implement */

    /* sema_init(&bus, BUS_CAPACITY); */
    num_of_priorities = 0;
    num_of_bus_user = 0;
    bus_direction = SENDER;
    lock_init(&monitor_lock);
    cond_init(&bus);
}

/*
 *  Creates a memory bus sub-system  with num_tasks_send + num_priority_send
 *  sending data to the accelerator and num_task_receive + num_priority_receive tasks
 *  reading data/results from the accelerator.
 *
 *  Every task is represented by its own thread. 
 *  Task requires and gets slot on bus system (1)
 *  process data and the bus (2)
 *  Leave the bus (3).
 */

void batchScheduler(unsigned int num_tasks_send, unsigned int num_task_receive,
        unsigned int num_priority_send, unsigned int num_priority_receive)
{
    /* msg("NOT IMPLEMENTED");*/
    /* FIXME implement */
    char aux = 'n';
    unsigned int i;
    for(i = 0;i < num_priority_send; i++)
        {
            thread_create("senderPriorityTask", PRI_MAX, &receiverTask, &aux);
        }
    for(i = 0;i < num_priority_receive; i++)
        {
            thread_create("receiverPriorityTask", PRI_MAX, &receiverPriorityTask, &aux);
        }
    for(i = 0;i < num_tasks_send; i++)
        {
            thread_create("senderTask", PRI_DEFAULT, &senderTask, &aux);
        }
    for(i = 0;i < num_task_receive; i++)
        {
            thread_create("receiverTask", PRI_DEFAULT, &senderPriorityTask, &aux);
        }


}

/* Normal task,  sending data to the accelerator */
void senderTask(void *aux UNUSED){
    task_t task = {SENDER, NORMAL};
    oneTask(task);
}

/* High priority task, sending data to the accelerator */
void senderPriorityTask(void *aux UNUSED){
    task_t task = {SENDER, HIGH};
    oneTask(task);
}

/* Normal task, reading data from the accelerator */
void receiverTask(void *aux UNUSED){
    task_t task = {RECEIVER, NORMAL};
    oneTask(task);
}

/* High priority task, reading data from the accelerator */
void receiverPriorityTask(void *aux UNUSED){
    task_t task = {RECEIVER, HIGH};
    oneTask(task);
}

/* abstract task execution*/
void oneTask(task_t task) {
  getSlot(task);
  transferData(task);
  leaveSlot(task);
}


/* task tries to get slot on the bus subsystem */
void getSlot(task_t task) 
{
    /* msg("NOT IMPLEMENTED"); */
    /* FIXME implement */
    lock_acquire (&monitor_lock);
    if (task.priority == HIGH)
    {
        while ((bus_direction != task.direction || num_of_bus_user != 0) &&
            num_of_bus_user >= 3)
        {
            cond_wait (&bus, &monitor_lock);
        }
        num_of_priorities++;
        bus_direction = task.direction;
        num_of_bus_user++;
        cond_signal(&bus, &monitor_lock);
    }
    else
    {
        while ((bus_direction != task.direction || num_of_bus_user != 0) &&
            num_of_priorities != 0 && num_of_bus_user >= 3)
        {
            cond_wait (&bus,&monitor_lock);
        }
        bus_direction = task.direction;
        num_of_bus_user++;
        cond_signal(&bus, &monitor_lock);
    }
    lock_release(&monitor_lock);
    /* transferData(task); */
}

/* task processes data on the bus send/receive */
void transferData(task_t task) 
{
    /* msg("NOT IMPLEMENTED"); */
    /* FIXME implement */
    int64_t ticks;
    int wait = 0;
    ticks = (int64_t) random_ulong();
    while(wait < (ticks % 1000))
        wait ++;
    /* printf("Start transferring data into the buss...");
    printf("Leaving from the buss...");
    timer_sleep(ticks); */
    /*leaveSlot(task);*/
}

/* task releases the slot */
void leaveSlot(task_t task) 
{
    /* msg("NOT IMPLEMENTED"); */
    /* FIXME implement */
    lock_acquire(&monitor_lock);
    if (task.priority == HIGH)
    {
        num_of_priorities--;
        num_of_bus_user--;
    }
    else
    {
        num_of_bus_user--;
    }
    cond_broadcast(&bus, &monitor_lock);
    lock_release(&monitor_lock);
}

