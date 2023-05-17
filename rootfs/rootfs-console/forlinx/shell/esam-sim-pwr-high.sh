echo 116 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio116/direction
echo 1  > /sys/class/gpio/gpio116/value
echo "ESAM-SIM-PWR high"
echo 116 > /sys/class/gpio/unexport

