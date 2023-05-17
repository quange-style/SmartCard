echo 1 > /sys/class/gpio/gpio137/value
echo "sys_run_green OFF"
echo 137 > /sys/class/gpio/unexport
echo 1 > /sys/class/gpio/gpio9/value
echo "sys_error_red OFF"
echo 9 > /sys/class/gpio/unexport

