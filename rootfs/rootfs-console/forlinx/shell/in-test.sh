#!/bin/bash
echo Exporting pin $1.
echo $1> /sys/class/gpio/export
echo Setting pin in.
echo in> /sys/class/gpio/gpio$1/direction
cat /sys/class/gpio/gpio$1/value
echo $1> /sys/class/gpio/unexport

