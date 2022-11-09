#include "util.h"


Stack *stack_init() {
    Stack *stack = calloc(1, sizeof *stack);
    stack->head = NULL;
    stack->tail = NULL;

    return stack;
}

void stack_push(Stack *stack, Position pos) {
    // Create a new node to push
    Node *new = calloc(1, sizeof *new);
    if (!new) {
        fprintf(stderr, "ERROR : malloc, in func stack_push\n");
    }
    new->next = NULL;
    new->pos = pos;
    // Push it to stack
    if (stack_is_empty(stack)) {
        stack->head = new;
        stack->tail = new;
    } else {
        new->next = stack->head;
        stack->head = new;
    }
}

void stack_pop(Stack *stack) {
    if (!stack_is_empty(stack)) {
        Node *pop = stack->head;
        stack->head = stack->head->next;
        pop->next = NULL;
        free(pop);
        printf("popped\n");
    }
}

void stack_free(Stack *stack) {
    while (!stack_is_empty(stack)) {
        stack_pop(stack);
    }
    free(stack);
}

void stack_display(Stack *stack) {
    Node *tmp = stack->head;
    while (tmp != NULL) {
        printf("(%d, %d)->", tmp->pos.x, tmp->pos.y);
        tmp = tmp->next;
    }
    if (tmp == NULL) {
        printf("NULL\n");
    }
}

bool stack_is_empty(Stack *stack) {
    return (stack->head == NULL);
}

uint16_t stack_len(Stack *stack) {
    uint16_t length = 0;
    Node *tmp = NULL;
    for (tmp = stack->head; tmp != NULL; tmp = tmp->next) {
        length++;
    }

    return length;
}