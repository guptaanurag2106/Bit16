     /* AddFourNumbers.asm

section .text
global _start */

.start 0x0000:

    MW A, 0          // Initialize A to 0 (the result)
    
    MWL 0x00
    MWH 0xfe
    LW B, HL
    ADD A, B

    MWL 0x01
    MWH 0xfe
    LW B, HL
    ADD A, B
    
    MW B, 0xc3
    ADD A, B

    MWL 0x02
    MWH 0xfe
    SW HL, A

    // Halt the program or continue with other instructions as needed
    HALT

