.global main
.text
main:
    ldrb r0, [r1]
    ldrb r1, [r0]
    ldrb r2, [r1]
    swi 0x123456
.data
limite:
    .word 0x12345678
