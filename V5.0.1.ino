#include <SimpleModbusSlave.h>
#include <PWM.h>

/* 
   SimpleModbusSlaveV10 supports function 3, 6 & 16.
   
   The modbus_update() method updates the holdingRegs register array and checks
   communication.

   Note:  
   The Arduino serial ring buffer is 64 bytes or 32 registers.
   Most of the time you will connect the arduino to a master via serial
   using a MAX485 or similar.
 
   In a function 3 request the master will attempt to read from your
   slave and since 5 bytes is already used for ID, FUNCTION, NO OF BYTES
   and two BYTES CRC the master can only request 58 bytes or 29 registers.
 
   In a function 16 request the master will attempt to write to your 
   slave and since a 9 bytes is already used for ID, FUNCTION, ADDRESS, 
   NO OF REGISTERS, NO OF BYTES and two BYTES CRC the master can only write
   54 bytes or 27 registers.
 
   Using a USB to Serial converter the maximum bytes you can send is 
   limited to its internal buffer which differs between manufactures. 
*/  

// Using the enum instruction allows for an easy method for adding and 
// removing registers. Doing it this way saves you #defining the size 
// of your slaves register array each time you want to add more registers
// and at a glimpse informs you of your slaves register layout.

//////////////// registers of your slave ///////////////////
enum 
{     
  // just add or remove registers and your good to go...
  // The first register starts at address 0
  ADC0_VAL,
  ADC1_VAL,
  ADC2_VAL,
  ADC3_VAL,
  ADC4_VAL,
  ADC5_VAL,
  FRQ9_VAL,
  PWM9_VAL,
  FRQ10_VAL,
  PWM10_VAL,
  FRQ3_VAL,
  PWM3_VAL,
  FRQ11_VAL,
  PWM11_VAL,
  PWM5_VAL,
  PWM6_VAL,
  LOG4_VAL,
  LOG7_VAL,
  LOG8_VAL,
  LOG12_VAL, 
  HOLDING_REGS_SIZE // leave this one
  // total number of registers for function 3 and 16 share the same register array
  // i.e. the same address space
};

unsigned int holdingRegs[HOLDING_REGS_SIZE]; // function 3 and 16 register array
////////////////////////////////////////////////////////////

void setup()
{
  /* parameters(HardwareSerial* SerialPort,
                long baudrate, 
		unsigned char byteFormat,
                unsigned char ID, 
                unsigned char transmit enable pin, 
                unsigned int holding registers size,
                unsigned int* holding register array)
  */
  
  /* Valid modbus byte formats are:
     SERIAL_8N2: 1 start bit, 8 data bits, 2 stop bits
     SERIAL_8E1: 1 start bit, 8 data bits, 1 Even parity bit, 1 stop bit
     SERIAL_8O1: 1 start bit, 8 data bits, 1 Odd parity bit, 1 stop bit
     
     You can obviously use SERIAL_8N1 but this does not adhere to the
     Modbus specifications. That said, I have tested the SERIAL_8N1 option 
     on various commercial masters and slaves that were suppose to adhere
     to this specification and was always able to communicate... Go figure.
     
     These byte formats are already defined in the Arduino global name space. 
  */
	
  modbus_configure(&Serial, 9600, SERIAL_8N2, 1, 2, HOLDING_REGS_SIZE, holdingRegs);

  // modbus_update_comms(baud, byteFormat, id) is not needed but allows for easy update of the
  // port variables and slave id dynamically in any function.
  modbus_update_comms(9600, SERIAL_8N2, 1);
  
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
  
  //initialize all timers except for 0, to save time keeping functions
  InitTimersSafe();
}

void loop()
{
  
  modbus_update();
  
  holdingRegs[ADC0_VAL] = analogRead(A0);
  holdingRegs[ADC1_VAL] = analogRead(A1);
  holdingRegs[ADC2_VAL] = analogRead(A2);
  holdingRegs[ADC3_VAL] = analogRead(A3);
  holdingRegs[ADC4_VAL] = analogRead(A4);
  holdingRegs[ADC5_VAL] = analogRead(A5);
  
  // Digital 9 PWM
  SetPinFrequency(9, holdingRegs[FRQ9_VAL]);
  pwmWrite(9, holdingRegs[PWM9_VAL]);
  
  // Digital 10 PWM
  SetPinFrequency(10, holdingRegs[FRQ10_VAL]);
  pwmWrite(10, holdingRegs[PWM10_VAL]);
  
  // Digital 3 PWM
  SetPinFrequency(3, holdingRegs[FRQ3_VAL]);
  pwmWrite(3, holdingRegs[PWM3_VAL]);
  
  // Digital 11 PWM
  SetPinFrequency(11, holdingRegs[FRQ11_VAL]);
  pwmWrite(11, holdingRegs[PWM11_VAL]);
  
  // Digital 5 PWM
  analogWrite(5, holdingRegs[PWM5_VAL]);
  
  // Digital 6 PWM
  analogWrite(6, holdingRegs[PWM6_VAL]);
  
  // Digital activation/deactivation
  digitalWrite(4, holdingRegs[LOG4_VAL]);
  digitalWrite(7, holdingRegs[LOG7_VAL]);
  digitalWrite(8, holdingRegs[LOG8_VAL]);
  digitalWrite(12, holdingRegs[LOG12_VAL]);
  
}

