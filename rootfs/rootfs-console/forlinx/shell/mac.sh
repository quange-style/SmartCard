#!/bin/bash
serialnum=`cat /proc/cpuinfo|grep "Serial"|awk '{print $3}'`
md5num=`echo -n $serialnum |md5sum |awk '{print $1}'`
eth0_mac=00:14:${md5num:8:2}:${md5num:6:2}:${md5num:4:2}:${md5num:2:2}
ifconfig eth0 hw ether $eth0_mac
eth1_mac=00:14:${md5num:16:2}:${md5num:14:2}:${md5num:12:2}:${md5num:10:2}
ifconfig eth1 hw ether $eth1_mac
