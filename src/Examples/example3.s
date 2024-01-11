.global main
.text
main:
    mov r0, #5
    mov r1, #45
    strb r0, [r1]
    ldrb r0, [r1]

    strh r0, [r1]
    ldrh r0, [r1]
    swi 0x123456
.data
limite:
    .word 0x12345678
adam:
    .byte 0x01

