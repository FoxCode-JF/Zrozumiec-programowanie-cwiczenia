#pragma once

#include <stdint.h>
#include <string>

class bf_interpreter
{
private:
    uint8_t* mem_ptr;
    uint8_t* ret_addr;    
    std::string termination_str = "quit";
    uint8_t array[30000] = {0};
    void increment_ptr();
    void decrement_ptr();
    void increment_val();
    void decrement_val();
    void putchar();
    void getchar();
    void begin_while();
    void end_while();
    int8_t parse_console_input(char bf_instruction);

public:
    bf_interpreter() {
        mem_ptr = array;
        ret_addr = NULL;
    }
    ~bf_interpreter() {}

   
    void start();
};