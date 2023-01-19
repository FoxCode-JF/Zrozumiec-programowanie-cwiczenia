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
    // if (*this->mem_ptr != 0)
    // if (this->ret_addr_stack.top() != NULL)
    // {
    //    this->ret_addr = this->mem_ptr;
    //    this->ret_addr_stack.push(this->mem_ptr);
    // } 
    // else
    // {
    //     this->ret_addr = NULL;
    // }
    
}

void bf_interpreter::end_while()
{
    // if (this->ret_addr != NULL)
    // {
    //    this->mem_ptr = ret_addr;
    // }
    // {
    //     std::cerr << "No return address, check if \"[\" instruction was issued before";
    //     exit(0);
    // }
    // if (this->ret_addr_stack.top() != NULL)
    // {
        // this->mem_ptr = this->ret_addr_stack.top();
        // printf("%x\n", this->ret_addr_stack.top());
        // std::cout << std::hex << this->ret_addr_stack.top() << std::endl;
        // this->ret_addr_stack.pop();
    // }
    
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
        // this->begin_while();
        // if (*this->mem_ptr != 0)
        {
            ret_val = bf_instruction;
        }
        break;
    case ']':
        // this->end_while();
        ret_val = bf_instruction;
        break;
    
    default:
        std::cerr << "\"" << *this->mem_ptr << "\" is not a valis brainfuck instruction, please check your code" << std::endl;
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
        // for (instr_ptr = ; i < count; i++)
        // for (size_t i = 0; i < input_line.size(); i++)
        {
            loop_instr = this->parse_console_input(*instr_ptr);

            if (loop_instr == '[')
            {
                if(*this->mem_ptr != 0)
                {
                    this->while_begin_stack.push(instr_ptr);
                }
                else
                {
                    // go to one instruction after matching ]
                    instr_ptr = this->while_end_stack.top();
                    // instr_ptr++;
                    this->while_end_stack.pop();   
                }
            } 
            else if (loop_instr == ']')
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
            // std::cout << "instruction: " << *instr_ptr << std::endl;// ", mem"            
        }
    }
    
}
