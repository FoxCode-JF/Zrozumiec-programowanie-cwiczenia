/**
 * @file bf_interpreter.hpp
 * @author FoxCode - Jarek Filipiuk-Ohradka (filipiukjarek@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-01-20
 * 
 * @copyright Copyright (c) 2023
 * 
 * @details Brainfuck interpreter.
 * More info and example test programs can be found on wikipedia: https://en.wikipedia.org/wiki/Brainfuck#Hello_World!
 */
#pragma once

#include <stdint.h>
#include <string>
#include <stack>

class bf_interpreter
{
private:
    uint8_t *mem_ptr;
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
    }
    ~bf_interpreter() {}
    void start();
};