#!/bin/sh
gdb=arm-none-eabi-gdb

# Vérifier la présence du simulateur et de la version étudiante
if [ ! -x ./arm_simulator ]
then
    echo "Error: Missing simulator or student version"
    exit 1
fi

# Itérer sur le premier fichier d'exemple dans Examples/
file="Examples/example1"
base=$(basename "$file" .s)

# Lancer le simulateur en arrière-plan
./arm_simulator --gdb-port 58001 > /dev/null 2>&1 &

# Attente courte pour permettre au simulateur de démarrer
sleep 1

# Lancer GDB en mode interactif pour ouvrir le fichier d'exemple et se connecter au simulateur
$gdb -ex "file $file" -ex "target remote localhost:58001" -ex "load"