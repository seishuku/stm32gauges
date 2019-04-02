#include "stm32f4xx_hal.h"
#include "spi.h"

SPI_HandleTypeDef SpiHandle;

void SPI_Init(void)
{
	GPIO_InitTypeDef   GPIO_InitStructure;

	if(HAL_SPI_GetState(&SpiHandle)==HAL_SPI_STATE_RESET)
	{
		__SPI5_CLK_ENABLE();
		__GPIOF_CLK_ENABLE();

		GPIO_InitStructure.Pin=GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9; // 7=CLK, 8=MISO, 9=MOSI
		GPIO_InitStructure.Mode=GPIO_MODE_AF_PP;
		GPIO_InitStructure.Pull=GPIO_PULLDOWN;
		GPIO_InitStructure.Speed=GPIO_SPEED_LOW;
		GPIO_InitStructure.Alternate=GPIO_AF5_SPI5;
		HAL_GPIO_Init(GPIOF, &GPIO_InitStructure);

		SpiHandle.Instance=SPI5;
		SpiHandle.Init.BaudRatePrescaler=SPI_BAUDRATEPRESCALER_16;
		SpiHandle.Init.Direction=SPI_DIRECTION_2LINES;
		SpiHandle.Init.CLKPhase=SPI_PHASE_1EDGE;
		SpiHandle.Init.CLKPolarity=SPI_POLARITY_LOW;
		SpiHandle.Init.CRCCalculation=SPI_CRCCALCULATION_DISABLED;
		SpiHandle.Init.CRCPolynomial=7;
		SpiHandle.Init.DataSize=SPI_DATASIZE_8BIT;
		SpiHandle.Init.FirstBit=SPI_FIRSTBIT_MSB;
		SpiHandle.Init.NSS=SPI_NSS_SOFT;
		SpiHandle.Init.TIMode=SPI_TIMODE_DISABLED;
		SpiHandle.Init.Mode=SPI_MODE_MASTER;
		HAL_SPI_Init(&SpiHandle);
	}
}

uint8_t SPI_Read(void)
{
	uint8_t read=0;

	if(HAL_SPI_Receive(&SpiHandle, &read, 1, 1000)!=HAL_OK)
	{
		HAL_SPI_DeInit(&SpiHandle);
		SPI_Init();
	}

	return read;
}

void SPI_Write(uint8_t write)
{
	if(HAL_SPI_Transmit(&SpiHandle, &write, 1, 1000)!=HAL_OK)
	{
		HAL_SPI_DeInit(&SpiHandle);
		SPI_Init();
	}
}

uint8_t SPI_WriteRead(uint8_t write)
{
	uint8_t read=0;

	if(HAL_SPI_TransmitReceive(&SpiHandle, &write, &read, 1, 1000)!=HAL_OK)
	{
		HAL_SPI_DeInit(&SpiHandle);
		SPI_Init();
	}

	return read;
}
