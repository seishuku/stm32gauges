#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "stm32f4xx_hal.h"
#include "lcd.h"

#ifndef clamp
#define clamp(a, min, max) (((a)<(min))?(min):(((a)>(max))?(max):(a)))
#endif

#ifndef min
#define min(a, b) ((a)<(b)?(a):(b))
#endif

#ifndef max
#define max(a, b) ((a)>(b)?(a):(b))
#endif

#ifndef deg2rad
#define deg2rad(x) ((x)*3.1415926f/180.0f)
#endif

#ifndef rad2deg
#define rad2deg(x) ((x)*180.0f/3.1415926f)
#endif

#define ARGB(a, r, g, b) (((a)&0xFF)<<24)|(((r)&0xFF)<<16)|(((g)&0xFF)<<8)|((b)&0xFF)

volatile uint32_t *DWT_CYCCNT=(volatile uint32_t *)0xE0001004;
volatile uint32_t *DWT_CONTROL=(volatile uint32_t *)0xE0001000;
volatile uint32_t *SCB_DEMCR=(volatile uint32_t *)0xE000EDFC;

CAN_HandleTypeDef CanHandle;
CanTxMsgTypeDef CanTxMessage;
CanRxMsgTypeDef CanRxMessage;

float RPM=0.0f, prevRPM=0.0f;
float CLT=0.0f, prevCLT=0.0f;
float MPH=0.0f, prevMPH=0.0f;
float FuelLevel=0.0f;

void SystemClock_Config(void);
void LED_Init(void);
void LED_GreenOn(void);
void LED_GreenOff(void);
void LED_GreenToggle(void);
void LED_RedOn(void);
void LED_RedOff(void);
void LED_RedToggle(void);
void Button_Init(void);
uint32_t Button_GetState(void);

uint32_t TimingGetMs(void)
{
	return (*DWT_CYCCNT)/9600000;
}

float fsinf(float v)
{
	float fx=v*0.1591549f+0.5f;
	float ix=fx-(float)floor(fx);
	float x=ix*6.2831852f-3.1415926f;
	float x2=x*x;
	float x3=x2*x;

	return x-x3/6.0f
			+x3*x2/120.0f
			-x3*x2*x2/5040.0f
			+x3*x2*x2*x2/362880.0f
			-x3*x2*x2*x2*x2/39916800.0f
			+x3*x2*x2*x2*x2*x2/6227020800.0f
			-x3*x2*x2*x2*x2*x2*x2/1307674279936.0f
			+x3*x2*x2*x2*x2*x2*x2*x2/355687414628352.0f;
}

float fcosf(float v)
{
	float fx=v*0.1591549f+0.5f;
	float ix=fx-(float)floor(fx);
	float x=ix*6.2831852f-3.1415926f;
	float x2=x*x;
	float x4=x2*x2;

	return 1-x2/2.0f
			+x4/24.0f
			-x4*x2/720.0f
			+x4*x4/40320.0f
			-x4*x4*x2/3628800.0f
			+x4*x4*x4/479001600.0f
			-x4*x4*x4*x2/87178289152.0f
			+x4*x4*x4*x4/20922788478976.0f;
}

//This uses sin/cos too much, it's a bit faster using taylor series, but could be better.
//Ugh, radians.
//This uses sin/cos too much, it's a bit faster using taylor series, but could be better.
void DrawGauge(int x, int y, int r, int min, int max, int num, float val, char *title)
{
	float i, j, temp;

	//Gauge dial border.
	LCD_DrawFillCircle(x, y, r, 0xFF505050);
	LCD_DrawCircle(x, y, r, 0xFFFF0000);
	//Draw the gauge title text.
	LCD_Printf(x-(FONT_WIDTH*strlen(title))/2, y+FONT_HEIGHT, "%s", title);
	//Draw the gauge value.
	LCD_Printf(x-FONT_WIDTH, y+FONT_HEIGHT*2, "%d", (int)val);

	if(max>min)
		val=clamp(val, min, max);
	else
		val=clamp(val, max, min);

	//Range val into min and max and rescale the value to corespond with the adjusted gauge range and offset degree.
	temp=315.0f*((val-min)/(max-min))+112.5f;

	//The gauge is 315 degree sweep, but the way it's drawn, we offset it by 112.5 degrees to bring it "upright."
	//'j' is the counter for the printed text, there is also a check for if it's over 99, to correct for 3 digit numbers.
	//Also the increment for 'i' is fudged a bit, rounding error?
	for(i=deg2rad(112.5f), j=(float)min;i<deg2rad(315.0f+112.5f+1.0f);i+=deg2rad(315.0f/num), j+=(float)(max-min)/num)
	{
		LCD_Printf(
			(int)(fcosf(i)*(r-((FONT_WIDTH*4)/3))+x)-(j>99?FONT_WIDTH*2:FONT_WIDTH),
			(int)(fsinf(i)*(r-((FONT_WIDTH*4)/3))+y)-(FONT_HEIGHT/3),
			"%d", (int)j);
	}

	//Gauge needle.
	LCD_DrawFillCircle(x, y, (int)(r*0.05f), 0xFFFF0000);
	LCD_DrawTriangle(
		(int)(fsinf(deg2rad(-temp-135.0f))*(r*0.05f))+x,
		(int)(fcosf(deg2rad(temp+135.0f))*(r*0.05f))+y,
		(int)(fcosf(deg2rad(-temp-135.0f))*(r*0.05f))+x,
		(int)(fsinf(deg2rad(temp+135.0f))*(r*0.05f))+y,
		(int)(fcosf(deg2rad(temp))*r)+x,
		(int)(fsinf(deg2rad(temp))*r)+y, 0xFFFF0000);
}

void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef *CanHandle)
{
	if(CanHandle->pRxMsg->StdId==0x359)
	{
		prevMPH=MPH;
		MPH=(((float)((CanHandle->pRxMsg->Data[2]<<8)+CanHandle->pRxMsg->Data[1])/322.0f)+prevMPH)*0.5f;
	}

	if(CanHandle->pRxMsg->StdId==0x35B)
	{
		prevRPM=RPM; // Save previous value
		RPM=(((float)((CanHandle->pRxMsg->Data[2]<<8)+CanHandle->pRxMsg->Data[1])*0.25f)+prevRPM)*0.5f; // RPM = ((HiByte*256)+LowByte)/4
		prevCLT=CLT; // Save previous value
		CLT=((((float)(CanHandle->pRxMsg->Data[3]-64)*0.75)*1.8f+32.0f)+prevCLT)*0.5f; // DegC = (Byte-64)*0.75 (<<< also converting to Deg F)
	}

	if(CanHandle->pRxMsg->StdId==0x621)
		FuelLevel=((float)(CanHandle->pRxMsg->Data[3]&0x7F)/52.0f)*100.0f;

	if(HAL_CAN_Receive_IT(CanHandle, CAN_FIFO0)!=HAL_OK)
		LED_RedOn(); //Red LED = CAN controller crashed
}

HAL_StatusTypeDef CAN_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	CAN_FilterConfTypeDef sFilterConfig;

	HAL_CAN_DeInit(&CanHandle);

	__HAL_RCC_CAN1_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitStruct.Pin=GPIO_PIN_12|GPIO_PIN_11; //TX = PA12 RX = PA11
	GPIO_InitStruct.Mode=GPIO_MODE_AF_PP;
	GPIO_InitStruct.Speed=GPIO_SPEED_FAST;
	GPIO_InitStruct.Pull=GPIO_PULLUP;
	GPIO_InitStruct.Alternate=GPIO_AF9_CAN1;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	CanHandle.Instance=CAN1;
	CanHandle.pTxMsg=&CanTxMessage;
	CanHandle.pRxMsg=&CanRxMessage;
	CanHandle.Init.TTCM=DISABLE;
	CanHandle.Init.ABOM=DISABLE;
	CanHandle.Init.AWUM=DISABLE;
	CanHandle.Init.NART=DISABLE;
	CanHandle.Init.RFLM=DISABLE;
	CanHandle.Init.TXFP=DISABLE;
	CanHandle.Init.Mode=CAN_MODE_NORMAL;
	CanHandle.Init.SJW=CAN_SJW_1TQ;
	CanHandle.Init.BS1=CAN_BS1_16TQ;
	CanHandle.Init.BS2=CAN_BS2_4TQ;
	//prescale=bus/(canbus_baud*(sjw+bs1+bs2))
//	CanHandle.Init.Prescaler=4;	//500kbps @ 42MHz 1SJW/16BS1/4BS2
	CanHandle.Init.Prescaler=20;	//100kbps @ 42MHz 1SJW/16BS1/4BS2
//	CanHandle.Init.Prescaler=12;	//100kbps @ 24MHz 1SJW/15BS1/4BS2

	if(HAL_CAN_Init(&CanHandle)!=HAL_OK)
		return HAL_ERROR;

	sFilterConfig.FilterNumber=0;
	sFilterConfig.FilterMode=CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale=CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterIdHigh=0x0000;
	sFilterConfig.FilterIdLow=0x0000;
	sFilterConfig.FilterMaskIdHigh=0x0000;
	sFilterConfig.FilterMaskIdLow=0x0000;
	sFilterConfig.FilterFIFOAssignment=0;
	sFilterConfig.FilterActivation=ENABLE;
	sFilterConfig.BankNumber=0;

	if(HAL_CAN_ConfigFilter(&CanHandle, &sFilterConfig)!=HAL_OK)
		return HAL_ERROR;

	HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);

	return HAL_OK;
}

int main(void)
{
	HAL_Init();
	SystemClock_Config();

	// Set up cycle counter for timing
	*SCB_DEMCR|=0x01000000;
	*DWT_CYCCNT=0;
	*DWT_CONTROL|=1;

	// LEDs are used for error status
	LED_Init();

	// Set up button GPIO
	Button_Init();

	// Initalize local LCD and turn it on
	LCD_Init();
	LCD_DisplayOn();

	if(CAN_Init()!=HAL_OK)
		LED_RedOn();

	// Start CANbus receiving interrupt
	if(HAL_CAN_Receive_IT(&CanHandle, CAN_FIFO0)!=HAL_OK)
		LED_RedOn();

	// Main loop
	while(1)
	{
		// Clear LCD back buffer
		LCD_Clear(1, 0xFF000000);

		DrawGauge(61,			61,		60, 0, 60, 16, RPM/100.0f, "RPM (x100)");
		DrawGauge(121+60+15,	61,		60, -40, 250, 16, CLT, "Coolant (F)");
		DrawGauge(61+60,		120+59,	60, 0, 160, 16, MPH, "MPH");
		DrawGauge(121+60+60+15,	120+59,	60, 0, 100, 16, FuelLevel, "Fuel Level (%)");

		// Copy back buffer to front buffer (double buffered)
		LCD_CopyBuffers();
	}
}

void SystemClock_Config(void)
{
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	RCC_OscInitStruct.OscillatorType=RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState=RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState=RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource=RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM=8;
	RCC_OscInitStruct.PLL.PLLN=336;	//168MHz for testing
	RCC_OscInitStruct.PLL.PLLP=RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ=7; // USB divider for 168MHz
	HAL_RCC_OscConfig(&RCC_OscInitStruct);
	HAL_PWREx_EnableOverDrive();
  
	RCC_ClkInitStruct.ClockType=RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource=RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider=RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider=RCC_HCLK_DIV4;  
	RCC_ClkInitStruct.APB2CLKDivider=RCC_HCLK_DIV2;  
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);

	PeriphClkInitStruct.PeriphClockSelection=RCC_PERIPHCLK_LTDC;
	PeriphClkInitStruct.PLLSAI.PLLSAIN=192;
	PeriphClkInitStruct.PLLSAI.PLLSAIR=4;
	PeriphClkInitStruct.PLLSAIDivR=RCC_PLLSAIDIVR_8;
	HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct); 
}

void LED_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;

	__GPIOG_CLK_ENABLE();
	GPIO_InitStruct.Pin=GPIO_PIN_13|GPIO_PIN_14;
	GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull=GPIO_PULLUP;
	GPIO_InitStruct.Speed=GPIO_SPEED_FAST;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_RESET);
}

void LED_GreenOn(void)
{
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_SET);
}

void LED_GreenOff(void)
{
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_RESET);
}

void LED_GreenToggle(void)
{
	HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_13);
}

void LED_RedOn(void)
{
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_SET);
}

void LED_RedOff(void)
{
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_RESET);
}

void LED_RedToggle(void)
{
	HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_14);
}

void Button_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	__GPIOA_CLK_ENABLE();

	GPIO_InitStruct.Pin=GPIO_PIN_0;
	GPIO_InitStruct.Mode=GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull=GPIO_PULLDOWN;
	GPIO_InitStruct.Speed=GPIO_SPEED_FAST;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

uint32_t Button_GetState(void)
{
	return HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
}
