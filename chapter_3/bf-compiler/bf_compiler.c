#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "bf_compiler.h"

// #define DEBUG

#if !defined(DEBUG)
#define printf(...)
#endif // DEBUG

#define ARG_NOT_USED (-1)
/**
 * @brief Structure which holds a single node of a stack.
 *
 */
typedef struct stack
{
    uint16_t data;
    struct stack *next;
    struct stack *prev;
} stack_t;

static void bf_init(FILE *outputFile);
static void write_opcode(FILE *fp, uint8_t op, int16_t rd, int16_t rs, uint16_t imm, int8_t immLen);
static void write_zeroes(FILE *outputFile, uint32_t len);
static uint32_t stack_top(stack_t *pStack);
static void stack_pop(stack_t **pStack);
static void stack_push(stack_t **pStack, uint32_t data);
static bool stack_empty(stack_t *pStack);

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

    stack_t *pLoop_stack = NULL;
    char out_path[] = {"./bf-compiler/out"};
    uint32_t loop_begin;
    uint32_t loop_end;
    uint32_t offset_to_loop_begin;
    uint32_t offset_to_jump;
    uint16_t current;
    uint8_t imm16_buf[2];

    // Open file in read mode
    pInputFile = fopen(path, "r");
    pOutputFile = fopen(out_path, "w+b");

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
            write_opcode(pOutputFile, 0x10, 0x01, 0x02, 0, 0);
            break;
        case '<':
            // decrement_ptr:
            // vsub r1, r2(0x01)
            // machine code: '11 02 01'
            write_opcode(pOutputFile, 0x11, 0x01, 0x02, 0, 0);
            break;
        case '+':
            // increment_val:
            write_opcode(pOutputFile, 0x04, 0x03, 0x01, 0, 0); // vldb r3, r1
            write_opcode(pOutputFile, 0x10, 0x03, 0x02, 0, 0); // vadd r3, r2
            write_opcode(pOutputFile, 0x05, 0x01, 0x03, 0, 0); // vstb r1, r3
            break;
        case '-':
            // decrement_val:
            write_opcode(pOutputFile, 0x04, 0x03, 0x01, 0, 0); // vldb r3, r1
            write_opcode(pOutputFile, 0x11, 0x03, 0x02, 0, 0); // vsub r3, r2
            write_opcode(pOutputFile, 0x05, 0x01, 0x03, 0, 0); // vstb r1, r3
            break;
        case '.':
            // putchar:
            write_opcode(pOutputFile, 0x04, 0x03, 0x01, 0, 0); // vldb r3, r1
            write_opcode(pOutputFile, 0xf2, 0x03, 0x20, 0, 0); // voutb 0x20, r3
            break;
        case ',':
            // getchar:
            write_opcode(pOutputFile, 0xf3, 0x03, 0x20, 0, 0); // vinb 0x20, r3
            write_opcode(pOutputFile, 0x05, 0x03, 0x01, 0, 0); // vstb r1, r3
            break;
        case '[':
            // push current instruction address to the stack to come back here later when ending bracket is met
            stack_push(&pLoop_stack, ftell(pOutputFile));
            // begin_while:
            write_opcode(pOutputFile, 0x04, 0x03, 0x01, 0, 0);                      // vldb r3 r1
            write_opcode(pOutputFile, 0x20, 0x03, 0xA, 0, 0);                       // vcmp r3 r10
            write_opcode(pOutputFile, 0x21, ARG_NOT_USED, ARG_NOT_USED, 0x0000, 2); // vje imm16 // jump to the address of ] instruction if r3 contents is 0
            // otherwise continue until ]
            break;
        case ']':
            // peek the address of loop begin then pop the address from the loop stack
            loop_begin = stack_top(pLoop_stack);
            stack_pop(&pLoop_stack);
            loop_end = ftell(pOutputFile);

            // end_loop:
            // write correct jump address to the instruction vje of [
            // calculate jump address of [
            offset_to_loop_begin = (loop_begin - loop_end - 3) & 0xffff;
            // calculate address of vje (jump addr)
            offset_to_jump = (loop_end - (loop_begin + 6)) & 0xffff;
            // jump to [,
            write_opcode(pOutputFile, 0x40, ARG_NOT_USED, ARG_NOT_USED, offset_to_loop_begin /* offset_to_loop_begin */, 2);

            // write jump address to jump from loop beginning
            // save current position of the file pointer
            current = ftell(pOutputFile);
            // set file pointer to the position of imm16
            fseek(pOutputFile, loop_begin + 7, SEEK_SET);
            // write the value of the jump offset to the position
            imm16_buf[0] = (uint8_t)offset_to_jump & 0xff;
            imm16_buf[1] = (uint8_t)(offset_to_jump >> 8) & 0xff;

            fwrite(imm16_buf, sizeof(imm16_buf), 1, pOutputFile);

            // return the file pointer back to where it was
            fseek(pOutputFile, current, SEEK_SET);
            break;
        default:
            break;
        }
    }

    // voff /* close the virtual machine */
    write_opcode(pOutputFile, 0xff, ARG_NOT_USED, ARG_NOT_USED, 0, 0);
    uint32_t end_of_prog = (uint32_t)ftell(pOutputFile);
    write_zeroes(pOutputFile, 256);
    fseek(pOutputFile, 8, SEEK_SET);
    // write the value of the jump offset to the position
    uint8_t imm32_buf[4] = {0};
    imm32_buf[0] = (uint8_t)end_of_prog & 0xff;
    imm32_buf[1] = (uint8_t)(end_of_prog >> 8) & 0xff;
    imm32_buf[2] = (uint8_t)(end_of_prog >> 16) & 0xff;
    imm32_buf[3] = (uint8_t)(end_of_prog >> 24) & 0xff;
    fwrite(imm32_buf, sizeof(imm32_buf), 1, pOutputFile);

    // return the file pointer back to where it was
    fseek(pOutputFile, end_of_prog, SEEK_SET);
    // close files
    fclose(pOutputFile);
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
            0x01, 0x0A, 0x00,
            0x00, 0x00, 0x00,
            // initialize register with memory pointer (address)
            // vset r1, 0x0000
            // mem pointer is 0 just at the init, later it will be overwritten by program end address
            0x01, 0x01, 0x00,
            0x00, 0x00, 0x00,
            // Initialize constant of 0x01 wich will be used for increment/decrement
            // vset r2, 0x01
            0x01, 0x02, 0x01,
            0x00, 0x00, 0x00};
    fwrite(init_buf, sizeof(init_buf), 1, outputFile);
}
/*
  Writes specified number of zero bytes. Used to create free byte array at the end of the file.
*/
static void write_zeroes(FILE *outputFile, uint32_t len)
{
    uint8_t *buf = (uint8_t *)calloc(len, sizeof(uint8_t));
    fwrite(buf, 1, len, outputFile);
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
static void stack_pop(stack_t **pStack)
{
    stack_t *tmp;
    if (true == !stack_empty(*pStack))
    {
        tmp = (*pStack);
        (*pStack) = (*pStack)->prev;
        if (NULL != (*pStack))
        {
            (*pStack)->next = NULL;
        }
        free(tmp);
    }
}

/**
 * @brief Push new data onto the stack
 *
 * @param pStack Pointer to the stack top element.
 * @param data
 */
static void stack_push(stack_t **pStack, uint32_t data)
{
    stack_t *new = (stack_t *)malloc(sizeof(stack_t));
    new->data = data;

    if (true == stack_empty((*pStack)))
    {
        (*pStack) = new;
        (*pStack)->prev = NULL;
        (*pStack)->next = NULL;
        printf("PUSH init: (%d)\n", (*pStack)->data);
    }
    else
    {
        new->next = NULL;
        new->prev = (stack_t *)(*pStack);
        (*pStack)->next = new;
        *pStack = (*pStack)->next;
        printf("PUSH: (%d)\n", (*pStack)->data);
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
    {
        retVal = true;
        printf("STACK EMPTY\n");
    }
    return retVal;
}

/**
 * @brief Writes opcode with parameters as a machine code to the file specified
 *
 * @param fp File pointer
 * @param op Opcode
 * @param rd Destination register. treated as imm8 in some instructions
 * @param rs Source register
 * @param imm Immediate value 8/16/32 bits
 * @param immLen Immediate value length 1,2 bytes
 */
static void write_opcode(FILE *fp, uint8_t op, int16_t rd, int16_t rs, uint16_t imm, int8_t immLen)
{
    uint8_t len = 1 + ((rd != ARG_NOT_USED) ? 1 : 0) + ((rs != ARG_NOT_USED) ? 1 : 0) + immLen;
    uint8_t *write_buffer = (uint8_t *)calloc(len, sizeof(uint8_t));
    uint8_t idx = 0;

#if defined(DEBUG)
    static int op_cnt = 0;
#endif // DEBUG

    write_buffer[idx] = op;
    idx++;
    if (1U == immLen || 2U == immLen)
    {
        write_buffer[idx] = ((imm) & (uint8_t)0x00ff);
        idx++;
    }
    if (2U == immLen)
    {
        write_buffer[idx] = (uint8_t)((imm >> 8) & 0xff);
        idx++;
    }

    if (ARG_NOT_USED != rd)
    {
        write_buffer[idx] = rd;
        idx++;
    };
    if (ARG_NOT_USED != rs)
    {
        write_buffer[idx] = rs;
        idx++;
    };
#if defined(DEBUG)
    printf("%d :: ", op_cnt++);
    for (size_t i = 0; i < len; i++)
    {
        printf("%.2x", write_buffer[i]);
    }
    printf("\n");
#endif // DEBUG

    fwrite(write_buffer, 1, len, fp);
}