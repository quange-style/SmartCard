#!/bin/bash

udhcpc -i eth$1
echo 1 > /proc/sys/net/ipv4/ip_forward
iptables -t nat -A POSTROUTING -o eth$1 -j MASQUERADE

ifconfig wlan0 192.168.0.1
udhcpd -f /etc/udhcpd/udhcpd.conf &
hostapd /etc/hostapd/hostapd.conf &
