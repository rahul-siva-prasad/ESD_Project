#include"UART_Config.h"
void UART_config(void)
{
	//Enable UART clock and GPIO clock
	RCC->APB1ENR |= (1<<17); //UART2 CLOCK
	RCC->AHB1ENR |= (1<<0);  //GPIOA CLOCK
	
	//GPIOA Alternate function
	GPIOA->MODER &= 0XFFFFFF0F;
	GPIOA->MODER |= (2<<2*2); //Alternate Function PA2 USART2 Tx
	GPIOA->MODER |= (2<<2*3);	//Alternate Function PA3 USART2 Rx

	//GPIOA->OSPEEDR |= (3<<4)|(3<<6); //High Speed for Pins PA2 and PA3
	GPIOA->AFR[0] |= (7<<2*4);  //AF7 USART2 at PA2
	GPIOA->AFR[0] |= (7<<3*4);  //AF7 USART2 at PA3
	
	//Select the 9600 baud rate 42Mhz
	USART2->BRR = 0x1117;
	
	//Enable UART2
	USART2->CR1 = 0x00; //Clear all
	USART2->CR1=0X200C;
}

void UART2_SendChar(uint8_t c)
{
	while (!(USART2->SR& 0X0080));//while(!(USART2->SR & (1<<6)));
	USART2->SR&= 0XFFBF;
	USART2->DR =(c&0xFF);
}
void UART2_SendString(char *string)
{
	while(*string){
		UART2_SendChar(*string);
		string+=1;
	}
}
