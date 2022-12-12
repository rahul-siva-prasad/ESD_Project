#include"Oximeter.h"
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
	OXY_Read(MAX30100,MAX30100_REV_ID,dev_info,2);//REV ID + PART ID
	
	OXY_Write(MAX30100,MAX30100_MODE_CONFIG,0x03);
	OXY_Read(MAX30100,MAX30100_MODE_CONFIG,&mode_config,1);
	
	OXY_Write(MAX30100,MAX30100_LED_CONFIG,(i21<<4)|i21);//50mA current draw(0x08<<4)|i50
	OXY_Read(MAX30100,MAX30100_LED_CONFIG,&led_config,1);
	
	OXY_Write(MAX30100,MAX30100_SPO2_CONFIG,(sr100<<2)|pw1600);//100samples per second 1600PW
	OXY_Read(MAX30100,MAX30100_SPO2_CONFIG,&spo2_config,1);
	OXY_FIFO_reset();
}
void OXY_FIFO_reset(void)
{
	OXY_Write(MAX30100,MAX30100_FIFO_WR_PTR,0);
	OXY_Write(MAX30100,MAX30100_FIFO_RD_PTR,0);
	OXY_Write(MAX30100,MAX30100_OVRFLOW_CTR,0);
}
void OXY_Interrupt(void)
{
	uint8_t check;
	OXY_Write(MAX30100,MAX30100_INT_ENABLE,(1<<7));
	OXY_Read(MAX30100,MAX30100_INT_STATUS,&check,1);
}
