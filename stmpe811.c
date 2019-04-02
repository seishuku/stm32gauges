#include "stm32f4xx_hal.h"
#include "stmpe811.h"
#include "../i2c.h"

#define STMPE811_MAX_INSTANCE 2 

static uint16_t          TsXBoundary, TsYBoundary; 
uint8_t stmpe811[STMPE811_MAX_INSTANCE] = {0};

static uint8_t stmpe811_GetInstance(uint16_t DeviceAddr); 

void stmpe811_Init(uint16_t DeviceAddr)
{
	uint8_t instance, empty;

	instance=stmpe811_GetInstance(DeviceAddr);

	if(instance==0xFF)
	{
		empty=stmpe811_GetInstance(0);

		if(empty<STMPE811_MAX_INSTANCE)
		{
			stmpe811[empty]=DeviceAddr;

			I2C_Init();

			stmpe811_Reset(DeviceAddr);
		}
	}
}

void stmpe811_Reset(uint16_t DeviceAddr)
{
	I2C_WriteData(DeviceAddr, STMPE811_REG_SYS_CTRL1, 2);
	HAL_Delay(10);
	I2C_WriteData(DeviceAddr, STMPE811_REG_SYS_CTRL1, 0);
	HAL_Delay(2);
}

uint16_t stmpe811_ReadID(uint16_t DeviceAddr)
{
	I2C_Init();

	return ((I2C_ReadData(DeviceAddr, STMPE811_REG_CHP_ID_LSB)<<8)|(I2C_ReadData(DeviceAddr, STMPE811_REG_CHP_ID_MSB)));
}

void stmpe811_EnableGlobalIT(uint16_t DeviceAddr)
{
	uint8_t tmp=I2C_ReadData(DeviceAddr, STMPE811_REG_INT_CTRL);

	I2C_WriteData(DeviceAddr, STMPE811_REG_INT_CTRL, tmp|STMPE811_GIT_EN);
}

void stmpe811_DisableGlobalIT(uint16_t DeviceAddr)
{
	uint8_t tmp=I2C_ReadData(DeviceAddr, STMPE811_REG_INT_CTRL);

	I2C_WriteData(DeviceAddr, STMPE811_REG_INT_CTRL, tmp&~STMPE811_GIT_EN);
}

void stmpe811_EnableITSource(uint16_t DeviceAddr, uint8_t Source)
{
	uint8_t tmp=I2C_ReadData(DeviceAddr, STMPE811_REG_INT_EN);

	I2C_WriteData(DeviceAddr, STMPE811_REG_INT_EN, tmp|Source);
}

void stmpe811_DisableITSource(uint16_t DeviceAddr, uint8_t Source)
{
	uint8_t tmp=I2C_ReadData(DeviceAddr, STMPE811_REG_INT_EN);

	I2C_WriteData(DeviceAddr, STMPE811_REG_INT_EN, tmp&~Source);
}

void stmpe811_SetITPolarity(uint16_t DeviceAddr, uint8_t Polarity)
{
	uint8_t tmp=I2C_ReadData(DeviceAddr, STMPE811_REG_INT_CTRL);

	I2C_WriteData(DeviceAddr, STMPE811_REG_INT_CTRL, (tmp&~0x04)|Polarity);
}

void stmpe811_SetITType(uint16_t DeviceAddr, uint8_t Type)
{
	uint8_t tmp=I2C_ReadData(DeviceAddr, STMPE811_REG_INT_CTRL);

	I2C_WriteData(DeviceAddr, STMPE811_REG_INT_CTRL, (tmp&~0x02)|Type);
}

uint8_t stmpe811_GlobalITStatus(uint16_t DeviceAddr, uint8_t Source)
{
	return ((I2C_ReadData(DeviceAddr, STMPE811_REG_INT_STA)&Source)==Source);
}

uint8_t stmpe811_ReadGITStatus(uint16_t DeviceAddr, uint8_t Source)
{
	return (I2C_ReadData(DeviceAddr, STMPE811_REG_INT_STA)&Source);
}

void stmpe811_ClearGlobalIT(uint16_t DeviceAddr, uint8_t Source)
{
	I2C_WriteData(DeviceAddr, STMPE811_REG_INT_STA, Source);
}

void stmpe811_IO_Start(uint16_t DeviceAddr, uint16_t IO_Pin)
{
	uint8_t mode=I2C_ReadData(DeviceAddr, STMPE811_REG_SYS_CTRL2);

	I2C_WriteData(DeviceAddr, STMPE811_REG_SYS_CTRL2, mode&~(STMPE811_IO_FCT|STMPE811_ADC_FCT));
	stmpe811_IO_DisableAF(DeviceAddr, (uint8_t)IO_Pin);
}

void stmpe811_IO_Config(uint16_t DeviceAddr, uint16_t IO_Pin, IO_ModeTypedef IO_Mode)
{
	switch(IO_Mode)
	{
		case IO_MODE_INPUT:
			stmpe811_IO_InitPin(DeviceAddr, IO_Pin, STMPE811_DIRECTION_IN);
			break;

		case IO_MODE_OUTPUT:
			stmpe811_IO_InitPin(DeviceAddr, IO_Pin, STMPE811_DIRECTION_OUT);
			break;

		case IO_MODE_IT_RISING_EDGE:
			stmpe811_IO_EnableIT(DeviceAddr);
			stmpe811_IO_EnablePinIT(DeviceAddr, IO_Pin);
			stmpe811_IO_InitPin(DeviceAddr, IO_Pin, STMPE811_DIRECTION_IN);
			stmpe811_SetITType(DeviceAddr, STMPE811_TYPE_EDGE);
			stmpe811_IO_SetEdgeMode(DeviceAddr, IO_Pin, STMPE811_EDGE_RISING);
			break;

		case IO_MODE_IT_FALLING_EDGE:
			stmpe811_IO_EnableIT(DeviceAddr);
			stmpe811_IO_EnablePinIT(DeviceAddr, IO_Pin);
			stmpe811_IO_InitPin(DeviceAddr, IO_Pin, STMPE811_DIRECTION_IN);
			stmpe811_SetITType(DeviceAddr, STMPE811_TYPE_EDGE);
			stmpe811_IO_SetEdgeMode(DeviceAddr, IO_Pin, STMPE811_EDGE_FALLING);
			break;

		case IO_MODE_IT_LOW_LEVEL:
			stmpe811_IO_EnableIT(DeviceAddr);
			stmpe811_IO_EnablePinIT(DeviceAddr, IO_Pin);
			stmpe811_IO_InitPin(DeviceAddr, IO_Pin, STMPE811_DIRECTION_IN);
			stmpe811_SetITType(DeviceAddr, STMPE811_TYPE_LEVEL);
			stmpe811_SetITPolarity(DeviceAddr, STMPE811_POLARITY_LOW);
			break;

		case IO_MODE_IT_HIGH_LEVEL:
			stmpe811_IO_EnableIT(DeviceAddr);
			stmpe811_IO_EnablePinIT(DeviceAddr, IO_Pin);
			stmpe811_IO_InitPin(DeviceAddr, IO_Pin, STMPE811_DIRECTION_IN);
			stmpe811_SetITType(DeviceAddr, STMPE811_TYPE_LEVEL);
			stmpe811_SetITPolarity(DeviceAddr, STMPE811_POLARITY_HIGH);
			break;
	}
}

void stmpe811_IO_InitPin(uint16_t DeviceAddr, uint16_t IO_Pin, uint8_t Direction)
{
	uint8_t tmp=I2C_ReadData(DeviceAddr, STMPE811_REG_IO_DIR);

	if(Direction!=STMPE811_DIRECTION_IN)
		tmp|=(uint8_t)IO_Pin;
	else
		tmp&=~(uint8_t)IO_Pin;

	I2C_WriteData(DeviceAddr, STMPE811_REG_IO_DIR, tmp);
}

void stmpe811_IO_DisableAF(uint16_t DeviceAddr, uint16_t IO_Pin)
{
	uint8_t tmp=I2C_ReadData(DeviceAddr, STMPE811_REG_IO_AF);

	I2C_WriteData(DeviceAddr, STMPE811_REG_IO_AF, tmp|IO_Pin);
}

void stmpe811_IO_EnableAF(uint16_t DeviceAddr, uint16_t IO_Pin)
{
	uint8_t tmp=I2C_ReadData(DeviceAddr, STMPE811_REG_IO_AF);

	I2C_WriteData(DeviceAddr, STMPE811_REG_IO_AF, tmp&~IO_Pin);
}

void stmpe811_IO_SetEdgeMode(uint16_t DeviceAddr, uint16_t IO_Pin, uint8_t Edge)
{
	uint8_t tmp1=I2C_ReadData(DeviceAddr, STMPE811_REG_IO_FE);
	uint8_t tmp2=I2C_ReadData(DeviceAddr, STMPE811_REG_IO_RE);

	tmp1&=~(uint8_t)IO_Pin;
	tmp2&=~(uint8_t)IO_Pin;

	if(Edge&STMPE811_EDGE_FALLING)
		tmp1|=(uint8_t)IO_Pin;

	if(Edge&STMPE811_EDGE_RISING)
		tmp2|=(uint8_t)IO_Pin;


	I2C_WriteData(DeviceAddr, STMPE811_REG_IO_FE, tmp1);
	I2C_WriteData(DeviceAddr, STMPE811_REG_IO_RE, tmp2);
}

void stmpe811_IO_WritePin(uint16_t DeviceAddr, uint16_t IO_Pin, uint8_t PinState)
{
	if(PinState!=0)
		I2C_WriteData(DeviceAddr, STMPE811_REG_IO_SET_PIN, (uint8_t)IO_Pin);
	else
		I2C_WriteData(DeviceAddr, STMPE811_REG_IO_CLR_PIN, (uint8_t)IO_Pin);
}

uint16_t stmpe811_IO_ReadPin(uint16_t DeviceAddr, uint16_t IO_Pin)
{
	return (uint16_t)(I2C_ReadData(DeviceAddr, STMPE811_REG_IO_MP_STA)&(uint8_t)IO_Pin);
}

void stmpe811_IO_EnableIT(uint16_t DeviceAddr)
{
	I2C_ITConfig();
	stmpe811_EnableITSource(DeviceAddr, STMPE811_GIT_IO);
	stmpe811_EnableGlobalIT(DeviceAddr);
}

void stmpe811_IO_DisableIT(uint16_t DeviceAddr)
{
	stmpe811_DisableGlobalIT(DeviceAddr);
	stmpe811_DisableITSource(DeviceAddr, STMPE811_GIT_IO);
}

void stmpe811_IO_EnablePinIT(uint16_t DeviceAddr, uint16_t IO_Pin)
{
	uint8_t tmp=I2C_ReadData(DeviceAddr, STMPE811_REG_IO_INT_EN);

	tmp|=(uint8_t)IO_Pin;
	I2C_WriteData(DeviceAddr, STMPE811_REG_IO_INT_EN, tmp);
}

void stmpe811_IO_DisablePinIT(uint16_t DeviceAddr, uint16_t IO_Pin)
{
	uint8_t tmp=I2C_ReadData(DeviceAddr, STMPE811_REG_IO_INT_EN);

	tmp&=~(uint8_t)IO_Pin;
	I2C_WriteData(DeviceAddr, STMPE811_REG_IO_INT_EN, tmp);
}

uint8_t stmpe811_IO_ITStatus(uint16_t DeviceAddr, uint16_t IO_Pin)
{
	return I2C_ReadData(DeviceAddr, STMPE811_REG_IO_INT_STA)&(uint8_t)IO_Pin; 
}

void stmpe811_IO_ClearIT(uint16_t DeviceAddr, uint16_t IO_Pin)
{
	stmpe811_ClearGlobalIT(DeviceAddr, STMPE811_GIT_IO);
	I2C_WriteData(DeviceAddr, STMPE811_REG_IO_INT_STA, (uint8_t)IO_Pin);
	I2C_WriteData(DeviceAddr, STMPE811_REG_IO_ED, (uint8_t)IO_Pin);
	I2C_WriteData(DeviceAddr, STMPE811_REG_IO_RE, (uint8_t)IO_Pin);
	I2C_WriteData(DeviceAddr, STMPE811_REG_IO_FE, (uint8_t)IO_Pin); 
}

void stmpe811_TS_Start(uint16_t DeviceAddr)
{
	uint8_t mode=I2C_ReadData(DeviceAddr, STMPE811_REG_SYS_CTRL2);

	mode&=~(STMPE811_IO_FCT);
	I2C_WriteData(DeviceAddr, STMPE811_REG_SYS_CTRL2, mode);

	stmpe811_IO_EnableAF(DeviceAddr, STMPE811_TOUCH_IO_ALL);

	mode&=~(STMPE811_TS_FCT|STMPE811_ADC_FCT);
	I2C_WriteData(DeviceAddr, STMPE811_REG_SYS_CTRL2, mode);
	I2C_WriteData(DeviceAddr, STMPE811_REG_ADC_CTRL1, 0x49);
	HAL_Delay(2);

	I2C_WriteData(DeviceAddr, STMPE811_REG_ADC_CTRL2, 0x01);
	I2C_WriteData(DeviceAddr, STMPE811_REG_TSC_CFG, 0x9A);
	I2C_WriteData(DeviceAddr, STMPE811_REG_FIFO_TH, 0x01);
	I2C_WriteData(DeviceAddr, STMPE811_REG_FIFO_STA, 0x01);
	I2C_WriteData(DeviceAddr, STMPE811_REG_FIFO_STA, 0x00);
	I2C_WriteData(DeviceAddr, STMPE811_REG_TSC_FRACT_XYZ, 0x01);
	I2C_WriteData(DeviceAddr, STMPE811_REG_TSC_I_DRIVE, 0x01);
	I2C_WriteData(DeviceAddr, STMPE811_REG_TSC_CTRL, 0x01);
	I2C_WriteData(DeviceAddr, STMPE811_REG_INT_STA, 0xFF);
	HAL_Delay(2);
}

uint8_t stmpe811_TS_DetectTouch(uint16_t DeviceAddr)
{
	uint8_t ret=0, state=((I2C_ReadData(DeviceAddr, STMPE811_REG_TSC_CTRL)&(uint8_t)STMPE811_TS_CTRL_STATUS)==(uint8_t)0x80);

	if(state>0)
	{
		if(I2C_ReadData(DeviceAddr, STMPE811_REG_FIFO_SIZE)>0)
			ret=1;
	}
	else
	{
		I2C_WriteData(DeviceAddr, STMPE811_REG_FIFO_STA, 0x01);
		I2C_WriteData(DeviceAddr, STMPE811_REG_FIFO_STA, 0x00);
	}

	return ret;
}

void stmpe811_TS_GetXY(uint16_t DeviceAddr, uint16_t *X, uint16_t *Y)
{
	uint8_t  dataXYZ[4];
	uint32_t uldataXYZ;

	I2C_ReadBuffer(DeviceAddr, STMPE811_REG_TSC_DATA_NON_INC, dataXYZ, sizeof(dataXYZ));

	uldataXYZ=(dataXYZ[0]<<24)|(dataXYZ[1]<<16)|(dataXYZ[2]<<8)|(dataXYZ[3]<<0);
	*X=(uldataXYZ>>20)&0x00000FFF;
	*Y=(uldataXYZ>>8)&0x00000FFF;

	I2C_WriteData(DeviceAddr, STMPE811_REG_FIFO_STA, 0x01);
	I2C_WriteData(DeviceAddr, STMPE811_REG_FIFO_STA, 0x00);
}

void stmpe811_TS_EnableIT(uint16_t DeviceAddr)
{
	I2C_ITConfig();
	stmpe811_EnableITSource(DeviceAddr, STMPE811_TS_IT);
	stmpe811_EnableGlobalIT(DeviceAddr);
}

void stmpe811_TS_DisableIT(uint16_t DeviceAddr)
{
	stmpe811_DisableGlobalIT(DeviceAddr);
	stmpe811_DisableITSource(DeviceAddr, STMPE811_TS_IT);
}

uint8_t stmpe811_TS_ITStatus(uint16_t DeviceAddr)
{
	return stmpe811_ReadGITStatus(DeviceAddr, STMPE811_TS_IT);
}

void stmpe811_TS_ClearIT(uint16_t DeviceAddr)
{
	stmpe811_ClearGlobalIT(DeviceAddr, STMPE811_TS_IT);
}

static uint8_t stmpe811_GetInstance(uint16_t DeviceAddr)
{
	uint8_t idx=0;

	for(idx=0;idx<STMPE811_MAX_INSTANCE;idx++)
	{
		if(stmpe811[idx]==DeviceAddr)
			return idx;
	}

	return 0xFF;
}

uint8_t TS_Init(uint16_t XSize, uint16_t YSize)
{
	TsXBoundary = XSize;
	TsYBoundary = YSize;

	if(stmpe811_ReadID(0x82)==STMPE811_ID)
	{
		stmpe811_Init(0x82);
		stmpe811_TS_Start(0x82);
	}
	else
		return TS_ERROR;

	return TS_OK;
}

void TS_GetState(TS_StateTypeDef *TsState)
{
	static uint32_t _x=0, _y=0;
	uint16_t xDiff, yDiff , x , y, xr, yr;

	TsState->TouchDetected=stmpe811_TS_DetectTouch(0x82);

	if(TsState->TouchDetected)
	{
		stmpe811_TS_GetXY(0x82, &x, &y);

		y -= 360;
		yr=y/11;

		if(yr<=0)
			yr=0;
		else
		{
			if(yr>TsYBoundary)
				yr=TsYBoundary-1;
		}

		y=yr;

		if(x<=3000)
			x=3870-x;
		else
			x=3800-x;

		xr=x/15;

		if(xr<=0)
			xr=0;
		else
		{
			if(xr>TsXBoundary)
				xr=TsXBoundary-1;
		}

		x=xr;
		xDiff=x>_x?(x-_x):(_x-x);
		yDiff=y>_y?(y-_y):(_y-y);

		if(xDiff+yDiff>5)
		{
			_x=x;
			_y=y;
		}

		TsState->X=_x;
		TsState->Y=_y;
	}
}
