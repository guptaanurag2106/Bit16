// MultiplyTwoNumbers.asm

// section .data
// result db 0         // Variable to store the result
// number1 db 10       // First number
// number2 db 5        // Second number

// section .text
// global _start

.start 0x0000:
   MW A, 0x05
   MW B, 0x20

   MW C, A

.loop 0x0010:
    ADD A, B
    SUB C, 1

    @end
    JMPZ C

    @loop
    JMPZ 0

.end 0x0020:
    HALT   // Halt the program or continue with other instructions as needed