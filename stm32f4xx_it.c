#include "stm32f4xx_hal.h"
#include "stm32f4xx_it.h"

extern CAN_HandleTypeDef CanHandle;

void SysTick_Handler(void)
{
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
}

void CAN1_RX0_IRQHandler(void)
{
	HAL_CAN_IRQHandler(&CanHandle);
}

void CAN1_RX1_IRQHandler(void)
{
	HAL_CAN_IRQHandler(&CanHandle);
}

void CAN1_TX_IRQHandler(void)
{
	HAL_CAN_IRQHandler(&CanHandle);
}
