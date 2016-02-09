#!/bin/bash
#Разряд
#./gpio.sh 23 0& 
#sleep 2
#./gpio.sh 23 1&
#sleep 1

#Вкл
#./gpio.sh 22 0&
#sleep 1
#./gpio.sh 22 1&
#sleep 1

#Заряд
./gpio.sh 17 0&
sleep 1.5
./gpio.sh 17 1&
sleep 1

#Разряд
./gpio.sh 23 0& 
sleep 10
./gpio.sh 23 1&
sleep 1
