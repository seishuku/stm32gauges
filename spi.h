#ifndef __SPI_H__
#define __SPI_H__

void SPI_Init(void);
uint8_t SPI_Read(void);
void SPI_Write(uint8_t write);
uint8_t SPI_WriteRead(uint8_t write);

#endif
