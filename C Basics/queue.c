#include "queue.h"

/*************************************************
 * Implement the following functions.
 * DO NOT add any global (static) variables,
 * Except in Part C.
 * You can add help functions as you need.
 ************************************************/


/*************************************************
 * ------------------Part A:---------------------- 
 * In this part, you will implement
 * a  dynamically allocated queue with
 * a linked list. 
 * DO NOT add any global (static) variables.
 *************************************************/

int queue_A_initialized = 0;

typedef struct queue_A_node{

/* Add code BEGIN */
    struct queue_A_node *next_node;
    // Reverse order
    struct queue_A_node *prev_node;


/* Add code END */

    void* item;

} 
queue_A_node_t;

queue_A_node_t * queue_A_root = NULL;
queue_A_node_t * queue_A_tail = NULL;

/* Add code BEGIN */
// Help unctions only!



/* Add code END */

int queue_A_initialize(){

/* Add code BEGIN */
    // If queue has already initialized, then raise error
    if (queue_A_initialized == 1) {
        return ERR_INITIALIZED;
    }

    // If queue has not intialized, then set queue_A_initialized into 1
    queue_A_initialized = 1;
    
    // Dynamic allocated root and tail
    queue_A_root =  (queue_A_node_t *)malloc(sizeof(queue_A_node_t));
    queue_A_tail =  (queue_A_node_t *)malloc(sizeof(queue_A_node_t));
    
    // Initialize empty queue, both root and tail are NULL at this time
    queue_A_root -> next_node = queue_A_tail;
    queue_A_tail -> next_node = NULL;
    // Initialize reverse order of queue
    queue_A_root -> prev_node = NULL;
    queue_A_tail -> prev_node = queue_A_root;
    


/* Add code END */

    return 0;

}

int queue_A_enq(void* item){

/* Add code BEGIN */
    // If queue has not intialized, then raise error
    if(queue_A_initialized == 0){
        return ERR_NOT_INITALIZED;
    }

    // Initialize new node to store item
    queue_A_node_t *new_node = (queue_A_node_t*)malloc(sizeof(queue_A_node_t));
    
    // Set item of new node to item 
    new_node -> item = item;
    
    // Insert new_node into the node before tail of the queue (4 steps)
    // 1. Set next node of new node as the last node of queue
    new_node -> next_node = queue_A_tail;
    // 2. Set previous node of new node as previous node of last node of queue
    new_node -> prev_node = queue_A_tail -> prev_node;
    // 3. Set next node of previous node of tail to be new node
    queue_A_tail -> prev_node -> next_node = new_node;
    // 4. Set previous node of tail to be new node
    queue_A_tail -> prev_node = new_node;



/* Add code END */

    return 0;

}

int queue_A_deq(void** item){

/* Add code BEGIN */
    // If queue has not intialized, then raise error
    if (queue_A_initialized == 0) {
        return ERR_NOT_INITALIZED;
    }

    // If queue is empty, then raise error
    if (queue_A_root -> next_node == queue_A_tail) {
        return ERR_NO_ITEM;
    }

    // Create a temp queue node to store next node from root of original queue
    queue_A_node_t *root_next_queue = queue_A_root -> next_node;

    // Set item pointer to the item of next node from root of original queue
    *item = root_next_queue -> item;

    // Remove the next node of root node of queue (2 steps)
    // 1. Set next node of queue root to the next node from root of original queue
    queue_A_root -> next_node = root_next_queue -> next_node;
    // 2. Set previous node of next node of temp node to be root node of queue
    root_next_queue -> next_node -> prev_node = queue_A_root;

    // Free removed root node of queue 
    free(root_next_queue);


/* Add code END */

    return 0;

}

int queue_A_destroy(){

/* Add code BEGIN */
    // If queue has not intialized, then raise error
    if (queue_A_initialized == 0) {
        return ERR_NOT_INITALIZED;
    }

    // If queue has already intialized, then set queue to be not intialized
    queue_A_initialized = 0;

    // Initialize temp node to be the root node
    queue_A_node_t* temp = queue_A_root;

    while (temp != NULL) {
        // Initialize temp_next node to be the next node of temp node
        queue_A_node_t *temp_next = temp -> next_node;

        // Free temp node
        free(temp);

        // Set temp node to be the next node of temp node
        temp = temp_next;
    }


/* Add code END */

    return 0;

}

/* END of Part A */

/*************************************************
 * ------------------Part B:---------------------- 
 * In this part, you will implement a 
 * static queue with a max capacity using
 * the RING BUFFER introduced in the handout. 
 * DO NOT add any global (static) variables.
 * Rename the given variables to match your 
 * coding style. 
 *************************************************/

int queue_B_initialized = 0;

void * queue_B_items[PART_B_MAX_SIZE];

unsigned long queue_B_var_1 = 0, queue_B_var_2 = 0, queue_B_var_3 = 0;

/* Edit macros below for your convience BEGIN */
#define EDIT_VAR_NAME_1 queue_B_var_1 
#define EDIT_VAR_NAME_2 queue_B_var_2 
#define EDIT_VAR_NAME_3 queue_B_var_3
/* Edit macros END */ 


/* Add code BEGIN */
// Help unctions only!



/* Add code END */

int queue_B_initialize(){

/* Add code BEGIN */
// If ring buffer has already initialized, then raise error
    if (queue_B_initialized == 1) {
        return ERR_INITIALIZED;
    }

    // Set ring buffer to be initialized
    queue_B_initialized = 1;

    // Set variable1 to 0, represents the current enqueue buffer. 
    EDIT_VAR_NAME_1 = 0;
    // Set variable2 to 0, represents the current dequeue buffer. 
    EDIT_VAR_NAME_2 = 0;
    // Set variable3 to 0, represents the total buffers
    EDIT_VAR_NAME_3 = 0;


/* Add code END */

    return 0;

}

int queue_B_enq(void* item){

/* Add code BEGIN */
    // If ring buffer has not initialized, then raise error
    if (queue_B_initialized == 0) {
        return ERR_NOT_INITALIZED;
    }

    // If the total buffers exceeds MAX SIZE of the ring buffer, then raise error
    if (EDIT_VAR_NAME_3 >= PART_B_MAX_SIZE) {
        return ERR_NO_MEM;
    }

    // Add total buffer by 1
    EDIT_VAR_NAME_3 += 1;

    // Set current buffer to item
    queue_B_items[EDIT_VAR_NAME_1] = item;
    EDIT_VAR_NAME_1 += 1;
    EDIT_VAR_NAME_1 %= PART_B_MAX_SIZE;


    



/* Add code END */

    return 0;

}

int queue_B_deq(void** item){

/* Add code BEGIN */
    // If ring buffer has not initialized, then raise error
    if (queue_B_initialized == 0) {
        return ERR_NOT_INITALIZED;
    }

    // If the ring buffer is empty, then raise error
    if (EDIT_VAR_NAME_3 == 0){
        return ERR_NO_ITEM;
    }

    // Reduce total buffer by 1
    EDIT_VAR_NAME_3 -= 1;

    // Set current buffer to item
    *item = queue_B_items[EDIT_VAR_NAME_2];
    EDIT_VAR_NAME_2 += 1;
    EDIT_VAR_NAME_2 %= PART_B_MAX_SIZE;



/* Add code END */

    return 0;

}

int queue_B_destroy(){

/* Add code BEGIN */
    // If ring buffer has not intialized, then raise error
    if (queue_B_initialized == 0) {
        return ERR_NOT_INITALIZED;
    }

    // If ring buffer has already intialized, then set ring buffer to be not intialized
    queue_B_initialized = 0;

    // Reset the current buffer to 0
    EDIT_VAR_NAME_1 = 0;
    EDIT_VAR_NAME_2 = 0;
    EDIT_VAR_NAME_3 = 0;


/* Add code END */

    return 0;

}

/* END of Part B */

/*************************************************
 * ------------------Part C:---------------------- 
 * In this part, you will implement a
 *  dynamically allocated queue. The queue 
 * should support any large number of items.
 * 
 * You are free to call malloc as you need.
 * However, unlike Part A where each enqueue
 * needs a malloc, you need to follow
 * the rules below WHEN the number of
 * elements reaches 1000:
 * 1.   the number of malloc calls shall not
 *      exceed 1 per 100 elements added
 * 2.   the overall space efficiency 
 *      (total bytes of the items stored/
 *      total bytes malloced) shall be higher
 *      than 80% 
 * You are also required to 
 * recycle all the space freed. That is,
 * the above rules shall always be kept
 * even in the process of dequeue. 
 * 
 * You MAY add any necessary global (static)
 * variables in this part, as well as
 * structs/types and help functions. 
 *************************************************/

int queue_C_initialized = 0;

/* Add code BEGIN */
#define PART_C_MAX_SIZE 1000
#define MAX_STEP 160
#define MIN_STEP 115
#define RIGHT_MIN 12

unsigned long queue_C_var_1 = 0, queue_C_var_2 = 0, queue_C_var_3 = 0, queue_C_var_4 = 0;

/* Edit macros below for your convience BEGIN */
#define ENQUEUE queue_C_var_1
#define DEQUEUE queue_C_var_2
#define ALL_ENQUEUE queue_C_var_3
#define TOTAL_SPACE queue_C_var_4

void ** queue_C_items;

/* Add code END */

int queue_C_initialize(){

/* Add code BEGIN */
    // If queue has already initialized, then raise error
    if (queue_C_initialized == 1) {
        return ERR_INITIALIZED;
    }

    // If stack has not intialized, then set stack_C_initialized into 1
    queue_C_initialized = 1;
    // malloc queue items space
    queue_C_items = malloc(PART_C_MAX_SIZE * sizeof(void *));

    // EXIST enqueue total number
    ENQUEUE = 0;
    // ALL enqueue total number
    ALL_ENQUEUE = 0;
    // EXIST dequeue total number
    DEQUEUE = 0;
    // total max number
    TOTAL_SPACE = PART_C_MAX_SIZE;


/* Add code END */

    return 0;

}

int queue_C_enq(void* item) {

/* Add code BEGIN */
    // If queue has not intialized, then raise error
    if (queue_C_initialized == 0) {
        return ERR_NOT_INITALIZED;
    }

    // Set queue items
    queue_C_items[ENQUEUE] = item;
    // Increase the end of the queue by 1
    ENQUEUE += 1;
    // Increase the total enqueue number by 1
    ALL_ENQUEUE += 1;

    // The total enqueue number must larger than 1000
    if (ALL_ENQUEUE >= PART_C_MAX_SIZE) {
        // If at this time, the right end side of queue just left a few number of the total space,
        // then we need to let all queue shift left 
        if ((TOTAL_SPACE - ENQUEUE) <= RIGHT_MIN) {
        // if ((DEQUEUE + TOTAL_SPACE - ENQUEUE) >= MAX_STEP || (TOTAL_SPACE - ENQUEUE) <= RIGHT_MIN) {
            // Let all queue shift left by Min space number of position
            TOTAL_SPACE += MIN_STEP;
            // Realloc all queue items
            queue_C_items = realloc(queue_C_items, (TOTAL_SPACE) * sizeof(void *));
            // move queue to the front
            for (int i = 0; i < ENQUEUE - DEQUEUE; i++) {
                queue_C_items[i] = queue_C_items[i + DEQUEUE];
            }

            // Since all queue items shift left by DEQUEUE position, 
            // both the end of queue and the dequeue number will be reduced by *DEQUEUE* position
            ENQUEUE -= DEQUEUE;
            // TOTAL_SPACE -= DEQUEUE;
            DEQUEUE = 0;
        }
    }

/* Add code END */

    return 0;

}

int queue_C_deq(void** item){

/* Add code BEGIN */
    // If queue has not intialized, then raise error
    if (queue_C_initialized == 0) {
        return ERR_NOT_INITALIZED;
    }

    // If queue is empty, then raise error
    if (ENQUEUE - DEQUEUE == 0) {
        return ERR_NO_ITEM;
    }

    // Set queue items
    *item = queue_C_items[DEQUEUE];
    // Increase total dequeue number by 1
    DEQUEUE += 1;

    // The total enqueue number must larger than 1000
    if (ALL_ENQUEUE >= PART_C_MAX_SIZE && (DEQUEUE + TOTAL_SPACE - ENQUEUE) >= MAX_STEP) {
        // DEQUEUE will cause the all array shift left when (left space + right space) >= Max space number,
        // so reduce total space number by Min space number
        TOTAL_SPACE -= MIN_STEP;
        // move queue to the front
        for (int i = 0; i < ENQUEUE - DEQUEUE; i++) {
            queue_C_items[i] = queue_C_items[i + DEQUEUE];
        }
        // realloc queue items
        queue_C_items = realloc(queue_C_items, (TOTAL_SPACE) * sizeof(void *));

        // Since all queue items shift left by DEQUEUE position, 
        // both the end of queue and the dequeue number will be reduced by *DEQUEUE* position
        ENQUEUE -= DEQUEUE;
        // TOTAL_SPACE -= DEQUEUE;
        DEQUEUE = 0;
    }


/* Add code END */

    return 0;

}

int queue_C_destroy(){

/* Add code BEGIN */
    // If queue has not intialized, then raise error
    if (queue_C_initialized == 0) {
        return ERR_NOT_INITALIZED;
    }

    queue_C_initialized = 0;

    free(queue_C_items);

    ENQUEUE = 0;
    DEQUEUE = 0;
    ALL_ENQUEUE = 0;
    TOTAL_SPACE = 0;


/* Add code END */

    return 0;

}

/* End of Part C */
