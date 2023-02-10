#include <assert.h>
#include <stdlib.h>
#include <ucontext.h>
#include "thread.h"
#include "interrupt.h"

#define STATUS_RUNNING 0 //Status:running
#define STATUS_SLEEP 1  //Status:blocked/sleep
#define STATUS_READY 2  //Status:ready
#define STATUS_EXIT 3   //Status:exit

// int current_running_thread_ID = -1;
// #define CURR_RUNNING_ID current_running_thread_ID
struct thread *current_running_thread;

struct thread *tcb_array[THREAD_MAX_THREADS];

// struct wait_queue *wq[THREAD_MAX_THREADS];

/* This is the wait queue structure */
struct wait_queue {
    /* ... Fill this in Assignment 3 ... */
    struct thread *wq_head;
    // struct wait_queue *wq_next;
};

/* This is the thread control block */
struct thread {
    /* ... Fill this in ... */
    // thread_id
    Tid id;

    // stack pointer represents the top of running stack
    void *sp;

    // helper stack pointer to free sp
    void *helper_sp;

    // Record current thread running status (running; blocked/sleep; ready; exited)
    int status;

    // Pointer of TCB to
    struct thread *rq_next;

    // current ucontext of TCB, including pc, sp, 2 argument registers
    ucontext_t current_ucontext;

    // Record exit_code
    int record_exit;

    // Waited Queue
    struct wait_queue *each_wq;

    // Whether waited: 1 waited; 0 not waited
    int waited;
};

struct thread *rq_head;
struct thread *exit_head;

void solve_memory_leak() {
    for (int i = 0; i < THREAD_MAX_THREADS; i++) {
        struct thread *temp = tcb_array[i];
        if (temp != NULL && temp -> status == STATUS_EXIT) {
            if (temp -> each_wq != NULL) {
                assert(temp -> each_wq -> wq_head == NULL);
                wait_queue_destroy(temp -> each_wq);
                temp -> each_wq = NULL;
            }
            if (temp -> helper_sp != NULL) {
                free(temp -> helper_sp);
                temp -> helper_sp = NULL;
            }
            free(temp);
            tcb_array[i] = NULL;
        }
    }
}

int rq_position(int num){
    if (num == 0) {
        if (rq_head == NULL) {
            return -100;
        }
        return rq_head -> id;
    }
    struct thread *temp = rq_head;
    while (num > 0) {
        if (temp->rq_next == NULL) {
            return -100;
        }
        temp = temp->rq_next;
        num -= 1;
    }
    return temp ->id;
}

void
thread_init(void)
{
    /* Add necessary initialization for your threads library here. */
	/* Initialize the thread control block for the first thread */

    /* perform any initialization needed by your threading system */
    // Initialize the first thread
    struct thread *t0 = (struct thread *)malloc(sizeof(struct thread));
    t0 -> id = 0;
    // t0 -> esp = NULL;
    // t0 -> func = NULL;
    t0 -> status = STATUS_RUNNING; // 0 represents running
    t0 -> rq_next = NULL;

    // Initialize the ready queue
    rq_head = NULL;

    t0 -> sp = NULL;
    t0 -> helper_sp = NULL;

    // Initialize record_exit
    t0 -> record_exit = THREAD_INVALID;

    t0 -> each_wq = wait_queue_create();

    t0 -> waited = 0;

    current_running_thread = t0;
    tcb_array[0] = t0;

    // Initialize TCB array
    for (int i = 1; i < THREAD_MAX_THREADS; i++) {
        tcb_array[i] = NULL;
    }

    exit_head = NULL;
}

Tid
thread_id()
{
    /* return the thread identifier of the currently running thread */
    if (current_running_thread -> id >= 0 && current_running_thread -> id < THREAD_MAX_THREADS) {
        return current_running_thread -> id;
    }
    // TBD();
    return THREAD_INVALID;
}

/* New thread starts by calling thread_stub. The arguments to thread_stub are
 * the thread_main() function, and one argument to the thread_main() function. 
 */
void
thread_stub(void (*thread_main)(void *), void *arg)
{
    interrupts_on();
	thread_main(arg); // call thread_main() function with arg
	thread_exit(0);
}

Tid
thread_create(void (*fn) (void *), void *parg)
{
    /* thread_create should create a thread that starts running the function
     * fn(arg). Upon success, return the thread identifier. On failure, return the
     * following:
     *
     * THREAD_NOMORE: no more threads can be created.
     * THREAD_NOMEMORY: no more memory available to create a thread stack. */

    int enable = interrupts_off();
    // assert(!enable);
    // Creating new thread
    struct thread *new_thread;
    new_thread = (struct thread *)malloc(sizeof(struct thread));

    if (!new_thread) {
        interrupts_set(enable);
        return THREAD_NOMEMORY;
    }

    // Set num to 0 since it is impossible value for new thread id 
    int num = 0;
    // Find suitable thread position (i.e. Find thread id for creating new thread)
    while (num < THREAD_MAX_THREADS && tcb_array[num] != NULL) {
        num += 1;
    }
    if (num == THREAD_MAX_THREADS) {
        free(new_thread);
        new_thread = NULL;
        interrupts_set(enable);
        return THREAD_NOMORE;
    }
    // Put new thread into tcb_array
    tcb_array[num] = new_thread;

    int result = getcontext(&(new_thread->current_ucontext));
    assert(result == 0);
    // Initialize new thread
    new_thread -> id = num;
    // new_thread -> func = fn;
    new_thread -> status = STATUS_READY;
    new_thread -> rq_next = NULL;

    // Initialize record_exit
    new_thread -> record_exit = THREAD_INVALID;

    new_thread -> each_wq = wait_queue_create();

    new_thread -> waited = 0;
    
    new_thread -> sp = (void *)malloc(THREAD_MIN_STACK);
    if (!new_thread -> sp) {
        free(new_thread);
        new_thread = NULL;
        interrupts_set(enable);
        return THREAD_NOMEMORY;
    }
    // HELPER stack pointer to free sp
    new_thread -> helper_sp = new_thread -> sp;
    // stack pointer point to the top of stack
    new_thread -> current_ucontext.uc_mcontext.gregs[REG_RDI] = (unsigned long int) fn;
    new_thread -> current_ucontext.uc_mcontext.gregs[REG_RSI] = (unsigned long int) parg;
    new_thread -> current_ucontext.uc_mcontext.gregs[REG_RIP] = (unsigned long int) thread_stub;
    new_thread -> current_ucontext.uc_mcontext.gregs[REG_RSP] = (unsigned long int) new_thread -> sp + THREAD_MIN_STACK - 8;

    // SHOULD CALL thread_stub(fn, parg);

    // Put new thread into ready_queue
    if (rq_head == NULL) {
        // rq have no thread
        rq_head = new_thread;
        new_thread -> rq_next = NULL;
    } else {
        struct thread *tail = rq_head;
        while (tail -> rq_next != NULL) {
            tail = tail -> rq_next;
        }
        tail -> rq_next = new_thread;
        new_thread -> rq_next = NULL;
    }
    assert(num == new_thread -> id);
    interrupts_set(enable);
    return num;

    // TBD();
    // return THREAD_FAILED;
}

Tid
thread_yield(Tid want_tid)
{
    /* thread_yield should suspend the calling thread and run the thread with
    * identifier tid. The calling thread is put in the ready queue. tid can be
    * identifier of any available thread or the following constants:
    *
    * THREAD_ANY:	   run any thread in the ready queue.
    * THREAD_SELF:    continue executing calling thread, for debugging purposes.
    *
    * Upon success, return the identifier of the thread that ran. The calling
    * thread does not see this result until it runs later. Upon failure, the
    * calling thread continues running, and returns the following:
    *
    * THREAD_INVALID: identifier tid does not correspond to a valid thread.
    * THREAD_NONE:    no more threads, other than the caller, are available to
    *		   run; this can happen in response to a call with tid set to
    *		   THREAD_ANY. */


    int enable = interrupts_off();
    // Set want_tid to target_tid
    Tid target_tid = want_tid;
    // Get target_tid thread from ready queue and remove want_thread from the queue
    struct thread *want_thread;

    // Initialize control
    int control = 1;
    
    // want_id is invalid, i.e. impossible value for want_tid
    if (want_tid < -2 || want_tid >= THREAD_MAX_THREADS) {
        interrupts_set(enable);
        return THREAD_INVALID;
    } else if (want_tid == THREAD_SELF || want_tid == thread_id()) {
        // want_tid == -2
        // Continue executing current thread
        interrupts_set(enable);
        return current_running_thread -> id;
    } else if (want_tid == THREAD_ANY) {
        // want_tid == -1
        // ready_queue does not have any READY thread
        if (rq_head == NULL) {
            interrupts_set(enable);
            return THREAD_NONE;
        }
        // Since at this time, want_tid is negative. 
        // So, set it to the first thread id of ready_queue
        target_tid = rq_head -> id;

        // 0 <= target_tid < THREAD_MAX_THREADS

        // Switch current thread and target_tid thread

        // first READY thread in ready_queue is want_thread 
        want_thread = rq_head;
        rq_head = rq_head -> rq_next;
        // Set want_thread status to running
        want_thread -> rq_next = NULL;
        want_thread -> status = STATUS_RUNNING;

        // Get current thread
        struct thread *curr_thread = tcb_array[current_running_thread -> id];
        curr_thread -> rq_next = NULL; 
        curr_thread -> status = STATUS_READY;

        // Insert curr_thread into ready queue only if exiting thread is not -10
        if (rq_head == NULL) {
            // Original ready_queue only have one TCB and it was removed from queue
            // So no TCB in ready_queue at this time
            rq_head = curr_thread;
        } else {
            // At least one tcb in ready_queue
            
            struct thread *tail = rq_head;
            while (tail -> rq_next != NULL) {
                tail = tail -> rq_next;
            }
            tail -> rq_next = curr_thread;
        }
        curr_thread -> rq_next = NULL; 
    } else if (0 <= want_tid && want_tid < THREAD_MAX_THREADS) {
        // 0 <= want_tid < THREAD_MAX_THREADS
        if (tcb_array[want_tid] == NULL || tcb_array[want_tid] -> status != STATUS_READY) {
            // But want_id position in tcb_array does not exsit READY thread
            interrupts_set(enable);
            return THREAD_INVALID;
        }
        if (rq_head == NULL) {
            interrupts_set(enable);
            return THREAD_NONE;
        }

        // 0 <= target_tid < THREAD_MAX_THREADS

        
        if (rq_head -> id == want_tid) {
            // first READY thread in ready_queue is want_thread 
            want_thread = rq_head;
            rq_head = rq_head -> rq_next;
            // want_thread -> rq_next = NULL;
        } else {
            struct thread *tail = rq_head;
            while (tail -> rq_next -> rq_next != NULL && tail -> rq_next -> id != want_tid) {
                tail = tail -> rq_next;
            }
            if (tail -> rq_next-> rq_next != NULL) {
                want_thread = tail -> rq_next;
                // want_thread -> rq_next = NULL;
                tail -> rq_next = tail -> rq_next -> rq_next;
            } else {
                // tail -> rq_next-> rq_next == NULL
                want_thread = tail -> rq_next;
                // want_thread -> rq_next = NULL;
                tail -> rq_next = NULL;
            }
        }
        // Set want_thread status to running
        want_thread -> rq_next = NULL;
        want_thread -> status = STATUS_RUNNING;

        // Get current thread
        struct thread *curr_thread = tcb_array[current_running_thread -> id];
        curr_thread -> rq_next = NULL; 
        curr_thread -> status = STATUS_READY;

        // Insert curr_thread into ready queue only if exiting thread is not -10
        if (rq_head == NULL) {
            // Original ready_queue only have one TCB and it was removed from queue
            // So no TCB in ready_queue at this time
            rq_head = curr_thread;
        } else {
            // At least one tcb in ready_queue
            
            struct thread *tail = rq_head;
            while (tail -> rq_next != NULL) {
                tail = tail -> rq_next;
            }
            tail -> rq_next = curr_thread;
        }
    }

    // save all information of CPU register into thread stack pointer
    getcontext(&(tcb_array[current_running_thread -> id] -> current_ucontext));

    // Set running id to want_thread
    // current_running_thread_ID = want_thread -> id;
    // want_thread -> status = STATUS_RUNNING;

    if (control) {
        control = 0;

        // // Set running id to want_thread
        current_running_thread = want_thread;

        // want_thread -> status = STATUS_RUNNING;
        // // Reset saved register information into CPU 
        setcontext(&(want_thread -> current_ucontext));
    }

    struct thread *curr = exit_head;
    while (curr != NULL) {
        struct thread *temp = curr;
        curr = curr -> rq_next;
        if (temp -> each_wq != NULL) {
            wait_queue_destroy(temp -> each_wq);
            temp -> each_wq = NULL;
        }
        tcb_array[temp -> id] = NULL;
        free(temp -> helper_sp);
        temp -> helper_sp = NULL;
        free(temp);
        exit_head = NULL;
    }
    exit_head = NULL;
    interrupts_set(enable);
    return target_tid;
}

void
thread_exit(int exit_code)
{
    /* thread_exit should ensure that the current thread does not run after this
    * call, i.e., this function should never return. If there are other threads in
    * the system, one of them should be run. If there are no other threads (this is
    * the last thread invoking thread_exit), then the program should exit. In the
    * future, a new thread should be able to reuse this thread's identifier. The
    * function has no return values. */

    // thread_exit() terminates the current thread. It needs to set the state of 
    // the current thread's control block to signify that this thread is finished, 
    // and then yield. Any remaining cleanup such as freeing the stack has to be 
    // delegated a different thread (the next one that gets to run) because 
    // you can't free the stack of the currently executing thread.

    int enable = interrupts_off();
    // solve_memory_leak();
    // assert(rq_position(99) == -100);

    struct thread *record_thread = current_running_thread -> each_wq -> wq_head;
    thread_wakeup(current_running_thread -> each_wq, 1);
    if (record_thread != NULL) {
        record_thread -> waited = 1;
        record_thread -> record_exit = exit_code;
    }

    // if (current_running_thread -> each_wq -> wq_head != NULL) {
    //     if (current_running_thread -> each_wq -> wq_head -> record_exit != NULL) {
    //         *(current_running_thread -> each_wq -> wq_head -> record_exit) = exit_code;
    //     }
    //     thread_wakeup(current_running_thread -> each_wq, 1);
    // }
    // solve_memory_leak();

    // If ready_queue at least exist one READY thread
    // Remove current thread from tcb_array
    struct thread *exiting = current_running_thread;
    exiting -> status = STATUS_EXIT;
    // assert(rq_position(100) == -100);

    assert(exiting -> each_wq -> wq_head == NULL);
    wait_queue_destroy(exiting -> each_wq);
    exiting -> each_wq = NULL;
    assert(exiting -> each_wq == NULL);
    
    if (rq_head == NULL) {
        interrupts_set(enable);
        exit(exit_code);
    }
    if (exit_head == NULL) {
        exit_head = exiting;
        exiting -> rq_next = NULL;
    } else {
        struct thread *temp = exit_head;
        while (temp->rq_next != NULL) {
            temp = temp->rq_next;
        }
        temp->rq_next = exiting;
        exiting-> rq_next = NULL;
    }

    int control = 0;
    struct thread *target_thread = rq_head;
    rq_head = rq_head->rq_next;
    target_thread->rq_next = NULL;
    target_thread->status = STATUS_RUNNING;

    // solve_memory_leak();

    getcontext(&(current_running_thread->current_ucontext));  
    if (control == 0) {
        control = 1;
        current_running_thread = target_thread;
        setcontext(&(target_thread->current_ucontext));
    }
    interrupts_set(enable);

    // Last step, call yield
    // thread_yield(THREAD_ANY);

    // TBD();
}

Tid
thread_kill(Tid tid)
{
    /* Kill a thread whose identifier is tid. When a thread is killed, it should not
    * run any further. The calling thread continues to execute and receives the
    * result of the call. tid can be the identifier of any available thread.
    *
    * Upon success, return the identifier of the thread that was killed. Upon
    * failure, return the following:
    *
    * THREAD_INVALID: identifier tid does not correspond to a valid thread (e.g.,
    * any negative value of tid), or it is the current thread.*/

    // thread_kill() terminates a different thread (not the current one) given 
    // its tid. Because the thread being terminated is not currently running, 
    // you can do all the cleanup now.

     int enable = interrupts_off();
    // Check if tid is invalid
    if (tid == thread_id() || tid < 0 || tid >= THREAD_MAX_THREADS) {
        interrupts_set(enable);
        return THREAD_INVALID;
    }
    if (tcb_array[tid] == NULL) {
        interrupts_set(enable);
        return THREAD_INVALID;
    }

    // Get target_thread with tid
    struct thread *target_thread = tcb_array[tid];
    
    struct thread *iterator_wq = NULL;

    // for (int i = 0; i < THREAD_MAX_THREADS; i++) {
    //     if (tcb_array[i] != NULL && tcb_array[i] -> each_wq != NULL) {
    //         iterator_wq = tcb_array[i] -> each_wq -> wq_head;
    //         if (iterator_wq != NULL) {
    //             if (iterator_wq -> id == tid) {
    //                 tcb_array[i] -> each_wq -> wq_head =  tcb_array[i] -> each_wq -> wq_head -> rq_next;
    //             } else {
    //                 while (iterator_wq -> rq_next != NULL) {
    //                     if (iterator_wq -> rq_next -> id == tid) {
    //                         iterator_wq -> rq_next = iterator_wq -> rq_next -> rq_next;
    //                         break;
    //                     }
    //                     iterator_wq = iterator_wq -> rq_next;
    //                 } 
    //             }
    //         }
    //     }
    // }


    // if (target_thread -> each_wq -> wq_head != NULL) {
    //     if (target_thread -> each_wq -> wq_head -> record_exit != NULL) {
    //         *(target_thread -> each_wq -> wq_head -> record_exit) = THREAD_KILLED;
    //     }
    //     thread_wakeup(target_thread -> each_wq, 1);
    // }
    // assert(target_thread -> each_wq -> wq_head == NULL);
    // wait_queue_destroy(target_thread -> each_wq);
    // target_thread -> each_wq = NULL;

    // thread_wakeup(target_thread -> each_wq, 1);
    // if (target_thread -> each_wq -> wq_head != NULL) {
    //     target_thread -> each_wq -> wq_head -> waited = 1;
    //     target_thread -> each_wq -> wq_head -> record_exit = THREAD_KILLED;
    // }

    // assert(target_thread -> each_wq -> wq_head == NULL);
    // wait_queue_destroy(target_thread -> each_wq);
    // target_thread -> each_wq = NULL;

    if (target_thread -> status == STATUS_READY) {
        // target thread is READY thread, need to remove it from ready_queue
        if (rq_head -> id == tid) {
            // first READY thread in ready_queue is target_thread
            rq_head = target_thread -> rq_next;
        } else {
            struct thread *prev_thread = rq_head;
            while (prev_thread -> rq_next != NULL) {
                if (prev_thread -> rq_next -> id == tid) {
                    break;
                }
                prev_thread = prev_thread -> rq_next;
            }
            if (prev_thread -> rq_next != NULL) {
                prev_thread -> rq_next = prev_thread -> rq_next -> rq_next;
            } else {
                prev_thread -> rq_next = NULL;
            }
            
            // target_thread -> rq_next = NULL;
        }
    }
    else if (tcb_array[tid] -> status == STATUS_SLEEP) {
        for (int i = 0; i < THREAD_MAX_THREADS; i++) {
            if (tcb_array[i] != NULL && tcb_array[i] -> each_wq != NULL) {
                iterator_wq = tcb_array[i] -> each_wq -> wq_head;
                if (iterator_wq != NULL) {
                    if (iterator_wq -> id == tid) {
                        tcb_array[i] -> each_wq -> wq_head =  tcb_array[i] -> each_wq -> wq_head -> rq_next;
                    } else {
                        while (iterator_wq -> rq_next != NULL) {
                            if (iterator_wq -> rq_next -> id == tid) {
                                iterator_wq -> rq_next = iterator_wq -> rq_next -> rq_next;
                                break;
                            }
                            iterator_wq = iterator_wq -> rq_next;
                        } 
                    }
                }
            }
        }
    }

    struct thread *record_thread = target_thread -> each_wq -> wq_head;
    thread_wakeup(target_thread -> each_wq, 1);
    if (record_thread != NULL) {
        record_thread -> waited = 1;
        record_thread -> record_exit = THREAD_KILLED;
    }

    assert(target_thread -> each_wq -> wq_head == NULL);
    wait_queue_destroy(target_thread -> each_wq);
    target_thread -> each_wq = NULL;
    // Remove target_thread from tcb_array
    // free stack pointer of target_thread 

    // if (target_thread != NULL && target_thread -> helper_sp != NULL) {
        
    // }
    free(target_thread -> helper_sp);
    target_thread -> helper_sp = NULL;
    tcb_array[tid] -> helper_sp = NULL;
    // free target_thread
    free(target_thread);
    tcb_array[tid] = NULL;
    
    
    // TBD();
    interrupts_set(enable);
    return tid;
}

/**************************************************************************
 * Important: The rest of the code should be implemented in Assignment 3. *
 **************************************************************************/

/* make sure to fill the wait_queue structure defined above */
struct wait_queue *
wait_queue_create()
{
    /* create a queue of waiting threads. initially, the queue is empty. */
    struct wait_queue *wq;

    wq = malloc(sizeof(struct wait_queue));
    assert(wq);

    // TBD();
    wq -> wq_head = NULL;
    // wq -> wq_next = NULL;

    return wq;
}

void
wait_queue_destroy(struct wait_queue *wq)
{
    /* destroy the wait queue. be sure to check that the queue is empty when it is
    * being destroyed. */
    if (wq -> wq_head == NULL) {
        free(wq);
    }
}

Tid
thread_sleep(struct wait_queue *queue)
{
    /* suspend calling thread and run some other thread. The calling thread is put
    * in the wait queue. Upon success, return the identifier of the thread that
    * ran. The calling thread does not see this result until it runs later. Upon
    * failure, the calling thread continues running, and returns the following:
    *
    * THREAD_INVALID: queue is invalid, e.g., it is NULL.
    * THREAD_NONE:    no more threads, other than the caller, are available to
    *		   run. */
    int enable = interrupts_off();
    assert(!interrupts_enabled());

    if (queue == NULL) {
        interrupts_set(enable);
        return THREAD_INVALID;
    }

    if (rq_head == NULL) {
        interrupts_set(enable);
        return THREAD_NONE;
    }

    int control = 0;

    // Choose next runable thread in the ready queue and remove it from the ready queue
    struct thread *next_runable = rq_head;
    // struct thread *prev = NULL;

    Tid next_runable_ID = next_runable -> id;

    next_runable -> status = STATUS_RUNNING;

    rq_head = rq_head -> rq_next;
    next_runable -> rq_next = NULL;

    current_running_thread -> status = STATUS_SLEEP;
    current_running_thread -> rq_next = NULL;

    // Enqueue current_running_thread to wait_queue queue
    if (queue -> wq_head == NULL) {
        queue -> wq_head = current_running_thread;
        current_running_thread -> rq_next = NULL;
    } else {
        struct thread *temp = queue -> wq_head;
        while (temp -> rq_next != NULL) {
            temp = temp -> rq_next;
        }
        temp -> rq_next = current_running_thread;
        current_running_thread -> rq_next = NULL;
    }

    // Save current running thread ucontext
    getcontext(&(current_running_thread -> current_ucontext));

    if (control == 0) {
        control = 1;
        current_running_thread = next_runable;
        // if (next_runable -> status == STATUS_EXIT) {
        //     assert(0);
        // }
        //=============================================
        setcontext(&(next_runable -> current_ucontext));
    }

    struct thread *curr = exit_head;
    while (curr != NULL) {
        struct thread *temp = curr;
        curr = curr -> rq_next;
        if (temp -> each_wq != NULL) {
            wait_queue_destroy(temp -> each_wq);
            temp -> each_wq = NULL;
        }
        tcb_array[temp -> id] = NULL;
        free(temp -> helper_sp);
        temp -> helper_sp = NULL;
        free(temp);
        exit_head = NULL;
    }
    exit_head = NULL;

    // Tid next_runable_ID = next_runable -> id;
    interrupts_set(enable);
    return next_runable_ID;
    // return current_running_thread -> id;
    
    // TBD();
    // return THREAD_FAILED;
}

/* when the 'all' parameter is 1, wakeup all threads waiting in the queue.
 * returns whether a thread was woken up on not. */
int
thread_wakeup(struct wait_queue *queue, int all)
{
    /* wake up one or more threads that are suspended in the wait queue. These
    * threads are put in the ready queue. The calling thread continues to execute
    * and receives the result of the call. When "all" is 0, then one thread is
    * woken up.  When "all" is 1, all suspended threads are woken up. Wake up
    * threads in FIFO order, i.e., first thread to sleep must be woken up
    * first. The function returns the number of threads that were woken up. It can
    * return zero if there were no suspended threads in the wait queue. */
    int enable = interrupts_off();
    assert(!interrupts_enabled());
    // wait queue has no element i.e. invalid wait queue return 0
    if (queue == NULL || queue -> wq_head == NULL) {
        interrupts_set(enable);
        return 0;
    }

    // Only one thread wake up
    if (all == 0) {
        struct thread *adding_thread = NULL;
        if (queue -> wq_head != NULL) {
            struct thread *temp = queue -> wq_head;
            queue -> wq_head = temp -> rq_next;
            temp -> rq_next = NULL;
            adding_thread = temp;
        }
        adding_thread -> status = STATUS_READY;
        adding_thread -> rq_next = NULL;

        // Enqueue adding_thread to ready queue
        if (rq_head == NULL) {
            rq_head = adding_thread;
        } else {
            struct thread *curr = rq_head;
            while (curr -> rq_next) {
                curr = curr -> rq_next;
            }
            curr -> rq_next = adding_thread;
        }

        interrupts_set(enable);
        return 1;
    } else {
        // All threads wake up
        int total = 0;
        struct thread *iterator = NULL;
        if (queue -> wq_head != NULL) {
            iterator = queue -> wq_head;
            queue -> wq_head = NULL;
        }
        while (iterator != NULL) {
            struct thread *adding_thread = iterator;
            iterator = iterator -> rq_next;

            adding_thread -> status = STATUS_READY;
            adding_thread -> rq_next = NULL;

            // Enqueue adding_thread to ready queue
            if (rq_head == NULL) {
                rq_head = adding_thread;
            } else {
                struct thread *curr = rq_head;
                while (curr -> rq_next) {
                    curr = curr -> rq_next;
                }
                curr -> rq_next = adding_thread;
            }

            total += 1;
        }
        interrupts_set(enable);
        return total;
    }

    // TBD();
    // return 0;
}

/* suspend current thread until Thread tid exits */
Tid
thread_wait(Tid tid, int *exit_code)
{
    /* suspend current thread until the target thread (i.e., the thread whose 
    * identifier is tid) exits. If the target thread has already exited, then
    * thread_wait() returns immediately. 
    * Upon success, returns the identifier of the thread that exited. 
    * If exit_code is not NULL, then thread_wait() copies the exit status of the
    * target thread (i.e., the value that the target thread supplied to 
    * thread_exit) into the location pointed to by exit_code.
    * Upon failure, return the following:
    *
    * THREAD_INVALID: identifier tid does not correspond to a valid thread (e.g.,
    * any negative value of tid), or it is the current thread.
    */
    int enable = interrupts_off();
    // assert(rq_position(93) == -100);

    if (tid < 0 || tid >= THREAD_MAX_THREADS || tid == current_running_thread -> id 
    || tcb_array[tid] == NULL) {
        interrupts_set(enable);
        return THREAD_INVALID;
    } else {
        if (tcb_array[tid] -> status != STATUS_EXIT) {
            thread_sleep(tcb_array[tid] -> each_wq);
        }

        if (exit_code != NULL && current_running_thread -> waited == 1) {
            *exit_code = current_running_thread -> record_exit;
            interrupts_set(enable);
            return tid;
        } else {
            interrupts_set(enable);
            return THREAD_INVALID;
        }
        interrupts_set(enable);
        return 0;

    }

    

    // if (exit_code == NULL || current_running_thread -> waited == 0) {
    //     interrupts_set(enable);
    //     return THREAD_INVALID;
    // } else {
    //     *exit_code = current_running_thread -> record_exit;
    //     interrupts_set(enable);
    //     return tid;
    // }

    // if (exit_code != NULL) {
    //     current_running_thread -> record_exit = exit_code;
    // }

    // if (tcb_array[tid] -> each_wq != NULL && tcb_array[tid] -> each_wq -> wq_head != NULL) {
    //     struct thread *temp = tcb_array[tid] -> each_wq -> wq_head;
    //     while (temp -> rq_next != NULL) {
    //         if (temp -> id == tid) {
    //                 // if (tcb_array[tid] -> status != STATUS_EXIT) {
    //     thread_sleep(tcb_array[tid] -> each_wq);
    // // }
    //             interrupts_set(enable);
    //             return THREAD_INVALID;
    //         }
    //         temp = temp -> rq_next;
    //     }
    //     if (temp -> id == tid) {
    //             // if (tcb_array[tid] -> status != STATUS_EXIT) {
    //     thread_sleep(tcb_array[tid] -> each_wq);
    // // }
    //         interrupts_set(enable);
    //         return THREAD_INVALID;
    //     }
    //     temp -> rq_next = tcb_array[tid];
    //         // if (tcb_array[tid] -> status != STATUS_EXIT) {
    //     thread_sleep(tcb_array[tid] -> each_wq);
    // // }
    //     interrupts_set(enable);
    //     return THREAD_INVALID;
    // }

    // // if (tcb_array[tid] -> status == STATUS_EXIT) {
    // //     interrupts_set(enable);
    // //     return tid;
    // // }
    // // // Set record_exit to exit_code
    // // if (exit_code != NULL) {
    // //     current_running_thread -> record_exit = exit_code;
    // // }

    // // assert(current_running_thread -> status == STATUS_RUNNING);
    // // assert(current_running_thread -> rq_next == NULL);

    // // thread_sleep(tcb_array[tid] -> each_wq);
    
    // if (tcb_array[tid] -> status != STATUS_EXIT) {
    //     thread_sleep(tcb_array[tid] -> each_wq);
    // }

    // interrupts_set(enable);
    // // // assert(rq_position(94) == -100);
    // return tid;
}

struct lock {
    /* ... Fill this in ... */
    struct wait_queue *wq;
    int locked;
};

struct lock *
lock_create()
{
    int enable = interrupts_off();

    struct lock *lock;

    lock = malloc(sizeof(struct lock));
    assert(lock);

    lock -> wq = wait_queue_create();
    lock -> locked = 0;
    // TBD();
    interrupts_set(enable);
    return lock;
}

void
lock_destroy(struct lock *lock)
{
    int enable = interrupts_off();
    assert(lock != NULL);

    wait_queue_destroy(lock -> wq);
    free(lock);
    // TBD();
    interrupts_set(enable);
    // free(lock);
}

int test_and_set(struct lock *lock) {
    int old = lock -> locked;
    lock -> locked = 1;
    return old;
}

void
lock_acquire(struct lock *lock)
{
    int enable = interrupts_off();

    assert(lock != NULL);

    while (test_and_set(lock) == 1) {
        thread_sleep(lock -> wq);
    }
    // TBD();
    interrupts_set(enable);
}

void
lock_release(struct lock *lock)
{
    int enable = interrupts_off();
    assert(lock != NULL);
    if (lock -> locked  == 1) {
        lock -> locked = 0;
        thread_wakeup(lock -> wq, 1);
    }
    // TBD();
    interrupts_set(enable);
}

struct cv {
    /* ... Fill this in ... */
    struct wait_queue *wq;
};

struct cv *
cv_create()
{
    int enable = interrupts_off();
    struct cv *cv;

    cv = malloc(sizeof(struct cv));
    assert(cv);

    cv -> wq = wait_queue_create();
    // TBD();
    interrupts_set(enable);
    return cv;
}

void
cv_destroy(struct cv *cv)
{
    int enable = interrupts_off();
    assert(cv != NULL);

    // if (cv -> wq == NULL) {
    //     free(cv);
    // }
    wait_queue_destroy(cv -> wq);
    free(cv);
    // TBD();

    interrupts_set(enable);
}

void
cv_wait(struct cv *cv, struct lock *lock)
{
    int enable = interrupts_off();
    assert(cv != NULL);
    assert(lock != NULL);

    if (lock -> locked == 1) {
        lock_release(lock);
        thread_sleep(cv -> wq);
        lock_acquire(lock);
    }
    // TBD();
    interrupts_set(enable);
}

void
cv_signal(struct cv *cv, struct lock *lock)
{
    int enable = interrupts_off();
    assert(cv != NULL);
    assert(lock != NULL);

    if (lock -> locked == 1) {
        thread_wakeup(cv -> wq, 0);
    }

    // TBD();
    interrupts_set(enable);
}

void
cv_broadcast(struct cv *cv, struct lock *lock)
{
    int enable = interrupts_off();
    assert(cv != NULL);
    assert(lock != NULL);

    if (lock -> locked == 1) {
        thread_wakeup(cv -> wq, 1);
    }

    // TBD();
    interrupts_set(enable);
}
