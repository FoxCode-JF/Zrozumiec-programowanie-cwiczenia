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
    std::cout << *this->mem_ptr << std::endl;
}

void bf_interpreter::getchar()
{
    std::cin >> *this->mem_ptr;
}

void bf_interpreter::begin_while()
{
    if (*this->mem_ptr !=0)
    {
       this->ret_addr = this->mem_ptr;
    } else
    {
        this->ret_addr = NULL;
    }
    
}

void bf_interpreter::end_while()
{
    if (this->ret_addr != NULL)
    {
       this->mem_ptr = ret_addr;
    }
    // {
    //     std::cerr << "No return address, check if \"[\" instruction was issued before";
    //     exit(0);
    // }
}

int8_t bf_interpreter::parse_console_input(char bf_instruction)
{
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
        std::cerr << "\"" << *this->mem_ptr << "\" is not a valis brainfuck instruction, please check your code" << std::endl;
        break;
    }
}

void bf_interpreter::start()
{
    std::string input_line;
    
    while (std::getline(std::cin, input_line))
    {
        if (this->termination_str == input_line)
        {
           std::cout << "Brainfuck terminated... exiting interpreter" << std::endl;
           break;
        }
        
        for (size_t i = 0; i < input_line.size(); i++)
        {
            this->parse_console_input(input_line[i]);
        }
    }
    
}
