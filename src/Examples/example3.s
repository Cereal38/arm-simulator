.global main
.text
main:
    
    ldr r0, =limite
    ldr r0, [r0]
    ldrb r1,=adam
    ldrh r1, [r0]
    add r0, r0, #3
    ldrb r2, [r0]
    swi 0x123456
.data
limite:
    .word 0x12345678
adam:
    .byte 0x01

