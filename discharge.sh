echo "Discharging" | sudo tee /dev/tty1
./gpio.sh 23 0& 
sleep 10
./gpio.sh 23 1&
sleep 1
echo "Discharging complete" | sudo tee /dev/tty1
