#include"I2C_Config.h"
void I2C_Config(void)
{
	RCC->APB1ENR |=(1U<<21); //I2C1 CLK
	RCC->AHB1ENR |=(1U<<1);  //GPIOB Enable
	
	//Configure the I2C pins for Alternate functions
	GPIOB->MODER  |=(2<<16)|(2<<18); //Pin B8, B9
	GPIOB->OTYPER |=(0<<8) |(0<<9);
	GPIOB->OSPEEDR|=(3<<16)|(3<<18);
	GPIOB->PUPDR  |=GPIO_PUPDR_PUPDR8|GPIO_PUPDR_PUPDR9;
	GPIOB->AFR[1] |=(4<<0) |(4<<4);
	
	
	//Reset the I2C
	I2C1->CR1 |= (1<<15);
	I2C1->CR1 &=~(1U<<15);
	
	//Program the peripheral input clock
	I2C1->CR2 |=(42<<0); //PCL Freq in MHz
	//Configure the clock control registers
	I2C1->CCR = (210<<0);//CHECK***
	//Configure the rise time
	I2C1->TRISE = 10;//CHECK***12
	I2C1->CR1 |=(1<<0);
}


void I2C_Start(void)
{
	I2C1->CR1 |= (1<<10);
	I2C1->CR1 |=(1<<8);
	while(!(I2C1->SR1&(1<<0))); //Wait for SB bit to set
}
void I2C_Write(uint8_t data)
{
	while(!(I2C1->SR1 &(1<<7)));//wait for TX bit to set
	I2C1->DR= data;
	while(!(I2C1->SR1 &(1<<2)));
}
void I2C_Address(uint8_t Address)
{
	I2C1->DR = Address; //send the address
	while(!(I2C1->SR1 &(1<<1))); //wait for ADDR bit to set
	uint8_t temp = I2C1->SR1 | I2C1->SR2; //read SR1 and SR2 to clear ADDR bit
}
void I2C_Stop(void)
{
	I2C1->CR1 |=(1<<9); //Stop I2C
}
void I2C_WriteMulti(uint8_t *data,uint8_t size)
{
	while(!(I2C1->SR1 & (1<<7))); //wait for TXE bit to set
	while(size)
	{
		while(!(I2C1->SR1 & (1<<7))); //wait for TXE bit to set
		I2C1->DR = (volatile uint32_t)*data++;
		size--;
	}
	while(!(I2C1->SR1 &(1<<2))); //wait for BTF to set
}

void I2C_Read(uint8_t Address,uint8_t *buffer,uint8_t size)
{
	int remaining = size;
	if(size ==1)
	{
		I2C1->DR = Address; //Send the address
		while(!(I2C1->SR1&(1<<1))); //Wait for ADDR bit to set
		
		I2C1->CR1 &= ~(1U<<10); //clear ACK
		uint8_t temp = I2C1->SR1|I2C1->SR2;
		I2C1->SR1 |= (1<<9);//Stop I2C
		
		while(!(I2C1->SR1&(1<<6)));//wait for RxNE to set
		buffer[size-remaining] = I2C1->DR;
	}
	else
	{
		I2C1->DR = Address; //Send the address
		while(!(I2C1->SR1&(1<<1))); //Wait for ADDR bit to set
		uint8_t temp = I2C1->SR1|I2C1->SR2;
		while(remaining>2)
		{
			while(!(I2C1->SR1&(1<<6)));//wait for RxNE to set
			buffer[size-remaining] = I2C1->DR;
			I2C1->CR1 |=1<<10;
			remaining--;
		}
		while(!(I2C1->SR1 &(1<<6)));
		buffer[size-remaining] = I2C1->DR;
		I2C1->CR1 &= ~(1U<<10);
		I2C1->CR1 |= (1<<9);
		remaining--;
		while(!(I2C1->SR1 & (1<<6)));
		buffer[size-remaining] = I2C1->DR;
	}
}
