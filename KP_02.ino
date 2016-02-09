#include "ModbusRtu.h"
#include <Wire.h>
#include <PWM.h>
//#include <stdint.h>

#define ID   1      // адрес ведомого
#define stlPin  13  // номер выхода индикатора работы

// Задаём ведомому адрес, последовательный порт, выход управления TX
Modbus slave(ID, 0, 4); 
int8_t state = 0;
unsigned long tempus;

int mndrFrq_int = 0;
int install_int = 0;
char i2c;
int i2c_int = 0;
int i2c_to_ifm = 0;
int ic1 = 0;
int ic2 = 0;
int ic22 = 0;
int ic3 = 0;
int ic33 = 0;
int ic4 = 0;
int i1 = 0;
int i2 = 0;
int i3 = 0;

//volatile int mndr = 0;
volatile int mndrSum = 0;
int mndrFrq = 0;

// массив данных modbus
uint16_t au16data[11];

// Прерывание каждые 100 мкс
ISR(TIMER2_COMPA_vect)
{
  //static uint8_t  counter_20000us;
  
  ic1++;
  ic22++;
  ic33++;
  ic4++;
  
  // IFM1
  if (ic1 >= i2c_to_ifm + 12 && i1 == 0)
  //if (ic1 >= 0 && i1 == 0)
  { 
    PORTD &= 0xDF; // 0b11011111
    //PORTD |= 0x20; // 0b00100000 
    //digitalWrite(5, LOW );
    i1++;
  } 
  if (ic1 >= 99 && i1 == 1)
  {  
    PORTD |= 0x20; // 0b00100000
    //PORTD &= 0xDF; // 0b11011111
    //digitalWrite(5, HIGH );
    i1++;
  }
  if (ic1 >= 199)
  {
    ic1 = 0;
    i1 = 0;
  }
  
    // IFM2
  if (ic22 >= 66)
  {
  ic2++;
  if (ic2 >= i2c_to_ifm + 12 && i2 == 0)
  //if (ic2 >= 0 && i2 == 0)
  { 
    PORTD &= 0xBF; // 0b10111111
    //PORTD |= 0x40; // 0b01000000 
    //digitalWrite(6, LOW );
    i2++;
  } 
  if (ic2 >= 99 && i2 == 1)
  {  
    PORTD |= 0x40; // 0b01000000
    //PORTD &= 0xBF; // 0b10111111
    //digitalWrite(6, HIGH );
    i2++;
  }
  if (ic2 >= 199)
  {
    ic2 = 0;
    ic22 = 66;
    i2 = 0;
  }
  }
  
    // IFM3
  if (ic33 >= 133)
  {
  ic3++; 
  if (ic3 >= i2c_to_ifm + 12 && i3 == 0)
  //if (ic3 >= 0 && i3 == 0)
  { 
    PORTD &= 0x7F; // 0b01111111
    //PORTD |= 0x80; // 0b10000000 
    //digitalWrite(7, LOW );
    i3++;
  } 
  if (ic3 >= 99 && i3 == 1)
  {  
    PORTD |= 0x80; // 0b10000000
    //PORTD &= 0x7F; // 0b01111111
    //digitalWrite(7, HIGH );
    i3++;
  }
  if (ic3 >= 199)
  {
    ic3 = 0;
    ic33 = 133;
    i3 = 0;
  }
  }
  
  if ( ic4 >= 9999)
  {
    ic4 = 0;
    mndrFrq = mndrSum;
    mndrSum = 0;
  }
}

void Meandr_Inp() {
 
 mndrSum++;
 //digitalWrite(stlPin, LOW );
}

void Sinus_Inp() {
  
  ic1 = 0;
  //ic22 = 0;
  ic2 = 133;
  //ic33 = 0;
  ic3 = 66;
  TCNT2  = 0;
  //digitalWrite(stlPin, HIGH );
  
}

void init_timer_timer2(void)
{
  //TCCR2A |= (1<<WGM21); //00000010
  TCCR2A = 0x02; //00000010
  //TCCR2B |= (1<<CS22);
  TCCR2B = 0x02; //00000010

  // Тактировать с коэффициентом деления 64.
  TCNT2  = 0;
  OCR2A  = 199;  // OCR0 = F_CPU / (Prescaler * F_interrupt) - 1 =
                //        16000000 / (64 * 10000) - 1 = 24
  OCR2B  = 0;
  //TIMSK2 |= (1<<OCIE2A);  // Разрешить прерывания от таймера
  TIMSK2 = 0x02;  // Разрешить прерывания от таймера
}

void requestEvent(int numBytes) //int numBytes

{

 i2c_int = Wire.read(); // получаем байт данных

}

void setup() {
  // настраиваем входы и выходы
  io_setup();
  // настраиваем последовательный порт ведомого
  slave.begin( 19200 ); 
  // зажигаем светодиод на 100 мс
  tempus = millis() + 100; 
  digitalWrite(stlPin, HIGH );
  
  Wire.begin(0); // master begin
  Wire.onReceive(requestEvent); // создаем прерывание по появлению обрашения к шине
  
  InitTimersSafe(); // initialize all timers except for 0, to save time keeping functions
  //InitTimers();
  
  //sei();
  init_timer_timer2();
  
  attachInterrupt(0, Meandr_Inp, RISING); // Meandr interrupt
  attachInterrupt(1, Sinus_Inp, RISING);// Sinus interrupt
  
    // PWM initialization
  SetPinFrequencySafe(9, 20000); // PWM out
    // IFM preinstall
  //SetPinFrequency(6, 50); // IFM1
  //SetPinFrequency(10, 50); // IFM2
  //SetPinFrequency(11, 50); // IFM3
}

void io_setup() {
  digitalWrite(stlPin, HIGH ); 
  pinMode(stlPin, OUTPUT); 

  pinMode(3, INPUT_PULLUP); // Sin in
  //pinMode(10, OUTPUT); // IFM2 0c1b
  //pinMode(6, OUTPUT); // IFM1 0c0a
  pinMode(2, INPUT_PULLUP); // PWM in
  pinMode(9, OUTPUT); // PWM out 0c1a
  //pinMode(11, OUTPUT); // IFM3 0c2b
  pinMode(5, OUTPUT); // IFM1
  pinMode(6, OUTPUT); // IFM2
  pinMode(7, OUTPUT); // IFM3
  digitalWrite(5, HIGH );
  digitalWrite(6, HIGH ); 
  digitalWrite(7, HIGH );   
}

void loop() {
  
  // PORTD = PORTD*0xDF; // 0b11011111 PIN2 LOW
  // PORTD = PORTD + 0x20; // 0b00100000 PIN2 HIGH
  // обработка сообщений
  state = slave.poll( au16data, 11);  
  // если получили пакет без ошибок - зажигаем светодиод на 50 мс 
  if (state > 4) {
    tempus = millis() + 50;
    digitalWrite(stlPin, HIGH);
  }
  if (millis() > tempus) digitalWrite(stlPin, LOW );
  //обновляем данные в регистрах Modbus и в пользовательской программе
  
  io_poll();

  //i2c_int = atoi(&i2c);
  au16data[2] = i2c_int; // I2C_VALUE
  
  // IFM
  
  i2c_to_ifm =(-i2c_int + 100)*80/100; // IFM_AMP
  
} 

void io_poll() {
  //Копируем Coil[1] в Discrete[0]
  //au16data[0] = au16data[1];
  //Выводим значение регистра 1.3 на светодиод 
  //digitalWrite( ledPin, bitRead( au16data[1], 3 ));
  //Сохраняем состояние кнопки в регистр 0.3
  //bitWrite( au16data[0], 3, digitalRead( btnPin ));
  //Копируем Holding[5,6,7] в Input[2,3,4]
  //au16data[2] = au16data[5];
  //au16data[3] = au16data[6];
  //au16data[4] = au16data[7];
  //Сохраняем в регистры отладочную информацию
  au16data[8] = slave.getInCnt();
  au16data[9] = slave.getOutCnt();
  au16data[10] = slave.getErrCnt();
  
  // PWM install
   
  mndrFrq_int = mndrFrq*1; // Prescaler *1
  au16data[3] = mndrFrq_int;
  install_int = (int)(mndrFrq_int*0.00728571); // 255/35000=0.00728571
  au16data[4] = install_int;
  pwmWrite(9, install_int);
  
  // IFM install
  
  //pwmWrite(6, holdingRegs[IFM_AMP]);
  //pwmWrite(10, holdingRegs[IFM_AMP]);
  //pwmWrite(11, holdingRegs[IFM_AMP]);
}

