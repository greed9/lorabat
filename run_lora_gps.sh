#!/bin/bash
stty -F /dev/ttyACM0 -hupcl 9600
gpspipe -R | gawk -f sample_gps_lora.awk
