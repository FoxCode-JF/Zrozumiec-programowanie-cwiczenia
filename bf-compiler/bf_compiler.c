#include <stdio.h>
#include <stdbool.h>
#include "bf_compiler.h"

/**
 * @brief Structure which holds a single node of a stack.
 */
typedef struct
{
    uint32_t data;
    struct stack *next;
    struct stack *prev;
} stack_t;

static void bf_init(FILE *outputFile);
static uint32_t stack_top(stack_t *pStack);
static void stack_pop(stack_t *pStack);
static void stack_push(stack_t *pStack, uint32_t data);

/*
 *       BF instruction | equivalent in C
 *       ---------------+--------------------------
 *       (Init)         | char array[30000] = {0};
 *                      | char *p=array;
 *       ---------------|--------------------------
 *       >              | ++p;
 *       <              | --p;
 *       +              | ++(*p);
 *       -              | --(*p);
 *       .              | putchar(*p);
 *       ,              | *p=getchar();
 *       [              | while(*p)
 *       ]              | }
 */

/**
 * @brief Compiles brainfuck in the provided file into machine code
 *
 * @param path Path to the *.bf file
 * @return bf_errorType
 */
bf_errorType bf_compile(char *path)
{
    FILE *pInputFile;
    FILE *pOutputFile;
    uint32_t instr_cnt = 0;

    stack_t *while_begin_stack = NULL;
    stack_t *while_end_stack = NULL;

    // Open file in read mode
    pInputFile = fopen(path, "r");
    pOutputFile = fopen(path, "wb");

    // Check if file opened successfully
    if (pInputFile == NULL)
    {
        printf("Error opening the input file.\n");
        return BF_INPUT_FILE_ERROR;
    }

    if (pOutputFile == NULL)
    {
        printf("Error opening the output file.\n");
        return BF_OUTPUT_FILE_ERROR;
    }

    printf("File opened successfully.\n");
    char instruction;
    bf_init(pOutputFile);

    while (EOF != (instruction = getc(pInputFile)))
    {
        switch (instruction)
        {
        case '>':
            // increment_ptr:
            // vadd r1, r2(0x01)
            // machine code: '10 02 01'
            uint8_t inc_ptr_buf[] = {0x10, 0x02, 0x01};
            fwrite(inc_ptr_buf, sizeof(inc_ptr_buf), 1, pOutputFile);
            break;
        case '<':
            // decrement_ptr:
            // vsub r1, r2(0x01)
            // machine code: '11 02 01'
            uint8_t dec_ptr_buf[] = {0x11, 0x02, 0x01};
            fwrite(dec_ptr_buf, sizeof(dec_ptr_buf), 1, pOutputFile);
            break;
        case '+':
            // increment_val:
            // vxor r3, r3
            // vldb r3, r1
            // vadd r3, r2
            // vstb r1, r3
            uint8_t inc_val_buf[] =
                {
                    0x17, 0x03, 0x03,
                    0x04, 0x03, 0x01,
                    0x10, 0x03, 0x02,
                    0x05, 0x01, 0x03};
            fwrite(inc_val_buf, sizeof(inc_val_buf), 1, pOutputFile);
            break;
        case '-':
            // increment_val:
            // vxor r3, r3
            // vldb r3, r1
            // vsub r3, r2
            // vstb r1, r3
            uint8_t sub_val_buf[] =
                {
                    0x17, 0x03, 0x03,
                    0x04, 0x03, 0x01,
                    0x11, 0x03, 0x02,
                    0x05, 0x01, 0x03};
            fwrite(sub_val_buf, sizeof(sub_val_buf), 1, pOutputFile);
            break;
        case '.':
            // putchar:
            // vxor r3, r3
            // vldb r3, r1
            // voutb 0x20, r3
            uint8_t putchar_buf[] =
                {
                    0x17, 0x03, 0x03,
                    0x04, 0x03, 0x01,
                    0xf2, 0x03, 0x20};
            fwrite(putchar_buf, sizeof(putchar_buf), 1, pOutputFile);
            break;
        case ',':
            // getchar:
            // vxor r3, r3
            // vinb 0x20, r3
            // vldb r3, r1
            uint8_t getchar_buf[] =
                {
                    0x17, 0x03, 0x03,
                    0xf3, 0x03, 0x20,
                    0x04, 0x03, 0x01};
            fwrite(getchar_buf, sizeof(getchar_buf), 1, pOutputFile);
            break;
        case '[':
            // begin_while:
            // push current IP to the stack to come back here later when ending bracket is met
            // vpush r15
            // TODO: fix below code for the loop
            uint8_t begin_while_buf[] =
                {
                    0x00, 0x07, 0x0f,
                    0x11, 0x07, 0x02,
                    0x30, 0x07};
            fwrite(begin_while_buf, sizeof(begin_while_buf), 1, pOutputFile);
            break;
        case ']':
        // end_while:
        // vpop r7
        // vldb r3 r1
        // vcmp r3 r10
        // vje  0x02 (jump relative after next instruction - VJMPR takes two bytes)
        // otherwise jump unconditionally to r7 (this is where address of begin_while resides)
        // VJMPR r7
        TODO:
            uint8_t end_while_buf[] =
                {
                    0x04, 0x03, 0x01,
                    0x20, 0x03, 0x10,
                    0x30, 0x07};
            fwrite(end_while_buf, sizeof(end_while_buf), 1, pOutputFile);

            break;
        default:
            break;
        }
        instr_cnt++;
    }

    // Close the file
    fclose(pInputFile);
}

/**
 * @brief Initializes bytes array(memory) and a pointer to the firs element
 *        char array[30000] = {0};
 *        char *p=array;
 */
static void bf_init(FILE *outputFile)
{
    uint8_t init_buf[] =
        {
            // Zero value for comparing
            // vset r10, 0
            0x01, 0x10, 0x00,
            0x00, 0x00, 0x00,
            // initialize register with memory pointer (address)
            // vset r1, 0x1f00
            // machine code '01 01 00 1f 00 00'
            0x01, 0x01, 0x00,
            0x1f, 0x00, 0x00,
            // Initialize constant of 0x01 wich will be used for increment/decrement
            // vset r2, 0x01
            0x01, 0x02, 0x01,
            0x00, 0x00, 0x00};
    fwrite(init_buf, sizeof(init_buf), 1, outputFile);
}

/**
 * @brief Peeks the data from the top elemnt of the stack.
 *
 * @param pStack Pointer to the stack top element.
 * @return uint32_t Data elemnt form the top of the stack.
 */
static uint32_t stack_top(stack_t *pStack)
{
    return pStack->data;
}

/**
 * @brief Pop data from the top of the stack.
 *
 * @param pStack Pointer to the stack top element.
 */
static void stack_pop(stack_t *pStack)
{
    stack_t *tmp;
    if (true == !stack_empty(pStack))
    {
        tmp = pStack;
        pStack = pStack->prev;
        pStack->next = NULL;
        free(tmp);
    }
}

/**
 * @brief Push new data onto the stack
 *
 * @param pStack Pointer to the stack top element.
 * @param data
 */
static void stack_push(stack_t *pStack, uint32_t data)
{
    stack_t *new = (stack_t *)malloc(sizeof(stack_t));
    new->data = data;

    if (true == stack_empty(pStack))
    {
        pStack = new;
        pStack->prev = NULL;
        pStack->next = NULL;
    }
    else
    {
        new->next = NULL;
        new->prev = pStack;
        pStack->next = new;
        pStack = pStack->next;
    }
}

/**
 * @brief
 *
 * @param pStack Pointer to the stack top element.
 * @return true
 * @return false
 */
static bool stack_empty(stack_t *pStack)
{
    bool retVal = false;
    if (NULL == pStack)
        retVal = true;
    return retVal;
}