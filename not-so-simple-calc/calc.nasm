%include "../vm/vm.inc"

; ADDITION OF TWO NUMBERS
; read number from console until line feed character
; multiplier for conversion
vset r3, 10
; line feed char
vset r6, 0x0a
; '0' character ascii value
vset r4, 0x30

; set number 1 output register to 0
vxor r10, r10
; set number 2 output register to 0
vxor r11, r11


read_number_1:
    ; read byte from console (most significant digit) and put it in the register
    vxor r0, r0
    vinb 0x20, r0
    vcmp r0, r6
    vjz read_number_2
    ; convert to integer by subtracting '0' and put it in the output register --> r_out += (r0 - '0') * r_mul
    ; (r0 - '0')
    vsub r0, r4
    vmul r10, r3
    vadd r10, r0
    vjmp read_number_1

read_number_2:
    ; read byte from console (most significant digit) and put it in the register
    vxor r0, r0
    vinb 0x20, r0
    vcmp r0, r6
    vjz add_numbers
    ; convert to integer by subtracting '0' and put it in the output register --> r_out += (r0 - '0') * r_mul
    ; (r0 - '0')
    vsub r0, r4
    vmul r11, r3
    vadd r11, r0
    vjmp read_number_2

add_numbers:
    vadd r10, r11

; now result resides in r10 register, to display numbers to the console conversion must take place
; All registers are displayed by the virtual machine to verify results

;00011000 11001111 ^ 0000000 11111111 = 11001111 - '0'
;r_out += 11001111 - '0'
;
;00011000 11001111 >> 8 = 00011000
;00011000 - '0' * 10
;
;r_mul *=10
; if no more bytes break read_in_loop ()

.end:
voff