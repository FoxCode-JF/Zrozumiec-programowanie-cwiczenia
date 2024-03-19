#include "bf_compiler.h"

int main(int argc, char const *argv[])
{
    char *bf_path = argv[1];
    if ("" != bf_path)
    {
        bf_compile(bf_path);
    }
    else
    {
        bf_compile("./bf_compiler");
    }

    return 0;
}