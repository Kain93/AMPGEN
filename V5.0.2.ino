#include "ModbusRtu.h"
#include <PWM.h>

#define ID   1      // адрес ведомого
#define stlPin  13  // номер выхода индикатора работы
                    // расположен на плате Arduino
#define frq   20000      // pwm frequency

//Задаём ведомому адрес, последовательный порт, выход управления TX
Modbus slave(ID, 0, 2); 
boolean led;
int8_t state = 0;
unsigned long tempus;

// массив данных modbus
uint16_t au16data[11];

void setup() {
  // настраиваем входы и выходы
  io_setup();
  // настраиваем последовательный порт ведомого
  slave.begin( 19200 ); 
  // зажигаем светодиод на 100 мс
  tempus = millis() + 100; 
  digitalWrite(stlPin, HIGH );
  
  InitTimersSafe();
}

void io_setup() {
  digitalWrite(stlPin, HIGH );  
  pinMode(stlPin, OUTPUT);
  //pinMode(3, OUTPUT); // none
  pinMode(4, OUTPUT); // RELE_4
  pinMode(5, OUTPUT); 
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT); // RELE_7
  pinMode(8, OUTPUT); // RELE_8
  pinMode(9, OUTPUT); // PWM_9
  pinMode(10, OUTPUT); // PWM_10
  pinMode(11, OUTPUT); // PWM_11
  //pinMode(12, OUTPUT); // none
 
  SetPinFrequency(9, frq); // PIN_9 frequency
  SetPinFrequency(10, frq); // PIN_10 frequency
  SetPinFrequency(11, frq); // PIN_11 frequency
}

void loop() {
  // обработка сообщений
  state = slave.poll( au16data, 11);  
  // если получили пакет без ошибок - зажигаем светодиод на 50 мс 
  if (state > 4) {
    tempus = millis() + 50;
    digitalWrite(stlPin, HIGH);
  }
  if (millis() > tempus) digitalWrite(stlPin, LOW );
  //обновляем данные в регистрах Modbus и в пользовательской программе
  //io_poll();
  io_input();
} 

void io_input() {
  
  // "discrete" registers
  
  // "coil" registers
  digitalWrite( 4, bitRead( au16data[1], 1 )); // RELE_4 initialization
  digitalWrite( 7, bitRead( au16data[1], 2 )); // RELE_7 initialization
  digitalWrite( 8, bitRead( au16data[1], 3 )); // RELE_8 initialization
  
  // "input" registers
  // Чтение данных с аналоговых входов  
  au16data[2] = analogRead(A0); 
  // outputValue_0 = map(sensorValue_0, 0, 1023, 0, 255);    
  au16data[3] = analogRead(A1);    
  au16data[4] = analogRead(A2);
  // = analogRead(A3);    
  // = analogRead(A4);  
  // = analogRead(A5);
  
  // "holding" registers
  pwmWrite(9, au16data[5]);  // PIN_9 PWM initialization
  pwmWrite(10, au16data[6]);  // PIN_10 PWM initialization
  pwmWrite(11, au16data[7]);  // PIN_11 PWM initialization
}
/*
void io_poll() {
  //Копируем Coil[1] в Discrete[0]
  au16data[0] = au16data[1];
  //Выводим значение регистра 1.3 на светодиод 
  digitalWrite( ledPin, bitRead( au16data[1], 3 ));
  //Сохраняем состояние кнопки в регистр 0.3
  bitWrite( au16data[0], 3, digitalRead( btnPin ));
  //Копируем Holding[5,6,7] в Input[2,3,4]
  au16data[2] = au16data[5];
  au16data[3] = au16data[6];
  au16data[4] = au16data[7];
  //Сохраняем в регистры отладочную информацию
  au16data[8] = slave.getInCnt();
  au16data[9] = slave.getOutCnt();
  au16data[10] = slave.getErrCnt();
}
*/
