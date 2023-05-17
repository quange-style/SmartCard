#!/bin/bash
echo Exporting pin $1.
echo $1> /sys/class/gpio/export
echo Setting pin low.
echo 0 > /sys/class/gpio/gpio$1/value
echo $1> /sys/class/gpio/unexport

