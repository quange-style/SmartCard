# 1、rootfs制作



基于rootfs-console.tar.bz2文件系统解压。

## 1、增加所需的驱动以及驱动启动脚本

​	解压文件modules.tar.bz2，生成rootfs-console/lib/modules/4.1.15-00025-gc980b04/相关驱动，删除并补充相关misc驱动以及S80insmod.sh脚本

```sh
quange@quange-VirtualBox:~/Work/test/smartCard.git/rootfs$ ls rootfs-console/lib/modules/4.1.15-00025-gc980b04/
kernel       miscfram.ko  miscled.ko  miscsam.ko     modules.alias.bin  modules.builtin.bin  modules.dep.bin  modules.order    modules.symbols      S80insmod.sh  miscbeep.ko  miscgpio.ko  miscnfc.ko  modules.alias  modules.builtin    modules.dep          modules.devname  modules.softdep  modules.symbols.bin
```



## 2、增加开机启动脚本相关

开机启动级别可以参照链接：https://blog.csdn.net/lpw_cn/article/details/85028853

目前采用的时rc5，需要自己添加软连接

```sh
quange@quange-VirtualBox:~/Work/test/smartCard.git/rootfs$ ls rootfs-console/etc/rc5.d/S99rc.local -alh
lrwxrwxrwx 1 quange quange 18 5月  22 19:54 rootfs-console/etc/rc5.d/S99rc.local -> ../init.d/rc.local

```



并增加/etc/init.d/rc.local启动脚本(内容来源于rootfs-qt),改脚本设置了静态IP（支持调试电脑直连）、驱动加载、APP启动。

```sh
quange@quange-VirtualBox:~/Work/test/smartCard.git/rootfs$ cat  rootfs-console/etc/init.d/rc.local 
#! /bin/sh
### BEGIN INIT INFO
# Provides:          rc.local
# Required-Start:    $all
# Required-Stop:
# Default-Start:     2 3 4 5
# Default-Stop:
# Short-Description: Run /etc/rc.local if it exist
### END INIT INFO

PATH=/sbin:/usr/sbin:/bin:/usr/bin
#/etc/init.d/alsa-state start

do_start() {
	ifconfig eth0 192.168.2.174 netmask 255.255.255.0
	route add default gw 192.168.2.1
	if [ -x /etc/rc.local ]; then
		echo -n "Running local boot scripts (/etc/rc.local)"
		/etc/rc.local
		[ $? = 0 ] && echo "." || echo "error"
		return $ES
	fi
	cd /lib/modules/4.1.15-00025-gc980b04
	./S80insmod.sh
	/daemon.sh &
}

case "$1" in
    start)
	do_start
        ;;
    restart|reload|force-reload)
        echo "Error: argument '$1' not supported" >&2
        exit 3
        ;;
    stop)
        ;;
    *)
        echo "Usage: $0 start|stop" >&2
        exit 3
        ;;
esac

```



## 3、补充相关的libc++库

​	补充相关std++库(内容来源于rootfs-qt)

```sh
quange@quange-VirtualBox:~/Work/test/smartCard.git/rootfs$ ls rootfs-console/usr/lib/libstdc++* -alh
-rwxr-xr-x 1 quange quange  890 3月  13  2020 rootfs-console/usr/lib/libstdc++fs.la
-rwxr-xr-x 1 quange quange  950 3月  13  2020 rootfs-console/usr/lib/libstdc++.la
lrwxrwxrwx 1 quange quange   19 3月  13  2020 rootfs-console/usr/lib/libstdc++.so -> libstdc++.so.6.0.21
lrwxrwxrwx 1 quange quange   19 3月  13  2020 rootfs-console/usr/lib/libstdc++.so.6 -> libstdc++.so.6.0.21
-rwxr-xr-x 1 quange quange 1.2M 3月  13  2020 rootfs-console/usr/lib/libstdc++.so.6.0.21
```



## 4、添加需要的app环境

主要是APP启动脚本以及进程

```sh
quange@quange-VirtualBox:~/Work/test/smartCard.git/rootfs$ ls rootfs-console/daemon.sh  rootfs-console/app 
rootfs-console/daemon.sh

rootfs-console/app:
backupapp  Confirm.his  csafc  CSReader  IParamFolder  IPrmConfig.dat  ParamFolder  PrmConfig.dat  Tmp

```



## 5、rootfs打包

上面修改好的内容，进行系统打包。

```sh
quange@quange-VirtualBox:~/Work/test/smartCard.git/rootfs/rootfs-console$ tar cvjf rootfs.tar.bz2 *
```



## 6、替换升级包

在路径I:\tempfile\读卡器\mfgtools\Profiles\Linux\OS Firmware\files\linux下，将rootfs.tar.bz2替换成rootfs-console.tar.bz2。

