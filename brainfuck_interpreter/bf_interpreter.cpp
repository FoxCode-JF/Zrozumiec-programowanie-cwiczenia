#include <iostream>
#include "bf_interpreter.hpp"

void bf_interpreter::increment_ptr()
{
    this->mem_ptr++;
}

void bf_interpreter::decrement_ptr()
{
    this->mem_ptr--;
}

void bf_interpreter::increment_val()
{
    (*this->mem_ptr)++;
}

void bf_interpreter::decrement_val()
{
    (*this->mem_ptr)--;
}

void bf_interpreter::putchar()
{
    std::cout << *this->mem_ptr;
}

void bf_interpreter::getchar()
{
    std::cin >> *this->mem_ptr;
}

void bf_interpreter::begin_while()
{
    if(*this->mem_ptr != 0)
    {
        this->while_begin_stack.push(instr_ptr);
    }
    else
    {
        // go to one instruction after matching ]
        instr_ptr = this->while_end_stack.top();
        this->while_end_stack.pop();
    }
}

void bf_interpreter::end_while()
{
    if (*this->mem_ptr == 0)
    {
        this->while_end_stack.push(instr_ptr);
    }
    //  go to one before matching [ instruction
    instr_ptr = this->while_begin_stack.top();
    instr_ptr--;
    this->while_begin_stack.pop();
}

char bf_interpreter::parse_console_input(char bf_instruction)
{
    char ret_val = NULL;

    switch (bf_instruction)
    {
    case '>':
        this->increment_ptr();
        break;
    case '<':
        this->decrement_ptr();
        break;
    case '+':
        this->increment_val();
        break;
    case '-':
        this->decrement_val();
        break;
    case '.':
        this->putchar();
        break;
    case ',':
        this->getchar();
        break;
    case '[':
        this->begin_while();
        break;
    case ']':
        this->end_while();
        break;
    default:
        std::cerr << "\"" << *this->instr_ptr << "\" - Invalid valid brainfuck instruction!" << std::endl;
        break;
    }
    return ret_val;
}

void bf_interpreter::start()
{
    std::string input_line;
    char loop_instr = NULL;    
    while (std::getline(std::cin, input_line))
    {
        if (this->termination_str == input_line)
        {
           std::cout << "Brainfuck terminated... exiting interpreter" << std::endl;
           break;
        }

        for (instr_ptr = input_line.begin(); instr_ptr != input_line.end(); instr_ptr++)
        {
            loop_instr = this->parse_console_input(*instr_ptr);
        }
    }
    
}
