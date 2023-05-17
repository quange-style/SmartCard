#!/bin/sh
function usage()
{
    echo "Usage: -i <wifi> -s <ssid> -p <password>"
    echo "eg: ./wifi.sh -i 8189 -s bjforlinx -p 12345678 "
    echo "eg: ./wifi.sh -i 8188 -s bjforlinx -p NONE "
    echo "eg: ./wifi.sh -i 8723 -s bjforlinx -p NONE "
    echo " -i : 8189 8188 or 8723"
    echo " -s : wifi ssid"
    echo " -p : wifi password or NONE"
}

function parse_args()
{
    while true; do
        case "$1" in
            -i ) wifi=$2;echo wifi $wifi;shift 2 ;;
            -s ) ssid=$2;echo ssid $ssid;shift 2 ;;
            -p ) pasw=$2;echo pasw $pasw;shift 2 ;;
            -h ) usage; exit 1 ;;
            * ) break ;;
        esac
    done
}

if [ $# != 6 ]
then
    usage;
    exit 1;
fi

parse_args $@

if [ -e /etc/wpa_supplicant.conf ]
then
    rm /etc/wpa_supplicant.conf
fi
    echo \#PSK/TKIP >> /etc/wpa_supplicant.conf
        echo ctrl_interface=/var/run/wpa_supplicant >>/etc/wpa_supplicant.conf
        echo network={ >>/etc/wpa_supplicant.conf
    echo ssid=\"$ssid\" >>/etc/wpa_supplicant.conf
        echo scan_ssid=1 >>/etc/wpa_supplicant.conf
    if [ $pasw == NONE ]
        then
                echo key_mgmt=NONE >>/etc/wpa_supplicant.conf
        else
                echo psk=\"$pasw\" >>/etc/wpa_supplicant.conf
                echo key_mgmt=WPA-EAP WPA-PSK IEEE8021X NONE >>/etc/wpa_supplicant.conf
                echo group=CCMP TKIP WEP104 WEP40 >>/etc/wpa_supplicant.conf
        fi
    echo } >>/etc/wpa_supplicant.conf

ifconfig -a|grep wlan0 |grep -v grep > /dev/null
if [ $? -eq 0 ]
then
        ifconfig wlan0 down > /dev/null
fi

ifconfig -a|grep eth0 |grep -v grep > /dev/null
if [ $? -eq 0 ]
then
        ifconfig eth0 down > /dev/null
fi
ifconfig -a|grep eth1 |grep -v grep > /dev/null
if [ $? -eq 0 ]
then
        ifconfig eth1 down > /dev/null
fi

lsmod|grep 8723bu |grep -v grep > /dev/null
if [ $? -eq 0 ]
then
        rmmod 8723bu
fi

lsmod|grep 8189es |grep -v grep > /dev/null
if [ $? -eq 0 ]
then
        rmmod 8189es
fi
lsmod|grep 8188eu |grep -v grep > /dev/null
if [ $? -eq 0 ]
then
        rmmod 8188eu
fi

if [ $wifi == 8723 ]
then
        insmod /lib/modules/$(uname -r)/kernel/drivers/net/wireless/realtek/rtl8723BU/8723bu.ko > /dev/null
elif [ $wifi == 8189 ]
then
        insmod /lib/modules/$(uname -r)/kernel/drivers/net/wireless/realtek/rtl8189ES/8189es.ko > /dev/null
elif [ $wifi == 8188 ]
then
        insmod /lib/modules/$(uname -r)/kernel/drivers/net/wireless/realtek/rtl8188EUS/8188eu.ko > /dev/null
fi

sleep 3
ifconfig wlan0 up
sleep 1

ps -fe|grep wpa_supplicant |grep -v grep > /dev/null
if [ $? -eq 0 ]
then
        kill -9 $(pidof wpa_supplicant) > /dev/null
        rm /var/run/wpa_supplicant -r
fi

if [ $wifi == 8723 ] 
then
	wpa_supplicant -Dnl80211 -iwlan0 -c/etc/wpa_supplicant.conf &
else
	wpa_supplicant -Dwext -iwlan0 -c/etc/wpa_supplicant.conf &
fi

sleep 3
udhcpc -i wlan0
echo "Finshed!"

