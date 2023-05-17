echo 115 > /sys/class/gpio/export
echo "ESAM_SIM"
echo out > /sys/class/gpio/gpio115/direction
echo 1 > /sys/class/gpio/gpio115/value
echo "ESAM_SIM HIGH"

