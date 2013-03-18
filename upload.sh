#! /bin/bash

avrdude -p attiny85 -c avrisp -P /dev/ttyUSB0 -b 19200 -e -U flash:w:main.hex
