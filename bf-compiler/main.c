#include "bf_compiler.h"

int main(int argc, char const *argv[])
{

    char *bf_path = argv[1];
    if ("" != bf_path)
    {
        bf_compile(argv[1]);
    }

    return 0;
}