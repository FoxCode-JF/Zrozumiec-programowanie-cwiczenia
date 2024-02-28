#include <stdio.h>
#include "bf_compiler.h"

char *hexInstrMap[BF_INSTR_CNT];

static void bf_init(void);

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

    // Open file in read mode
    pInputFile = fopen(path, "r");
    pOutputFile = fopen(pOutputFile, "w");

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

    while (EOF != (instruction = getc(pInputFile)))
    {
        switch (instruction)
        {
        case '>':
            /* code */
            break;

        default:
            break;
        }
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
    //     ; initialize instruction pointer at some address
    // vset r4, 0x1000
    // machine code '01 04, 00 01'
    fprintf(outputFile, "010400010000");
    // ; initialize register with memory pointer (address)
    // vset r1, 0x1f00
    // machine code '01 01 00 1f 00 00'
    fprintf(outputFile, "0101001f0000");
}