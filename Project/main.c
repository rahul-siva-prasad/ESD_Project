#include "main.h"
void user_button_config(void)
{
	RCC->AHB1ENR |=RCC_AHB1ENR_GPIOAEN;
	GPIOA->MODER &= ~GPIO_MODER_MODE0;
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD0;
	EXTI->IMR |= EXTI_IMR_MR0;
	EXTI->FTSR |= EXTI_FTSR_TR0;
	
	NVIC_SetPriority(EXTI0_IRQn,1);
	NVIC_EnableIRQ(EXTI0_IRQn);
}
void OXY_Int_pin_EN(void)
{
	GPIOA->PUPDR |= (1U<<2);
	EXTI->IMR |= EXTI_IMR_MR1;
	EXTI->FTSR |= EXTI_FTSR_TR1;
	NVIC_SetPriority(EXTI1_IRQn,3);
	NVIC_EnableIRQ(EXTI1_IRQn);
}
void EXTI0_IRQHandler(void)
{
	if((EXTI->PR & EXTI_PR_PR0)&&(count1>1))
	{
		count1=0;
		EXTI->PR |= EXTI_PR_PR0;
		push_button++;
	}
	else if(EXTI->PR & EXTI_PR_PR0)
	{
		EXTI->PR |= EXTI_PR_PR0;
	}
}
void EXTI1_IRQHandler(void)
{
	uint8_t check;
	OXY_Read(MAX30100,MAX30100_INT_STATUS,&check,1);
	if(EXTI->PR & EXTI_PR_PR1)
	{
		EXTI->PR |= EXTI_PR_PR1;
		if(count>20)
		{
			uint8_t temp[4]={0};  // Temporary buffer for read values
			OXY_Read(MAX30100, MAX30100_FIFO_DATA,temp, 4);  // Read four times from the FIFO
			IR = (float)((temp[0]<<8) | temp[1]);    // Combine values to get the actual number
			RED = (float)((temp[2]<<8) | temp[3]);   // Combine values to get the actual number
			count=0;
			if(RED+IR > 5000)
			{
				spo2 = (110- 25*(RED/IR));
				int temp = (int)(spo2*100);
				spo2 = (float)temp;
				spo2/=100;
			}
			else
				spo2=0;
			OXY_FIFO_reset();
		}
	}
}
void TIM2_Initialize (void)
{
	const uint16_t PSC_val =  8400; //prescalar value
  	const uint16_t ARR_val= 	1000; //ARR value 1sec
	RCC->APB1ENR|=(1UL<<0);
  	TIM2->PSC=PSC_val-1;
	TIM2->ARR=ARR_val-1;//
	TIM2->DIER=(1UL<<0);
	TIM2->CR1=(1UL<<0);//set command register 
	TIM2->CCMR1|=(3UL<<12);//OCIREF toggles which serves as trigger for ADC
  	TIM2->CCER|=(1UL<<4);//CC2E set// refer programmers manual
	NVIC_SetPriority(TIM2_IRQn,2);
	NVIC_EnableIRQ(TIM2_IRQn);
}

void TIM2_IRQHandler(void)
{
	TIM2->SR &= ~(1u<<0);
	togg=~togg;
	count++;
	count1++;
	count2++;
}
void delay(uint8_t sec)
{
	count=0;
	while(count< sec);
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
	ADC3->CR1 |=(1UL<<24); //10bit ADC
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

int main(void)
{
	SysClockConfig();
	TIM2_Initialize();
	I2C_Config();
	OXY_Init();
	ADC_Initialize();
	UART_config();
	user_button_config();
	OXY_Interrupt();
	OXY_Int_pin_EN();
	while(1)
	{
		if(count2>10)
		{
			count2=0;
			UART2_SendChar(0x31);
		}
	}
}
