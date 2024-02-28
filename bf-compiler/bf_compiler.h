#if !defined(BF_COMPILER_H)
#define BF_COMPILER_H
#include <stdint.h>

#define BF_INSTR_CNT (8U)

typedef enum
{
    BF_INPUT_FILE_ERROR,
    BF_OUTPUT_FILE_ERROR,
    BF_OK
} bf_errorType;

extern char *hexInstrMap[BF_INSTR_CNT];

bf_errorType bf_compile(char *path);

#endif // BF_COMPILER_H
