#ifndef __UTIL_H__
#define __UTIL_H__

#include "common.h"

/**
 * Postion in x and y axes.
*/
typedef struct {
    uint8_t x;
    uint8_t y;
} Position;

/**
 * Node of stack.
*/
typedef struct Node {
    Position pos;
    struct Node *next;
} Node;

/**
 * Stack.
*/
typedef struct {
    Node *head;
    Node *tail;
} Stack;


/**
 * Initialize a stack.
 * \returns a valid Stack struct on success, NULL on error
*/
Stack *stack_init();

/**
 * Push a node to head of stack.
 * \param stack stack
 * \param pos data to push in stack
*/
void stack_push(Stack *stack, Position pos);

/**
 * Pop a node from head of stack.
 * \param stack stack
*/
void stack_pop(Stack *stack);

/**
 * Free all nodes in stack.
 * \param stack stack
*/
void stack_free(Stack *stack);

/**
 * Display stack.
 * \param stack stack
*/
void stack_display(Stack *stack);

/**
 * Check if stack is empty.
 * \param stack stack
 * \returns true on success, false on error
*/
bool stack_is_empty(Stack *stack);

/**
 * Get length of linked stack.
 * \param stack stack
 * \returns number of nodes in stack
*/
uint16_t stack_len(Stack *stack);


#endif