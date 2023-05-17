echo 137 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio137/direction
echo 0 > /sys/class/gpio/gpio137/value
echo "sys_run_green ON"
echo 9 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio9/direction
echo 0 > /sys/class/gpio/gpio9/value
echo "sys_error_red ON"

