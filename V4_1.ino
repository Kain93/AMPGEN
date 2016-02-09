#include <string.h>
#include <PWM.h>
using namespace std;
 
char unitID_in[10];
char command_in[10];
char data_in[10];
char data_in_res[10];
int data_in_int;
int data_in_int_res;
 
// Используем аналоговые входы

int sensorValue_0 = 0;
int sensorValue_1 = 0;
int sensorValue_2 = 0;
int sensorValue_3 = 0;
int sensorValue_4 = 0;
int sensorValue_5 = 0;

int analogID;
int digitalID;
int sensorValue;
int i=0;
char buffer[40];
int j=-1;

volatile int state = LOW;

//ISR(USART_RX_vect)
//{
    //текст программы обработки прерывания
    
//}

//ISR(USART_TX_vect)
//{
    //текст программы обработки прерывания
    
//}

void infout()
{   
      
  /*    Serial.print("Buffer ");
      Serial.print(buffer);
      Serial.print("\n");

      // Отправляем то что разобрано
      
      Serial.print("Unit ");
      Serial.print(unitID_in);
      Serial.print(" command ");
      Serial.print(command_in);
      Serial.print("  ");
      Serial.print("data: ");
      Serial.print(data_in);      
      Serial.print("  ");
      Serial.print("data_res: ");
      Serial.print(data_in_res);      
      Serial.print("\n");
      
      digitalWrite(2, state);

      */

      
      
      

}

void analogread () {

   if ( (String)unitID_in == "0a0"||(String)unitID_in == "0a1"||(String)unitID_in == "0a2"||
   (String)unitID_in == "0a3"||(String)unitID_in == "0a4"||(String)unitID_in == "0a5"){
     
     if((String)unitID_in == "0a0") { sensorValue = sensorValue_0;}
     if((String)unitID_in == "0a1") { sensorValue = sensorValue_1;}
     if((String)unitID_in == "0a2") { sensorValue = sensorValue_2;}
     if((String)unitID_in == "0a3") { sensorValue = sensorValue_3;}
     if((String)unitID_in == "0a4") { sensorValue = sensorValue_4;}
     if((String)unitID_in == "0a5") { sensorValue = sensorValue_5;}
        
      if ((String)command_in == "read"){
    
          digitalWrite(2, HIGH);
          
          // Формирование пакета данных для отправки
          Serial.print(unitID_in);
          Serial.print(",");
          Serial.print("signal");
          Serial.print(":");
          Serial.print(sensorValue);
          Serial.print("\n");
          
          delay(200);
          digitalWrite(2, LOW);  
 
      }
      
      memset (unitID_in, 0, 10);
      memset (command_in, 0, 10);
      memset (data_in, 0, 10);
      memset (data_in_res, 0, 10);
      memset (buffer, 0, 40);    
      
   }   
  
}

void digitalPWM () {
  
if ((String)unitID_in == "009"||(String)unitID_in == "010") {
  
  if((String)unitID_in == "009") {digitalID = 9;}
  if((String)unitID_in == "010") {digitalID = 10;}
       
      if ((String)command_in == "active"){
        
          data_in_int=atoi(data_in);
          data_in_int_res=atoi(data_in_res);
          
          SetPinFrequency(digitalID, data_in_int_res);
          pwmWrite(digitalID, data_in_int);
      
          digitalWrite(2, HIGH);
          
          Serial.print("Unit ");
          Serial.print(unitID_in);
          Serial.print(" PWM ");
          Serial.print(command_in);
          Serial.print("\n");
          Serial.print("power: ");
          Serial.print(data_in);      
          Serial.print("\n");
          Serial.print("frequency: ");
          Serial.print(data_in_res);      
          Serial.print("\n");
      
          delay(200);
          digitalWrite(2, LOW);
      
      }
      
      if ((String)command_in == "deactive"){
        
          analogWrite(digitalID, 0);
          
          digitalWrite(2, HIGH);
          
          Serial.print("Unit ");
          Serial.print(unitID_in);
          Serial.print(" PWM ");
          Serial.print(command_in);
          Serial.print("\n");
      
          delay(200);
          digitalWrite(2, LOW);
      
      }   
     
      memset (unitID_in, 0, 10);
      memset (command_in, 0, 10);
      memset (data_in, 0, 10);
      memset (data_in_res, 0, 10);
      memset (buffer, 0, 20);      
           
    }
}

void digitalnoPWM () {
  
if ((String)unitID_in == "003"||(String)unitID_in == "005"
||(String)unitID_in == "006"||(String)unitID_in == "011") {
  
  if((String)unitID_in == "003") {digitalID = 3;}
  if((String)unitID_in == "005") {digitalID = 5;}
  if((String)unitID_in == "006") {digitalID = 6;}
  if((String)unitID_in == "011") {digitalID = 11;}
       
      if ((String)command_in == "active"){
        
          data_in_int=atoi(data_in);
          
          analogWrite(digitalID, data_in_int);
      
          digitalWrite(2, HIGH);
          
          Serial.print("Unit ");
          Serial.print(unitID_in);
          Serial.print(" PWM ");
          Serial.print(command_in);
          Serial.print("\n");
          Serial.print("power: ");
          Serial.print(data_in);      
          Serial.print("\n");
      
          delay(200);
          digitalWrite(2, LOW);
      
      }
      
      if ((String)command_in == "deactive"){
        
          analogWrite(digitalID, 0);
          
          digitalWrite(2, HIGH);
          
          Serial.print("Unit ");
          Serial.print(unitID_in);
          Serial.print(" PWM ");
          Serial.print(command_in);
          Serial.print("\n");
      
          //delay(200);
          digitalWrite(2, LOW);
      
      }   
     
      memset (unitID_in, 0, 10);
      memset (command_in, 0, 10);
      memset (data_in, 0, 10);
      memset (data_in_res, 0, 10);
      memset (buffer, 0, 40);      
           
    }  

}

void digitalRele () {
     //if((String)unitID_in == "004") {digitalWrite(13, state);}
     if ((String)unitID_in == "004"||(String)unitID_in == "007"||(String)unitID_in == "008"||(String)unitID_in == "012") {
       
      if((String)unitID_in == "004") {digitalID = 4;}
      if((String)unitID_in == "007") {digitalID = 7;}
      if((String)unitID_in == "008") {digitalID = 8;}
      if((String)unitID_in == "012") {digitalID = 12;}
       
      if ((String)command_in == "active"){
        
          digitalWrite(digitalID, LOW);
      
          digitalWrite(2, HIGH);
          
          
          
          Serial.print("Unit ");
          Serial.print(unitID_in);
          Serial.print(" RELE ");
          Serial.print(command_in);
          Serial.print("\n");     

          delay(200);
          digitalWrite(2, LOW);

      }
      
      if ((String)command_in == "deactive") {
        
          digitalWrite(digitalID, HIGH);
      
          digitalWrite(2, HIGH);
          
          Serial.print("Unit ");
          Serial.print(unitID_in);
          Serial.print(" RELE ");
          Serial.print(command_in);
          Serial.print("\n");
      
          delay(200);
          digitalWrite(2, LOW);
      
      }   
      
      memset (unitID_in, 0, 10);
      memset (command_in, 0, 10);
      memset (buffer, 0, 40);      
           
    }
  
}

void setup()
{
  // Открываем порт передачи данных
  Serial.begin(9600);
  
  // Открываем пины
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  // Инициализируем таймеры
  InitTimersSafe();
  
     sei();
     attachInterrupt(0, infout, RISING);
    
   if(j<0) {
    digitalWrite(4, HIGH);
    digitalWrite(7, HIGH);
    digitalWrite(8, HIGH);
    digitalWrite(12, HIGH);
    
    j=1;
   }
  
}
 
void loop()
  {         
   

 
    digitalWrite(2, LOW);
 
    // Считываем данные порта
    
    while(Serial.available()>0) {

       delay(100);
      
       // Заполняем буфер
       while( Serial.available() && i < 40) {
        
         buffer[i++] = Serial.read();
       }
     // Закрываем массив
       buffer[i++]='\0';
 
  
  // Отправляем обратно то что пришло
      
      digitalWrite(2, HIGH);
      
      Serial.print("Buffer ");
      Serial.print(buffer);
      Serial.print("\n");
  
      // Разбираем буфер на части     
      while(i>0) {      
      sscanf(buffer, "%[^','],%[^','],%[^','],%[^',']", &unitID_in, &command_in, &data_in, &data_in_res);
      i=0;
             }

      // Отправляем то что разобрано
      
      Serial.print("Unit ");
      Serial.print(unitID_in);
      Serial.print(" command ");
      Serial.print(command_in);
      Serial.print("  ");
      Serial.print("data: ");
      Serial.print(data_in);      
      Serial.print("  ");
      Serial.print("data_res: ");
      Serial.print(data_in_res);      
      Serial.print("\n");
      
      //delay(200);
      digitalWrite(2, LOW);
   }     
    // Исполнительная часть
  
    // Чтение данных с аналоговых входов  
    sensorValue_0 = analogRead(A0); 
    // outputValue_0 = map(sensorValue_0, 0, 1023, 0, 255);    
    sensorValue_1 = analogRead(A1);    
    sensorValue_2 = analogRead(A2);
    sensorValue_3 = analogRead(A3);    
    sensorValue_4 = analogRead(A4);  
    sensorValue_5 = analogRead(A5);  
  
    analogread();
    digitalPWM();
    digitalnoPWM();
    digitalRele();
    

}
