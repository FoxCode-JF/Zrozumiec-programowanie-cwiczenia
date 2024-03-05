#include "bf_compiler.h"

int main(int argc, char const *argv[])
{
    char *bf_path = argv[1];
    if ("" != bf_path)
    {
        bf_compile(argv[1]);
    }
    else
    {
        bf_compile("./program.bf");
    }

    return 0;
}