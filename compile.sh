#! /bin/bash

avr-gcc -Wall -Os -fpack-struct -fshort-enums -funsigned-char -funsigned-bitfields -mmcu=attiny85 -c main.c USI_TWI_Master.c USI_TWI.c BMP085.c 

avr-gcc -Os -mmcu=attiny85 -o main.elf USI_TWI.o USI_TWI_Master.o BMP085.c main.o -lm

avr-objcopy -R .eeprom -O ihex main.elf main.hex

avr-size main.hex

rm *.o
rm *.elf
