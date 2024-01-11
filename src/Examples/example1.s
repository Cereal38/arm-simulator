 .global main
.text
main:
    MRS R0, CPSR 
	ORR R0, R0, #0x80 
	MSR CPSR_c, R0
    swi 0x123456
.data
limite:
    .word 0x12345678