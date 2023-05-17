echo 1 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio1/direction
echo 0 > /sys/class/gpio/gpio1/value
echo 2 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio2/direction
echo 1 > /sys/class/gpio/gpio2/value
sleep 2
echo 0 > /sys/class/gpio/gpio2/value
sleep 2
echo 1 > /sys/class/gpio/gpio2/value
echo "GPRS ON"
echo 2 > /sys/class/gpio/unexport
echo 1 > /sys/class/gpio/unexport

