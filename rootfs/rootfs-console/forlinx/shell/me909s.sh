#!/bin/sh
echo "ATE0" > /dev/ttyUSB2
echo "AT^NDISDUP=1,1,\"cmnet\""> /dev/ttyUSB2

ifconfig -a|grep eth0 |grep -v grep
if [ $? -eq 0 ]
then
        ifconfig eth0 down
fi

ifconfig -a|grep eth1 |grep -v grep
if [ $? -eq 0 ]
then
        ifconfig eth1 down
fi

ifconfig -a|grep wlan0 |grep -v grep
if [ $? -eq 0 ]
then
        ifconfig wlan0 down
fi

ifconfig -a|grep ppp0 |grep -v grep
if [ $? -eq 0 ]
then
        ifconfig ppp0 down
fi

sleep 1
ifconfig usb0 up
udhcpc -iusb0
ifconfig eth0 up
ifconfig eth1 up

echo "Finished!"

