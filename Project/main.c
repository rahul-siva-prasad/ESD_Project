#include "main.h"

void TIM2_Initialize (void)
{

	const uint16_t PSC_val =  8400; //prescalar value
  const uint16_t ARR_val= 	10000; //ARR value 1sec
	RCC->APB1ENR|=(1UL<<0);
  TIM2->PSC=PSC_val-1;
	TIM2->ARR=ARR_val-1;//
	TIM2->DIER=(1UL<<0);
	TIM2->CR1=(1UL<<0);//set command register 
	TIM2->CCMR1|=(3UL<<12);//OCIREF toggles which serves as trigger for ADC
  TIM2->CCER|=(1UL<<4);//CC2E set// refer programmers manual
	NVIC_EnableIRQ(TIM2_IRQn);
}

void TIM2_IRQHandler(void)
{
	TIM2->SR &= ~(1u<<0);
	uint8_t temp[4];  // Temporary buffer for read values
	for(int i=0;i<100;i++)//100samples per second, just read one sample
		OXY_Read(MAX30100, MAX30100_FIFO_DATA, &temp[0], 4);  // Read four times from the FIFO
  IR = (temp[0]<<8) | temp[1];    // Combine values to get the actual number
  RED = (temp[2]<<8) | temp[3];   // Combine values to get the actual number
}
void ADC_Initialize(void) 
{
	RCC->APB2ENR|=(1UL<<10); //Enable ADC3 clock     
	RCC->AHB1ENR|=(1UL<<2);//Enable GPIOC Clock     
	GPIOC->MODER|=(3UL<<2*1);//PC1 in Analog mode     
	ADC3->SQR1=0;     
	ADC3->SQR2=0; 	
	ADC3->SQR3=(11UL<<0); //SQ1 channel selection     
	
	ADC3->SMPR1=   0;       
	ADC3->SMPR2= (7UL<<3*5);//time=480 cycles         
	ADC3->CR2 &=~(2U); //Single conversion mode  
  ADC3->CR2|=3UL<<28;	//RISING AND FALLING EDGE DETECTION
	ADC3->CR2|=3UL<<24;	//Timer 2 - CC2
	ADC3->CR1|=(1UL<<5);//Enable EOC IRQ     
	ADC3->CR2|=(1UL<<0);// ADC Enable     
	NVIC_EnableIRQ(ADC_IRQn);   
	
}
void ADC_IRQHandler (void)
{
	ADC3->SR = ~(2U); //Clear EOC Flag  
	adcValue+=(ADC3->DR); //read ADC value 
	
	if ((TIM2->SR &0x0001)!=0)
		TIM2->SR &=~(1U<<0);
	if(count_adc>9)
	{
		resistance = adcValue/10;
		adcValue=0;
		count_adc=0;
	}
	count_adc++;
}	


void OXY_Write(uint8_t Address,uint8_t Reg,uint8_t Data)
{
	I2C_Start();
	I2C_Address(Address);
	I2C_Write(Reg);
	I2C_Write(Data);
	I2C_Stop();
}
void OXY_Read(uint8_t Address,uint8_t Reg,uint8_t *buffer,uint8_t size)
{
	I2C_Start();
	I2C_Address(Address);
	I2C_Write(Reg);
	I2C_Start();
	I2C_Read(Address+0x01,buffer,size);
	I2C_Stop();
}
void OXY_Init(void)
{
	uint8_t dev_info[2];
	OXY_Read(MAX30100,MAX30100_REV_ID,&dev_info[0],2);//REV ID + PART ID
	OXY_Write(MAX30100,MAX30100_MODE_CONFIG,0x03);
	OXY_Write(MAX30100,MAX30100_LED_CONFIG,i50);//50mA current draw
	OXY_Write(MAX30100,MAX30100_SPO2_CONFIG,(sr100<<2)|pw1600);//100samples per second 1600PW
}
int main(void)
{
	
	SysClockConfig();
	I2C_Config();
	OXY_Init();
	TIM2_Initialize();
	ADC_Initialize();
	UART_config();
	while(1)
	{
		UART2_SendChar(0x31);
		uint32_t delay=1000000;
		while(delay)
			delay--;
	}
}
