#include "stm32f407xx.h"
#include "SysClockConfig.h"
#include "UART_Config.h"
#include "I2C_Config.h"
#include "Oximeter.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>


void TIM2_Initialize(void);
void TIM2_IRQHandler(void);

void OXY_Int_pin_EN(void);
void EXTI1_IRQHandler(void);

void ADC_Initialize(void);
void ADC_IRQHandler (void);
void EXTI0_IRQHandler(void);



void user_button_config(void);

void delay(uint8_t sec);

static uint8_t count;
static uint8_t count1;
static uint8_t count2;
static uint8_t togg;

static uint32_t adcValue;
static uint32_t resistance;
static uint8_t count_adc;
static float spo2;
static uint16_t push_button;
static float IR = 0; 
static float RED = 0;
