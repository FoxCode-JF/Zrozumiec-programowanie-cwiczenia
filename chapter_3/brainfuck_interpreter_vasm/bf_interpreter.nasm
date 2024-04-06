%include "../vm/vm.inc"
%include "bf_interpreter.inc"

; initialize instruction pointer at some address
vset r4, 0x1000

; initialize register with memory pointer (address)
vset r1, 0x1f00

; CONSTANTS
; line feed character
vset r11, 0x0a
; Memory increment value
vset r2, 1
; Zero value for comparing
vset r10, 0

getline:
; read_single_instruction begin
vxor r0, r0
vinb 0x20, r0
;store the instruction for later use and increment instr_ptr
vstb r4, r0
vadd r4, r2
; if r0 == '\n' jump to parsing from memory
vcmp r0, r11 
vjne getline
vset r4, 0x1000
vcall parse_console_input_loop
; revert instruction pointer to beginning
vset r4, 0x1000
vjmp getline

parse_console_input_loop:
; parse input characters from memory 
; one by one and process immediately
vxor r0, r0
vldb r0, r4
; increment instr_ptr
vadd r4, r2

; identify bf command
is_increment_ptr:
vset r5, 0x3e
vcmp r0, r5
vje increment_ptr

is_decrement_ptr:
vset r5, 0x3c
vcmp r0, r5
vje decrement_ptr

is_increment_val:
vset r5, 0x2b
vcmp r0, r5
vje increment_val

is_decrement_val:
vset r5, 0x2d
vcmp r0, r5
vje decrement_val

is_putchar:
vset r5, 0x2e
vcmp r0, r5
vje putchar

is_getchar:
vset r5, 0x2c
vcmp r0, r5
vje getchar

is_beginwhile:
vset r5, 0x5b
vcmp r0, r5
vje begin_while

is_endwhile:
vset r5, 0x5d
vcmp r0, r5
vje end_while

; else if is endline character jump to getline to save next line to memory
vcmp r0, r11
vjne end
vret

; implement the instructions 
increment_ptr:
vadd r1, r2
vjmp parse_console_input_loop

decrement_ptr:
vsub r1, r2
vjmp parse_console_input_loop

increment_val:
vxor r3, r3
vldb r3, r1
vadd r3, r2
vstb r1, r3 
;voff
vjmp parse_console_input_loop

decrement_val:
vxor r3, r3
vldb r3, r1
vsub r3, r2
vstb r1, r3 
vjmp parse_console_input_loop

putchar:
vxor r3, r3
vldb r3, r1
voutb 0x20, r3
; voff
vjmp parse_console_input_loop

getchar:
vxor r3, r3
vinb 0x20, r3
vstb r1, r3
vjmp parse_console_input_loop

begin_while:
vmov r7, r4
vsub r7, r2
vpush r7


vjmp parse_console_input_loop
 
end_while:
vldb r3, r1
vcmp r3, r10
vje dummy_pop_and_back_to_parse
vpop r4
vjmp parse_console_input_loop 

dummy_pop_and_back_to_parse:
vpop r8
vjmp parse_console_input_loop  

end:
voff