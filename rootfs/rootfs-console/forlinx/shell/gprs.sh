#!/bin/sh
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

if [ -e /dev/ttymxc1 ]
then
    pppd call gprs /dev/ttymxc1 &
fi

