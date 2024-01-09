#!/bin/bash


#on verifie qu'on a un argument
if [ $# -eq 0 ]
then
    echo "Il faut un argument !"
    exit 1
fi
port=$1
arm-none-eabi-gdb 
#< file Examples/example1 < target remote localhost:$port

 $gdb -ex file Examples/example1
 $gdb -ex target remote localhost:

