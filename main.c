#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_tim.h"
#include "misc.h"

#define OBJ_SZ 123
#define SETUP 4 //setup elements

/*#define ADC_CH 4  //количество каналов
#define OVER_SAMPL 32 //количество семплов для усреднения
uint32_t ADC_SUM[ADC_CH]; //буффер для суммирования 
uint16_t curr_sample;//считаем количество семлов
uint16_t AI[ADC_CH],ADC_VAL[ADC_CH];//АI- финальный буфер, ADC_VAL- в нее пишем DMA*/

//PARAMETERRS ARRAY 0 PARAMETER = MODBUS ADDRESS
uint8_t SET_PAR[SETUP];

//OBJECT ARRAY WHERE READING AND WRITING OCCURS
union {
int32_t regs[OBJ_SZ];
int8_t bytes[OBJ_SZ*2];
}res_table;

//buffer uart
#define BUF_SZ 256
#define MODBUS_WRD_SZ (BUF_SZ-5)/2 //max quantity of words in responce

//uart structure
typedef struct {
uint8_t buffer[BUF_SZ];
uint16_t rxtimer;
uint8_t rxcnt;
uint8_t txcnt;
uint8_t txlen;
uint8_t rxgap;
uint8_t protocol;
uint8_t delay;
} UART_DATA;

UART_DATA uart3,uart1;

void MODBUS_SLAVE(UART_DATA *MODBUS);



uint8_t tranz = 0;
//uint16_t prescaler;

unsigned int period;
uint32_t adc_res_1;
uint32_t adc_res_2;
uint32_t adc_res_3;
uint32_t adc_res_4;	

unsigned char mb_flag = 0;
unsigned long int delay;


/***************************************************************************//**
 * Declare function prototypes
 ******************************************************************************/
void SetupClock(void);
void SetupGPIO(void);
void InitDAC(void);
void InitTIM3(void);
void InstallingTIM3(void);
//void SetupADC(void);
//void adc_init(void);
//void Setup_ADC1(void);
//void DMA1_Channel1_IRQHandler(void);
//void ADC_AVERAGING(void);
void ADC_Configuration(void);
u16 readADC1(u8 channel);

unsigned int Crc16(unsigned char *ptrByte, int byte_cnt);
void TX_03_04(UART_DATA *MODBUS);
void TX_06(UART_DATA *MODBUS);
void TX_EXCEPTION(UART_DATA *MODBUS,unsigned char error_type);

void SetupUSART3(void);
void SetupTIM7(void);

//***************************************************************************
//send data from uart3 if data is ready
//***************************************************************************
void net_tx3(UART_DATA *uart)
{
  if((uart->txlen>0)&(uart->txcnt==0))
  {
	    USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
	    USART_ITConfig(USART3, USART_IT_TC, ENABLE);

	    //rs485 txenable
	  	GPIO_WriteBit(GPIOB,GPIO_Pin_12,Bit_SET);
	  	//led pc8
	  	GPIO_WriteBit(GPIOC,GPIO_Pin_8,Bit_SET);

	  	USART_SendData(USART3, uart->buffer[uart->txcnt++]);
  }

}

//***************************************************************************
// *  USART3 interrupt
// **************************************************************************
void USART3_IRQHandler(void)
{
	//Receive Data register not empty interrupt
  	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
   {
  		 USART_ClearITPendingBit(USART3, USART_IT_RXNE);
  		uart3.rxtimer=0;

  			if(uart3.rxcnt>(BUF_SZ-2))
  			uart3.rxcnt=0;

  		 	uart3.buffer[uart3.rxcnt++]=USART_ReceiveData (USART3);


   }

  	//Transmission complete interrupt
    if(USART_GetITStatus(USART3, USART_IT_TC) != RESET)
  	{

  		USART_ClearITPendingBit(USART3, USART_IT_TC);
  		  if(uart3.txcnt<uart3.txlen)
  		{
  			USART_SendData(USART3,uart3.buffer[uart3.txcnt++]);
  		}
  		 else
  		{
  		 uart3.txlen=0;
  		 //rs485 tx disable
  		 GPIO_WriteBit(GPIOB,GPIO_Pin_12,Bit_RESET);
  		 //led PC8
  		 GPIO_WriteBit(GPIOC,GPIO_Pin_8,Bit_RESET);

  		 USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
  		 USART_ITConfig(USART3, USART_IT_TC, DISABLE);
  		}
  	}

}

//***************************************************************************
//Timer interrupt
//***************************************************************************
void TIM7_IRQHandler(void)
{
	TIM_ClearITPendingBit(TIM7, TIM_IT_Update);

	//blinking with PC9
	if(GPIO_ReadOutputDataBit  ( GPIOC,GPIO_Pin_9))
	     GPIO_WriteBit(GPIOC,GPIO_Pin_9,Bit_RESET);
	else
		GPIO_WriteBit(GPIOC,GPIO_Pin_9,Bit_SET);

	   if((uart3.rxtimer++>uart3.delay)&(uart3.rxcnt>1))
	   uart3.rxgap=1;
	   else
	   uart3.rxgap=0;

}

uint16_t get_adc_value()
{
 ADC_SoftwareStartConvCmd(ADC1, ENABLE);
 while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
 return ADC_GetConversionValue(ADC1);
}

//***************************************************************************
//main()
//***************************************************************************
int main(void)
{				
	
			

	
			 SetupClock();

       //tim6 +interrupts
       SetupTIM7();
	
		   //uarts inint+interrupts
       SetupUSART3(); //RS485

       //RS485 TXE PIN AND LED PINS
       SetupGPIO();

       SET_PAR[0]=1;//modbus address

       //timer 0.0001sec one symbol on 9600 ~1ms
       uart3.delay=30; //modbus gap
	
			 InitDAC();
	
			 //SetupADC();
			 //adc_init();
			 //ADC_SoftwareStartConvCmd(ADC1, ENABLE);
			 //DMA_Cmd(DMA1_Channel1, ENABLE);
			 //Setup_ADC1();
			 ADC_Configuration();
	
			 InitTIM3();
			 
			 //InstallingTIM3();



     //Main loop
     while(1)
     {

    	if(uart3.rxgap==1)
    	 {
     	 MODBUS_SLAVE(&uart3);
    	 net_tx3(&uart3);
				 tranz++;
				 mb_flag = 1;
    	 }
			 
			 // DAC initialization
			 
			 DAC->DHR12R1 = res_table.regs[7];	
			 DAC->SWTRIGR |= DAC_SWTRIGR_SWTRIG1;    // программное события запуска 
       DAC->DHR12R2 = res_table.regs[8];	
			 DAC->SWTRIGR |= DAC_SWTRIGR_SWTRIG2;    // программное события запуска			 
			 
			 // ADC initialization
			 /*
			 ADC1->JSQR |= (0<<15); // канал АЦП_0
			 while (!(ADC1->SR & ADC_SR_JEOC));
			 res_table.regs[4] = ADC1->JDR1;
			 
			 ADC1->JSQR |= (1<<15); // канал АЦП_1
			 while (!(ADC1->SR & ADC_SR_JEOC));
			 res_table.regs[5] = ADC1->JDR1;
			 
			 ADC1->JSQR |= (2<<15); // канал АЦП_2
			 while (!(ADC1->SR & ADC_SR_JEOC));
			 res_table.regs[6] = ADC1->JDR1;
			 
			 ADC1->JSQR |= (3<<15); // канал АЦП_3
			 while (!(ADC1->SR & ADC_SR_JEOC));
			 res_table.regs[7] = ADC1->JDR1;
			 */
			 //res_table.regs[4] = ADC1->JDR1;
			 //res_table.regs[5] = adc_res_2;
			 //res_table.regs[6] = adc_res_3;
			 //res_table.regs[7] = adc_res_4;
			 
			 //res_table.regs[10] = get_adc_value();
			 //ADC_AVERAGING();
			 //res_table.regs[4] = ADC_VAL[0];
			 //res_table.regs[5] = ADC_VAL[1];
			 //res_table.regs[6] = ADC_VAL[2];
			 //res_table.regs[7] = ADC_VAL[3];
			 res_table.regs[10] = readADC1(0);
			 res_table.regs[11] = readADC1(1);
			 res_table.regs[12] = readADC1(2);
			 res_table.regs[13] = readADC1(3);
			 
			if(tranz == 0)
			 {
				GPIO_ResetBits(GPIOC, GPIO_Pin_7);
   			GPIO_ResetBits(GPIOC, GPIO_Pin_10);
				GPIO_ResetBits(GPIOC, GPIO_Pin_11);
				GPIO_ResetBits(GPIOC, GPIO_Pin_12);
			 }
			if(tranz == 1) GPIO_SetBits(GPIOC, GPIO_Pin_7);
			if(tranz == 2)
			{
				GPIO_ResetBits(GPIOC, GPIO_Pin_7);
				GPIO_SetBits(GPIOC, GPIO_Pin_10);
			}
			if(tranz == 3)	GPIO_SetBits(GPIOC, GPIO_Pin_7);
			
			if(tranz == 4)
			{
				GPIO_ResetBits(GPIOC, GPIO_Pin_7);
				GPIO_ResetBits(GPIOC, GPIO_Pin_10);
				GPIO_SetBits(GPIOC, GPIO_Pin_11);
			}
			if(tranz == 5)	GPIO_SetBits(GPIOC, GPIO_Pin_7);
			
			if(tranz == 6)
			{
				GPIO_ResetBits(GPIOC, GPIO_Pin_7);
				GPIO_SetBits(GPIOC, GPIO_Pin_10);
			}
			if(tranz == 7)	GPIO_SetBits(GPIOC, GPIO_Pin_7);
			
			if(tranz == 8)
			{
				GPIO_ResetBits(GPIOC, GPIO_Pin_7);
   			GPIO_ResetBits(GPIOC, GPIO_Pin_10);
				GPIO_ResetBits(GPIOC, GPIO_Pin_11);
				GPIO_SetBits(GPIOC, GPIO_Pin_12);
			}
			if(tranz == 9)	GPIO_SetBits(GPIOC, GPIO_Pin_7);
			
			if(tranz > 9) tranz = 0;
			
			 // Relay module
			  if(res_table.regs[0] > 0) GPIO_ResetBits(GPIOC, GPIO_Pin_0);
				else GPIO_SetBits(GPIOC, GPIO_Pin_0);
			
			  if(res_table.regs[1] > 0) GPIO_ResetBits(GPIOC, GPIO_Pin_1);
				else GPIO_SetBits(GPIOC, GPIO_Pin_1);
			
			  if(res_table.regs[2] > 0) GPIO_ResetBits(GPIOC, GPIO_Pin_2);
				else GPIO_SetBits(GPIOC, GPIO_Pin_2);
			
			  if(res_table.regs[3] > 0) GPIO_ResetBits(GPIOC, GPIO_Pin_3);
				else GPIO_SetBits(GPIOC, GPIO_Pin_3);
				
				if(res_table.regs[4] > 0) GPIO_ResetBits(GPIOC, GPIO_Pin_4);
				else GPIO_SetBits(GPIOC, GPIO_Pin_4);
				
				if(res_table.regs[5] > 0) GPIO_ResetBits(GPIOC, GPIO_Pin_5);
				else GPIO_SetBits(GPIOC, GPIO_Pin_5);
				
				if(res_table.regs[6] > 0) GPIO_ResetBits(GPIOC, GPIO_Pin_6);
				else GPIO_SetBits(GPIOC, GPIO_Pin_6);
				
				// Indicators
				
				if(res_table.regs[14] > 0) GPIO_SetBits(GPIOB, GPIO_Pin_13);
				else GPIO_ResetBits(GPIOB, GPIO_Pin_13);
				
				if(res_table.regs[15] > 0) GPIO_SetBits(GPIOB, GPIO_Pin_14);
				else GPIO_ResetBits(GPIOB, GPIO_Pin_14);
				
				if(res_table.regs[16] > 0) GPIO_SetBits(GPIOB, GPIO_Pin_15);
				else GPIO_ResetBits(GPIOB, GPIO_Pin_15);
				
				// PWM initiallization
				//prescaler = res_table.regs[9];
				period = res_table.regs[9];
				//delay = 100000;
				while(delay) delay--;
				
				if(mb_flag > 0)
				{
					InstallingTIM3();
					//ADC_AVERAGING();
			 //res_table.regs[4] = ADC_VAL[0];
			 //res_table.regs[5] = ADC_VAL[1];
			 //res_table.regs[6] = ADC_VAL[2];
			 //res_table.regs[7] = ADC_VAL[3];
			 //res_table.regs[10] = readADC1(0);
			 //res_table.regs[11] = readADC1(1);
			 //res_table.regs[12] = readADC1(2);
			 //res_table.regs[13] = readADC1(3);
					mb_flag = 0;
					
				}
				
				
			 /*
			  if(res_table.regs[6] > 0) GPIO_SetBits(GPIOB, GPIO_Pin_7);
				else GPIO_ResetBits(GPIOB, GPIO_Pin_7);
					
			 if(res_table.regs[5])
				GPIO_WriteBit(GPIOB,GPIO_Pin_3,Bit_SET);
	     else
				GPIO_WriteBit(GPIOC,GPIO_Pin_3,Bit_RESET);
			 */

			 
     }
}

/***************************************************************************//**
 * @brief Setup clocks
 ******************************************************************************/
void SetupClock()
{
      RCC_DeInit ();                    /* RCC system reset(for debug purpose)*/
      RCC_HSEConfig (RCC_HSE_ON);       /* Enable HSE                         */

      /* Wait till HSE is ready                                               */
      while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET);

      RCC_HCLKConfig   (RCC_SYSCLK_Div1);   /* HCLK   = SYSCLK                */
      RCC_PCLK2Config  (RCC_HCLK_Div1);     /* PCLK2  = HCLK                  */
      RCC_PCLK1Config  (RCC_HCLK_Div2);     /* PCLK1  = HCLK/2                */
      RCC_ADCCLKConfig (RCC_PCLK2_Div4);    /* ADCCLK = PCLK2/4               */

      /* PLLCLK = 8MHz * 3 = 24 MHz                                           */
      RCC_PLLConfig (RCC_PLLSource_PREDIV1, RCC_PLLMul_3);

      RCC_PLLCmd (ENABLE);                  /* Enable PLL                     */

      /* Wait till PLL is ready                                               */
      while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

      /* Select PLL as system clock source                                    */
      RCC_SYSCLKConfig (RCC_SYSCLKSource_PLLCLK);

      /* Wait till PLL is used as system clock source                         */
      while (RCC_GetSYSCLKSource() != 0x08);

      /* Enable USART1 and GPIOA clock                                        */
   //   RCC_APB2PeriphClockCmd (RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

}
//*************************************************************************************************
void SetupUSART3(void)
{
	             NVIC_InitTypeDef  NVIC_InitStructure;
	             GPIO_InitTypeDef  GPIO_InitStructure;
	             USART_InitTypeDef USART_InitStructure;


	             /* Enable GPIOB clock                                                   */
                 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
                 RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

                 /* Configure USART3 Rx (PA11) as input floating                         */
                 GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11;
                 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
                 GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
                 GPIO_Init(GPIOB, &GPIO_InitStructure);

                 /* Configure USART3 Tx (PA10) as alternate function push-pull            */
                 GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
                 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
                 GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
                 GPIO_Init(GPIOB, &GPIO_InitStructure);

                 //setting parametrs common for all uarts
                 USART_InitStructure.USART_BaudRate            = 9600;
                 USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
                 USART_InitStructure.USART_StopBits            = USART_StopBits_1;
                 USART_InitStructure.USART_Parity              = USART_Parity_No ;
                 USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
                 USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;

            	  USART_Init(USART3, &USART_InitStructure);
              	  USART_Cmd(USART3, ENABLE);

              	  //Setting interrupts

                  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
                  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
                  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
                  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
                  NVIC_Init(&NVIC_InitStructure);

                  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);  /* Enable Receive interrupt */

}

//*******************************************************

void SetupTIM7()
{
	NVIC_InitTypeDef  NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7 , ENABLE);
    TIM_DeInit(TIM7);

    //0.0001 sec setup APB=36Mhz/(36*100)
    TIM_TimeBaseStructure.TIM_Prescaler= 24;
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period=100;//till what value timer will count

    TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM7, TIM_FLAG_Update);
    TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM7, ENABLE);


   // NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
      NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
      NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
      NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
      NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
      NVIC_Init(&NVIC_InitStructure);

      TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE);
}

//***********************************************************************


void SetupGPIO(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    /* Configure PB12 as rs485 tx select           */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    /* Configure PC8,9 leds          */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	  //GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11;
    //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    //GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    //GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	  //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	
	    /* Configure external leds pb3, pb4         */
    //GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
    //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    //GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    //GPIO_Init(GPIOD, &GPIO_InitStructure);
		
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	    /* Configure external          */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}
//***************************************************************************

//Функция ининиализации ЦАП
void InitDAC()
{
	
	  GPIO_InitTypeDef GPIO_InitStructure;
	
    /* Включаем ЦАП */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
	
	DAC->CR |= DAC_CR_TEN1; // включение вывода данных от события
	DAC->CR |= DAC_CR_TSEL1; // программное событие для цап_1
	DAC->CR |= DAC_CR_TSEL2; // программное событие для цап_2
	//DAC->CR |= DAC_CR_BOFF1;        // выкл выходной буфер 1
	//DAC->CR |= DAC_CR_BOFF2;        // выкл выходной буфер 2
	
	  /* Настраиваем ногу ЦАПа_1 */

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
		  /* Настраиваем ногу ЦАПа_2 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	
	 /* Включить DAC1 */
  DAC->CR |= DAC_CR_EN1;
	
	 /* Включить DAC2 */
  DAC->CR |= DAC_CR_EN2;
}

//Функция ининиализации таймера 3 и вывода шим
void InitTIM3()
{		
				// Конфигурация порта
    GPIO_InitTypeDef GPIO_Config;
			  // Конфигурация таймера
    TIM_TimeBaseInitTypeDef TIM_BaseConfig;
				// Конфигурация выхода таймера
    TIM_OCInitTypeDef TIM_OCConfig;
	
	    // Подаем тактовую частоту на таймер
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
		

    // Пин 6
    GPIO_Config.GPIO_Pin = GPIO_Pin_7;
    // Альтернативная функция (в нашем случае - выход таймера), Push-Pull
    GPIO_Config.GPIO_Mode = GPIO_Mode_AF_PP;
    // Частота - 50 MHz
    GPIO_Config.GPIO_Speed = GPIO_Speed_2MHz;

    // Инициализируем порт A этой конфигурацией
    GPIO_Init(GPIOA, &GPIO_Config);
	

	
		// Запускаем таймер на тактовой частоте в 8000 kHz
    TIM_BaseConfig.TIM_Prescaler = 0;//(uint16_t) (SystemCoreClock / 8000000) - 1;
    // Период - 10 тактов => 8000/10 = 800 kHz
    TIM_BaseConfig.TIM_Period = 10;
    TIM_BaseConfig.TIM_ClockDivision = 0;
    // Отсчет от нуля до TIM_Period
    TIM_BaseConfig.TIM_CounterMode = TIM_CounterMode_Up;

    // Инициализируем таймер №3 (его выходы как раз на порту A)
    TIM_TimeBaseInit(TIM3, &TIM_BaseConfig);

    // Конфигурируем выход таймера, режим - PWM1
    TIM_OCConfig.TIM_OCMode = TIM_OCMode_PWM1;
    // Собственно - выход включен
    TIM_OCConfig.TIM_OutputState = TIM_OutputState_Enable;
    // Пульс длинной 9 тактов => 9/10 = 90%
    TIM_OCConfig.TIM_Pulse = 5;
    // Полярность => пульс - это единица (+3.3V)
    TIM_OCConfig.TIM_OCPolarity = TIM_OCPolarity_High;

    // Инициализируем второй выход таймера №3 (у HD это PA7)
    TIM_OC2Init(TIM3, &TIM_OCConfig);
		
		// Перезарядка таймера
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
    //TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM3, ENABLE);

    // Включаем таймер
    TIM_Cmd(TIM3, ENABLE);
}

void InstallingTIM3()
{
	/*
		  // Конфигурация таймера
    TIM_TimeBaseInitTypeDef TIM_BaseConfig;
		TIM_OCInitTypeDef TIM_OCConfig;
		
		//TIM_BaseConfig.TIM_Prescaler = (uint16_t) (SystemCoreClock / prescaler) - 1;
	
		TIM_BaseConfig.TIM_Period = period;
		TIM_OCConfig.TIM_Pulse = period - 1;
	*/
	
	
			// Конфигурация порта
    //GPIO_InitTypeDef GPIO_Config;
			  // Конфигурация таймера
    //TIM_TimeBaseInitTypeDef TIM_BaseConfig;
				// Конфигурация выхода таймера
    //TIM_OCInitTypeDef TIM_OCConfig;
	
	  /*  // Подаем тактовую частоту на таймер
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
		

    // Пин 6
    GPIO_Config.GPIO_Pin = GPIO_Pin_6;
    // Альтернативная функция (в нашем случае - выход таймера), Push-Pull
    GPIO_Config.GPIO_Mode = GPIO_Mode_AF_PP;
    // Частота - 50 MHz
    GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;

    // Инициализируем порт A этой конфигурацией
    GPIO_Init(GPIOA, &GPIO_Config);
	
		*/
	
		// Запускаем таймер на тактовой частоте в 8000 kHz
    //TIM_BaseConfig.TIM_Prescaler = (uint16_t) (SystemCoreClock / 8000000) - 1;
    // Период - 10 тактов => 8000/10 = 800 kHz
    //TIM_BaseConfig.TIM_Period = period;
    //TIM_BaseConfig.TIM_ClockDivision = 0;
    // Отсчет от нуля до TIM_Period
    //TIM_BaseConfig.TIM_CounterMode = TIM_CounterMode_Up;
		if(period < 10) period = 10;		
		TIM3->ARR = period;
		//if(TIM3->ARR > 240)TIM3->ARR =24;
		
    // Инициализируем таймер №3 (его выходы как раз на порту A)
    //TIM_TimeBaseInit(TIM3, &TIM_BaseConfig);

    // Конфигурируем выход таймера, режим - PWM1
    //TIM_OCConfig.TIM_OCMode = TIM_OCMode_PWM1;
    // Собственно - выход включен
    //TIM_OCConfig.TIM_OutputState = TIM_OutputState_Enable;
    // Пульс длинной 9 тактов => 9/10 = 90%
    //TIM_OCConfig.TIM_Pulse = period - 1;
    // Полярность => пульс - это единица (+3.3V)
    //TIM_OCConfig.TIM_OCPolarity = TIM_OCPolarity_High;


			TIM3->CCR2 = 3*period/4;

    // Инициализируем первый выход таймера №3 (у HD это PA6)
    //TIM_OC1Init(TIM3, &TIM_OCConfig);
		
		// Перезарядка таймера
    //TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
    //TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
    //TIM_ARRPreloadConfig(TIM3, ENABLE);

    // Включаем таймер
    //TIM_Cmd(TIM3, ENABLE);
}

/*
void SetupADC ()
{
  RCC_APB2PeriphClockCmd(RCC_APB2ENR_ADC1EN, ENABLE); //Включаем тактирование АЦП
  ADC1->CR2 |= ADC_CR2_CAL; //Запуск калибровки АЦП
  while (!(ADC1->CR2 & ADC_CR2_CAL)); //Ожидаем окончания калибровки
  ADC1->SMPR2 |= (ADC_SMPR2_SMP1_2 | ADC_SMPR2_SMP1_1 | ADC_SMPR2_SMP1_0); //Задаем
                                                                           // длительность выборки
  ADC1->CR2 |= ADC_CR2_JEXTSEL; //Преобразование инжектированной группы
                                //запустится установкой бита JSWSTART
  ADC1->CR2 |= ADC_CR2_JEXTTRIG; //Разрешаем внешний запуск инжектированной группы
  ADC1->CR2 |= ADC_CR2_CONT; //Преобразования запускаются одно за другим
  ADC1->CR1 |= ADC_CR1_JAUTO; //Разрешить преобразование инжектированной группы
                              //после регулярной. Не понятно зачем, но без этого не работает
  ADC1->JSQR |= (1<<15); //Задаем номер канала (выбран ADC1)
  ADC1->CR2 |= ADC_CR2_ADON;//Теперь включаем АЦП
  ADC1->CR2 |= ADC_CR2_JSWSTART; //Запуск преобразований
  while (!(ADC1->SR & ADC_SR_JEOC)); //ждем пока первое преобразование завершится
                                     //Теперь можно читать результат из JDR1
	
}
*/

/*void adc_init()
{
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

 // настройки ADC
 ADC_InitTypeDef ADC_InitStructure;
	
 ADC_StructInit(&ADC_InitStructure);
 ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; // режим работы - одиночный, независимый
 ADC_InitStructure.ADC_ScanConvMode = DISABLE; // не сканировать каналы, просто измерить один канал
 ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; // однократное измерение
 ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // без внешнего триггера
 ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; //выравнивание битов результат - прижать вправо
 ADC_InitStructure.ADC_NbrOfChannel = 1; //количество каналов - одна штука
 ADC_Init(ADC1, &ADC_InitStructure);
 ADC_Cmd(ADC1, ENABLE);
 
 // настройка канала
 ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_55Cycles5);
 
 // калибровка АЦП
 ADC_ResetCalibration(ADC1);
 while (ADC_GetResetCalibrationStatus(ADC1));
 ADC_StartCalibration(ADC1);
 while (ADC_GetCalibrationStatus(ADC1)); 
}
*/

//Настраиваем АЦП на работу в режиме DMA с прерыванием по Transfer complete
/*void Setup_ADC1(void)
{

        //==Definitions==
        #define ADC1_DR_Address    ((uint32_t)0x4001244C)


        NVIC_InitTypeDef  NVIC_InitStructure;
        DMA_InitTypeDef DMA_InitStructure; //Variable used to setup the DMA

        ADC_InitTypeDef ADC_InitStructure;

        //--Enable DMA1 clock--
				RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

        //==Configure DMA1 - Channel1==
           DMA_DeInit(DMA1_Channel1); //Set DMA registers to default values
           DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address; //Address of peripheral the DMA must map to
           DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) & ADC_VAL; //Variable to which ADC values will be stored
           DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
           DMA_InitStructure.DMA_BufferSize = ADC_CH; //Buffer size (8 because we using 8 channels)
           DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
           DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
           DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
           DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
           DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
           DMA_InitStructure.DMA_Priority = DMA_Priority_High;
           DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;


           //Настройка Прерывание -по окончании трансфера

           NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn ;
           NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
           NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
           NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
           NVIC_Init(&NVIC_InitStructure);

           DMA_ITConfig(DMA1_Channel1, DMA1_IT_TC1, ENABLE);
           DMA_Init(DMA1_Channel1, &DMA_InitStructure); //Initialise the DMA


           //Настройка параметров  ADC1 - Channel 0 -7

              ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
              ADC_InitStructure.ADC_ScanConvMode = ENABLE;
              ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
              ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
              ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
              ADC_InitStructure.ADC_NbrOfChannel = ADC_CH; //We using 8 channels

              ADC_Init(ADC1, &ADC_InitStructure); //Initialise ADC1

              //Порядок оцифровки

              #define ADC_SampleTime ADC_SampleTime_239Cycles5*/
              /*
							ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 1, ADC_SampleTime);
              ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 2, ADC_SampleTime);
              ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 3, ADC_SampleTime);
              ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 4, ADC_SampleTime);
              ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 5, ADC_SampleTime);
              ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 6, ADC_SampleTime);
              ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 7, ADC_SampleTime);
              ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 8, ADC_SampleTime);
							*/
/*
							ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime);
              ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime);
              ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime);
              ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime);
              //ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 5, ADC_SampleTime);
              //ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 6, ADC_SampleTime);
              //ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 7, ADC_SampleTime);
              //ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 8, ADC_SampleTime);

              ADC_DMACmd(ADC1, ENABLE); //Enable ADC1 DMA
              ADC_Cmd(ADC1, ENABLE); //Enable ADC1


               //Калибровка ADC1

                 //Enable ADC1 reset calibaration register
                 ADC_ResetCalibration(ADC1);
                 while (ADC_GetResetCalibrationStatus(ADC1)); //Check the end of ADC1 reset calibration register

                 //Start ADC1 calibaration
                ADC_StartCalibration(ADC1);
                while (ADC_GetCalibrationStatus(ADC1)); //Check the end of ADC1 calibration


}

void DMA1_Channel1_IRQHandler(void)
{
   DMA_ClearITPendingBit( DMA1_IT_TC1);
   DMA_ITConfig(DMA1_Channel1, DMA1_IT_TC1, DISABLE);

   if(curr_sample<OVER_SAMPL)
   {
   ADC_SUM[0]+=ADC_VAL[0];
   ADC_SUM[1]+=ADC_VAL[1];
   ADC_SUM[2]+=ADC_VAL[2];
   ADC_SUM[3]+=ADC_VAL[3];
   //ADC_SUM[4]+=ADC_VAL[4];
   //ADC_SUM[5]+=ADC_VAL[5];
   //ADC_SUM[6]+=ADC_VAL[6];
   //ADC_SUM[7]+=ADC_VAL[7];
   }

   curr_sample++;

   DMA_ITConfig(DMA1_Channel1, DMA1_IT_TC1, ENABLE);

   if(curr_sample>=OVER_SAMPL)
   DMA_ITConfig(DMA1_Channel1, DMA1_IT_TC1, DISABLE);

}

void ADC_AVERAGING(void)
{
 int16_t i;

 if(curr_sample>=OVER_SAMPL)
 {
    for(i=0;i<ADC_CH;i++)
   {
   //averaging
   AI[i]=(AI[i]*2+(ADC_SUM[i]/OVER_SAMPL))/3;
   ADC_SUM[i]=0;
   }
   curr_sample=0;
   DMA_ITConfig(DMA1_Channel1, DMA1_IT_TC1, ENABLE);

 }

}
*/

void ADC_Configuration(void)
{
  ADC_InitTypeDef  ADC_InitStructure;
  /* PCLK2 is the APB2 clock */
  /* ADCCLK = PCLK2/6 = 72/6 = 12MHz*/
  RCC_ADCCLKConfig(RCC_PCLK2_Div6);

  /* Enable ADC1 clock so that we can talk to it */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  /* Put everything back to power-on defaults */
  ADC_DeInit(ADC1);

  /* ADC1 Configuration ------------------------------------------------------*/
  /* ADC1 and ADC2 operate independently */
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  /* Disable the scan conversion so we do one at a time */
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  /* Don't do contimuous conversions - do them on demand */
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  /* Start conversin by software, not an external trigger */
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  /* Conversions are 12 bit - put them in the lower 12 bits of the result */
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  /* Say how many channels would be used by the sequencer */
  ADC_InitStructure.ADC_NbrOfChannel = 1;

  /* Now do the setup */
  ADC_Init(ADC1, &ADC_InitStructure);
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);

  /* Enable ADC1 reset calibaration register */
  ADC_ResetCalibration(ADC1);
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));
  /* Start ADC1 calibaration */
  ADC_StartCalibration(ADC1);
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));
}

u16 readADC1(u8 channel)
{
  ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_1Cycles5);
  // Start the conversion
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
  // Wait until conversion completion
  while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
  // Get the conversion value
  return ADC_GetConversionValue(ADC1);
}

//*********************************************************************
//Modbus slave function
//*********************************************************************
 void MODBUS_SLAVE(UART_DATA *MODBUS)
{
  unsigned int tmp;


     //recive and checking rx query
	if((MODBUS->buffer[0]!=0)&(MODBUS->rxcnt>5)& ((MODBUS->buffer[0]==SET_PAR[0])|(MODBUS->buffer[0]==255)))
 {
	  tmp=Crc16(MODBUS->buffer,MODBUS->rxcnt-2);

     if((MODBUS->buffer[MODBUS->rxcnt-2]==(tmp&0x00FF)) & (MODBUS->buffer[MODBUS->rxcnt-1]==(tmp>>8)))
    {
     
	 
         //choosing function
	     switch(MODBUS->buffer[1])
       {
       case 3:
       TX_03_04(MODBUS);
       break;

       case 4:
       TX_03_04(MODBUS);
       break;

       case 6:
       TX_06(MODBUS);
       break;

       default:
       //illegal operation
       TX_EXCEPTION(MODBUS,0x01);
	   }

	     //adding CRC16 to reply
   	     tmp=Crc16(MODBUS->buffer,MODBUS->txlen-2);
         MODBUS->buffer[MODBUS->txlen-2]=tmp;
         MODBUS->buffer[MODBUS->txlen-1]=tmp>>8;
         MODBUS->txcnt=0;

	}
 
 }

	  MODBUS->rxgap=0;
	  MODBUS->rxcnt=0;
	  MODBUS->rxtimer=0xFFFF;

}

//******************************************************************
//READING input & holding registers
//*******************************************************************
void TX_03_04(UART_DATA *MODBUS)
{
unsigned int tmp,tmp1;
unsigned int m=0,n=0;
int tmp_val,tmp_val_pos;
  
   //MODBUS->buffer[0] =SET_PAR[0]; // adress - stays a same as in received
   //MODBUS->buffer[1] = 3; //query type - - stay a same as in recived
   //MODBUS->buffer[2] = data byte count

    //2-3  - starting address
   tmp=((MODBUS->buffer[2]<<8)+MODBUS->buffer[3]); 
   
   //4-5 - number of registers
   tmp1=((MODBUS->buffer[4]<<8)+MODBUS->buffer[5]);

   //default answer length if error
   n=3; 

  if((((tmp+tmp1)<OBJ_SZ)&(tmp1<MODBUS_WRD_SZ+1)))
   {

    for(m=0;m<tmp1;m++)
    {
      tmp_val=res_table.regs[m+tmp];

    if(tmp_val<0)
    {
    tmp_val_pos=tmp_val;
			//MODBUS->buffer[n]=(tmp_val_pos>>8)|0b10000000;
			MODBUS->buffer[n]=tmp_val_pos;
    MODBUS->buffer[n+1]=tmp_val_pos>>8;
    }
    else
    {
     MODBUS->buffer[n]=tmp_val>>8;
     MODBUS->buffer[n+1]=tmp_val;
    }
     n=n+2;
    }

     MODBUS->buffer[2]=m*2; //byte count
     MODBUS->txlen=m*2+5; //responce length
    
   }
  else 
  {
   //exception illegal data adress 0x02
   TX_EXCEPTION(MODBUS,0x02);
  }
 
}
//*******************************************************
//Writing
//*******************************************************
void TX_06(UART_DATA *MODBUS)
{
  unsigned int tmp;


   //MODBUS[0] =SET_PAR[0]; // adress - stays a same as in recived
   //MODBUS[1] = 6; //query type - - stay a same as in recived
  
   //2-3  - adress   , 4-5 - value
   
   tmp=((MODBUS->buffer[2]<<8)+MODBUS->buffer[3]); //adress

   //MODBUS->buffer[2]  - byte count a same as in rx query

   if(tmp<OBJ_SZ)
   {
    MODBUS->txlen=MODBUS->rxcnt; //responce length
    res_table.regs[tmp]=(MODBUS->buffer[4]<<8)+MODBUS->buffer[5];

   }
   else
   {
    //illegal data
    TX_EXCEPTION(MODBUS,0x02) ;
   }

}

//********************************************************************
//Exception if wrong query
//*********************************************************************

//modbus exception - illegal data=01 ,adress=02 etc 
void TX_EXCEPTION(UART_DATA *MODBUS,unsigned char error_type)
{
 //illegal operation	 
  MODBUS->buffer[2]=error_type; //exception
  MODBUS->txlen=5; //responce length  
}

//*********************************************************************
//CRC16 for Modbus Calculation
//*********************************************************************
unsigned int Crc16(unsigned char *ptrByte, int byte_cnt)
{
unsigned int w=0;
char shift_cnt;

 if(ptrByte)
 {
   w=0xffffU;
   for(; byte_cnt>0; byte_cnt--)
   {
    w=(unsigned int)((w/256U)*256U+((w%256U)^(*ptrByte++)));
    for(shift_cnt=0; shift_cnt<8; shift_cnt++)
    {
     if((w&0x1)==1)
     w=(unsigned int)((w>>1)^0xa001U);
     else
     w>>=1;
    }
   }
 }
return w;
}


