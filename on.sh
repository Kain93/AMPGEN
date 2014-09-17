echo "Power ON" | sudo tee /dev/tty1
./gpio.sh 22 0
sleep 1

./gpio.sh 22 1
sleep 1
echo "Power OFF" | sudo tee /dev/tty1
