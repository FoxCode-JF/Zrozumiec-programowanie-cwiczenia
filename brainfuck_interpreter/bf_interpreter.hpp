#pragma once

#include <stdint.h>
#include <string>
#include <stack>

class bf_interpreter
{
private:
    uint8_t *mem_ptr;
    // uint8_t* ret_addr;
    std::string::iterator instr_ptr;
    std::stack<std::string::iterator> while_begin_stack;
    std::stack<std::string::iterator> while_end_stack;   
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
    char parse_console_input(char bf_instruction);

public:
    bf_interpreter() {
        this->mem_ptr = array;
        // ret_addr = NULL;
    //    this->while_end_stack.push(NULL);
    //    this->while_begin_stack.push(NULL) 
    }
    ~bf_interpreter() {}

   
    void start();
};