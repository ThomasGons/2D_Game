#include "util.h"


Stack *stack_init() {
    Stack *stack = calloc(1, sizeof (*stack));
    if (!stack) {
        fprintf(stderr, "\x1b[1m%s:\x1b[0m In function '%s':\033[31m %s:%i: error: calloc failed\033[0m\n",
            __FILE__, __func__, __FILE__, ((__LINE__) - 3));
        exit(2);
    }
    stack->head = NULL;
    stack->tail = NULL;

    return stack;
}

void stack_push(Stack *stack, Position pos, Action act) {
    // Create a new node to push
    SNode *new = calloc(1, sizeof (*new));
    if (!new) {
        fprintf(stderr, "\x1b[1m%s:\x1b[0m In function '%s':\033[31m %s:%i: error: calloc failed\033[0m\n",
            __FILE__, __func__, __FILE__, ((__LINE__) - 3));
        exit(2);
    }
    new->next = NULL;
    new->pos = pos;
    new->action = act;
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
        SNode *pop = stack->head;
        stack->head = stack->head->next;
        pop->next = NULL;
        free(pop);
    }
}

Position stack_remove(Stack *stack, uint16_t index) {
    Position ret = {0, 0};
    if (!stack_is_empty(stack)) {
        SNode *tmp = NULL;
        SNode *curr = stack->head;
        for (uint16_t count = 0; count < index; count++) {
            tmp = curr;
            curr = curr->next;
        }
        tmp->next = curr->next;
        ret = curr->pos;
        free(curr);
        return ret;
    }
    return ret;
}

int16_t stack_get_index(Stack *stack, Position pos) {
    int16_t index = 0;
    SNode *tmp = stack->head;
    while (tmp != NULL) {
        if (tmp->pos.l == pos.l && tmp->pos.c == pos.c) {
            return index;
        }
        index++;
        tmp = tmp->next;
    }
    return -1;
}

void stack_free(Stack *stack) {
    while (!stack_is_empty(stack)) {
        stack_pop(stack);
    }
    free(stack);
}

void stack_display(Stack *stack) {
    SNode *tmp = stack->head;
    while (tmp != NULL) {
        printf("(l:%d, c:%d, act:%d)->", tmp->pos.l, tmp->pos.c, tmp->action);
        tmp = tmp->next;
    }
    if (tmp == NULL) {
        printf("NULL\n");
    }
    printf("\n");
}

bool stack_is_empty(Stack *stack) {
    return (stack->head == NULL);
}

uint16_t stack_len(Stack *stack) {
    uint16_t length = 0;
    SNode *tmp = NULL;
    for (tmp = stack->head; tmp != NULL; tmp = tmp->next) {
        length++;
    }
    return length;
}

Stack *stack_change_order(Stack *stack, Stack *new_stack) {
    SNode *tmp = stack->head;
    while (tmp != NULL){
        stack_push(new_stack, tmp->pos, tmp->action);
        tmp = tmp->next;
    }
    stack_free(stack);
    return new_stack;
}