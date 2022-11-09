.text
.syntax unified /* Unified Assembly Syntax - UAL */
.thumb
/* Thumb Instr. Set */
.cpu cortex-m0plus
.type My_Div, %function
.global My_Div
/* Make the function visible from outside */
// R0: Dividend, updated each iteration with the result from subtraction
// R1: Divisor
// R2: Quotient
My_Div:
MOVS R2, #0
CONT:
CMP R0, R1
BLT FIN
SUBS R0, R1
ADDS R2, R2, #1
B CONT
FIN:
MOVS R0, R2
BX LR
.end

