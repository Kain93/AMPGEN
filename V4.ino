#include <string.h>
#include <PWM.h>
 
char unitID_in[10];
char command_in[10];
char data_in[10];
char data_in_res[10];
int data_in_int;
int data_in_int_res;
 
// Используем аналоговые входы

int analogOuttoDigital_0 = A0;
int analogOuttoDigital_1 = A1;
int analogOuttoDigital_2 = A2;

int sensorValue_0 = 0;
int sensorValue_1 = 0;
int sensorValue_2 = 0;
 
void setup()
{
  // Открываем порт передачи данных
  Serial.begin(9600); 
  // Открываем пины
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(7, OUTPUT);
  // Инициализируем таймеры
  InitTimersSafe();
  
}
 
void loop()
  {         
        
    int i=0;
    char buffer[100];
 
        digitalWrite(12, LOW);
 
    // Считываем данные порта
    
    while(Serial.available()>0){

       delay(1000);
      
       // Заполняем буфер
       while( Serial.available() && i< 99) {
        
         buffer[i++] = Serial.read();
       }
     // Закрываем массив
       buffer[i++]='\0';
//       digitalWrite(12, LOW);
 }
 
   //delay(1000);       
   //digitalWrite(12, HIGH);
   //delay(1000); 
  
  // Разбираем буфер на части

       
  while(i>0){
        digitalWrite(12, HIGH);     
    
      // Отправляем обратно то что пришло
  
      Serial.print("Buffer ");

      Serial.print(buffer);

      Serial.print("\n");
      //digitalWrite(12, LOW);    
    sscanf(buffer, "%[^','],%[^','],%[^','],%[^',']", &unitID_in, &command_in, &data_in, &data_in_res);
    
      // Отправляем то что разобрано
      Serial.print("unit ");
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
      
      i=0;
      delay(100);
      digitalWrite(12, LOW);
}
      //

   
   // Исполнительная часть

    // Выполнение команды устройством 009 - Шим на пине 9
     if ((String)unitID_in == "009"){
       
      if ((String)command_in == "active"){
        
          data_in_int=atoi(data_in);
 //         analogWrite(9, data_in_int);
          data_in_int_res=atoi(data_in_res);
          
          SetPinFrequency(9, data_in_int_res);
          pwmWrite(9, data_in_int);
      
      digitalWrite(12, HIGH);
          
      Serial.print("009,arduino recive: unit ");
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
      

      
      memset (unitID_in, 0, 10);
      memset (command_in, 0, 10);
      memset (data_in, 0, 10);
      memset (data_in_res, 0, 10);
      
      // Очищаем буффер
      memset (buffer, 0, 50);

      delay(200);
      digitalWrite(12, LOW);      
      }
      
      if ((String)command_in == "deactive"){
          analogWrite(9, 0);
          
      digitalWrite(12, HIGH);
          
      Serial.print("009,arduino recive: unit ");
      Serial.print(unitID_in);
      Serial.print(" PWM ");
      Serial.print(command_in);
      Serial.print("\n");
      

      
      memset (unitID_in, 0, 10);
      memset (command_in, 0, 10);
      
      
      
      // Очищаем буффер
      memset (buffer, 0, 50);
      
      delay(200);
      digitalWrite(12, LOW);
      
      }   
     
     // Очищаем буффер
      memset (buffer, 0, 50);      
           
    }
     
    // Выполнение команды устройством 010 - Шим на пине 10
     if ((String)unitID_in == "010"){
       
      if ((String)command_in == "active"){
        
          data_in_int=atoi(data_in);
 //         analogWrite(10, data_in_int);
          data_in_int_res=atoi(data_in_res);
          
          SetPinFrequency(10, data_in_int_res);
          pwmWrite(10, data_in_int);
   
      digitalWrite(12, HIGH);   
          
      Serial.print("010,arduino recive: unit ");
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
      

      
      memset (unitID_in, 0, 10);
      memset (command_in, 0, 10);
      memset (data_in, 0, 10);
      memset (data_in_res, 0, 10);
      
      // Очищаем буффер
      memset (buffer, 0, 50);      
      
      delay(200);
      digitalWrite(12, LOW);
      
      }
      
      if ((String)command_in == "deactive"){
          analogWrite(10, 0);
      
      digitalWrite(12, HIGH);      
          
      Serial.print("010,arduino recive: unit ");
      Serial.print(unitID_in);
      Serial.print(" PWM ");
      Serial.print(command_in);
      Serial.print("\n");
      


      memset (unitID_in, 0, 10);
      memset (command_in, 0, 10);
      
      // Очищаем буффер
      memset (buffer, 0, 50);      
      
      delay(200);
      digitalWrite(12, LOW);
      
      }   
      
      // Очищаем буффер
      memset (buffer, 0, 50);      
           
    }
    
    // Выполнение команды устройством 003 - Шим на пине 3
     if ((String)unitID_in == "003"){
       
      if ((String)command_in == "active"){
        
          data_in_int=atoi(data_in);
          analogWrite(3, data_in_int);
 //         data_in_int_res=atoi(data_in_res);
          
 //         SetPinFrequency(11, data_in_int_res);
 //         pwmWrite(11, data_in_int);
      
      digitalWrite(12, HIGH);
          
      Serial.print("003,arduino recive: unit ");
      Serial.print(unitID_in);
      Serial.print(" PWM ");
      Serial.print(command_in);
      Serial.print("\n");
      Serial.print("power: ");
      Serial.print(data_in);      
      Serial.print("\n");
 //     Serial.print("frequency: ");
 //     Serial.print(data_in_res);      
 //     Serial.print("\n");
      

      
      memset (unitID_in, 0, 10);
      memset (command_in, 0, 10);
      memset (data_in, 0, 10);
 //     memset (data_in_res, 0, 10);
      
      // Очищаем буффер
      memset (buffer, 0, 50);      
      
      delay(200);
      digitalWrite(12, LOW);
      
      }
      
      if ((String)command_in == "deactive"){
          analogWrite(3, 0);
      
      digitalWrite(12, HIGH);
          
      Serial.print("003,arduino recive: unit ");
      Serial.print(unitID_in);
      Serial.print(" PWM ");
      Serial.print(command_in);
      Serial.print("\n");
      


      memset (unitID_in, 0, 10);
      memset (command_in, 0, 10);
      
      // Очищаем буффер
      memset (buffer, 0, 50);      
      
      delay(200);
      digitalWrite(12, LOW);
      
      }   
      
      // Очищаем буффер
      memset (buffer, 0, 50);      
           
    }
    
    // Чтение данных с аналоговых входов  
    sensorValue_0 = analogRead(analogOuttoDigital_0); 
    //      outputValue_0 = map(sensorValue_0, 0, 1023, 0, 255);
    
    sensorValue_1 = analogRead(analogOuttoDigital_1);
    
    sensorValue_2 = analogRead(analogOuttoDigital_2);
    
    
   
   if ((String)unitID_in == "000"){
       
      if ((String)command_in == "read"){
    
      digitalWrite(12, HIGH);  
        
      // Формирование пакета данных для отправки
      Serial.print("000");
      Serial.print(",");
      Serial.print("signal");
      Serial.print(",");
      Serial.print(sensorValue_0);
      Serial.print("\n");
      

      
      memset (unitID_in, 0, 10);
      memset (command_in, 0, 10);
      
      // Очищаем буффер
      memset (buffer, 0, 50);

      delay(200);
      digitalWrite(12, LOW);      
       
      }
      
      // Очищаем буффер
      memset (buffer, 0, 50);      
      
   }   
   
      if ((String)unitID_in == "100"){
       
      if ((String)command_in == "read"){
    
      digitalWrite(12, HIGH); 
        
      // Формирование пакета данных для отправки
      Serial.print("100");
      Serial.print(",");
      Serial.print("signal");
      Serial.print(",");
      Serial.print(sensorValue_1);
      Serial.print("\n");
      

      
      memset (unitID_in, 0, 10);
      memset (command_in, 0, 10);
      
      // Очищаем буффер
      memset (buffer, 0, 50);

      delay(200);
      digitalWrite(12, LOW);      
       
      }
      
      // Очищаем буффер
      memset (buffer, 0, 50);      
   }  
   
      if ((String)unitID_in == "200"){
       
      if ((String)command_in == "read"){
    
      digitalWrite(12, HIGH);  
        
      // Формирование пакета данных для отправки
      Serial.print("200");
      Serial.print(",");
      Serial.print("signal");
      Serial.print(",");
      Serial.print(sensorValue_2);
      Serial.print("\n");
      
      
      
      memset (unitID_in, 0, 10);
      memset (command_in, 0, 10);
       
      // Очищаем буффер
      memset (buffer, 0, 50);      
      
      delay(200);
      digitalWrite(12, LOW);
      
      }
      
      // Очищаем буффер
      memset (buffer, 0, 50);      
   }  
 
 // Выполнение команды устройством 002 
     if ((String)unitID_in == "002"){
       
      if ((String)command_in == "active"){
        
          digitalWrite(2, HIGH);
      
      digitalWrite(12, HIGH);
          
      Serial.print("002,arduino recive: unit ");
      Serial.print(unitID_in);
      Serial.print(" RELE ");
      Serial.print(command_in);
      Serial.print("\n");
      
      
      
      memset (unitID_in, 0, 10);
      memset (command_in, 0, 10);
      
      // Очищаем буффер
      memset (buffer, 0, 50);      

      delay(200);
      digitalWrite(12, LOW);

      }
      
      if ((String)command_in == "deactive"){
          digitalWrite(2, LOW);
      
              digitalWrite(12, HIGH);
          
      Serial.print("002,arduino recive: unit ");
      Serial.print(unitID_in);
      Serial.print(" RELE ");
      Serial.print(command_in);
      Serial.print("\n");
      


      memset (unitID_in, 0, 10);
      memset (command_in, 0, 10);
      
      // Очищаем буффер
      memset (buffer, 0, 50);      
      
      delay(200);
      digitalWrite(12, LOW);
      
      }   
      
      // Очищаем буффер
      memset (buffer, 0, 50);      
           
    }
 
  // Выполнение команды устройством 004
     if ((String)unitID_in == "004"){
       
      if ((String)command_in == "active"){
        
          digitalWrite(4, HIGH);
      
      digitalWrite(12, HIGH);
          
      Serial.print("004,arduino recive: unit ");
      Serial.print(unitID_in);
      Serial.print(" RELE ");
      Serial.print(command_in);
      Serial.print("\n");
      

      
      memset (unitID_in, 0, 10);
      memset (command_in, 0, 10);
      
      // Очищаем буффер
      memset (buffer, 0, 50);
      
      delay(200);
      digitalWrite(12, LOW);

      }
      
      if ((String)command_in == "deactive"){
          digitalWrite(4, LOW);
      
      digitalWrite(12, HIGH);
          
      Serial.print("004,arduino recive: unit ");
      Serial.print(unitID_in);
      Serial.print(" RELE ");
      Serial.print(command_in);
      Serial.print("\n");
      
     
      
      memset (unitID_in, 0, 10);
      memset (command_in, 0, 10);
      
      // Очищаем буффер
      memset (buffer, 0, 50);

      delay(200);
      digitalWrite(12, LOW);
      
      }   
           
      // Очищаем буффер
      memset (buffer, 0, 50);      
           
    }
 
  // Выполнение команды устройством 007 
     if ((String)unitID_in == "007"){
       
      if ((String)command_in == "active"){
        
          digitalWrite(7, HIGH);
      
      digitalWrite(12, HIGH);
          
      Serial.print("007,arduino recive: unit ");
      Serial.print(unitID_in);
      Serial.print(" RELE ");
      Serial.print(command_in);
      Serial.print("\n");
      

      
      memset (unitID_in, 0, 10);
      memset (command_in, 0, 10);

      // Очищаем буффер
      memset (buffer, 0, 50);

      delay(200);
      digitalWrite(12, LOW);      

      }
      
      if ((String)command_in == "deactive"){
          digitalWrite(7, LOW);
      
      digitalWrite(12, HIGH);
          
      Serial.print("007,arduino recive: unit ");
      Serial.print(unitID_in);
      Serial.print(" RELE ");
      Serial.print(command_in);
      Serial.print("\n");
      

      
      memset (unitID_in, 0, 10);
      memset (command_in, 0, 10);
      
      // Очищаем буффер
      memset (buffer, 0, 50);

      delay(200);
      digitalWrite(12, LOW);
      
      }   
           
      // Очищаем буффер
      memset (buffer, 0, 50);      
    }
 
}


