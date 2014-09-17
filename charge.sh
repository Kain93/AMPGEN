echo "Charging" | sudo tee /dev/tty1
./gpio.sh 17 0
sleep 1.5
./gpio.sh 17 1
sleep 1
echo "Charging complete" | sudo tee /dev/tty1
