#include <Servo.h>
#include <string.h>
using namespace std;
 
char unitID_in[10];
char command_in[10];
char data_in[10];
char data_in_res[10];
int data_in_int;
int data_in_int_res;

int digitalID;
int i=0;
char buffer[40];
int j=-1;

volatile int state = LOW;

int ItrFlag = 0;
int ItrCounter = 0;

Servo myservo;  // create servo object to control a servo

// Прерывание каждые 100 мкс
ISR(TIMER2_COMPA_vect)
{ 
  // На каждом шаге прибавляем +1 на счетчик
  ItrCounter++;
  // При достижении 60сек = 100мкс*10000*60
  // Выставляем флаг
  if(ItrCounter > 600000) ItrFlag = 1;
  
}

void init_timer_timer2(void)
{
  TCCR2A |= (1<<WGM21);
  TCCR2B |= (1<<CS22);

  // Тактировать с коэффициентом деления 64.
  TCNT2  = 0;
  OCR2A  = 24;  // OCR0 = F_CPU / (Prescaler * F_interrupt) - 1 =
                //        16000000 / (64 * 10000) - 1 = 24
  OCR2B  = 0;
  //TIMSK0|=(1<<TOIE0);   // Разрешить прерывания от таймера
  TIMSK2 |= (1<<OCIE2A);  // Разрешить прерывания от таймера
}

//ISR(USART_RX_vect)
//{
    //текст программы обработки прерывания
    
//}

//ISR(USART_TX_vect)
//{
    //текст программы обработки прерывания
    
//}

void digitalServo () {
  
if ((String)unitID_in == "009") {
  
  if((String)unitID_in == "009") {digitalID = 9;}
       
      if ((String)command_in == "servo"){
        
          data_in_int=atoi(data_in);
          
          myservo.write(data_in_int);

          // Скидываем флаг отсчета таймера
          ItrFlag = 0;
          
          Serial.print("Unit ");
          Serial.print(unitID_in);
          Serial.print(" Servo ");
          Serial.print(command_in);
          Serial.print(" Degree: ");
          Serial.print(data_in);      
          Serial.print("\n");
      
          delay(200);
      
      }
     
      memset (unitID_in, 0, 10);
      memset (command_in, 0, 10);
      memset (data_in, 0, 10);
      memset (data_in_res, 0, 10);
      memset (buffer, 0, 20);      
           
    }
}

/*void digitalRele () {
     if ((String)unitID_in == "004"||(String)unitID_in == "007"||(String)unitID_in == "008"||(String)unitID_in == "012") {
       
      if((String)unitID_in == "004") {digitalID = 4;}
      if((String)unitID_in == "007") {digitalID = 7;}
      if((String)unitID_in == "008") {digitalID = 8;}
      if((String)unitID_in == "012") {digitalID = 12;}
       
      if ((String)command_in == "active"){
        
          digitalWrite(digitalID, LOW);
    
          Serial.print("Unit ");
          Serial.print(unitID_in);
          Serial.print(" RELE ");
          Serial.print(command_in);
          Serial.print("\n");     

          delay(200);

      }
      
      if ((String)command_in == "deactive") {
        
          digitalWrite(digitalID, HIGH);
          
          Serial.print("Unit ");
          Serial.print(unitID_in);
          Serial.print(" RELE ");
          Serial.print(command_in);
          Serial.print("\n");
      
          delay(200);
      
      }   
      
      memset (unitID_in, 0, 10);
      memset (command_in, 0, 10);
      memset (buffer, 0, 40);      
           
    }
  
}*/

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

    
   if(j<0) {
    digitalWrite(4, HIGH);
    digitalWrite(7, HIGH);
    digitalWrite(8, HIGH);
    digitalWrite(12, HIGH);
    
    j=1;
   }

  myservo.attach(9);  // attaches the servo on pin 9 to the servo object

  init_timer_timer2();
  
}
 
void loop()
  {         
   

 
    digitalWrite(13, LOW);
 
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
      
      digitalWrite(13, HIGH);
      
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
      digitalWrite(13, LOW);
   }     
    // Исполнительная часть  
  
    digitalServo();
    //digitalRele();

    // Если молчим минуту
    if(ItrFlag == 1)
    {
      myservo.write(90);
      ItrFlag = 0;
    }
}
