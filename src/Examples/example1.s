 .global main
.text
main:
    
    MOV r1,#7
    MOV r2,#8
    ADD r1,r1,r2
    B main
    ADC r3,r1,r2,LSL #2
    ADC r4,r1,r2,LSR #2
    ADC r5,r1,r2,ROR #2
    ADC r6,r1,r2,ASL #2
    ADC r7,r1,r2,ASR #2
    ADCS r3,r1,r2,LSL #2
    ADCS r4,r1,r2,LSR #2
    ADCS r5,r1,r2,ROR #2
    ADCS r6,r1,r2,ASL #2
    ADCS r7,r1,r2,ASR #2
    swi 0x123456
.data
limite:
    .word 0x12345678