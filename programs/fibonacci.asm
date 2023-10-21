// Fibonacci sequence program

// section .data
// fib_numbers db 10 dup(0)  // Array to store Fibonacci numbers (10 bytes)

// section .text
// global _start

.start 0x0000:
    // Initialize the first two Fibonacci numbers
    MWL 0xff
    MWH 0xff
    MW SP, HL
    MW A, 0          // Fibonacci number 0
    MW B, 1          // Fibonacci number 1
    MW C, 10         // Number of Fibonacci numbers to generate

.fib_loop 0x0010:
    // Check if we've generated all 10 Fibonacci numbers
    SUB C, 1
    @end
    JMPZ C

    // Calculate the next Fibonacci number (Fib(n) = Fib(n-1) + Fib(n-2))
    ADD A, B

    // Store the result in memory
    PUSH A

    // Swap values in registers A and B for the next iteration
    MW C, A
    MW A, B
    MW B, C

    // Continue the loop
    @fib_loop
    JMPZ 0

.end 0x0030:
    // Halt the program
    HALT
