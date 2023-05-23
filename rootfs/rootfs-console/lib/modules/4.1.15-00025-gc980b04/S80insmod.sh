#!/bin/sh

# rmmod
#rmmod miscbeep.ko
#rmmod miscfram.ko
#rmmod miscgpio.ko
#rmmod miscled.ko
#rmmod miscsam.ko
#rmmod miscnfc.ko

# insmod
insmod miscbeep.ko
insmod miscfram.ko
insmod miscgpio.ko
insmod miscled.ko
insmod miscsam.ko
insmod miscnfc.ko

# map tty
ln -s /dev/ttymxc1 /dev/ttyS0
ln -s /dev/ttymxc2 /dev/ttyS2
ln -s /dev/ttymxc3 /dev/ttyS1
ln -s /dev/ttymxc4 /dev/ttyS4
ln -s /dev/ttymxc5 /dev/ttyS5
ln -s /dev/ttymxc6 /dev/ttyS6
ln -s /dev/ttymxc7 /dev/ttyS7