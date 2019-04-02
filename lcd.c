#include <stdarg.h>
#include <math.h>
#include "stm32f4xx_hal.h"
#include "lcd.h"

#define ILI9341_ID									0x9341
#define ILI9341_HSYNC								9		// Horizontal synchronization
#define ILI9341_HBP									29		// Horizontal back porch
#define ILI9341_HFP									2		// Horizontal front porch
#define ILI9341_VSYNC								1		// Vertical synchronization
#define ILI9341_VBP									3		// Vertical back porch
#define ILI9341_VFP									2		// Vertical front porch
#define ILI9341_SWRESET								0x01	// Software Reset
#define ILI9341_READ_DISPLAY_ID						0x04	// Read display identification information
#define ILI9341_RDDST								0x09	// Read Display Status
#define ILI9341_RDDPM								0x0A	// Read Display Power Mode
#define ILI9341_RDDMADCTL							0x0B	// Read Display MADCTL
#define ILI9341_RDDCOLMOD							0x0C	// Read Display Pixel Format
#define ILI9341_RDDIM								0x0D	// Read Display Image Format
#define ILI9341_RDDSM								0x0E	// Read Display Signal Mode
#define ILI9341_RDDSDR								0x0F	// Read Display Self-Diagnostic Result
#define ILI9341_SPLIN								0x10	// Enter Sleep Mode
#define ILI9341_SLEEP_OUT							0x11	// Sleep out register
#define ILI9341_PTLON								0x12	// Partial Mode ON
#define ILI9341_NORMAL_MODE_ON						0x13	// Normal Display Mode ON
#define ILI9341_DINVOFF								0x20	// Display Inversion OFF
#define ILI9341_DINVON								0x21	// Display Inversion ON
#define ILI9341_GAMMA								0x26	// Gamma register
#define ILI9341_DISPLAY_OFF							0x28	// Display off register
#define ILI9341_DISPLAY_ON							0x29	// Display on register
#define ILI9341_COLUMN_ADDR							0x2A	// Colomn address register
#define ILI9341_PAGE_ADDR							0x2B	// Page address register
#define ILI9341_GRAM								0x2C	// GRAM register
#define ILI9341_RGBSET								0x2D	// Color SET
#define ILI9341_RAMRD								0x2E	// Memory Read
#define ILI9341_PLTAR								0x30	// Partial Area
#define ILI9341_VSCRDEF								0x33	// Vertical Scrolling Definition
#define ILI9341_TEOFF								0x34	// Tearing Effect Line OFF
#define ILI9341_TEON								0x35	// Tearing Effect Line ON
#define ILI9341_MAC									0x36	// Memory Access Control register
#define ILI9341_VSCRSADD							0x37	// Vertical Scrolling Start Address
#define ILI9341_IDMOFF								0x38	// Idle Mode OFF
#define ILI9341_IDMON								0x39	// Idle Mode ON
#define ILI9341_PIXEL_FORMAT						0x3A	// Pixel Format register
#define ILI9341_WRITE_MEM_CONTINUE					0x3C	// Write Memory Continue
#define ILI9341_READ_MEM_CONTINUE					0x3E	// Read Memory Continue
#define ILI9341_SET_TEAR_SCANLINE					0x44	// Set Tear Scanline
#define ILI9341_GET_SCANLINE						0x45	// Get Scanline
#define ILI9341_WDB									0x51	// Write Brightness Display register
#define ILI9341_RDDISBV								0x52	// Read Display Brightness
#define ILI9341_WCD									0x53	// Write Control Display register
#define ILI9341_RDCTRLD								0x54	// Read CTRL Display
#define ILI9341_WRCABC								0x55	// Write Content Adaptive Brightness Control
#define ILI9341_RDCABC								0x56	// Read Content Adaptive Brightness Control
#define ILI9341_WRITE_CABC							0x5E	// Write CABC Minimum Brightness
#define ILI9341_READ_CABC							0x5F	// Read CABC Minimum Brightness
#define ILI9341_READ_ID1							0xDA	// Read ID1
#define ILI9341_READ_ID2							0xDB	// Read ID2
#define ILI9341_READ_ID3							0xDC	// Read ID3
#define ILI9341_READ_ID4							0xD3	// Read ID4
#define ILI9341_READ_ID4_SIZE						3		// Size of Read ID4
#define ILI9341_RGB_INTERFACE						0xB0	// RGB Interface Signal Control
#define ILI9341_FRMCTR1								0xB1	// Frame Rate Control (In Normal Mode)
#define ILI9341_FRMCTR2								0xB2	// Frame Rate Control (In Idle Mode)
#define ILI9341_FRMCTR3								0xB3	// Frame Rate Control (In Partial Mode)
#define ILI9341_INVTR								0xB4	// Display Inversion Control
#define ILI9341_BPC									0xB5	// Blanking Porch Control register
#define ILI9341_DFC									0xB6	// Display Function Control register
#define ILI9341_ETMOD								0xB7	// Entry Mode Set
#define ILI9341_BACKLIGHT1							0xB8	// Backlight Control 1
#define ILI9341_BACKLIGHT2							0xB9	// Backlight Control 2
#define ILI9341_BACKLIGHT3							0xBA	// Backlight Control 3
#define ILI9341_BACKLIGHT4							0xBB	// Backlight Control 4
#define ILI9341_BACKLIGHT5							0xBC	// Backlight Control 5
#define ILI9341_BACKLIGHT7							0xBE	// Backlight Control 7
#define ILI9341_BACKLIGHT8							0xBF	// Backlight Control 8
#define ILI9341_POWER1								0xC0	// Power Control 1 register
#define ILI9341_POWER2								0xC1	// Power Control 2 register
#define ILI9341_VCOM1								0xC5	// VCOM Control 1 register
#define ILI9341_VCOM2								0xC7	// VCOM Control 2 register
#define ILI9341_NVMWR								0xD0	// NV Memory Write
#define ILI9341_NVMPKEY								0xD1	// NV Memory Protection Key
#define ILI9341_RDNVM								0xD2	// NV Memory Status Read
#define ILI9341_PGAMMA								0xE0	// Positive Gamma Correction register
#define ILI9341_NGAMMA								0xE1	// Negative Gamma Correction register
#define ILI9341_DGAMCTRL1							0xE2	// Digital Gamma Control 1
#define ILI9341_DGAMCTRL2							0xE3	// Digital Gamma Control 2
#define ILI9341_INTERFACE							0xF6	// Interface control register
#define ILI9341_POWERA								0xCB	// Power control A register
#define ILI9341_POWERB								0xCF	// Power control B register
#define ILI9341_DTCA								0xE8	// Driver timing control A
#define ILI9341_DTCB								0xEA	// Driver timing control B
#define ILI9341_POWER_SEQ							0xED	// Power on sequence register
#define ILI9341_3GAMMA_EN							0xF2	// Gamma enable register
#define ILI9341_PRC									0xF7	// Pump ratio control register

#define SDRAM_MODEREG_BURST_LENGTH_1				0x0000
#define SDRAM_MODEREG_BURST_LENGTH_2				0x0001
#define SDRAM_MODEREG_BURST_LENGTH_4				0x0002
#define SDRAM_MODEREG_BURST_LENGTH_8				0x0004
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL			0x0000
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED		0x0008
#define SDRAM_MODEREG_CAS_LATENCY_2					0x0020
#define SDRAM_MODEREG_CAS_LATENCY_3					0x0030
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD		0x0000
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED	0x0000
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE		0x0200

LTDC_HandleTypeDef LtdcHandler;
DMA2D_HandleTypeDef Dma2dHandler;
SDRAM_HandleTypeDef SdramHandle;
DMA_HandleTypeDef dmaHandle;

void LCD_WriteData(uint16_t RegValue)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
	SPI_Write(RegValue);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET);
}

void LCD_WriteReg(uint8_t Reg)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
	SPI_Write(Reg);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET);
}

void LTDC_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

	PeriphClkInitStruct.PeriphClockSelection=RCC_PERIPHCLK_LTDC;
	PeriphClkInitStruct.PLLSAI.PLLSAIN=192;
	PeriphClkInitStruct.PLLSAI.PLLSAIR=4;
	PeriphClkInitStruct.PLLSAIDivR=RCC_PLLSAIDIVR_8;
	HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct); 
  
	__LTDC_CLK_ENABLE();
	__DMA2D_CLK_ENABLE(); 
  
	__GPIOA_CLK_ENABLE();
	__GPIOB_CLK_ENABLE();
	__GPIOC_CLK_ENABLE();
	__GPIOD_CLK_ENABLE();
	__GPIOF_CLK_ENABLE();
	__GPIOG_CLK_ENABLE();

	GPIO_InitStructure.Pin=GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_6|GPIO_PIN_11|GPIO_PIN_12; //Need 11 and 12 for CANBUS
	GPIO_InitStructure.Mode=GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull=GPIO_NOPULL;
	GPIO_InitStructure.Speed=GPIO_SPEED_FAST;
	GPIO_InitStructure.Alternate=GPIO_AF14_LTDC;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.Pin=GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.Pin=GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_10;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.Pin=GPIO_PIN_3|GPIO_PIN_6;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);
  
	GPIO_InitStructure.Pin=GPIO_PIN_10;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStructure);

	GPIO_InitStructure.Pin=GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_11;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);
 
	GPIO_InitStructure.Pin=GPIO_PIN_0|GPIO_PIN_1;
	GPIO_InitStructure.Alternate=GPIO_AF9_LTDC;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.Pin=GPIO_PIN_10|GPIO_PIN_12;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);

	LtdcHandler.Instance=LTDC;
	LtdcHandler.Init.HorizontalSync=ILI9341_HSYNC;
	LtdcHandler.Init.VerticalSync=ILI9341_VSYNC;
	LtdcHandler.Init.AccumulatedHBP=ILI9341_HBP;
	LtdcHandler.Init.AccumulatedVBP=ILI9341_VBP;
	LtdcHandler.Init.AccumulatedActiveW=269;
	LtdcHandler.Init.AccumulatedActiveH=323;
	LtdcHandler.Init.TotalWidth=279;
	LtdcHandler.Init.TotalHeigh=327;
	LtdcHandler.Init.Backcolor.Red=0;
	LtdcHandler.Init.Backcolor.Blue=0;
	LtdcHandler.Init.Backcolor.Green=0;
	LtdcHandler.Init.HSPolarity=LTDC_HSPOLARITY_AL;
	LtdcHandler.Init.VSPolarity=LTDC_VSPOLARITY_AL;
	LtdcHandler.Init.DEPolarity=LTDC_DEPOLARITY_AL;
	LtdcHandler.Init.PCPolarity=LTDC_PCPOLARITY_IPC;
	HAL_LTDC_Init(&LtdcHandler); 
}

void SDRAM_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	FMC_SDRAM_TimingTypeDef Timing;
	FMC_SDRAM_CommandTypeDef Command;

	Timing.LoadToActiveDelay=2;
	Timing.ExitSelfRefreshDelay=7;
	Timing.SelfRefreshTime=4;
	Timing.RowCycleDelay=7;
	Timing.WriteRecoveryTime=2;
	Timing.RPDelay=2;
	Timing.RCDDelay=2;

	SdramHandle.Instance=FMC_SDRAM_DEVICE;
	SdramHandle.Init.SDBank=FMC_SDRAM_BANK2;
	SdramHandle.Init.ColumnBitsNumber=FMC_SDRAM_COLUMN_BITS_NUM_8;
	SdramHandle.Init.RowBitsNumber=FMC_SDRAM_ROW_BITS_NUM_12;
	SdramHandle.Init.MemoryDataWidth=FMC_SDRAM_MEM_BUS_WIDTH_16;
	SdramHandle.Init.InternalBankNumber=FMC_SDRAM_INTERN_BANKS_NUM_4;
	SdramHandle.Init.CASLatency=FMC_SDRAM_CAS_LATENCY_3;
	SdramHandle.Init.WriteProtection=FMC_SDRAM_WRITE_PROTECTION_DISABLE;
	SdramHandle.Init.SDClockPeriod=FMC_SDRAM_CLOCK_PERIOD_2;
	SdramHandle.Init.ReadBurst=FMC_SDRAM_RBURST_DISABLE;
	SdramHandle.Init.ReadPipeDelay=FMC_SDRAM_RPIPE_DELAY_1;

	__FMC_CLK_ENABLE();
	__DMA2_CLK_ENABLE();
	__GPIOB_CLK_ENABLE();
	__GPIOC_CLK_ENABLE();
	__GPIOD_CLK_ENABLE();
	__GPIOE_CLK_ENABLE();
	__GPIOF_CLK_ENABLE();
	__GPIOG_CLK_ENABLE();

	GPIO_InitStructure.Mode=GPIO_MODE_AF_PP;
	GPIO_InitStructure.Speed=GPIO_SPEED_FAST;
	GPIO_InitStructure.Pull=GPIO_NOPULL;
	GPIO_InitStructure.Alternate=GPIO_AF12_FMC;

	GPIO_InitStructure.Pin=GPIO_PIN_5|GPIO_PIN_6;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);  

	GPIO_InitStructure.Pin=GPIO_PIN_0;      
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);  

	GPIO_InitStructure.Pin=GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_14|GPIO_PIN_15;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.Pin=GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_InitStructure.Pin=GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStructure);

	GPIO_InitStructure.Pin=GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_8|GPIO_PIN_15;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);

	dmaHandle.Init.Channel=DMA_CHANNEL_0;
	dmaHandle.Init.Direction=DMA_MEMORY_TO_MEMORY;
	dmaHandle.Init.PeriphInc=DMA_PINC_ENABLE;
	dmaHandle.Init.MemInc=DMA_MINC_ENABLE;
	dmaHandle.Init.PeriphDataAlignment=DMA_PDATAALIGN_WORD;
	dmaHandle.Init.MemDataAlignment=DMA_MDATAALIGN_WORD;
	dmaHandle.Init.Mode=DMA_NORMAL;
	dmaHandle.Init.Priority=DMA_PRIORITY_HIGH;
	dmaHandle.Init.FIFOMode=DMA_FIFOMODE_DISABLE;
	dmaHandle.Init.FIFOThreshold=DMA_FIFO_THRESHOLD_FULL;
	dmaHandle.Init.MemBurst=DMA_MBURST_SINGLE;
	dmaHandle.Init.PeriphBurst=DMA_PBURST_SINGLE;
	dmaHandle.Instance=DMA2_Stream0;

	__HAL_LINKDMA(&SdramHandle, hdma, dmaHandle);

	HAL_DMA_DeInit(&dmaHandle);
	HAL_DMA_Init(&dmaHandle); 

	HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
	HAL_SDRAM_Init(&SdramHandle, &Timing);

	Command.CommandMode=FMC_SDRAM_CMD_CLK_ENABLE;
	Command.CommandTarget=FMC_SDRAM_CMD_TARGET_BANK2;
	Command.AutoRefreshNumber=1;
	Command.ModeRegisterDefinition=0;
	HAL_SDRAM_SendCommand(&SdramHandle, &Command, 0xFFFF);
	HAL_Delay(1);

	Command.CommandMode=FMC_SDRAM_CMD_PALL;
	Command.CommandTarget=FMC_SDRAM_CMD_TARGET_BANK2;
	Command.AutoRefreshNumber=1;
	Command.ModeRegisterDefinition=0;
	HAL_SDRAM_SendCommand(&SdramHandle, &Command, 0xFFFF);

	Command.CommandMode=FMC_SDRAM_CMD_AUTOREFRESH_MODE;
	Command.CommandTarget=FMC_SDRAM_CMD_TARGET_BANK2;
	Command.AutoRefreshNumber=4;
	Command.ModeRegisterDefinition=0;
	HAL_SDRAM_SendCommand(&SdramHandle, &Command, 0xFFFF);

	Command.CommandMode=FMC_SDRAM_CMD_LOAD_MODE;
	Command.CommandTarget=FMC_SDRAM_CMD_TARGET_BANK2;
	Command.AutoRefreshNumber=1;
	Command.ModeRegisterDefinition=SDRAM_MODEREG_BURST_LENGTH_1|SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL|SDRAM_MODEREG_CAS_LATENCY_3|SDRAM_MODEREG_OPERATING_MODE_STANDARD|SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;
	HAL_SDRAM_SendCommand(&SdramHandle, &Command, 0xFFFF);

	HAL_SDRAM_ProgramRefreshRate(&SdramHandle, 1386); 
}

void LCD_Init(void)
{ 
	GPIO_InitTypeDef GPIO_InitStructure;

	//Set up LTDC
	LTDC_Init();

	//Set up SDRAM
	SDRAM_Init();

	//Set up SPI
	SPI_Init();

	//Set up LCD
	__GPIOD_CLK_ENABLE();
	GPIO_InitStructure.Pin=GPIO_PIN_12|GPIO_PIN_13; //PD12=RDX, PD13=WRX
	GPIO_InitStructure.Mode=GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull=GPIO_NOPULL;
	GPIO_InitStructure.Speed=GPIO_SPEED_FAST;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);

	__GPIOC_CLK_ENABLE();
	GPIO_InitStructure.Pin=GPIO_PIN_2; //PC2=NCS
	GPIO_InitStructure.Mode=GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull=GPIO_NOPULL;
	GPIO_InitStructure.Speed=GPIO_SPEED_FAST;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET);

	LCD_WriteReg(0xCA);
	LCD_WriteData(0xC3);	LCD_WriteData(0x08);	LCD_WriteData(0x50);

	LCD_WriteReg(ILI9341_POWERB);
	LCD_WriteData(0x00);	LCD_WriteData(0xC1);	LCD_WriteData(0x30);

	LCD_WriteReg(ILI9341_POWER_SEQ);
	LCD_WriteData(0x64);	LCD_WriteData(0x03);	LCD_WriteData(0x12);	LCD_WriteData(0x81);

	LCD_WriteReg(ILI9341_DTCA);
	LCD_WriteData(0x85);	LCD_WriteData(0x00);	LCD_WriteData(0x78);

	LCD_WriteReg(ILI9341_POWERA);
	LCD_WriteData(0x39);	LCD_WriteData(0x2C);	LCD_WriteData(0x00);	LCD_WriteData(0x34);	LCD_WriteData(0x02);

	LCD_WriteReg(ILI9341_PRC);
	LCD_WriteData(0x20);

	LCD_WriteReg(ILI9341_DTCB);
	LCD_WriteData(0x00);	LCD_WriteData(0x00);

	LCD_WriteReg(ILI9341_FRMCTR1);
	LCD_WriteData(0x00);	LCD_WriteData(0x1B);

	LCD_WriteReg(ILI9341_DFC);
	LCD_WriteData(0x0A);	LCD_WriteData(0xA2);

	LCD_WriteReg(ILI9341_POWER1);
	LCD_WriteData(0x10);

	LCD_WriteReg(ILI9341_POWER2);
	LCD_WriteData(0x10);

	LCD_WriteReg(ILI9341_VCOM1);
	LCD_WriteData(0x45);	LCD_WriteData(0x15);

	LCD_WriteReg(ILI9341_VCOM2);
	LCD_WriteData(0x90);

	LCD_WriteReg(ILI9341_MAC);
	LCD_WriteData(0xC8);

	LCD_WriteReg(ILI9341_3GAMMA_EN);
	LCD_WriteData(0x00);

	LCD_WriteReg(ILI9341_RGB_INTERFACE);
	LCD_WriteData(0xC2);

	LCD_WriteReg(ILI9341_DFC);
	LCD_WriteData(0x0A);	LCD_WriteData(0xA7);	LCD_WriteData(0x27);	LCD_WriteData(0x04);

	LCD_WriteReg(ILI9341_COLUMN_ADDR);
	LCD_WriteData(0x00);	LCD_WriteData(0x00);	LCD_WriteData(0x00);	LCD_WriteData(0xEF);

	LCD_WriteReg(ILI9341_PAGE_ADDR);
	LCD_WriteData(0x00);	LCD_WriteData(0x00);	LCD_WriteData(0x01);	LCD_WriteData(0x3F);

	LCD_WriteReg(ILI9341_INTERFACE);
	LCD_WriteData(0x01);	LCD_WriteData(0x00);	LCD_WriteData(0x06);

	LCD_WriteReg(ILI9341_GRAM);
	HAL_Delay(200);

	LCD_WriteReg(ILI9341_GAMMA);
	LCD_WriteData(0x01);

	LCD_WriteReg(ILI9341_PGAMMA);
	LCD_WriteData(0x0F);	LCD_WriteData(0x29);	LCD_WriteData(0x24);
	LCD_WriteData(0x0C);	LCD_WriteData(0x0E);	LCD_WriteData(0x09);
	LCD_WriteData(0x4E);	LCD_WriteData(0x78);	LCD_WriteData(0x3C);
	LCD_WriteData(0x09);	LCD_WriteData(0x13);	LCD_WriteData(0x05);
	LCD_WriteData(0x17);	LCD_WriteData(0x11);	LCD_WriteData(0x00);

	LCD_WriteReg(ILI9341_NGAMMA);
	LCD_WriteData(0x00);	LCD_WriteData(0x16);	LCD_WriteData(0x1B);
	LCD_WriteData(0x04);	LCD_WriteData(0x11);	LCD_WriteData(0x07);
	LCD_WriteData(0x31);	LCD_WriteData(0x33);	LCD_WriteData(0x42);
	LCD_WriteData(0x05);	LCD_WriteData(0x0C);	LCD_WriteData(0x0A);
	LCD_WriteData(0x28);	LCD_WriteData(0x2F);	LCD_WriteData(0x0F);

	LCD_WriteReg(ILI9341_SLEEP_OUT);
	HAL_Delay(200);

	LCD_WriteReg(ILI9341_DISPLAY_ON);
	LCD_WriteReg(ILI9341_GRAM);

	//Set up front and back buffers, then clear them
	LCD_LayerDefaultInit(0, (uint32_t)LCD_FRONT_BUFFER);
	LCD_LayerDefaultInit(1, (uint32_t)LCD_BACK_BUFFER);

	LCD_SetLayerVisible(0, ENABLE);
	LCD_SetLayerVisible(1, DISABLE);

	LCD_Clear(0, 0xFF000000);
	LCD_Clear(1, 0xFF000000);
}  

void LCD_LayerDefaultInit(uint32_t LayerIndex, uint32_t FB_Address)
{     
	LTDC_LayerCfgTypeDef Layercfg;

	Layercfg.WindowX0=0;
	Layercfg.WindowX1=LCD_WIDTH;
	Layercfg.WindowY0=0;
	Layercfg.WindowY1=LCD_HEIGHT;
	Layercfg.PixelFormat=LTDC_PIXEL_FORMAT_ARGB8888;
	Layercfg.FBStartAdress=FB_Address;
	Layercfg.Alpha=255;
	Layercfg.Alpha0=0;
	Layercfg.Backcolor.Blue=0;
	Layercfg.Backcolor.Green=0;
	Layercfg.Backcolor.Red=0;
	Layercfg.BlendingFactor1=LTDC_BLENDING_FACTOR1_PAxCA;
	Layercfg.BlendingFactor2=LTDC_BLENDING_FACTOR2_PAxCA;
	Layercfg.ImageWidth=LCD_WIDTH;
	Layercfg.ImageHeight=LCD_HEIGHT;
	HAL_LTDC_ConfigLayer(&LtdcHandler, &Layercfg, LayerIndex);

	HAL_LTDC_EnableDither(&LtdcHandler);
}

void LCD_SetLayerVisible(uint32_t LayerIndex, FunctionalState state)
{
	if(state==ENABLE)
		__HAL_LTDC_LAYER_ENABLE(&LtdcHandler, LayerIndex);
	else
		__HAL_LTDC_LAYER_DISABLE(&LtdcHandler, LayerIndex);

	__HAL_LTDC_RELOAD_CONFIG(&LtdcHandler);
} 

void LCD_Clear(uint32_t LayerIndex, uint32_t Color)
{
	Dma2dHandler.Init.Mode=DMA2D_R2M;
	Dma2dHandler.Init.ColorMode=DMA2D_ARGB8888;
	Dma2dHandler.Init.OutputOffset=0;
	Dma2dHandler.Instance=DMA2D;

	if(HAL_DMA2D_Init(&Dma2dHandler)==HAL_OK)
	{
		if(HAL_DMA2D_ConfigLayer(&Dma2dHandler, LayerIndex)==HAL_OK)
		{
			if(HAL_DMA2D_Start(&Dma2dHandler, Color, LtdcHandler.LayerCfg[LayerIndex].FBStartAdress, LCD_WIDTH, LCD_HEIGHT)==HAL_OK)
				HAL_DMA2D_PollForTransfer(&Dma2dHandler, 10);
		}
	} 
}

void LCD_CopyBuffers(void)
{
	Dma2dHandler.Init.Mode=DMA2D_M2M;
	Dma2dHandler.Init.ColorMode=DMA2D_ARGB8888;
	Dma2dHandler.Init.OutputOffset=0;
	Dma2dHandler.Instance=DMA2D;

	if(HAL_DMA2D_Init(&Dma2dHandler)==HAL_OK)
	{
		if(HAL_DMA2D_ConfigLayer(&Dma2dHandler, 1)==HAL_OK)
		{
			if(HAL_DMA2D_Start(&Dma2dHandler, LtdcHandler.LayerCfg[1].FBStartAdress, LtdcHandler.LayerCfg[0].FBStartAdress, LCD_WIDTH, LCD_HEIGHT)==HAL_OK)
				HAL_DMA2D_PollForTransfer(&Dma2dHandler, 10);
		}
	} 
}

void LCD_DisplayOn(void)
{
	LCD_WriteReg(ILI9341_DISPLAY_ON);
}

void LCD_DisplayOff(void)
{
	LCD_WriteReg(ILI9341_DISPLAY_OFF);
}

void LCD_DrawPixel(uint16_t x, uint16_t y, uint32_t color)
{
	if(x<=0||x>=LCD_HEIGHT)
		return;
	if(y<=0||y>=LCD_WIDTH)
		return;

	((uint32_t *)LtdcHandler.LayerCfg[1].FBStartAdress)[x*LCD_WIDTH+(LCD_WIDTH-y)]=color;
}

void LCD_DrawLine(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2, uint32_t Color)
{
	int16_t deltax=0, deltay=0, x=0, y=0, xinc1=0, xinc2=0, yinc1=0, yinc2=0, den=0, num=0, numadd=0, numpixels=0, curpixel=0;

	deltax=abs(X2-X1);
	deltay=abs(Y2-Y1);

	x=X1;
	y=Y1;

	if(X2>=X1)
	{
		xinc1=1;
		xinc2=1;
	}
	else
	{
		xinc1=-1;
		xinc2=-1;
	}

	if(Y2>=Y1)
	{
		yinc1=1;
		yinc2=1;
	}
	else
	{
		yinc1=-1;
		yinc2=-1;
	}

	if(deltax>=deltay)
	{
		xinc1=0;
		yinc2=0;
		den=deltax;
		num=deltax/2;
		numadd=deltay;
		numpixels=deltax;
	}
	else
	{
		xinc2=0;
		yinc1=0;
		den=deltay;
		num=deltay/2;
		numadd=deltax;
		numpixels=deltay;
	}

	for(curpixel=0;curpixel<=numpixels;curpixel++)
	{
		LCD_DrawPixel(x, y, Color);
		num+=numadd;

		if(num>=den)
		{
			num-=den;
			x+=xinc1;
			y+=yinc1;
		}

		x+=xinc2;
		y+=yinc2;
	}
}

void LCD_DrawCircle(uint16_t x, uint16_t y, uint16_t r, uint32_t color)
{
	int32_t  d;
	uint32_t curx, cury;

	d=3-(r<<1);
	curx=0;
	cury=r;

	while(curx<=cury)
	{
		LCD_DrawPixel(x+curx, y-cury, color);
		LCD_DrawPixel(x-curx, y-cury, color);
		LCD_DrawPixel(x+cury, y-curx, color);
		LCD_DrawPixel(x-cury, y-curx, color);
		LCD_DrawPixel(x+curx, y+cury, color);
		LCD_DrawPixel(x-curx, y+cury, color);
		LCD_DrawPixel(x+cury, y+curx, color);
		LCD_DrawPixel(x-cury, y+curx, color);

		if(d<0)
			d+=(curx<<2)+6;
		else
		{
			d+=((curx-cury)<<2)+10;
			cury--;
		}

		curx++;
	}
}

void LCD_DrawVLine(short x, short y, short h, unsigned long color)
{
	while(h-->1)
		LCD_DrawPixel(x, y+h, color);
}

// Used to do circles and roundrects
void LCD_DrawFillCircle(short x0, short y0, short r, unsigned long color)
{
	short f=1-r;
	short ddF_x=1;
	short ddF_y=-2*r;
	short x=0;
	short y=r;

	LCD_DrawVLine(x0, y0-r, 2*r+1, color);

	while(x<y)
	{
		if(f>=0)
		{
			y--;
			ddF_y+=2;
			f+=ddF_y;
		}

		x++;
		ddF_x+=2;
		f+=ddF_x;

		LCD_DrawVLine(x0+x, y0-y, 2*y+1, color);
		LCD_DrawVLine(x0+y, y0-x, 2*x+1, color);
		LCD_DrawVLine(x0-x, y0-y, 2*y+1, color);
		LCD_DrawVLine(x0-y, y0-x, 2*x+1, color);
	}
}

void LCD_DrawTriangle(short x0, short y0, short x1, short y1, short x2, short y2, unsigned long color)
{
	LCD_DrawLine(x0, y0, x1, y1, color);
	LCD_DrawLine(x1, y1, x2, y2, color);
	LCD_DrawLine(x2, y2, x0, y0, color);
}

void LCD_PutChar(int x, int y, char c)
{
	int i, j;

	for(j=0;j<FONT_HEIGHT;j++)
	{
		for(i=0;i<FONT_WIDTH;i++)
		{
			if(fontdata[(c*FONT_HEIGHT)+j]&(0x80>>i))
				LCD_DrawPixel(x+i, y+j, 0xFFFFFFFF);
		}
	}
}

void LCD_Printf(int x, int y, char *string, ...)
{
	char *ptr, text[(LCD_WIDTH/FONT_WIDTH)*(LCD_HEIGHT/FONT_HEIGHT)]; //Big enough for full screen.
	va_list	ap;
	int sx=x;

	if(string==NULL)
		return;

	va_start(ap, string);
		vsprintf(text, string, ap);
	va_end(ap);

	for(ptr=text;*ptr!='\0';ptr++)
	{
		if(*ptr=='\n'||*ptr=='\r')
		{
			x=sx;
			y+=FONT_HEIGHT;
			continue;
		}
		if(*ptr=='\t')
		{
			x+=FONT_WIDTH*4;
			continue;
		}

		LCD_PutChar(x, y, *ptr);
		x+=FONT_WIDTH;
	}
}
