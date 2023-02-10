#include "stack.h"

/************************
 * Implement the following functions.
 * DO NOT add any global (static) variables,
 * Except in Part C.
 * You can add help functions as you need.
 * **********************/

/*************************************************
 * ------------------Part A:---------------------- 
 * In this part, you will implement
 * a  dynamically allocated stack with
 * a linked list. 
 * DO NOT add any global (static) variables.
 *************************************************/

int stack_A_initialized = 0;

typedef struct stack_A_node{

/* Add code BEGIN */
    struct stack_A_node* next_node;



/* Add code END */

    void* item;

} 
stack_A_node_t;

stack_A_node_t * stack_A_root = NULL;
stack_A_node_t * stack_A_tail = NULL;

/* Add code BEGIN */
// Help unctions only!



/* Add code END */

int stack_A_initialize(){

/* Add code BEGIN */
    // If stack has already initialized, then raise error
    if (stack_A_initialized == 1) {
        return ERR_INITIALIZED;
    }

    // If stack has not intialized, then set stack_A_initialized into 1
    stack_A_initialized = 1;
    
    // Dynamic allocated root and tail
    stack_A_root =  (stack_A_node_t *)malloc(sizeof(stack_A_node_t));
    stack_A_tail =  (stack_A_node_t *)malloc(sizeof(stack_A_node_t));
    
    // Initialize empty stack, both root and tail are NULL at this time
    stack_A_root -> next_node = stack_A_tail;
    stack_A_tail -> next_node = NULL;

    




/* Add code END */

    return 0;

}

int stack_A_pop(void** item){

/* Add code BEGIN */
    // If stack has not intialized, then raise error
    if (stack_A_initialized == 0) {
        return ERR_NOT_INITALIZED;
    }

    // If stack is empty, then raise error
    if (stack_A_root -> next_node == stack_A_tail) {
        return ERR_NO_ITEM;
    }

    // Create a temp stack to store next node from root of original stack
    stack_A_node_t *root_next_stack = stack_A_root -> next_node;

    // Set item pointer to the item of next node from root of original stack
    *item = root_next_stack -> item;

    // Set next node of stack root to the next node from root of original stack
    stack_A_root -> next_node = root_next_stack -> next_node;

    // Free removed root node of stack 
    free(root_next_stack);


/* Add code END */

    return 0;

}

int stack_A_push(void* item){

/* Add code BEGIN */
    // If stack has not intialized, then raise error
    if (stack_A_initialized == 0) {
        return ERR_NOT_INITALIZED;
    }

    // Initialize new node to store item
    stack_A_node_t *new_node = (stack_A_node_t*)malloc(sizeof(stack_A_node_t));
    
    // Set item of new node to item 
    new_node -> item = item;
    
    // Set next node of new node as the first node of stack
    new_node -> next_node = stack_A_root -> next_node;
    
    // Set the next node of stack root to the new node
    stack_A_root -> next_node = new_node;


/* Add code END */

    return 0;

}

int stack_A_destroy(){

/* Add code BEGIN */
    // If stack has not intialized, then raise error
    if (stack_A_initialized == 0) {
        return ERR_NOT_INITALIZED;
    }

    // If stack has already intialized, then set stack to be not intialized
    stack_A_initialized = 0;

    // Initialize temp node to be the root node
    stack_A_node_t* temp = stack_A_root;

    while (temp != NULL) {
        // Initialize temp_next node to be the next node of temp node
        stack_A_node_t *temp_next = temp -> next_node;

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
 * static stack with a max capacity using
 * the RING BUFFER introduced in the handout. 
 * DO NOT add any global (static) variables.
 * Rename the given variables to match your 
 * coding style. 
 *************************************************/

int stack_B_initialized = 0;

void * stack_B_items[PART_B_MAX_SIZE];

unsigned long stack_B_var_1 = 0, stack_B_var_2 = 0, stack_B_var_3 = 0;

/* Edit macros below for your convience BEGIN */
#define EDIT_VAR_NAME_1 stack_B_var_1 
#define EDIT_VAR_NAME_2 stack_B_var_2 
#define EDIT_VAR_NAME_3 stack_B_var_3
/* Edit macros END */ 


/* Add code BEGIN */
// Help unctions only!



/* Add code END */

int stack_B_initialize(){

/* Add code BEGIN */
    // If ring buffer has already initialized, then raise error
    if (stack_B_initialized == 1) {
        return ERR_INITIALIZED;
    }

    // Set ring buffer to be initialized
    stack_B_initialized = 1;
    
    // Set variable1 to 0, represents the current buffer. 
    // At this time, current buffer is the head of ring buffer
    EDIT_VAR_NAME_1 = 0;

    // Set variable2 to PART_B_MAX_SIZE, represents we have already initialized 
    EDIT_VAR_NAME_2 = PART_B_MAX_SIZE;



/* Add code END */

    return 0;

}

int stack_B_pop(void** item){

/* Add code BEGIN */
    // If ring buffer has not initialized, then raise error
    if (stack_B_initialized == 0) {
        return ERR_NOT_INITALIZED;
    }

    // If the head of ring buffer == 0 and the tail of ring buffer is PART_B_MAX_SIZE, 
    // then this ring buffer must be empty and raise error
    if (EDIT_VAR_NAME_1 == 0 && EDIT_VAR_NAME_2 == PART_B_MAX_SIZE){
        return ERR_NO_ITEM;
    }

    // Set the pointer of item to be the current buffer
    *item = stack_B_items[EDIT_VAR_NAME_1 - 1];
    EDIT_VAR_NAME_1 -= 1;


/* Add code END */

    return 0;

}

int stack_B_push(void* item){

/* Add code BEGIN */
    // If ring buffer has not initialized, then raise error
    if (stack_B_initialized == 0) {
        return ERR_NOT_INITALIZED;
    }

    // If the current buffer reached the end of the ring buffer, then raise error
    if (EDIT_VAR_NAME_1 >= EDIT_VAR_NAME_2) {
        return ERR_NO_MEM;
    }

    // Set current buffer to item
    stack_B_items[EDIT_VAR_NAME_1] = item;

    // Set current buffer to the next buffer
    EDIT_VAR_NAME_1 += 1;

/* Add code END */

    return 0;

}

int stack_B_destroy(){

/* Add code BEGIN */
    // If ring buffer has not intialized, then raise error
    if (stack_B_initialized == 0) {
        return ERR_NOT_INITALIZED;
    }

    // If ring buffer has already intialized, then set ring buffer to be not intialized
    stack_B_initialized = 0;

    // Reset the current buffer to 0
    EDIT_VAR_NAME_1 = 0;
    // Reset the tail buffer to 0
    EDIT_VAR_NAME_2 = 0;


/* Add code END */

    return 0;

}

/* END of Part B */

/*************************************************
 * ------------------Part C:---------------------- 
 * In this part, you will implement a
 *  dynamically allocated stack. The stack 
 * should support any large number of items.
 * 
 * You are free to call malloc as you need.
 * However, unlike Part A where each push
 * needs a malloc, you need to follow
 * the rules below to minimize the use of 
 * the malloc WHEN the number of
 * elements reaches 1000:
 * 1.   the number of malloc calls shall not
 *      exceed 1 per 100 elements added
 * 2.   the overall space efficiency 
 *      (total bytes of the items stored
 *      total bytes malloced) shall be higher
 *      than 80% 
 * You are also required to 
 * recycle all the space freed. That is,
 * the above rules shall always be kept
 * even in the process of pop. 
 * 
 * You MAY add any necessary global (static)
 * variables in this part, as well as
 * structs/types and help functions. 
 *************************************************/

int stack_C_initialized = 0;

/* Add code BEGIN */

#define PART_C_MAX_SIZE 1000
#define EFFICIENCY 0.85

unsigned long stack_C_var_1 = 0, stack_C_var_2 = 0, stack_C_var_3 = 0;

/* Edit macros below for your convience BEGIN */
#define C_EDIT_VAR_NAME_1 stack_C_var_1 
#define C_EDIT_VAR_NAME_2 stack_C_var_2 
#define C_EDIT_VAR_NAME_3 stack_C_var_3

void ** stack_C_items;



/* Add code END */

int stack_C_initialize(){

/* Add code BEGIN */
    // If stack has already initialized, then raise error
    if (stack_C_initialized == 1) {
        return ERR_INITIALIZED;
    }

    // If stack has not intialized, then set stack_C_initialized into 1
    stack_C_initialized = 1;

    stack_C_items = malloc(PART_C_MAX_SIZE * sizeof(void *));

    // NAME_1 represents the pointer to the end of array
    C_EDIT_VAR_NAME_1 = 0;
    // Set NAME_2 to be 1000 to initialize the stack
    C_EDIT_VAR_NAME_2 = PART_C_MAX_SIZE;



/* Add code END */

    return 0;

}

int stack_C_pop(void** item){

/* Add code BEGIN */
    // If stack has not intialized, then raise error
    if (stack_C_initialized == 0) {
        return ERR_NOT_INITALIZED;
    }

    // If stack is empty, then raise error
    if (C_EDIT_VAR_NAME_1 == 0) {
        return ERR_NO_ITEM;
    }
    // If item is NULL, raise error
    if (item == NULL) {
        return ERR_INVALID_ARG;
    }

    // Pop element will reduce NAME_1 by 1
    C_EDIT_VAR_NAME_1 -= 1;
    // Set items
    *item = stack_C_items[C_EDIT_VAR_NAME_1];

    // If the empty space is large enough
    float temp_result = (float) C_EDIT_VAR_NAME_1 / C_EDIT_VAR_NAME_2;
    if (C_EDIT_VAR_NAME_2 > PART_C_MAX_SIZE && temp_result < EFFICIENCY) {
        
        // Decrase the (1 - EFFICIENCY) percentage of the end of stack
        C_EDIT_VAR_NAME_2 *= EFFICIENCY;

        // realloc total size of original stack
        stack_C_items = realloc(stack_C_items, C_EDIT_VAR_NAME_2 * sizeof(void *));

        // Check stack is NULL or not
        if (stack_C_items == NULL) {
            return ERR_NO_MEM;
        }
    }




    


/* Add code END */

    return 0;

}

int stack_C_push(void* item){

/* Add code BEGIN */
    // If stack has not intialized, then raise error
    if (stack_C_initialized == 0) {
        return ERR_NOT_INITALIZED;
    }

    // If the element in stack is full
    if (C_EDIT_VAR_NAME_1 == C_EDIT_VAR_NAME_2) {

        // Increase 15% of orignial stack space
        C_EDIT_VAR_NAME_2 *= (2 - EFFICIENCY);
        C_EDIT_VAR_NAME_2 = (int) C_EDIT_VAR_NAME_2;

        // realloc stack elements
        stack_C_items = realloc(stack_C_items, C_EDIT_VAR_NAME_2 * sizeof(void *));

        // Check stack is NULL or not
        if (stack_C_items == NULL) {
            return ERR_NO_MEM;
        }
    }
    
    // Set items
    stack_C_items[C_EDIT_VAR_NAME_1] = item;
    // Increase the start of stack by 1
    C_EDIT_VAR_NAME_1 += 1;


    


/* Add code END */

    return 0;

}

int stack_C_destroy(){

/* Add code BEGIN */
    // If stack has not intialized, then raise error
    if (stack_C_initialized == 0) {
        return ERR_NOT_INITALIZED;
    }

    stack_C_initialized = 0;

    free(stack_C_items);

    C_EDIT_VAR_NAME_1 = 0;
    C_EDIT_VAR_NAME_2 = 0;




/* Add code END */

    return 0;

}

/* End of Part C */
