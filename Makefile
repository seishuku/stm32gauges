OBJS += CMSIS/syscalls.o
OBJS += CMSIS/system_stm32f4xx.o
OBJS += CMSIS/startup_stm32f429xx.o

OBJS += HAL/stm32f4xx_hal.o
#OBJS += HAL/stm32f4xx_hal_adc.o
#OBJS += HAL/stm32f4xx_hal_adc_ex.o
OBJS += HAL/stm32f4xx_hal_can.o
#OBJS += HAL/stm32f4xx_hal_cec.o
OBJS += HAL/stm32f4xx_hal_cortex.o
#OBJS += HAL/stm32f4xx_hal_crc.o
#OBJS += HAL/stm32f4xx_hal_cryp.o
#OBJS += HAL/stm32f4xx_hal_cryp_ex.o
#OBJS += HAL/stm32f4xx_hal_dac.o
#OBJS += HAL/stm32f4xx_hal_dac_ex.o
#OBJS += HAL/stm32f4xx_hal_dcmi.o
#OBJS += HAL/stm32f4xx_hal_dcmi_ex.o
OBJS += HAL/stm32f4xx_hal_dma.o
OBJS += HAL/stm32f4xx_hal_dma2d.o
OBJS += HAL/stm32f4xx_hal_dma_ex.o
#OBJS += HAL/stm32f4xx_hal_eth.o
#OBJS += HAL/stm32f4xx_hal_flash.o
#OBJS += HAL/stm32f4xx_hal_flash_ex.o
#OBJS += HAL/stm32f4xx_hal_flash_ramfunc.o
#OBJS += HAL/stm32f4xx_hal_fmpi2c.o
#OBJS += HAL/stm32f4xx_hal_fmpi2c_ex.o
OBJS += HAL/stm32f4xx_hal_gpio.o
#OBJS += HAL/stm32f4xx_hal_hash.o
#OBJS += HAL/stm32f4xx_hal_hash_ex.o
#OBJS += HAL/stm32f4xx_hal_hcd.o
OBJS += HAL/stm32f4xx_hal_i2c.o
OBJS += HAL/stm32f4xx_hal_i2c_ex.o
#OBJS += HAL/stm32f4xx_hal_i2s.o
#OBJS += HAL/stm32f4xx_hal_i2s_ex.o
#OBJS += HAL/stm32f4xx_hal_irda.o
#OBJS += HAL/stm32f4xx_hal_iwdg.o
OBJS += HAL/stm32f4xx_hal_ltdc.o
#OBJS += HAL/stm32f4xx_hal_nand.o
#OBJS += HAL/stm32f4xx_hal_nor.o
#OBJS += HAL/stm32f4xx_hal_pccard.o
#OBJS += HAL/stm32f4xx_hal_pcd.o
#OBJS += HAL/stm32f4xx_hal_pcd_ex.o
OBJS += HAL/stm32f4xx_hal_pwr.o
OBJS += HAL/stm32f4xx_hal_pwr_ex.o
#OBJS += HAL/stm32f4xx_hal_qspi.o
OBJS += HAL/stm32f4xx_hal_rcc.o
OBJS += HAL/stm32f4xx_hal_rcc_ex.o
#OBJS += HAL/stm32f4xx_hal_rng.o
#OBJS += HAL/stm32f4xx_hal_rtc.o
#OBJS += HAL/stm32f4xx_hal_rtc_ex.o
#OBJS += HAL/stm32f4xx_hal_sai.o
#OBJS += HAL/stm32f4xx_hal_sai_ex.o
#OBJS += HAL/stm32f4xx_hal_sd.o
OBJS += HAL/stm32f4xx_hal_sdram.o
#OBJS += HAL/stm32f4xx_hal_smartcard.o
#OBJS += HAL/stm32f4xx_hal_spdifrx.o
OBJS += HAL/stm32f4xx_hal_spi.o
OBJS += HAL/stm32f4xx_hal_sram.o
#OBJS += HAL/stm32f4xx_hal_tim.o
#OBJS += HAL/stm32f4xx_hal_tim_ex.o
#OBJS += HAL/stm32f4xx_hal_uart.o
#OBJS += HAL/stm32f4xx_hal_usart.o
#OBJS += HAL/stm32f4xx_hal_wwdg.o
OBJS += HAL/stm32f4xx_ll_fmc.o
OBJS += HAL/stm32f4xx_ll_fsmc.o
#OBJS += HAL/stm32f4xx_ll_sdmmc.o
#OBJS += HAL/stm32f4xx_ll_usb.o

OBJS += stm32f4xx_it.o
OBJS += stmpe811.o
OBJS += spi.o
OBJS += i2c.o
OBJS += lcd.o
OBJS += main.o

PROJECT=main

CFLAGS = -Wall -fno-common -mcpu=cortex-m4 -mthumb -O3 -mfloat-abi=hard -ICMSIS -IHAL -IUSB -I. -DSTM32F429xx
ASFLAGS = -mcpu=cortex-m4
LDFLAGS  = -lm -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -nostartfiles --specs=nano.specs -TCMSIS/STM32F429ZITx_FLASH.ld

CC = arm-eabi-gcc
AS = arm-eabi-as
OBJCOPY = arm-eabi-objcopy

all:: $(PROJECT).bin

run: $(PROJECT).bin
	st-link_cli -P $(PROJECT).bin 0x08000000 -W32 -Run

$(PROJECT).bin: $(PROJECT).elf
	$(OBJCOPY) -R .stack -O binary $(PROJECT).elf $(PROJECT).bin

$(PROJECT).elf: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(PROJECT).elf

clean:
	rm -f $(OBJS) $(PROJECT).elf $(PROJECT).bin

.c.o :
	$(CC) $(CFLAGS) -c $< -o $@    

.cpp.o :
	$(CC) $(CFLAGS) -c $< -o $@

.s.o :
	$(AS) $(ASFLAGS) -o $@ $<
