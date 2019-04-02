#include "stm32f4xx_hal.h"
#include "i2c.h"

I2C_HandleTypeDef I2cHandle;

void I2C_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;

	if(HAL_I2C_GetState(&I2cHandle)==HAL_I2C_STATE_RESET)
	{
		__GPIOC_CLK_ENABLE();
		__GPIOA_CLK_ENABLE();

		GPIO_InitStruct.Pin=GPIO_PIN_8; // PA8
		GPIO_InitStruct.Mode=GPIO_MODE_AF_OD;
		GPIO_InitStruct.Pull=GPIO_NOPULL;
		GPIO_InitStruct.Speed=GPIO_SPEED_FAST;
		GPIO_InitStruct.Alternate=GPIO_AF4_I2C3;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		GPIO_InitStruct.Pin=GPIO_PIN_9; // PC9
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

		__I2C3_CLK_ENABLE();
		__I2C3_FORCE_RESET();
		__I2C3_RELEASE_RESET();

		HAL_NVIC_SetPriority(I2C3_EV_IRQn, 0x00, 0);
		HAL_NVIC_EnableIRQ(I2C3_EV_IRQn);

		HAL_NVIC_SetPriority(I2C3_ER_IRQn, 0x00, 0);
		HAL_NVIC_EnableIRQ(I2C3_ER_IRQn);

		I2cHandle.Instance=I2C3;
		I2cHandle.Init.ClockSpeed=100000;
		I2cHandle.Init.DutyCycle=I2C_DUTYCYCLE_2;
		I2cHandle.Init.OwnAddress1=0;
		I2cHandle.Init.AddressingMode=I2C_ADDRESSINGMODE_7BIT;
		I2cHandle.Init.DualAddressMode=I2C_DUALADDRESS_DISABLED;
		I2cHandle.Init.OwnAddress2=0;
		I2cHandle.Init.GeneralCallMode=I2C_GENERALCALL_DISABLED;
		I2cHandle.Init.NoStretchMode=I2C_NOSTRETCH_DISABLED;
		HAL_I2C_Init(&I2cHandle);
	}
}

void I2C_WriteData(uint8_t Addr, uint8_t Reg, uint8_t Value)
{
	if(HAL_I2C_Mem_Write(&I2cHandle, Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT, &Value, 1, 1000)!=HAL_OK)
	{
		HAL_I2C_DeInit(&I2cHandle);
		I2C_Init();
	}
}

void I2C_WriteBuffer(uint8_t Addr, uint8_t Reg,  uint8_t *pBuffer, uint16_t Length)
{
	if(HAL_I2C_Mem_Write(&I2cHandle, Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT, pBuffer, Length, 1000)!=HAL_OK)
	{
		HAL_I2C_DeInit(&I2cHandle);
		I2C_Init();
	}
}

uint8_t I2C_ReadData(uint8_t Addr, uint8_t Reg)
{
	uint8_t value=0;

	if(HAL_I2C_Mem_Read(&I2cHandle, Addr, Reg, I2C_MEMADD_SIZE_8BIT, &value, 1, 1000)!=HAL_OK)
	{
		HAL_I2C_DeInit(&I2cHandle);
		I2C_Init();
	}

	return value;
}

uint8_t I2C_ReadBuffer(uint8_t Addr, uint8_t Reg, uint8_t *pBuffer, uint16_t Length)
{
	if(HAL_I2C_Mem_Read(&I2cHandle, Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT, pBuffer, Length, 1000)==HAL_OK)
		return 0;
	else
	{
		HAL_I2C_DeInit(&I2cHandle);
		I2C_Init();
	}

	return 1;
}

void I2C_ITConfig(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;

	__GPIOA_CLK_ENABLE();

	GPIO_InitStruct.Pin=GPIO_PIN_15;
	GPIO_InitStruct.Pull=GPIO_PULLUP;
	GPIO_InitStruct.Speed=GPIO_SPEED_LOW;
	GPIO_InitStruct.Mode=GPIO_MODE_IT_FALLING;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0x00, 0x00);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}
